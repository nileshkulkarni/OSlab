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
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
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


struct page_table_entry;
struct page_table;
struct page_fault;
struct vm_page;
struct main_memory;
struct sharing

struct page_table_entry{
	int dirty_flag;
	int valid_flag;
	int reference_flag;
	struct *vm_page;
	struct *mem_page_t;
};

struct page_fault{
	int dirty_flag;
	int valid_flag;
	int reference_flag;
	struct *vm_page;
	struct *mem_page_t;
};


#endif

