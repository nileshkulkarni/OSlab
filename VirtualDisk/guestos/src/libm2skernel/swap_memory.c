#include <m2skernel.h>
#include <sys/mman.h>

int swap_mem_safe_mode = 1;
unsigned long swap_mem_mapped_space = 0;
unsigned long swap_mem_max_mapped_space = 0;
FILE* swap_fd; 

/* Return mem page corresponding to an address. */
struct swap_mem_page_t *swap_mem_page_get(struct swap_mem_t *swap_mem, uint32_t addr)
{
	uint32_t index, tag;
	struct swap_mem_page_t *prev, *page;
	
	tag = addr & ~(MEM_PAGESIZE - 1);
	index = (addr >> MEM_LOGPAGESIZE) % MEM_PAGE_COUNT;
	page = swap_mem->pages[index];
	
	prev = NULL;
	
	/* Look for page */
	while (page && page->tag != tag) {
		prev = page;
		page = page->next;
	}
	
	/* Place page into list head */
	if (prev && page) {
		prev->next = page->next;
		page->next = mem->pages[index];
		mem->pages[index] = page;
	}
	
	/* Return found page */
	return page;
}

/* Return the memory page following addr in the current memory map. This function
 * is useful to reconstruct consecutive ranges of mapped pages. */
struct swap_mem_page_t *swap_mem_page_get_next(struct swap_mem_t *swap_mem, uint32_t addr)
{
	uint32_t tag, index, mintag;
	struct swap_mem_page_t *prev, *page, *minpage;

	/* Get tag of the page just following addr */
	tag = (addr + MEM_PAGESIZE) & ~(MEM_PAGESIZE - 1);
	if (!tag)
		return NULL;
	index = (tag >> MEM_LOGPAGESIZE) % MEM_PAGE_COUNT;
	page = swap_mem->pages[index];
	prev = NULL;

	/* Look for a page exactly following addr. If it is found, return it. */
	while (page && page->tag != tag) {
		prev = page;
		page = page->next;
	}
	if (page)
		return page;
	
	/* Page following addr is not found, so check all memory pages to find
	 * the one with the lowest tag following addr. */
	mintag = 0xffffffff;
	minpage = NULL;
	for (index = 0; index < MEM_PAGE_COUNT; index++) {
		for (page = swap_mem->pages[index]; page; page = page->next) {
			if (page->tag > tag && page->tag < mintag) {
				mintag = page->tag;
				minpage = page;
			}
		}
	}

	/* Return the found page (or NULL) */
	return minpage;
}


/* Create new mem page */
static struct swap_mem_page_t *swap_mem_page_create(struct swap_mem_t *swap_mem, uint32_t addr, int perm)
{
	uint32_t index, tag;
	struct mem_page_t *page;

	tag = addr & ~(MEM_PAGESIZE - 1);
	index = (addr >> MEM_LOGPAGESIZE) % MEM_PAGE_COUNT;
	
	/* Create new page */
	page = calloc(1, sizeof(struct swap_mem_page_t));
	page->tag = tag;
	page->perm = perm;
	
	/* Insert in pages hash table */
	page->next = swap_mem->pages[index];
	swap_mem->pages[index] = page;
	swap_mem_mapped_space += MEM_PAGESIZE;
	swap_mem_max_mapped_space = MAX(swap_mem_max_mapped_space, swap_mem_mapped_space);
	return page;
}

/* Free mem pages */
static void swap_mem_page_free(struct swap_mem_t *mem, uint32_t addr)
{
	uint32_t index, tag;
	struct swap_mem_page_t *prev, *page;
	struct mem_host_mapping_t *hm;
	
	tag = addr & ~(MEM_PAGESIZE - 1);
	index = (addr >> MEM_LOGPAGESIZE) % MEM_PAGE_COUNT;
	prev = NULL;

	/* Find page */
	page = swap_mem->pages[index];
	while (page && page->tag != tag) {
		prev = page;
		page = page->next;
	}
	if (!page)
		return;
	
	/* If page belongs to a host mapping, release it if
	 * this is the last page allocated for it. */
	/*hm = page->host_mapping;
	if (hm) {
		assert(hm->pages > 0);
		assert(tag >= hm->addr && tag + MEM_PAGESIZE <= hm->addr + hm->size);
		hm->pages--;
		page->data = NULL;
		page->host_mapping = NULL;
		if (!hm->pages)
			mem_unmap_host(mem, hm->addr);
	}
	*/
	/* Free page */
	if (prev)
		prev->next = page->next;
	else
		mem->pages[index] = page->next;
		
	swap_mem_mapped_space -= MEM_PAGESIZE;
	if (page->bytes_in_use==0)
		swap_free(page->fpos);
	free(page);
}


/* Return the buffer corresponding to address 'addr' in the simulated
 * mem. The returned buffer is null if addr+size exceeds the page
 * boundaries. */
void *swap_mem_get_buffer(struct swap_mem_t *mem, uint32_t addr, int size,
	enum mem_access_enum access)
{
	struct swap_mem_page_t *page;
	uint32_t offset;

	/* Get page offset and check page bounds */
	offset = addr & (MEM_PAGESIZE - 1);
	if (offset + size > MEM_PAGESIZE)
		return NULL;
	
	/* Look for page */
	page = swap_mem_page_get(mem, addr);
	if (!page)
		return NULL;
	
	/* Check page permissions */
	if ((page->perm & access) != access && mem->safe)
		fatal("mem_get_buffer: permission denied at 0x%x", addr);
	
	void * buf = calloc(1,size);
	/* Allocate and initialize page data if it does not exist yet. */
	if (page->bytes_in_use ==0){
		swap_fd = open_swap_disk();
		fpos_t new_page_start_address  = swap_mem->next_free_page_start_address;
		fseek (swap_fd , new_page_start_address, SEEK_SET);
		//fwrite (buf,size,1,swap_fd);
		swap_mem->next_free_page_start_address = new_page_start_address + MEM_PAGESIZE;
		page->bytes_in_use = MEM_PAGESIZE;
		page->fpos = new_page_start_address;
		memset(buf,0,size);
	}
	else{
		
		swap_fd = open_swap_disk();
		fpos_t page_start_address  = page->fpos;
		fseek (swap_fd , page_start_address, SEEK_SET);
		fread (buf,size,1,swap_fd);
		/* Return pointer to page data */
		return buf;
	}
}


/* Access memory without exceeding page boundaries. */
static void swap_mem_access_page_boundary(struct swap_mem_t *swap_mem, uint32_t addr,
	int size, void *buf, enum mem_access_enum access)
{
	struct swap_mem_page_t *page;
	uint32_t offset;

	/* Find memory page and compute offset. */
	page = swap_mem_page_get(swap_mem, addr);
	offset = addr & (MEM_PAGESIZE - 1);
	assert(offset + size <= MEM_PAGESIZE);

	/* On nonexistent page, raise segmentation fault in safe mode,
	 * or create page with full privileges for writes in unsafe mode. */
	if (!page) {
		if (swap_mem->safe)
			fatal("illegal access at 0x%x: page not allocated", addr);
		if (access == mem_access_read || access == mem_access_exec) {
			memset(buf, 0, size);
			return;
		}
		if (access == mem_access_write || access == mem_access_init)
			page = swap_mem_page_create(swap_mem, addr, mem_access_read |
				mem_access_write | mem_access_exec |
				mem_access_init);
	}
	assert(page);

	/* If it is a write access, set the 'modified' flag in the page
	 * attributes (perm). This is not done for 'initialize' access. */
	if (access == mem_access_write)
		page->perm |= mem_access_modif;

	/* Check permissions in safe mode */
	if (mem->safe && (page->perm & access) != access){
		//fatal("mem_access: permission denied at 0x%x", addr);
            raise(SIGSEGV);
        }

	/* Read/execute access */
	if (access == mem_access_read || access == mem_access_exec) {
		if (page->bytes_in_use !=0){
			memcpy(buf, page->data + offset, size);
			//read from file 
			swap_fd = open_swap_disk();
			fpos_t read_start_offset = swap_mem->fpos + offset 
			fseek (swap_fd , read_start_offset, SEEK_SET);
			fread (buf,size,1,swap_fd);
			fclose(swap_fd);
		}
		else
			memset(buf, 0, size);
		return;
	}

	/* Write/initialize access */
	if (access == mem_access_write || access == mem_access_init) {
		swap_fd = open_swap_disk();
		if (page->bytes_in_use ==0){
			//page-> = calloc(1, MEM_PAGESIZE);
			fpos_t new_page_start_address  = swap_mem->next_free_page_start_address;
			fseek (swap_fd , new_page_start_address, SEEK_SET);
			fwrite (buf,size,1,swap_fd);
			swap_mem->next_free_page_start_address = new_page_start_address+MEM_PAGESIZE;
			page->bytes_in_use = size;
			page->fpos = new_page_start_address;
		}
		else{
			fpos_t page_start_address  = page->fpos + offset;
			fseek (swap_fd , page_start_address, SEEK_SET);
			fwrite (buf,size,1,swap_fd);
		}
		fclose(swap_fd);
		return;
	}

	/* Shouldn't get here. */
	abort();
}

/* Access mem at address 'addr'.
 * This access can cross page boundaries. */
void swap_mem_access(struct swap_mem_t *swap_mem, uint32_t addr, int size, void *buf,
	enum mem_access_enum access)
{
	uint32_t offset;
	int chunksize;

	mem->last_address = addr;
	while (size) {
		offset = addr & (MEM_PAGESIZE - 1);
		chunksize = MIN(size, MEM_PAGESIZE - offset);
		swap_mem_access_page_boundary(swap_mem, addr, chunksize, buf, access);

		size -= chunksize;
		buf += chunksize;
		addr += chunksize;
	}
}


/* Creation and destruction of swap space*/
struct swap_mem_t *swap_mem_create()
{
	struct swap_mem_t *swap_mem;
	swap_mem = calloc(1, sizeof(struct swap_mem_t));
	swap_mem->sharing = 1;
	swap_mem->safe = mem_safe_mode;
	return swap_mem;
	
}



void swap_mem_free(struct swap_mem_t *swap_mem)
{
	int i;
	
	/* Free pages */
	for (i = 0; i < MEM_PAGE_COUNT; i++)
		while (swap_mem->pages[i])
			swap_mem_page_free(swap_mem, swap_mem->pages[i]->tag);

	/* This must have released all host mappings.
	 * Now, free memory structure. */
	assert(!swap_mem->host_mapping_list);
	free(swap_mem);
}



/* Allocate (if not already allocated) all necessary memory pages to
 * access 'size' bytes at 'addr'. These two fields do not need to be
 * aligned to page boundaries.
 * If some page already exists, add permissions. */
void swap_mem_map(struct swap_mem_t *swap_mem, uint32_t addr, int size,
	enum mem_access_enum perm)
{
	uint32_t tag1, tag2, tag;
	struct mem_page_t *page;

	/* Calculate page boundaries */
	tag1 = addr & ~(MEM_PAGESIZE-1);
	tag2 = (addr + size - 1) & ~(MEM_PAGESIZE-1);

	/* Allocate pages */
	for (tag = tag1; tag <= tag2; tag += MEM_PAGESIZE) {
		page = swap_mem_page_get(swap_mem, tag);
		if (!page)
			page = swap_mem_page_create(swap_mem, tag, perm);
		page->perm |= perm;
	}
}



/* Deallocate memory pages. The addr and size parameters must be both
 * multiple of the page size.
 * If some page was not allocated, the corresponding address range is skipped.
 * If a host mapping is caught in the range, it is deallocated with a call
 * to 'mem_unmap_host'. */
void swap_mem_unmap(struct swap_mem_t *swap_mem, uint32_t addr, int size)
{
	uint32_t tag1, tag2, tag;

	/* Calculate page boundaries */
	assert(!(addr & (MEM_PAGESIZE - 1)));
	assert(!(size & (MEM_PAGESIZE - 1)));
	tag1 = addr & ~(MEM_PAGESIZE-1);
	tag2 = (addr + size - 1) & ~(MEM_PAGESIZE-1);

	/* Deallocate pages */
	for (tag = tag1; tag <= tag2; tag += MEM_PAGESIZE)
		swap_mem_page_free(swap_mem, tag);
}


void swap_mem_write_string(struct swap_mem_t *swap_mem, uint32_t addr, char *str)
{
	swap_mem_access(swap_mem, addr, strlen(str) + 1, str, mem_access_write);
}


/* Read a string from memory and return the length of the read string.
 * If the return length is equal to max_size, it means that the string did not
 * fit in the destination buffer. */
int swap_mem_read_string(struct mem_t *swap_mem, uint32_t addr, int size, char *str)
{
	int i;
	for (i = 0; i < size; i++) {
		swap_mem_access(swap_mem, addr + i, 1, str + i, mem_access_read);
		if (!str[i])
			break;
	}
	return i;
}
