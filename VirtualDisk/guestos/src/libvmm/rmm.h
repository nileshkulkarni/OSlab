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

#ifndef RMM_H
#define RMM_H

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

struct ram_mem_page_t {
	uint32_t tag;
	enum mem_access_enum perm;  /* Access permissions; combination of flags */
	struct ram_mem_page_t *next;
	unsigned char *data;
	struct mem_host_mapping_t *host_mapping;  /* If other than null, page is host mapping */
	struct ctx_t * context;
    // TODO mem_host_mapping_t not known why it exists
};

struct ram_mem_t {
	struct ram_mem_page_t *pages[MEM_PAGE_COUNT];
	int sharing;  /* Number of contexts sharing memory map */
	uint32_t last_address;  /* Address of last access */
	int safe;  /* Safe mode */
	struct mem_host_mapping_t *host_mapping_list;  /* List of host mappings */
	
	
	struct ram_mem_page_t * free_list;
	struct ram_mem_page_t * occupied_list;
	
	
};

struct ram_mem_page_t* get_free_ram_page();
void get_ram_page_to_replace(struct ram_mem_page_t*);
void free_ram_page(struct ram_mem_page_t*);
void insert_ram_page(struct swap_mem_page_t*);


