///TODO : put function comments in appropriate places



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


int swap_page_count_used=0;
/* Safe mode */
int mem_safe_mode = 1;
struct swap_mem_t * swap_mem;




struct mem_page_t* get_free_ram_page(){
	
	int i;
	for(i=0;i<RAM_MEM_PAGE_COUNT;i++){
		if(ke->ram->pages[i].free_flag != 0)
			return &(ke->ram->pages[i]);
	}
	return NULL;
}









struct mem_page_t *mem_page_get(struct mem_t *mem, uint32_t addr)
{
	uint32_t index, tag;
	struct mem_page_t *prev, *page;

	tag = addr & ~(MEM_PAGESIZE - 1);
	index = (addr >> MEM_LOGPAGESIZE) % MEM_PAGE_COUNT;
	page = mem->ram_pages[index];
	
	prev = NULL;
	
	uint32_t dummy_addr = tag;
	uint32_t index1, tag1;
	tag1 = dummy_addr & ~(MEM_PAGESIZE - 1);
	index1 = (dummy_addr >> MEM_LOGPAGESIZE) % MEM_PAGE_COUNT;
	assert((tag1==tag) && (index1 == index));
	
	/* Look for page */
	while (page && page->tag != tag) {
		prev = page;
		page = page->next;
	}
	
	/* Place page into list head */
	if (prev && page) {
		prev->next = page->next;
		page->next = mem->ram_pages[index];
		mem->ram_pages[index] = page;
	}
	
	
	if(!page){
	    //printf("COULD NOT FIND A PAGE , EXECUTE PAGE FAULT ROUTINE \n");
		page = page_fault_routine(mem, addr);		
	}
 
	/* Return found page */
	return page;
}













/* Return mem page corresponding to an address. */
struct mem_page_t *swap_mem_page_get(struct mem_t *mem, uint32_t addr)
{
	uint32_t index, tag;
	struct mem_page_t *prev, *page;
	
	tag = addr & ~(MEM_PAGESIZE - 1);
	index = (addr >> MEM_LOGPAGESIZE) % MEM_PAGE_COUNT;
	page = mem->pages[index];
    
    if(page == NULL){
     //  printf("SWAP page tag for the null page is %u \n", tag);
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







/* Free mem pages */
///TODO rewrite mem_map_space
uint32_t mem_map_space(struct mem_t *mem, uint32_t addr, int size)
{
    uint32_t tag_start, tag_end;

	if(((addr >> MEM_LOGPAGESIZE) <= 32840) && (((addr+size)>> MEM_LOGPAGESIZE) >= 32840)){
		printf("Page 32840 should  be created here %d , %d \n" , 
			(addr >> MEM_LOGPAGESIZE) ,
			((addr+size)>> MEM_LOGPAGESIZE));
	}
	


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


///TODO Rewrite mem_map_space_down
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
		if (page->data){
			free(page->data);
			page->data = NULL;
		}

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
	struct mem_page_t *page, *swap_page;
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
		swap_page = swap_mem_page_get(mem, tag);
		assert(swap_page);
		swap_page->perm = perm;
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
	if(!fp)
		printf("unable to open file, please check for unclosed version\n");
	return fp;
}

void swap_free(fpos_t fpos){
    printf("Freeing page\n");	
	
}




struct mem_page_t* page_fault_routine(struct mem_t *mem, uint32_t addr){
	if(!ke->loading_in_progress){
		//printf("INSIDE PAGE FAULT ROUTINE %u \n",addr);
	}
	uint32_t index, tag;
	struct mem_page_t *page, *prev;
	
	tag = addr & ~(MEM_PAGESIZE - 1);
	index = (addr >> MEM_LOGPAGESIZE) % MEM_PAGE_COUNT;	
	page = mem->ram_pages[index];
	/* Look for page */
	while (page && page->tag != tag) {
		prev = page;
		page = page->next;
	}

	
	assert(!page); //That's why this routine was called
	
	unsigned char *data;
	/* !TODO FOR SWAP_SPACE JUNTA
	 * - put replaced_page->data in swap_space in appropriate place
	 * - get data of page in char *data
	 * - call swap_out_page(replaced_page);
	 * - struct swap_mem_page_t* swapin_page = swap_in_page()
	 * - set all the fields of the field of page @page_to_be_replaced
	 * - mem->pages[page_to_be_replaced] = swapin_page
	 */
	 
	
	struct mem_page_t* page_from_swap_space = swap_mem_page_get(mem, addr);
	
	//IF page is not found, return NULL
	
	if(!page_from_swap_space)
		return page_from_swap_space;
	
	// creating a new
	if(!ke->loading_in_progress){
		mem->total_faults++;
		mem->current_inst_faults++;
	}	
	
	
	 
	 //data = (unsigned char*)mem_get_buffer(mem , addr , MEM_PAGESIZE, mem_access_read);
     data = read_swap_page(page_from_swap_space); 
	
     
   // need ctx here to find out which process was faulted; 
    struct mem_page_t* new_page = ram_get_new_page(mem);
        
    memcpy(new_page->data,data,MEM_PAGESIZE); 
    free(data);
    new_page->tag = page_from_swap_space->tag;
    new_page->perm = page_from_swap_space->perm;
    new_page->free_flag = 0;
    
   /* Adding new_page to appropriate list_head */ 
    new_page->next = mem->ram_pages[index];
    mem->ram_pages[index] = new_page;
    
   // printf("page fault handled successfully at addr %u \n",addr); 
    return new_page;
}



struct mem_page_t*  ram_get_new_page(struct mem_t * mem){
    
    //check this here
    //update page table entries for the process
    // randomly chosing a page from the allocated page list;
    srand (time(NULL) );
    /* Generate a random number: */
    int i=0;
    int j=0;

    struct mem_page_t* new_free_ram_page; 
    
    
   // printf("%d :::: %d ::::: %d \n", mem->pages_in_ram, mem->max_pages_in_ram, new_free_ram_page);
        
    if(mem->pages_in_ram < mem->max_pages_in_ram){
        // return a free ram page    
        new_free_ram_page  = get_free_ram_page();
        if(new_free_ram_page){
            // page is available on ram , so directly use it.
            mem->pages_in_ram++;
            return new_free_ram_page;
        }
    }
    
    
    assert(mem->pages_in_ram);
    int rand_page = rand() % mem->pages_in_ram;
   // rand_page = 7; 
    //printf("******************************************Swapping out page \n");
                 
    for(j=0;j<MEM_PAGE_COUNT;j++){
        struct mem_page_t* iter = mem->ram_pages[j];  
        struct mem_page_t* prev =NULL;
        while(iter){
           if(i == rand_page){
                if(prev){
                    // normal page to be replaced
                   prev->next = iter->next; 
                }
                else{
                    // condition when head of the list is being replaced is to be replaced
                    mem->ram_pages[j] = iter->next;
                }
                //!TODO update dirty bit of the new page and write-back the old page if(dirty bit)
                //!TODO update the list heads.
                //printf("******************************************Swapping out page \n");
                if(iter->dirty){
                    uint32_t write_back_page_addr = iter->tag;
                    swap_write_back_page(mem,iter, write_back_page_addr); 
                }
                iter->free_flag = 1;
                iter->dirty = 0;
                return iter;
           }
            prev  = iter;
			iter= iter->next;
           //printf("Incrementing i\n");
           i++; 
        }
   }  
    fatal("ram_get_new_page :: Should never come here %d %d \n ", mem->pages_in_ram , rand_page);
}




void swap_write_back_page(struct mem_t *mem, struct mem_page_t* ram_page,uint32_t addr ){
    
   struct  mem_page_t * swap_page = swap_mem_page_get(mem,addr); 
    // Write to swap disk   
    swap_fd = open_swap_disk();
    fseek(swap_fd, swap_page->fpos.__pos, SEEK_CUR);
    fwrite(ram_page->data,MEM_PAGESIZE,1,swap_fd);
    fclose(swap_fd);
    //printf("page written to swap \n");
}


void swap_write_bytes(struct mem_t *mem, uint32_t addr,uint32_t size,void *buf ){
    
   struct  mem_page_t * swap_page = swap_mem_page_get(mem,addr); 
    // Write to swap disk   
    int offset =  addr & (MEM_PAGESIZE - 1); 
    swap_fd = open_swap_disk();
    fseek(swap_fd, swap_page->fpos.__pos + offset, SEEK_CUR);
    fwrite(buf,size,1,swap_fd);
    fclose(swap_fd);
    //printf("page written to swap \n");
}

void* read_swap_page(struct mem_page_t * page){
    assert(page);
    swap_fd = open_swap_disk();
    void * buf = calloc(1,MEM_PAGESIZE);
    if(!buf)
		fatal("failed to allocated memory \n");
	
    //printf("Page fpos is %u \n", page->fpos.__pos);
   
    fseek(swap_fd,page->fpos.__pos,SEEK_SET);
    //printf("after fseek \n");
	fread (buf,MEM_PAGESIZE,1,swap_fd);
   
    fclose(swap_fd);
    return buf;
}




///TODO: Rewrite mem_page_get_next
/* Return the memory page following addr in the current memory map. This function
 * is useful to reconstruct consecutive ranges of mapped pages. */
struct mem_page_t *mem_page_get_next(struct mem_t *mem, uint32_t addr)
{
	
	printf("comes here::::::::::::::::::::::::::::::::::::::::::::::::\n");
	uint32_t tag, index, mintag;
	struct mem_page_t *prev, *page, *minpage;

	/* Get tag of the page just following addr */
	tag = (addr + MEM_PAGESIZE) & ~(MEM_PAGESIZE - 1);
	if (!tag)
		return NULL;
	index = (tag >> MEM_LOGPAGESIZE) % MEM_PAGE_COUNT;
	page = mem->ram_pages[index];
	prev = NULL;

	/* Look for a page exactly following addr. If it is found, return it. */
	while (page && page->tag != tag) {
		prev = page;
		page = page->next;
	}
	if (page)
		return page;
    	


    /* get the page from the swap space */
    printf("Handling page fault here");
	struct mem_page_t* page_from_swap_space = swap_mem_page_get(mem, addr);
	assert(page_from_swap_space);	
	 //data = (unsigned char*)mem_get_buffer(mem , addr , MEM_PAGESIZE, mem_access_read);
    void* data = read_swap_page(page_from_swap_space); 
	
   // need ctx here to find out which process was faulted; 
    struct mem_page_t* new_page = ram_get_new_page(mem);
        
    memcpy(new_page->data,data,MEM_PAGESIZE); 
    free(data);
    new_page->tag = page_from_swap_space->tag;
    new_page->perm = page_from_swap_space->perm;
    new_page->free_flag = 0;
    
   /* Adding new_page to appropriate list_head */ 
    new_page->next = mem->ram_pages[index];
    mem->ram_pages[index] = new_page;
   // printf("page fault handled successfully at addr %u \n",addr); 
    if(new_page)
        return new_page; 

     

	/* Page following addr is not found, so check all memory pages to find
	 * the one with the lowest tag following addr. */
    
	mintag = 0xffffffff;
	minpage = NULL;
    //checking the page that exist on swap space which is very near to the demanded page
	for (index = 0; index < MEM_PAGE_COUNT; index++) {
		for (page = mem->pages[index]; page; page = page->next) {
			if (page->tag > tag && page->tag < mintag) {
				mintag = page->tag;
				minpage = page;
			}
		}
	}

	/* Return the found page (or NULL) */
	/*!!!!!  */
	//return minpage;
	
    if(minpage){
        // now check if the minpage exist on the ram itself
        index = (tag >> MEM_LOGPAGESIZE) % MEM_PAGE_COUNT;
        for (page = mem->pages[index]; page; page = page->next) {
            if (page->tag == minpage->tag) {
                break;
            }
        }
        if(page){
            return page;
        }
	}

    printf("Page fault: getting a page nearest to the required to page\n");

    page_from_swap_space = swap_mem_page_get(mem, addr);
	assert(page_from_swap_space);	
	 //data = (unsigned char*)mem_get_buffer(mem , addr , MEM_PAGESIZE, mem_access_read);
    data = read_swap_page(page_from_swap_space); 
	
   // need ctx here to find out which process was faulted; 
    new_page = ram_get_new_page(mem);
        
    memcpy(new_page->data,data,MEM_PAGESIZE); 
    free(data);
    new_page->tag = page_from_swap_space->tag;
    new_page->perm = page_from_swap_space->perm;
    new_page->free_flag = 0;
    
   /* Adding new_page to appropriate list_head */ 
    new_page->next = mem->ram_pages[index];
    mem->ram_pages[index] = new_page;
    
    printf("page fault handled successfully at addr %u \n",addr); 
    if(new_page)
        return new_page;
    else
        return NULL;
}




/* Create new mem page in swap space*/
struct mem_page_t *mem_page_create(struct mem_t *mem, uint32_t addr, int perm)
{
	uint32_t index, tag;
	struct mem_page_t *page;

	tag = addr & ~(MEM_PAGESIZE - 1);
	index = (addr >> MEM_LOGPAGESIZE) % MEM_PAGE_COUNT;
	
	
	if((tag >> MEM_LOGPAGESIZE) == 32840){
		printf("Page 32840 is being created here \n");
	}
	
	/* Create new page */
	page = calloc(1, sizeof(struct mem_page_t));
   /* page->fpos = mem->next_free_page_start_address; 
    mem->next_free_page_start_address.__pos = mem->next_free_page_start_address.__pos + MEM_PAGESIZE; 
    */
    struct mem_page_t* new_page = get_new_swap_page();
	if(new_page->fpos.__pos == 18223104){
			printf("*****Page allocated 18223104 this to process %d\n",mem->context->uid);
	}
    (page->fpos).__pos = (new_page->fpos).__pos;

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



/* Create new mem page in swap space*/
struct mem_page_t *swap_mem_page_create(struct mem_t *mem, uint32_t addr, int perm)
{
	uint32_t index, tag;
	struct mem_page_t *page;

	tag = addr & ~(MEM_PAGESIZE - 1);
	index = (addr >> MEM_LOGPAGESIZE) % MEM_PAGE_COUNT;
	
	
	if((addr >> MEM_LOGPAGESIZE) == 32840){
		printf("Page 32840 is being created here \n");
	}
	
	/* Create new page */
	page = calloc(1, sizeof(struct mem_page_t));
   /* page->fpos = mem->next_free_page_start_address; 
    mem->next_free_page_start_address.__pos = mem->next_free_page_start_address.__pos + MEM_PAGESIZE; 
    */
    struct mem_page_t* new_page = get_new_swap_page();
    (page->fpos).__pos = (new_page->fpos).__pos;
    
    if(page->fpos.__pos == 18223104){
		printf("Swap page Create for page: %u , pid %d fpos %u \n", addr,
						mem->context->uid,page->fpos.__pos);
   }

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
	
	//printf("Freeing mem_page \n");
	uint32_t index, tag;
	struct mem_page_t *prev, *page;
	struct mem_host_mapping_t *hm;
	
	tag = addr & ~(MEM_PAGESIZE - 1);
	index = (addr >> MEM_LOGPAGESIZE) % MEM_PAGE_COUNT;
	prev = NULL;

	/* Find page */
	page = mem->ram_pages[index];
	while (page && page->tag != tag) {
		prev = page;
		page = page->next;
	}
	
	if (page){
	
		/* If page belongs to a host mapping, release it if
		 * this is the last page allocated for it. */
		hm = page->host_mapping;
		if (hm) {
			assert(hm->pages > 0);
			assert(tag >= hm->addr && tag + MEM_PAGESIZE <= hm->addr + hm->size);
			hm->pages--;
			page->data = NULL;
			page->host_mapping = NULL;
			if (!hm->pages)
				mem_unmap_host(mem, hm->addr);
		}
		/* Free page */
		if (prev)
			prev->next = page->next;
		else
			mem->ram_pages[index] = page->next;
			
		mem_mapped_space -= MEM_PAGESIZE;
		page->free_flag = 1;
		
	}
	
	/* Find page in swap_space */
	page = mem->pages[index];
	while (page && page->tag != tag) {
		prev = page;
		page = page->next;
	}
	
	if(!page) 
		return;
	
	free_a_swap_page(page, mem);	
	/* Free page */
	if (prev)
		prev->next = page->next;
	else
		mem->pages[index] = page->next;
/*	
	if (page->bytes_in_use==0)
		swap_free(page->fpos);
*/	
	//printf("Freeing mem_page Done \n");
	
}







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
        printf("page not found at all \n");
		return NULL;
    }
    
   
	/* Check page permissions */
	/** !TODO : TOCHECK **/
	
	if ((page->perm & access) != access && mem->safe){
		printf("Index is is: %d, perm: %d , access: %d\n", 
			(page->tag >> MEM_LOGPAGESIZE) % MEM_PAGE_COUNT, page->perm, access); 
		assert(0);
		fatal("mem_get_buffer: permission denied at 0x%x", addr);
	}
	
	/* Allocate and initialize page data if it does not exist yet. */
	///TOCHECK : why this check?
	if (page){
	//	assert(page->data);
    //    printf("page tag %u has bytes used %d \n" ,page->tag, page->bytes_in_use); 
        return (page->data + offset);
	}
}












/* Return the buffer corresponding to address 'addr' in the simulated
 * mem. The returned buffer is null if addr+size exceeds the page
 * boundaries. */
void *swap_mem_get_buffer(struct mem_t *mem, uint32_t addr, int size, enum mem_access_enum access)
{
	struct mem_page_t *page;
	uint32_t offset;

	/* Get page offset and check page bounds */
	offset = addr & (MEM_PAGESIZE - 1);
	if (offset + size > MEM_PAGESIZE)
		return NULL;
	
	/* Look for page */
	page = swap_mem_page_get(mem, addr);
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
	if (page->bytes_in_use == 0){
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
        printf("Buffer isn in swap_mem_get_buffer %s\n",(char *)buf);
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



/* Initial Access to swap memory without exceeding page boundaries. This doesnt go through ram 
 * Yet to be completed TODO
 * */
void swap_mem_access_page_boundary(struct mem_t *mem, uint32_t addr,int size, void *buf, enum mem_access_enum access)
{	
    int static first_access =0;
	struct mem_page_t *page;
	uint32_t offset;

	/* Find memory page and compute offset. */
	page = swap_mem_page_get(mem, addr);
	offset = addr & (MEM_PAGESIZE - 1);
	assert(offset + size <= MEM_PAGESIZE);

	/* On nonexistent page, raise segmentation fault in safe mode,
	 * or create page with full privileges for writes in unsafe mode. */
	if (!page) {
		fatal("Page not found on swap space, something wrong with mem_map\n");
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
		void * temp_buf;
		temp_buf = read_swap_page(page);
		memcpy(buf,temp_buf+offset,size);
		free(temp_buf);
	}
	/* Write/initialize access */
	if (access == mem_access_write || access == mem_access_init) {
		/*
		struct mem_page_t* page_ram = mem_page_get(mem,addr);
		memcpy(page_ram->data+offset, buf, size);
		page_ram->dirty =1;
		swap_write_back_page(mem,page_ram, addr);
		page_ram->dirty = 0;
		
		
		struct mem_page_t *temp_page = swap_mem_page_get(mem, addr);
		
		void * temp_buf = read_swap_page(temp_page);
		
		memcpy(temp_buf+offset, buf,size);
		
		struct mem_page_t *temp_ram_page = malloc(sizeof(struct mem_page_t));
		temp_ram_page->data = temp_buf;
		swap_write_back_page(mem,temp_ram_page,addr);
		free(temp_buf);
		free(temp_ram_page);
		*/
		return;
		
	}

	/* Shouldn't get here. */
	abort();
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
		if (page->data){
			memcpy(buf , page->data+offset , size);
		}
		else{
			assert(0);
			memset(buf, 0, size);
		}
		return;
	}

	/* Write/initialize access */
	if (access == mem_access_write || access == mem_access_init) {
		if (!page->data){
			assert(0);
		}
		else{
			memcpy(page->data+offset, buf,  size);
		    page->dirty = 1;
		}
		return;
	}

	/* Shouldn't get here. */
	abort();
}

/* Access mem at address 'addr', this directly accesses pages on the swap space to load the program into
so that it doesnot result in any page fault's */
void swap_mem_access(struct mem_t *mem, uint32_t addr, int size, void *buf,
	enum mem_access_enum access)
{
	uint32_t offset;
	int chunksize;
    
	mem->last_address = addr;

	while (size) {
		offset = addr & (MEM_PAGESIZE - 1);
		chunksize = MIN(size, MEM_PAGESIZE - offset);
		swap_mem_access_page_boundary(mem, addr, chunksize, buf, access);
		size -= chunksize;
		buf += chunksize;
		addr += chunksize;
	}
}


void addInterruptForProcess(struct mem_t* mem,int faults){
	if(mem->current_inst_faults != 0){
		struct interrupt_t *newInterrupt = malloc(sizeof(struct interrupt_t));
		// replace with appropriate function for the proiorty queue 
		newInterrupt->instruction_no = ke->instruction_no + 100*(mem->current_inst_faults); 
		newInterrupt->context = mem->context;
		printf("PID is %d , adding a interrupt for pagefault routine\n" , mem->context->pid);
		newInterrupt->type = PAGE_FAULT;
		// removing process from the running list and putting it to suspended list
		ke_list_remove(ke_list_running,mem->context);
		ke_list_insert_tail(ke_list_suspended,mem->context);
		// insterting an interrupt for the process to recover
		insertInterrupt(newInterrupt);
	}
	return;
}



/* Access mem at address 'addr'.
 * This access can cross page boundaries. */
void mem_access(struct mem_t *mem, uint32_t addr, int size, void *buf,enum mem_access_enum access)
{
	uint32_t offset;
	int chunksize;
    
	mem->last_address = addr;
	uint32_t temp_size = size;
	
	while (size) {
		offset = addr & (MEM_PAGESIZE - 1);
		chunksize = MIN(size, MEM_PAGESIZE - offset);
		mem_access_page_boundary(mem, addr, chunksize, buf, access);
		size -= chunksize;
		buf += chunksize;
		addr += chunksize;
	}
	
	if(!temp_size){
		printf("mem current inst faults %d and size %u\n", mem->current_inst_faults,temp_size);
	}
	
	//do page fault handling here
	//addInterruptForProcess(mem,mem->current_inst_faults);
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


//Bug is Here at page 72
void mem_free(struct mem_t *mem)
{
	int i;
	//printf("Freeing Memory  of %d\n", mem->context->uid);
	/* Free pages */
	for (i = 0; i < MEM_PAGE_COUNT; i++)	{
		while (mem->pages[i])
			mem_page_free(mem, mem->pages[i]->tag);
	}

	/* This must have released all host mappings.
	 * Now, free memory structure. */
	
	printf("Freeing Memory : Done \n");
	
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
	struct mem_page_t *page, *ram_page;

	/* Calculate page boundaries */
	tag1 = addr & ~(MEM_PAGESIZE-1);
	tag2 = (addr + size - 1) & ~(MEM_PAGESIZE-1);

	/* Allocate pages */
    int numberPages =0;
	for (tag = tag1; tag <= tag2; tag += MEM_PAGESIZE) {
		page = swap_mem_page_get(mem, tag);
		if (!page){
            numberPages++;
			page = swap_mem_page_create(mem, tag, perm);
            //printf("Creating page for addr %u , on swap space, page number is %u\n " ,tag,numberPages);
        }
        else{
			///TODO
			//update permission on ram page also if page already exists. Change this
			ram_page = mem_page_get(mem, tag);
			ram_page->perm |= perm;
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
struct mem_page_t* free_a_swap_page(struct mem_page_t * page, struct mem_t* mem){
 
   /*	
   if(mem->context->uid == 1)
		return;
  */
   struct mem_page_t* iter=NULL;
   struct mem_page_t* prev=NULL;
   iter = swap_mem->occupied_list_head;
   int flag_found =0;
   if(page->fpos.__pos == 18223104){
		printf("free page for page: %d , pid %d fpos %d \n", (page->tag >> MEM_LOGPAGESIZE)%MEM_PAGE_COUNT,
						mem->context->uid,page->fpos.__pos);
		if(iter)	{
			printf("Occupied list head is not null, fpos is %d \n", iter->fpos.__pos);
		}
        
   }
   while(iter){
	
       if(iter->fpos.__pos == page->fpos.__pos){
           flag_found = 1;     
           break;
       }
       prev = iter;
       iter = iter->newNext;
   }
   if(flag_found){
    
           if(!prev){
                /* when the head is to be updated */
               swap_mem->occupied_list_head = iter->newNext;
               if(swap_mem->occupied_list_head == NULL){
					swap_mem->occupied_list_tail = NULL;
				}
			}
			
           else
           {
            if(swap_mem->occupied_list_tail == iter){
				swap_mem->occupied_list_tail = prev;
			}
            prev->newNext = iter->newNext;
           }
           
           swap_mem->free_list_tail->newNext= iter ;
           swap_mem->free_list_tail = iter;
           iter->newNext = NULL;
           return iter;
      
   }
   else{
		fatal("[SWAP] :Trying to free a free page for pid %d fpos %u \n",mem->context->uid,page->fpos.__pos);
        return NULL;
   }
}


struct mem_page_t* get_new_swap_page(){
    if(!swap_mem->free_list_head){
        printf("Pages used are %d\n ", swap_page_count_used);
        fatal("No space on swap available\n");
        assert(0);
    }
    else{
        //printf("Allocating page #%d \n", swap_page_count_used);
        /* getting the page from the front of the list */
        struct mem_page_t* new_page = swap_mem->free_list_head ;
        /* updating free list to next page */
        swap_mem->free_list_head = new_page->newNext;
        if(swap_mem->free_list_head==NULL){
			swap_mem->free_list_tail = NULL;
		}
        add_occupied_page(new_page);
        //(swap_mem->occupied_list_tail)->next = new_page;
        //swap_mem->occupied_list_tail = new_page;
        new_page->newNext = NULL;
        swap_page_count_used++;
        if(new_page->fpos.__pos ==18223104){
			printf("page allocated 18223104 \n");
		}
        return new_page;
    }
}

void add_occupied_page(struct mem_page_t* page){
	
	//printf("add a occupied page %u \n", page->fpos.__pos);
    if(!(swap_mem->occupied_list_head) && !(swap_mem->occupied_list_tail)){
        swap_mem->occupied_list_head = page;
        swap_mem->occupied_list_tail = page;
        page->newNext = NULL;
    }
    
    else
    {
    		//printf("add a occupied page %u \n", page->fpos.__pos);
			/*
			if(page->fpos.__pos ==  18223104){
				printf("prev page is %d \n",(swap_mem->occupied_list_tail)->fpos.__pos);
			}
			*/ 
			(swap_mem->occupied_list_tail)->newNext = page;
            swap_mem->occupied_list_tail =page;
            page->newNext = NULL;
            if(page->fpos.__pos ==  18223104){
				printf("current head is %d \n",(swap_mem->occupied_list_head)->fpos.__pos);
			}
    }

    if(page->fpos.__pos == 18223104){
		//printf("Searching for page 18223104\n");
		struct mem_page_t* start = swap_mem->occupied_list_head;
		/*
		if(swap_mem->occupied_list_tail->fpos.__pos == page->fpos.__pos){
			printf("18223104 page is found\n");
		}
		else{
			printf("Haga be \n");
		}
		 
		while(start){
				//printf("%d\n",start->fpos.__pos);
				if(start->fpos.__pos == swap_mem->occupied_list_tail->fpos.__pos){
					printf("sahi chal raha hain reaches tail\n");
				}
				if(start->fpos.__pos == 18223104){
					printf("page found in occupied list 18223104\n");
					break;
				}
			start = start->newNext;
		}
		*/ 
	}
}



void swap_initialize(){
    swap_mem = malloc(sizeof(struct swap_mem_t));
    swap_mem->free_list_head=NULL;
    swap_mem->free_list_tail=NULL;
    swap_mem->occupied_list_head =NULL;
    swap_mem->occupied_list_tail =NULL;
    swap_fd = fopen("Sim_disk","w");
    fclose(swap_fd);
    swap_fd = open_swap_disk();   
	fseek(swap_fd ,0, SEEK_SET);
    int no_of_pages =0;
    while(no_of_pages < MEM_PAGE_COUNT){
        if(!(swap_mem->free_list_head) && !(swap_mem->free_list_tail)){ 
            struct mem_page_t* free_page = malloc(sizeof(struct mem_page_t)); 
            free_page->fpos.__pos = ftell(swap_fd);
            swap_mem->free_list_head = free_page;
            swap_mem->free_list_tail = free_page;
            free_page->newNext = NULL;
        }
        else{
            struct mem_page_t* free_page = malloc(sizeof(struct mem_page_t)); 
            free_page->fpos.__pos = ftell(swap_fd);
            (swap_mem->free_list_tail)->newNext= free_page;
            if(swap_mem->free_list_tail == swap_mem->free_list_head){
                (swap_mem->free_list_head)->newNext = free_page;  
            }
            swap_mem->free_list_tail = free_page; 
            free_page->newNext = NULL;
        }
        fseek(swap_fd,MEM_PAGESIZE,SEEK_CUR);
        no_of_pages++;
    }
    printf("Swap pages initialized, pages allocated on swap disk %d\n",no_of_pages);
}    



/* Copy memory pages. All parameters must be multiple of the page size.
 * The pages in the source and destination interval must exist. */
void mem_copy(struct mem_t *mem, uint32_t dest, uint32_t src, int size)
{
	struct mem_page_t *page_dest, *page_src;
    void* buf;
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
		page_src = mem_page_get(mem, src); 
		assert(page_src);
		assert((page_src->free_flag == 0));
		void *buf= NULL;
		if(page_src->data){
			// copy data to a temporary buffer;
			buf = malloc(MEM_PAGESIZE);
		}
		memcpy(buf, page_src->data, MEM_PAGESIZE);
		
		page_dest = mem_page_get(mem, dest);
		
		assert(page_dest);
		///TODO ... Check for a condition here, there's a chance that page_dest gets removed
		assert((page_dest->free_flag == 0)); 
		/* Different actions depending on whether source and
		 * destination page data are allocated. */
		
		printf("Inside MemCopy! if there are any errors, blame @sanchit-garg");
        
		
        if (buf) {
			
			if (!page_dest->data)
				page_dest->data = malloc(MEM_PAGESIZE);
			memcpy(page_dest->data, buf, MEM_PAGESIZE);
			free(buf);
		} 
		else {
			if (page_dest->data)
				memset(page_dest->data, 0, MEM_PAGESIZE);
		}
		
        //set dirty
        page_dest->dirty = 1;
        
        /////////////////////////////////////////////////
        /* Advance pointers */
		src += MEM_PAGESIZE;
		dest += MEM_PAGESIZE;
		size -= MEM_PAGESIZE;
	}
}
