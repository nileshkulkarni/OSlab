/*
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; 
 */ 

#ifndef VMM_H
#define VMM_H

#include <mhandle.h>
#include <debug.h>
#include <config.h>
#include <buffer.h>
#include <list.h>
#include <lnlist.h>
#include <misc.h>
#include <elf.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <assert.h>
#include <signal.h>
#include <disasm.h>
#include <time.h>
#include <pthread.h>
#include <poll.h>
#include <errno.h>
#include <gpukernel.h>
#include <sys/time.h>

/*
struct page_fault;

struct page_fault{
	
	struct *swap_mem_page_t;
	ctx_t *context;
};



int page_fault_routine(struct page_fault pf);
void vm_mem_access(struct ctx_t context, uint32_t addr, uint32_t size, mem_access_enum perm);
*/



#endif

