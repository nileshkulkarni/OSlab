/*
 *  Multi2Sim
 *  Copyright (C) 2007  Rafael Ubal Tena (raurte@gap.upv.es)
 *
 *  This program is free software; you can redistribute it and/or modify
swap_ *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include <m2skernel.h>
#include <sys/mman.h>





mem_check_table* mct_e;


int NUM_HEADS;
int NUM_TRACKS;
int NUM_SECTORS;




/* Total space allocated for memory pages */
unsigned long mem_mapped_space = 0;
unsigned long mem_max_mapped_space = 0;


/* Safe mode */
int mem_safe_mode = 1;



struct mem_page_t* get_free_ram_page(){
	
	int i;
	for(i=0;i<RAM_MEM_PAGE_COUNT;i++){
		if(ke->ram->pages[i].freeFlag != 0)
			return &(ke->ram->pages[i]);
	}
}





/* Return the memory page following addr in the current memory map. This function
 * is useful to reconstruct consecutive ranges of mapped pages. */




/* Create new mem page */


/* Free mem pages */

/* Copy memory pages. All parameters must be multiple of the page size.
 * The pages in the source and destination interval must exist. */
void mem_copy(struct mem_t *mem, uint32_t dest, uint32_t src, int size)
{
	struct mem_page_t *page_dest, *page_src;

	/* Restrictions. No overlapping allowed. */
	assert(!(dest & (MEM_PAGESIZE-1)));
	assert(!(src & (MEM_PAGESIZE-1)));
	assert(!(size & (MEM_PAGESIZE-1)));
	if ((src < dest && src + size > dest) ||
		(dest < src && dest + size > src))
		fatal("mem_copy: cannot copy overlapping regions");
	
	/* Copy */
	while (size > 0) {
		
		/* Get source and destination pages */
		page_dest = mem_page_get(mem, dest);
		page_src = mem_page_get(mem, src);
		assert(page_src && page_dest);
		
		/* Different actions depending on whether source and
		 * destination page data are allocated. */
		if (page_src->data) {
			if (!page_dest->data)
				page_dest->data = malloc(MEM_PAGESIZE);
			memcpy(page_dest->data, page_src->data, MEM_PAGESIZE);
		} else {
			if (page_dest->data)
				memset(page_dest->data, 0, MEM_PAGESIZE);
		}

		/* Advance pointers */
		src += MEM_PAGESIZE;
		dest += MEM_PAGESIZE;
		size -= MEM_PAGESIZE;
	}
}

uint32_t mem_map_space(struct mem_t *mem, uint32_t addr, int size)
{
    uint32_t tag_start, tag_end;

    assert(!(addr & (MEM_PAGESIZE - 1)));
    assert(!(size & (MEM_PAGESIZE - 1)));
    tag_start = addr;
    tag_end = addr;
    for (;;) {

        /* Address space overflow */
        if (!tag_end)
            return (uint32_t) -1;

        /* Not enough free pages in current region */
        if (mem_page_get(mem, tag_end)) {
            tag_end += MEM_PAGESIZE;
            tag_start = tag_end;
            continue;
        }

        /* Enough free pages */
        if (tag_end - tag_start + MEM_PAGESIZE == size)
            break;
        assert(tag_end - tag_start + MEM_PAGESIZE < size);

        /* we have a new free page */
        tag_end += MEM_PAGESIZE;
    }


    /* Return the start of the free space */
    return tag_start;
}
uint32_t mem_map_space_down(struct mem_t *mem, uint32_t addr, int size)
{
    uint32_t tag_start, tag_end;

    assert(!(addr & (MEM_PAGESIZE - 1)));
    assert(!(size & (MEM_PAGESIZE - 1)));
    tag_start = addr;
    tag_end = addr;
    for (;;) {

        /* Address space overflow */
        if (!tag_start)
            return (uint32_t) -1; 
    
        /* Not enough free pages in current region */
        if (mem_page_get(mem, tag_start)) {
            tag_start -= MEM_PAGESIZE;
            tag_end = tag_start;
            continue;
        }   
    
        /* Enough free pages */
        if (tag_end - tag_start + MEM_PAGESIZE == size)
            break;
        assert(tag_end - tag_start + MEM_PAGESIZE < size);
    
        /* we have a new free page */
        tag_start -= MEM_PAGESIZE;
    }   

    /* Return the start of the free space */
    return tag_start;
}




/* Return the buffer corresponding to address 'addr' in the simulated
 * mem. The returned buffer is null if addr+size exceeds the page
 * boundaries. */

/* Access memory without exceeding page boundaries. */

/* Access mem at address 'addr'.
 * This access can cross page boundaries. */


/* Creation and destruction */




/* This function finds a free memory region to allocate 'size' bytes
 * starting at address 'addr'. */


/* Allocate (if not already allocated) all necessary memory pages to
 * access 'size' bytes at 'addr'. These two fields do not need to be
 * aligned to page boundaries.
 * If some page already exists, add permissions. */


/* Deallocate memory pages. The addr and size parameters must be both
 * multiple of the page size.
 * If some page was not allocated, the corresponding address range is skipped.
 * If a host mapping is caught in the range, it is deallocated with a call
 * to 'mem_unmap_host'. */

/* Map guest pages with the data allocated by a host 'mmap' call.
 * When this space is allocated with 'mem_unmap', the host memory
 * will be freed with a host call to 'munmap'.
 * Guest pages must already exist.
 * Both 'addr' and 'size' must be a multiple of the page size. */

void mem_map_host(struct mem_t *mem, struct fd_t *fd, uint32_t addr, int size,
	enum mem_access_enum perm, void *host_ptr)
{
	uint32_t ptr;
	struct mem_page_t *page;
	struct mem_host_mapping_t *hm;

	/* Check restrictions */
	if (addr & ~MEM_PAGEMASK)
		fatal("mem_map_host: 'addr' not a multiple of page size");
	if (size & ~MEM_PAGEMASK)
		fatal("mem_map_host: 'size' not a multiple of page size");
	
	/* Create host mapping, and insert it into the list head. */
	hm = calloc(1, sizeof(struct mem_host_mapping_t));
	hm->host_ptr = host_ptr;
	hm->addr = addr;
	hm->size = size;
	hm->next = mem->host_mapping_list;
	strncpy(hm->path, fd->path, MAX_PATH_SIZE);
	mem->host_mapping_list = hm;
	syscall_debug("  host mapping created for '%s'\n", hm->path);
	
	/* Make page data point to new data */
	for (ptr = addr; ptr < addr + size; ptr += MEM_PAGESIZE) {
		page = mem_page_get(mem, ptr);
		if (!page)
			fatal("mem_map_host: requested range not allocated");

		/* It is not allowed that the page belong to a previous host
		 * mapping. If so, it should have been unmapped before. */
		if (page->host_mapping)
			fatal("mem_map_host: cannot overwrite a previous host mapping");

		/* If page is pointing to some data, overwrite it */
		if (page->data)
			free(page->data);

		/* Create host mapping */
		page->host_mapping = hm;
		page->data = ptr - addr + host_ptr;
		hm->pages++;
	}
}


/* Deallocate host mapping starting at address 'addr'.
 * A host call to 'munmap' is performed to unmap host space. */
void mem_unmap_host(struct mem_t *mem, uint32_t addr)
{
	int ret;
	struct mem_host_mapping_t *hm, *hmprev;

	/* Locate host mapping in the list */
	hmprev = NULL;
	hm = mem->host_mapping_list;
	while (hm && hm->addr != addr) {
		hmprev = hm;
		hm = hm->next;
	}

	/* Remove it from the list */
	assert(hm);
	if (hmprev)
		hmprev->next = hm->next;
	else
		mem->host_mapping_list = hm->next;
	
	/* Perform host call to 'munmap' */
	ret = munmap(hm->host_ptr, hm->size);
	if (ret < 0)
		fatal("mem_unmap_host: host call 'munmap' failed");
	
	/* Free host mapping */
	syscall_debug("  host mapping removed for '%s'\n", hm->path);
	free(hm);
}


/* Assign protection attributes to pages */
void mem_protect(struct mem_t *mem, uint32_t addr, int size, enum mem_access_enum perm)
{
	uint32_t tag1, tag2, tag;
	struct mem_page_t *page;
	int prot, err;

	/* Calculate page boundaries */
	assert(!(addr & (MEM_PAGESIZE - 1)));
	assert(!(size & (MEM_PAGESIZE - 1)));
	tag1 = addr & ~(MEM_PAGESIZE-1);
	tag2 = (addr + size - 1) & ~(MEM_PAGESIZE-1);

	/* Allocate pages */
	for (tag = tag1; tag <= tag2; tag += MEM_PAGESIZE) {
		page = mem_page_get(mem, tag);
		if (!page)
			continue;

		/* Set page new protection flags */
		page->perm = perm;

		/* If the page corresponds to a host mapping, host page must
		 * update its permissions, too */
		if (page->host_mapping) {
			prot = (perm & mem_access_read ? PROT_READ : 0) |
				(perm & mem_access_write ? PROT_WRITE : 0) |
				(perm & mem_access_exec ? PROT_EXEC : 0);
			err = mprotect(page->data, MEM_PAGESIZE, prot);
			if (err < 0)
				fatal("mem_protect: host call to 'mprotect' failed");
		}
	}
}





/* Read a string from memory and return the length of the read string.
 * If the return length is equal to max_size, it means that the string did not
 * fit in the destination buffer. */


void mem_zero(struct mem_t *mem, uint32_t addr, int size)
{
	unsigned char zero = 0;
	while (size--)
		mem_access(mem, addr++,0, &zero, mem_access_write);
}


void mem_dump(struct mem_t *mem, char *filename, uint32_t start, uint32_t end)
{
	FILE *f;
	uint32_t size;
	uint8_t buf[MEM_PAGESIZE];

	f = fopen(filename, "wb");
	if (!f)
		fatal("mem_dump: cannot open file '%s'", filename);
	
	/* Set unsafe mode and dump */
	mem->safe = 0;
	while (start < end) {
		size = MIN(MEM_PAGESIZE, end - start);
		mem_access(mem, start, size, buf, mem_access_read);
		fwrite(buf, size, 1, f);
		start += size;
	}

	/* Restore safe mode */
	mem->safe = mem_safe_mode;
	fclose(f);
}


void mem_load(struct mem_t *mem, char *filename, uint32_t start)
{
	FILE *f;
	uint32_t size;
	uint8_t buf[MEM_PAGESIZE];
	
	f = fopen(filename, "rb");
	if (!f)
		fatal("mem_load: cannot open file '%s'", filename);
	
	/* Set unsafe mode and load */
	mem->safe = 0;
	for (;;) {
		size = fread(buf, 1, MEM_PAGESIZE, f);
		if (!size)
			break;
		mem_access(mem, start, size, buf, mem_access_write);
		start += size;
	}

	/* Restore safe mode */
	mem->safe = mem_safe_mode;
	fclose(f);
}

/* swap starts here  */

////////////////////////////////////////////////////////////////////// ////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////// ///////////////////////////////////////////////////////////////////////

FILE* swap_fd; 

FILE* open_swap_disk(){
	
	FILE* fp = fopen("Sim_disk","rb+");
	return fp;
}

void swap_free(fpos_t fpos){
	
	
}

/* Return mem page corresponding to an address. */
struct mem_page_t *mem_page_get(struct mem_t *mem, uint32_t addr)
{
	uint32_t index, tag;
	struct mem_page_t *prev, *page;
	
	tag = addr & ~(MEM_PAGESIZE - 1);
	index = (addr >> MEM_LOGPAGESIZE) % MEM_PAGE_COUNT;
	page = mem->pages[index];
    if(page == NULL){
       //printf("SWAP page tag for the null page is %u \n", tag);
    }
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
struct mem_page_t *mem_page_get_next(struct mem_t *mem, uint32_t addr)
{
	uint32_t tag, index, mintag;
	struct mem_page_t *prev, *page, *minpage;

	/* Get tag of the page just following addr */
	tag = (addr + MEM_PAGESIZE) & ~(MEM_PAGESIZE - 1);
	if (!tag)
		return NULL;
	index = (tag >> MEM_LOGPAGESIZE) % MEM_PAGE_COUNT;
	page = mem->pages[index];
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
		for (page = mem->pages[index]; page; page = page->next) {
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
struct mem_page_t *mem_page_create(struct mem_t *mem, uint32_t addr, int perm)
{
	uint32_t index, tag;
	struct mem_page_t *page;

	tag = addr & ~(MEM_PAGESIZE - 1);
	index = (addr >> MEM_LOGPAGESIZE) % MEM_PAGE_COUNT;
	
	/* Create new page */
	page = calloc(1, sizeof(struct mem_page_t));
    page->fpos = mem->next_free_page_start_address; 
    mem->next_free_page_start_address.__pos = mem->next_free_page_start_address.__pos + MEM_PAGESIZE; 
    page->bytes_in_use = MEM_PAGESIZE;
	page->tag = tag;
	page->perm = perm;
	
	/* Insert in pages hash table */
	page->next = mem->pages[index];
	mem->pages[index] = page;
	mem_mapped_space += MEM_PAGESIZE;
	mem_max_mapped_space = MAX(mem_max_mapped_space, mem_mapped_space);
	return page;
}

/* Free mem pages */
void mem_page_free(struct mem_t *mem, uint32_t addr)
{
	uint32_t index, tag;
	struct mem_page_t *prev, *page;
	struct mem_host_mapping_t *hm;
	
	tag = addr & ~(MEM_PAGESIZE - 1);
	index = (addr >> MEM_LOGPAGESIZE) % MEM_PAGE_COUNT;
	prev = NULL;

	/* Find page */
	page = mem->pages[index];
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
		
	mem_mapped_space -= MEM_PAGESIZE;
	if (page->bytes_in_use==0)
		swap_free(page->fpos);
	free(page);
}


/* Return the buffer corresponding to address 'addr' in the simulated
 * mem. The returned buffer is null if addr+size exceeds the page
 * boundaries. */
void *mem_get_buffer(struct mem_t *mem, uint32_t addr, int size, enum mem_access_enum access)
{
	struct mem_page_t *page;
	uint32_t offset;

	/* Get page offset and check page bounds */
	offset = addr & (MEM_PAGESIZE - 1);
	if (offset + size > MEM_PAGESIZE)
		return NULL;
	
	/* Look for page */
	page = mem_page_get(mem, addr);
	if (!page){
        printf("page in null\n");
		return NULL;
    }
    //printf("page in not  null\n");
	
	/* Check page permissions */
	if ((page->perm & access) != access && mem->safe)
		fatal("mem_get_buffer: permission denied at 0x%x", addr);
	
	void * buf = calloc(1,size);
	/* Allocate and initialize page data if it does not exist yet. */
	if (page->bytes_in_use ==0){
       // printf("page tag %u has bytes used 0  \n" ,page->tag); 
        //printf("page to be created bytes in use =0\n");
		swap_fd = open_swap_disk();
		fpos_t new_page_start_address  = mem->next_free_page_start_address;
		//!TODO: swap manager se free address lo
		
		
		fseek(swap_fd , new_page_start_address.__pos, SEEK_SET);
		
		//!TODO this should be removed and updated to swap manager
		page->fpos = new_page_start_address;
		new_page_start_address.__pos = new_page_start_address.__pos + + MEM_PAGESIZE;
		mem->next_free_page_start_address = new_page_start_address ;
		page->bytes_in_use = MEM_PAGESIZE;
        printf("Buffer isn in swap_mem_get_buffer %s\n",buf);
        fclose(swap_fd);
        return buf;
	}
	else{
        		
		swap_fd = open_swap_disk();
		fpos_t page_start_address  = page->fpos;
		fseek (swap_fd , page_start_address.__pos + offset, SEEK_SET);
		fread (buf,size,1,swap_fd);
        fclose(swap_fd);
		/* Return pointer to page data */
		return buf;
	}
}


/* Access memory without exceeding page boundaries. */
void mem_access_page_boundary(struct mem_t *mem, uint32_t addr,int size, void *buf, enum mem_access_enum access)
{
    int static first_access =0;
	struct mem_page_t *page;
	uint32_t offset;

	/* Find memory page and compute offset. */
	page = mem_page_get(mem, addr);
	offset = addr & (MEM_PAGESIZE - 1);
	assert(offset + size <= MEM_PAGESIZE);

	/* On nonexistent page, raise segmentation fault in safe mode,
	 * or create page with full privileges for writes in unsafe mode. */
	if (!page) {
		if (mem->safe){
		    int * x;
            *x =2;
            fatal("Swap::illegal access at 0x%x: page not allocated",addr);
            
        }
		if (access == mem_access_read || access == mem_access_exec) {
			memset(buf, 0, size);
			return;
		}
		if (access == mem_access_write || access == mem_access_init)
			page = mem_page_create(mem, addr, mem_access_read |
				mem_access_write | mem_access_exec |
				mem_access_init);
	}
	assert(page);

	/* If it is a write access, set the 'modified' flag in the page
	 * attributes (perm). This is not done for 'initialize' access. */
    //printf("Swap mem page access %u \n", page->fpos);
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
			
			
			//read from file 
			swap_fd = open_swap_disk();
			
			//!CHECK
			fpos_t read_start_offset;
			read_start_offset.__pos = page->fpos.__pos + offset;
			 
			fseek (swap_fd , read_start_offset.__pos, SEEK_SET);
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
			
			//!TODO use free page manager here
			fpos_t new_page_start_address  = mem->next_free_page_start_address;
			//fseek (swap_fd , new_page_start_address.__pos, SEEK_SET);           
               // printf("*********************************** I am abouto write to disk \n");
                //fseek (swap_fd , 0, SEEK_SET);           
                fseek (swap_fd , new_page_start_address.__pos, SEEK_SET);           
                printf("page with tag %u, created \n",page->tag); 
                fwrite (buf,size,1,swap_fd);
                mem->next_free_page_start_address.__pos = new_page_start_address.__pos +MEM_PAGESIZE;
                page->bytes_in_use = size;
                page->fpos = new_page_start_address;
		}
		else{
			fpos_t page_start_address;
			page_start_address.__pos = page->fpos.__pos + offset;
			fseek (swap_fd , page_start_address.__pos, SEEK_SET);
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
void mem_access(struct mem_t *mem, uint32_t addr, int size, void *buf,
	enum mem_access_enum access)
{
	uint32_t offset;
	int chunksize;
    
	mem->last_address = addr;

	while (size) {
		offset = addr & (MEM_PAGESIZE - 1);
		chunksize = MIN(size, MEM_PAGESIZE - offset);
		mem_access_page_boundary(mem, addr, chunksize, buf, access);

		size -= chunksize;
		buf += chunksize;
		addr += chunksize;
	}
}


/* Creation and destruction of swap space*/
struct mem_t *mem_create()
{
	struct mem_t *mem;
	mem = calloc(1, sizeof(struct mem_t));
	mem->sharing = 1;
	mem->safe = mem_safe_mode;
    //TODO edit here
    // mem->next_free_page_start_address =(fpos_t)0;
	return mem;
	
}



void mem_free(struct mem_t *mem)
{
	int i;
	
	/* Free pages */
	for (i = 0; i < MEM_PAGE_COUNT; i++)
		while (mem->pages[i])
			mem_page_free(mem, mem->pages[i]->tag);

	/* This must have released all host mappings.
	 * Now, free memory structure. */
	assert(!mem->host_mapping_list);
	free(mem);
}



/* Allocate (if not already allocated) all necessary memory pages to
 * access 'size' bytes at 'addr'. These two fields do not need to be
 * aligned to page boundaries.
 * If some page already exists, add permissions. */
void mem_map(struct mem_t *mem, uint32_t addr, int size,
	enum mem_access_enum perm)
{
	uint32_t tag1, tag2, tag;
	struct mem_page_t *page;

	/* Calculate page boundaries */
	tag1 = addr & ~(MEM_PAGESIZE-1);
	tag2 = (addr + size - 1) & ~(MEM_PAGESIZE-1);

	/* Allocate pages */
    int numberPages =0;
	for (tag = tag1; tag <= tag2; tag += MEM_PAGESIZE) {
		page = mem_page_get(mem, tag);
		if (!page){
            numberPages++;
			page = mem_page_create(mem, tag, perm);
            //printf("Creating page for addr %u , on swap space, page number is %u\n " ,tag,numberPages);
        }
		page->perm |= perm;
    
	}
}



/* Deallocate memory pages. The addr and size parameters must be both
 * multiple of the page size.
 * If some page was not allocated, the corresponding address range is skipped.
 * If a host mapping is caught in the range, it is deallocated with a call
 * to 'mem_unmap_host'. */
void mem_unmap(struct mem_t *mem, uint32_t addr, int size)
{
	uint32_t tag1, tag2, tag;

	/* Calculate page boundaries */
	assert(!(addr & (MEM_PAGESIZE - 1)));
	assert(!(size & (MEM_PAGESIZE - 1)));
	tag1 = addr & ~(MEM_PAGESIZE-1);
	tag2 = (addr + size - 1) & ~(MEM_PAGESIZE-1);

	/* Deallocate pages */
	for (tag = tag1; tag <= tag2; tag += MEM_PAGESIZE)
		mem_page_free(mem, tag);
}


void mem_write_string(struct mem_t *mem, uint32_t addr, char *str)
{
	mem_access(mem, addr, strlen(str) + 1, str, mem_access_write);
}


/* Read a string from memory and return the length of the read string.
 * If the return length is equal to max_size, it means that the string did not
 * fit in the destination buffer. */
int mem_read_string(struct mem_t *mem, uint32_t addr, int size, char *str)
{
	int i;
	for (i = 0; i < size; i++) {
		mem_access(mem, addr + i, 1, str + i, mem_access_read);
		if (!str[i])
			break;
	}
	return i;
}



/* Swap Space Manager */
struct mem_t* free_swap_page(struct mem_t * page){
   struct mem_t* iter;
   struct mem_t* prev;
   iter = swap_mem->occupied_list;
   int flag_found =0;
   while(iter){
       if(iter->offset.__pos ==page->offset.__pos){
           flag_found = 1;     
           break;
       }
       prev = iter;
       iter = iter->next;
   }
   if(flag_found){
       if(iter){
            prev->next = iter->next;
            iter->next = swap_mem->free_list;
            swap_mem->free_list = iter;
            return iter;
       }
   }
   else{
		fatal("[SWAP] :Trying to free a free page\n");
        return NULL;
   }
}


struct mem_t* get_new_swap_page(){
    if(!swap_mem->free_list){
        printf("No space on swap available\n");
    }
    else{
        /* getting the page from the front of the list */
        struct mem_t* new_page = swap_mem->free_list ;
        /* updating free list to next page */
        swap_mem->free_list = new_page->next;
        return new_page;
    }
}

void swap_initialise(){
    swap_mem = malloc(sizeof(struct swap_mem_t));
    swap_mem->free_list =NULL;
    swap_mem->occupied_list =NULL;
    swap_fd = swap_open_disk();   
	fseek(swap_fd ,0, SEEK_SET);
    int no_of_pages =0;
    while(no_of_pages < MEM_PAGE_COUNT){
        
        struct mem_t* free_page = malloc(sizeof(struct mem_t)); 
        free_list

    }
}
