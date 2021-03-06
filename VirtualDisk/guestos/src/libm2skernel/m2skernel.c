/*
 *  Multi2Sim
 *  Copyright (C) 2007  Rafael Ubal Tena (raurte@gap.upv.es)
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

#include <m2skernel.h>


/* Global Multi2Sim
 * Kernel Variable */
struct kernel_t *ke;




#define LIST_INSERT_HEAD(name, ctx) { \
	assert(!ctx->name##_next && !ctx->name##_prev); \
	ctx->name##_next = ke->name##_list_head; \
	if (ctx->name##_next) ctx->name##_next->name##_prev = ctx; \
	ke->name##_list_head = ctx; \
	if (!ke->name##_list_tail) ke->name##_list_tail = ctx; \
	ke->name##_count++; \
	ke->name##_max = MAX(ke->name##_max, ke->name##_count); }

#define LIST_INSERT_TAIL(name, ctx) { \
	assert(!ctx->name##_next && !ctx->name##_prev); \
	ctx->name##_prev = ke->name##_list_tail; \
	if (ctx->name##_prev) ctx->name##_prev->name##_next = ctx; \
	ke->name##_list_tail = ctx; \
	if (!ke->name##_list_head) ke->name##_list_head = ctx; \
	ke->name##_count++; }

#define LIST_REMOVE(name, ctx) { \
	if (ctx == ke->name##_list_head) ke->name##_list_head = ke->name##_list_head->name##_next; \
	if (ctx == ke->name##_list_tail) ke->name##_list_tail = ke->name##_list_tail->name##_prev; \
	if (ctx->name##_prev) ctx->name##_prev->name##_next = ctx->name##_next; \
	if (ctx->name##_next) ctx->name##_next->name##_prev = ctx->name##_prev; \
	ctx->name##_prev = ctx->name##_next = NULL; \
	ke->name##_count--; }

#define LIST_MEMBER(name, ctx) \
	(ke->name##_list_head == ctx || ctx->name##_prev || ctx->name##_next)
	
/* Initialization */

static uint64_t ke_init_time = 0;


void ke_ram_init(void)
{
	
	ke->ram = calloc(1, sizeof(struct ram_mem_t));
	ke->ram->pages = calloc(1, RAM_MEM_PAGE_COUNT * sizeof(struct mem_page_t));
	int i;
	
	for(i=0;i<RAM_MEM_PAGE_COUNT;i++){
		//ke->ram->pages[i] = calloc(1, sizeof(struct mem_page_t));
		ke->ram->pages[i].free_flag = 1;
		ke->ram->pages[i].next = NULL;
		ke->ram->pages[i].dirty = 0;
		ke->ram->pages[i].data = calloc(1, MEM_PAGESIZE);
	}
}



void ke_init(void)
{
	uint32_t endian = 0x44332211;
	unsigned char *pendian = (unsigned char *) &endian;
	
	


	/* Endian check */
	if (pendian[0] != 0x11 || pendian[3] != 0x44)
		fatal("cannot run kernel on a big endian machine");
	
	isa_init();
	ke = calloc(1, sizeof(struct kernel_t));
	
	ke_ram_init();
	
	int z;
	for(z=0;z<TLB_SIZE;z++){
		printf("comes here \n");
		ke->tlb[z].valid = 0;
		ke->tlb[z].ram_page = NULL;
		ke->tlb[z].context = NULL;
	}
	
	ke->current_pid = 1000;  /* Initial assigned pid */
	ke->instruction_no = 0;
	ke->current_track = 0;
	
	/* Initialize mutex for variables controlling calls to 'ke_process_events()' */
	pthread_mutex_init(&ke->process_events_mutex, NULL);

	/* Debug categories */
	isa_inst_debug_category = debug_new_category();
	isa_call_debug_category = debug_new_category();
	elf_debug_category = debug_new_category();
	ld_debug_category = debug_new_category();
	syscall_debug_category = debug_new_category();
	ctx_debug_category = debug_new_category();

	/* Initialize GPU kernel */
	gk_init();

	/* Record start time */
	ke_init_time = ke_timer();
}


/* Finalization */
void ke_done(void)
{
	struct ctx_t *ctx;

	/* Finish all contexts */
	for (ctx = ke->context_list_head; ctx; ctx = ctx->context_next)
		if (!ctx_get_status(ctx, ctx_finished))
			ctx_finish(ctx, 0);

	/* Free contexts */
	while (ke->context_list_head)
		ctx_free(ke->context_list_head);
	
	/* Finalize GPU kernel */
	gk_done();

	/* End */
	free(ke);
	isa_done();
	syscall_summary();
}

void insertInterrupt(interrupt_t* intpt){
	//printf("Interrupt inserted, process no longer in ctx_running\n");
    if(ke->interrupt_list_head == NULL){
        //printf("Adding interrupt, listr head null\n");
        ke->interrupt_list_head = intpt;
        ke->interrupt_list_tail = intpt;
    }
    ke->interrupt_list_tail->interrupt_next = intpt;
    ke->interrupt_list_tail = intpt;
    ke->interrupt_list_tail->interrupt_next = NULL;
}

void deleteInterrupt(interrupt_t* del_ptr){
    if(!ke->interrupt_list_head){
        //printf("No interrupt present\n");
        return;
    }
    int flag =0;
    interrupt_t* temp = ke->interrupt_list_head;
    //printf("there%d\n", temp->interrupt_next->instruction_no);
    if(temp == del_ptr){
        ke->interrupt_list_head = temp->interrupt_next;
        if(ke->interrupt_list_head == NULL){
			ke->interrupt_list_tail = NULL;
		}
        return;
    }
    
    while(temp->interrupt_next){
       // printf("loop%d\n",temp->instruction_no );
        if(temp->interrupt_next == del_ptr){
            temp->interrupt_next = del_ptr->interrupt_next;
            if(del_ptr == ke->interrupt_list_tail){
                ke->interrupt_list_tail = temp;
                //printf("here%d\n", ke->interrupt_list_head->instruction_no);
            }
            free(del_ptr);
			break;
        }
        temp = temp->interrupt_next;
    }
   
    return;

}

// returns the next lowest interrupt in the list
interrupt_t* getNextInterrupt(){
    
    interrupt_t* start = ke->interrupt_list_head;
    interrupt_t* minInterrupt = ke->interrupt_list_head;
    int minInstrNo = -1;
    if(start ==NULL){
       // printf("No IO interrupt present\n");               
    }
    else{
           minInstrNo = start->instruction_no;
           minInterrupt = start;
           start = start->interrupt_next;
           for(;start;start = start->interrupt_next){
               
               if(minInstrNo > start->instruction_no){
                       minInstrNo = start->instruction_no;
                       minInterrupt = start;
               }
           }
           
    }
    return minInterrupt;
}








int ke_handle_interrupts(void){

	interrupt_t *next_interrupt = getNextInterrupt();
	assert(next_interrupt == NULL || next_interrupt->instruction_no >= ke->instruction_no);
	while(next_interrupt != NULL && next_interrupt->instruction_no == ke->instruction_no){
		
		//printf("Handling interrupt. Instruction_no: %d, Process_id: %d \n" , ke->instruction_no , (next_interrupt->context)->pid);
		//***************DEBUG IF YOU CAN************************
	//	printf("Handling interrupt. Instruction_no: %d, " , ke->instruction_no);
		
	//	printf(next_interrupt->type==INPUT?"INPUT \n":"OUTPUT \n");
		//printf("%d \n",next_interrupt->type);
		ke_list_insert_tail(ke_list_running , next_interrupt->context);
		//printf("Process_Id : %d, back to running\n",(next_interrupt->context)->pid);
		ke_list_remove(ke_list_suspended , next_interrupt->context);
		//printf("Process Id: %d Removed from suspended\n",(next_interrupt->context)->pid);
		if((next_interrupt->type == INPUT_SWAP_IN) || (next_interrupt->type== OUTPUT_SWAP_IN)){
			// swap in the process here 
			swap_in_process(next_interrupt->context->mem);
			ke->current_io_time = ke->current_io_time - next_interrupt->io_time;
			printf("Swapping in Process %d at time %d\n", next_interrupt->context->uid, ke->instruction_no); 
		}
		else{
			//printf("Process %d is ready at time %d\n", next_interrupt->context->uid, ke->instruction_no); 
		}
		deleteInterrupt(next_interrupt);
		next_interrupt = getNextInterrupt();
		assert(next_interrupt == NULL || next_interrupt->instruction_no >= ke->instruction_no);
	}	
}


int max(int x,int y){

    return (x>y)?x:y;
}


/* Execute one instruction from each running context. */
void ke_run(void)
{
	struct ctx_t *ctx, *ctx_trav; 
	int flag = 0;

   if(ke->running_list_head == NULL){
	interrupt_t *next_interrupt = getNextInterrupt();
	if(next_interrupt != NULL){
		//printf("Interrupt found, Type : %d \n", next_interrupt->type);
        if(ke->current_io_time){
            ke->current_io_time= max(0,ke->current_io_time -( next_interrupt->instruction_no - ke->instruction_no));
        }
		ke->instruction_no = next_interrupt->instruction_no;
		ke_handle_interrupts();
		goto RUN;
	}
  }


	/* Run an instruction from every running process */
RUN :

	for (ctx = ke->running_list_head; ctx; ctx = ctx->running_next) {
		int i;
		//printf ("out - %p\n", ctx);
		for (i = 0 ; i < ctx->instr_slice; ++i) {
			//printf("Current Process is %d\n",ctx->uid);
			ke_handle_interrupts();
	    	if (ctx_get_status(ctx, ctx_finished))
                break;
			ctx->mem->current_inst_faults=0;
			ctx->mem->total_io_penalty =0;
			ctx->toBeSwappedOut = 0;
			ctx_execute_inst(ctx);
			ke->instruction_no++;
            if(ke->current_io_time){
			    ke->current_io_time--;
            }
            if(ctx->toBeSwappedOut){
				struct interrupt_t *newInterrupt = malloc(sizeof(struct interrupt_t));
				newInterrupt->context = ctx;
				newInterrupt->io_time = ctx->mem->total_io_penalty;
				newInterrupt->instruction_no = ke->instruction_no + ke->current_io_time + ctx->mem->total_io_penalty;
				newInterrupt->type = OUTPUT_SWAP_IN; //kuch bhi
				//printf("%d ************* %d \n", newInterrupt->instruction_no, ke->current_io_time);
				ke_list_remove(ke_list_running,ctx);
				ke_list_insert_tail(ke_list_suspended,ctx);
				insertInterrupt(newInterrupt);
				
				ke->current_io_time += ctx->mem->total_io_penalty;
				printf("Swapping out Process %d at time %d \n", ctx->uid, ke->instruction_no);
				swap_out_process(ctx->mem);
				return;
			}
			if(ctx->mem->current_inst_faults){
				//printf("Suspending Process %d at time %d \n", ctx->uid, ke->instruction_no);
				addInterruptForProcess(ctx->mem,ctx->mem->current_inst_faults);
				ctx->mem->current_inst_faults=0;
				return;
			}
			if(ctx!=ke->running_list_head)
				break;
			
		}
	}
	
			
	/* Free finished contexts */
	while (ke->finished_list_head)
		ctx_free(ke->finished_list_head);
	
	/* Process list of suspended contexts */
	ke_process_events();

}


void ke_dump(FILE *f)
{
	struct ctx_t *ctx;
	int n = 0;
	ctx = ke->context_list_head;
	fprintf(f, "List of kernel contexts (arbitrary order):\n");
	while (ctx) {
		fprintf(f, "kernel context #%d:\n", n);
		ctx_dump(ctx, f);
		ctx = ctx->context_next;
		n++;
	}
}



void ke_list_insert_head(enum ke_list_enum list, struct ctx_t *ctx)
{
	assert(!ke_list_member(list, ctx));
	switch (list) {
	case ke_list_context: LIST_INSERT_HEAD(context, ctx); break;
	case ke_list_running: LIST_INSERT_HEAD(running, ctx); break;
	case ke_list_finished: LIST_INSERT_HEAD(finished, ctx); break;
	case ke_list_zombie: LIST_INSERT_HEAD(zombie, ctx); break;
	case ke_list_suspended: LIST_INSERT_HEAD(suspended, ctx); break;
	case ke_list_alloc: LIST_INSERT_HEAD(alloc, ctx); break;
	//case ke_list_interrupt : LIST_INSERT_HEAD(interrupt, ctx); break;
	}
}


void ke_list_insert_tail(enum ke_list_enum list, struct ctx_t *ctx)
{
	assert(!ke_list_member(list, ctx));
	switch (list) {
	case ke_list_context: LIST_INSERT_TAIL(context, ctx); break;
	case ke_list_running: LIST_INSERT_TAIL(running, ctx); break;
	case ke_list_finished: LIST_INSERT_TAIL(finished, ctx); break;
	case ke_list_zombie: LIST_INSERT_TAIL(zombie, ctx); break;
	case ke_list_suspended: LIST_INSERT_TAIL(suspended, ctx); break;
	case ke_list_alloc: LIST_INSERT_TAIL(alloc, ctx); break;
//	case ke_list_interrupt : LIST_INSERT_TAIL(interrupt, ctx); break;

	}
}


void ke_list_remove(enum ke_list_enum list, struct ctx_t *ctx)
{
	assert(ke_list_member(list, ctx));
	switch (list) {
	case ke_list_context: LIST_REMOVE(context, ctx); break;
	case ke_list_running: LIST_REMOVE(running, ctx); break;
	case ke_list_finished: LIST_REMOVE(finished, ctx); break;
	case ke_list_zombie: LIST_REMOVE(zombie, ctx); break;
	case ke_list_suspended: LIST_REMOVE(suspended, ctx); break;
	case ke_list_alloc: LIST_REMOVE(alloc, ctx); break;
	//case ke_list_interrupt : LIST_REMOVE(interrupt, ctx); break;
	}
}


int ke_list_member(enum ke_list_enum list, struct ctx_t *ctx)
{
	switch (list) {
	case ke_list_context: return LIST_MEMBER(context, ctx);
	case ke_list_running: return LIST_MEMBER(running, ctx);
	case ke_list_finished: return LIST_MEMBER(finished, ctx);
	case ke_list_zombie: return LIST_MEMBER(zombie, ctx);
	case ke_list_suspended: return LIST_MEMBER(suspended, ctx);
	case ke_list_alloc: return LIST_MEMBER(alloc, ctx);
	//case ke_list_interrupt : return LIST_MEMBER(interrupt, ctx); 
	}
	return 0;
}


#undef LIST_INSERT
#undef LIST_REMOVE
#undef LIST_MEMBER


/* Return a counter of microseconds. */
uint64_t ke_timer()
{
	struct timeval tv;
	gettimeofday(&tv, NULL);
	return (uint64_t) tv.tv_sec * 1000000 + tv.tv_usec - ke_init_time;
}


/* Schedule a call to 'ke_process_events' */
void ke_process_events_schedule()
{
	pthread_mutex_lock(&ke->process_events_mutex);
	ke->process_events_force = 1;
	pthread_mutex_unlock(&ke->process_events_mutex);
}


/* Function that suspends the host thread waiting for an event to occur.
 * When the event finally occurs (i.e., before the function finishes, a
 * call to 'ke_process_events' is scheduled.
 * The argument 'arg' is the associated guest context. */
void *ke_host_thread_suspend(void *arg)
{
	struct ctx_t *ctx = (struct ctx_t *) arg;
	uint64_t now = ke_timer();

	/* Detach this thread - we don't want the parent to have to join it to release
	 * its resources. The thread termination can be observed by atomically checking
	 * the 'ctx->host_thread_suspend_active' flag. */
	pthread_detach(pthread_self());

	/* Context suspended in 'poll' system call */
	if (ctx_get_status(ctx, ctx_nanosleep)) {
		
		uint64_t timeout;
		
		/* Calculate remaining sleep time in microseconds */
		timeout = ctx->wakeup_time > now ? ctx->wakeup_time - now : 0;
		usleep(timeout);
	
	} else if (ctx_get_status(ctx, ctx_poll)) {

		struct fd_t *fd;
		struct pollfd host_fds;
		int err, timeout;
		
		/* Get file descriptor */
		fd = fdt_entry_get(ctx->fdt, ctx->wakeup_fd);
		if (!fd)
			fatal("syscall 'poll': invalid 'wakeup_fd'");

		/* Calculate timeout for host call in milliseconds from now */
		if (!ctx->wakeup_time)
			timeout = -1;
		else if (ctx->wakeup_time < now)
			timeout = 0;
		else
			timeout = (ctx->wakeup_time - now) / 1000;

		/* Perform blocking host 'poll' */
		host_fds.fd = fd->host_fd;
		host_fds.events = ((ctx->wakeup_events & 4) ? POLLOUT : 0) | ((ctx->wakeup_events & 1) ? POLLIN : 0);
		err = poll(&host_fds, 1, timeout);
		if (err < 0)
			fatal("syscall 'poll': unexpected error in host 'poll'");
	
	} else if (ctx_get_status(ctx, ctx_read)) {
		
		struct fd_t *fd;
		struct pollfd host_fds;
		int err;

		/* Get file descriptor */
		fd = fdt_entry_get(ctx->fdt, ctx->wakeup_fd);
		if (!fd)
			fatal("syscall 'read': invalid 'wakeup_fd'");

		/* Perform blocking host 'poll' */
		host_fds.fd = fd->host_fd;
		host_fds.events = POLLIN;
		err = poll(&host_fds, 1, -1);
		if (err < 0)
			fatal("syscall 'read': unexpected error in host 'poll'");
	
	} else if (ctx_get_status(ctx, ctx_write)) {
		
		struct fd_t *fd;
		struct pollfd host_fds;
		int err;

		/* Get file descriptor */
		fd = fdt_entry_get(ctx->fdt, ctx->wakeup_fd);
		if (!fd)
			fatal("syscall 'write': invalid 'wakeup_fd'");

		/* Perform blocking host 'poll' */
		host_fds.fd = fd->host_fd;
		host_fds.events = POLLOUT;
		err = poll(&host_fds, 1, -1);
		if (err < 0)
			fatal("syscall 'write': unexpected error in host 'write'");

	}

	/* Event occurred - thread finishes */
	pthread_mutex_lock(&ke->process_events_mutex);
	ke->process_events_force = 1;
	ctx->host_thread_suspend_active = 0;
	pthread_mutex_unlock(&ke->process_events_mutex);
	return NULL;
}


/* Function that suspends the host thread waiting for a timer to expire,
 * and then schedules a call to 'ke_process_events'. */
void *ke_host_thread_timer(void *arg)
{
	struct ctx_t *ctx = (struct ctx_t *) arg;
	uint64_t now = ke_timer();
	struct timespec ts;
	uint64_t sleep_time;  /* In usec */

	/* Detach this thread - we don't want the parent to have to join it to release
	 * its resources. The thread termination can be observed by thread-safely checking
	 * the 'ctx->host_thread_timer_active' flag. */
	pthread_detach(pthread_self());

	/* Calculate sleep time, and sleep only if it is greater than 0 */
	if (ctx->host_thread_timer_wakeup > now) {
		sleep_time = ctx->host_thread_timer_wakeup - now;
		ts.tv_sec = sleep_time / 1000000;
		ts.tv_nsec = (sleep_time % 1000000) * 1000;  /* nsec */
		nanosleep(&ts, NULL);
	}

	/* Timer expired, schedule call to 'ke_process_events' */
	pthread_mutex_lock(&ke->process_events_mutex);
	ke->process_events_force = 1;
	ctx->host_thread_timer_active = 0;
	pthread_mutex_unlock(&ke->process_events_mutex);
	return NULL;
}


/* Check for events detected in spawned host threads, like waking up contexts or
 * sending signals.
 * The list is only processed if flag 'ke->process_events_force' is set. */
void ke_process_events()
{
	struct ctx_t *ctx, *next;
	uint64_t now = ke_timer();
	
	/* Check if events need actually be checked. */
	pthread_mutex_lock(&ke->process_events_mutex);
	if (!ke->process_events_force) {
		pthread_mutex_unlock(&ke->process_events_mutex);
		return;
	}
	
	/* By default, no subsequent call to 'ke_process_events' is assumed */
	ke->process_events_force = 0;

	/*
	 * LOOP 1
	 * Look at the list of suspended contexts and try to find
	 * one that needs to be woken up.
	 */
	for (ctx = ke->suspended_list_head; ctx; ctx = next) {

		/* Save next */
		next = ctx->suspended_next;

		/* Context is suspended in 'nanosleep' system call. */
		if (ctx_get_status(ctx, ctx_nanosleep))
		{
			uint32_t rmtp = ctx->regs->ecx;
			uint64_t zero = 0;
			uint32_t sec, usec;
			uint64_t diff;

			/* If 'ke_host_thread_suspend' is still running for this context, do nothing. */
			if (ctx->host_thread_suspend_active)
				continue;

			/* Timeout expired */
			if (ctx->wakeup_time <= now) {
				if (rmtp)
					mem_write(ctx->mem, rmtp, 8, &zero);
				syscall_debug("syscall 'nanosleep' - continue (pid %d)\n", ctx->pid);
				syscall_debug("  return=0x%x\n", ctx->regs->eax);
				ctx_clear_status(ctx, ctx_suspended | ctx_nanosleep);
				continue;
			}

			/* Context received a signal */
			if (ctx->signal_masks->pending & ~ctx->signal_masks->blocked) {
				if (rmtp) {
					diff = ctx->wakeup_time - now;
					sec = diff / 1000000;
					usec = diff % 1000000;
					mem_write(ctx->mem, rmtp, 4, &sec);
					mem_write(ctx->mem, rmtp + 4, 4, &usec);
				}
				ctx->regs->eax = -EINTR;
				syscall_debug("syscall 'nanosleep' - interrupted by signal (pid %d)\n", ctx->pid);
				ctx_clear_status(ctx, ctx_suspended | ctx_nanosleep);
				continue;
			}

			/* No event available, launch 'ke_host_thread_suspend' again */
			ctx->host_thread_suspend_active = 1;
			if (pthread_create(&ctx->host_thread_suspend, NULL, ke_host_thread_suspend, ctx))
				fatal("syscall 'poll': could not create child thread");
			continue;
		}

		/* Context suspended in 'rt_sigsuspend' system call */
		if (ctx_get_status(ctx, ctx_sigsuspend))
		{
			/* Context received a signal */
			if (ctx->signal_masks->pending & ~ctx->signal_masks->blocked) {
				signal_handler_check_intr(ctx);
				ctx->signal_masks->blocked = ctx->signal_masks->backup;
				syscall_debug("syscall 'rt_sigsuspend' - interrupted by signal (pid %d)\n", ctx->pid);
				ctx_clear_status(ctx, ctx_suspended | ctx_sigsuspend);
				continue;
			}

			/* No event available. The context will never awake on its own, so no
			 * 'ke_host_thread_suspend' is necessary. */
			continue;
		}

		/* Context suspended in 'poll' system call */
		if (ctx_get_status(ctx, ctx_poll))
		{
			uint32_t prevents = ctx->regs->ebx + 6;
			uint16_t revents = 0;
			struct fd_t *fd;
			struct pollfd host_fds;
			int err;

			/* If 'ke_host_thread_suspend' is still running for this context, do nothing. */
			if (ctx->host_thread_suspend_active)
				continue;

			/* Get file descriptor */
			fd = fdt_entry_get(ctx->fdt, ctx->wakeup_fd);
			if (!fd)
				fatal("syscall 'poll': invalid 'wakeup_fd'");

			/* Context received a signal */
			if (ctx->signal_masks->pending & ~ctx->signal_masks->blocked) {
				signal_handler_check_intr(ctx);
				syscall_debug("syscall 'poll' - interrupted by signal (pid %d)\n", ctx->pid);
				ctx_clear_status(ctx, ctx_suspended | ctx_poll);
				continue;
			}

			/* Perform host 'poll' call */
			host_fds.fd = fd->host_fd;
			host_fds.events = ((ctx->wakeup_events & 4) ? POLLOUT : 0) | ((ctx->wakeup_events & 1) ? POLLIN : 0);
			err = poll(&host_fds, 1, 0);
			if (err < 0)
				fatal("syscall 'poll': unexpected error in host 'poll'");

			/* POLLOUT event available */
			if (ctx->wakeup_events & host_fds.revents & POLLOUT) {
				revents = POLLOUT;
				mem_write(ctx->mem, prevents, 2, &revents);
				ctx->regs->eax = 1;
				syscall_debug("syscall poll - continue (pid %d) - POLLOUT occurred in file\n", ctx->pid);
				syscall_debug("  retval=%d\n", ctx->regs->eax);
				ctx_clear_status(ctx, ctx_suspended | ctx_poll);
				continue;
			}

			/* POLLIN event available */
			if (ctx->wakeup_events & host_fds.revents & POLLIN) {
				revents = POLLIN;
				mem_write(ctx->mem, prevents, 2, &revents);
				ctx->regs->eax = 1;
				syscall_debug("syscall poll - continue (pid %d) - POLLIN occurred in file\n", ctx->pid);
				syscall_debug("  retval=%d\n", ctx->regs->eax);
				ctx_clear_status(ctx, ctx_suspended | ctx_poll);
				continue;
			}

			/* Timeout expired */
			if (ctx->wakeup_time && ctx->wakeup_time < now) {
				revents = 0;
				mem_write(ctx->mem, prevents, 2, &revents);
				syscall_debug("syscall poll - continue (pid %d) - time out\n", ctx->pid);
				syscall_debug("  return=0x%x\n", ctx->regs->eax);
				ctx_clear_status(ctx, ctx_suspended | ctx_poll);
				continue;
			}

			/* No event available, launch 'ke_host_thread_suspend' again */
			ctx->host_thread_suspend_active = 1;
			if (pthread_create(&ctx->host_thread_suspend, NULL, ke_host_thread_suspend, ctx))
				fatal("syscall 'poll': could not create child thread");
			continue;
		}


		/* Context suspended in a 'write' system call  */
		if (ctx_get_status(ctx, ctx_write))
		{
			struct fd_t *fd;
			int count, err;
			uint32_t pbuf;
			void *buf;
			struct pollfd host_fds;

			/* If 'ke_host_thread_suspend' is still running for this context, do nothing. */
			if (ctx->host_thread_suspend_active)
				continue;

			/* Context received a signal */
			if (ctx->signal_masks->pending & ~ctx->signal_masks->blocked) {
				signal_handler_check_intr(ctx);
				syscall_debug("syscall 'write' - interrupted by signal (pid %d)\n", ctx->pid);
				ctx_clear_status(ctx, ctx_suspended | ctx_write);
				continue;
			}

			/* Get file descriptor */
			fd = fdt_entry_get(ctx->fdt, ctx->wakeup_fd);
			if (!fd)
				fatal("syscall 'write': invalid 'wakeup_fd'");

			/* Check if data is ready in file by polling it */
			host_fds.fd = fd->host_fd;
			host_fds.events = POLLOUT;
			err = poll(&host_fds, 1, 0);
			if (err < 0)
				fatal("syscall 'write': unexpected error in host 'poll'");

			/* If data is ready in the file, wake up context */
			if (host_fds.revents) {
				pbuf = ctx->regs->ecx;
				count = ctx->regs->edx;
				buf = malloc(count);
				mem_read(ctx->mem, pbuf, count, buf);

				count = write(fd->host_fd, buf, count);
				if (count < 0)
					fatal("syscall 'write': unexpected error in host 'write'");

				ctx->regs->eax = count;
				free(buf);

				syscall_debug("syscall write - continue (pid %d)\n", ctx->pid);
				syscall_debug("  return=0x%x\n", ctx->regs->eax);
				ctx_clear_status(ctx, ctx_suspended | ctx_write);
				continue;
			}

			/* Data is not ready to be written - launch 'ke_host_thread_suspend' again */
			ctx->host_thread_suspend_active = 1;
			if (pthread_create(&ctx->host_thread_suspend, NULL, ke_host_thread_suspend, ctx))
				fatal("syscall 'write': could not create child thread");
			continue;
		}

		/* Context suspended in 'read' system call */
		if (ctx_get_status(ctx, ctx_read))
		{
			struct fd_t *fd;
			uint32_t pbuf;
			int count, err;
			void *buf;
			struct pollfd host_fds;

			/* If 'ke_host_thread_suspend' is still running for this context, do nothing. */
			if (ctx->host_thread_suspend_active)
				continue;

			/* Context received a signal */
			if (ctx->signal_masks->pending & ~ctx->signal_masks->blocked) {
				signal_handler_check_intr(ctx);
				syscall_debug("syscall 'read' - interrupted by signal (pid %d)\n", ctx->pid);
				ctx_clear_status(ctx, ctx_suspended | ctx_read);
				continue;
			}

			/* Get file descriptor */
			fd = fdt_entry_get(ctx->fdt, ctx->wakeup_fd);
			if (!fd)
				fatal("syscall 'read': invalid 'wakeup_fd'");

			/* Check if data is ready in file by polling it */
			host_fds.fd = fd->host_fd;
			host_fds.events = POLLIN;
			err = poll(&host_fds, 1, 0);
			if (err < 0)
				fatal("syscall 'read': unexpected error in host 'poll'");

			/* If data is ready, perform host 'read' call and wake up */
			if (host_fds.revents) {
				pbuf = ctx->regs->ecx;
				count = ctx->regs->edx;
				buf = malloc(count);
				
				count = read(fd->host_fd, buf, count);
				if (count < 0)
					fatal("syscall 'read': unexpected error in host 'read'");

				ctx->regs->eax = count;
				mem_write(ctx->mem, pbuf, count, buf);
				free(buf);

				syscall_debug("syscall 'read' - continue (pid %d)\n", ctx->pid);
				syscall_debug("  return=0x%x\n", ctx->regs->eax);
				ctx_clear_status(ctx, ctx_suspended | ctx_read);
				continue;
			}

			/* Data is not ready. Launch 'ke_host_thread_suspend' again */
			ctx->host_thread_suspend_active = 1;
			if (pthread_create(&ctx->host_thread_suspend, NULL, ke_host_thread_suspend, ctx))
				fatal("syscall 'read': could not create child thread");
			continue;
		}

		/* Context suspended in a 'waitpid' system call */
		if (ctx_get_status(ctx, ctx_waitpid))
		{
			struct ctx_t *child;
			uint32_t pstatus;

			/* A zombie child is available to 'waitpid' it */
			child = ctx_get_zombie(ctx, ctx->wakeup_pid);
			if (child) {

				/* Continue with 'waitpid' system call */
				pstatus = ctx->regs->ecx;
				ctx->regs->eax = child->pid;
				if (pstatus)
					mem_write(ctx->mem, pstatus, 4, &child->exit_code);
				ctx_set_status(child, ctx_finished);

				syscall_debug("syscall waitpid - continue (pid %d)\n", ctx->pid);
				syscall_debug("  return=0x%x\n", ctx->regs->eax);
				ctx_clear_status(ctx, ctx_suspended | ctx_waitpid);
				continue;
			}

			/* No event available. Since this context won't awake on its own, no
			 * 'ke_host_thread_suspend' is needed. */
			continue;
		}
	}


	/*
	 * LOOP 2
	 * Check list of all contexts for expired timers.
	 */
	for (ctx = ke->context_list_head; ctx; ctx = ctx->context_next)
	{
		int sig[3] = { 14, 26, 27 };  /* SIGALRM, SIGVTALRM, SIGPROF */
		int i;

		/* If there is already a 'ke_host_thread_timer' running, do nothing. */
		if (ctx->host_thread_timer_active)
			continue;

		/* Check for any expired 'itimer': itimer_value < now
		 * In this case, send corresponding signal to process.
		 * Then calculate next 'itimer' occurrence: itimer_value = now + itimer_interval */
		for (i = 0; i < 3; i++ ) {
			
			/* Timer inactive or not expired yet */
			if (!ctx->itimer_value[i] || ctx->itimer_value[i] > now)
				continue;

			/* Timer expired - send a signal.
			 * The target process might be suspended, so the host thread is canceled, and a new
			 * call to 'ke_process_events' is scheduled. Since 'ke_process_events_mutex' is
			 * already locked, the thread-unsafe version of 'ctx_host_thread_suspend_cancel' is used. */
			__ctx_host_thread_suspend_cancel(ctx);
			ke->process_events_force = 1;
			sim_sigset_add(&ctx->signal_masks->pending, sig[i]);

			/* Calculate next occurrence */
			ctx->itimer_value[i] = 0;
			if (ctx->itimer_interval[i])
				ctx->itimer_value[i] = now + ctx->itimer_interval[i];
		}

		/* Calculate the time when next wakeup occurs. */
		ctx->host_thread_timer_wakeup = 0;
		for (i = 0; i < 3; i++) {
			if (!ctx->itimer_value[i])
				continue;
			assert(ctx->itimer_value[i] >= now);
			if (!ctx->host_thread_timer_wakeup || ctx->itimer_value[i] < ctx->host_thread_timer_wakeup)
				ctx->host_thread_timer_wakeup = ctx->itimer_value[i];
		}

		/* If a new timer was set, launch 'ke_host_thread_timer' again */
		if (ctx->host_thread_timer_wakeup) {
			ctx->host_thread_timer_active = 1;
			if (pthread_create(&ctx->host_thread_timer, NULL, ke_host_thread_timer, ctx))
				fatal("%s: could not create child thread", __FUNCTION__);
		}
	}


	/*
	 * LOOP 3
	 * Process pending signals in running contexts to launch signal handlers
	 */
	for (ctx = ke->running_list_head; ctx; ctx = ctx->running_next)
	{
		signal_handler_check(ctx);
	}

	
	/* Unlock */
	pthread_mutex_unlock(&ke->process_events_mutex);
}

