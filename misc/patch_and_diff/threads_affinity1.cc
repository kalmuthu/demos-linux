/*
 * This file is part of the linuxapi project.
 * Copyright (C) 2011-2013 Mark Veltzer <mark.veltzer@gmail.com>
 *
 * The linuxapi package is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * The linuxapi package is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with the GNU C Library; if not, write to the Free
 * Software Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA
 * 02111-1307 USA.
 */

#include <firstinclude.h>
#include <pthread.h>
#include <sched.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>	// for EXIT_SUCCESS
#include <unistd.h>	// for sysconf(3)
#include <us_helper.h>	// for CHECK_ZERO_ERRNO()

/*
 * This shows how to create threads with a certain affinity
 *
 * EXTRA_LINK_FLAGS=-lpthread
 */
void print_cpu_set(cpu_set_t *p) {
	fprintf(stderr, "CPU_COUNT is %d\n", CPU_COUNT(p));
	fprintf(stderr, "CPU_SETSIZE is %d\n", CPU_SETSIZE);
	for (int j=0; j<CPU_SETSIZE; j++) {
		if (CPU_ISSET(j, p)) {
			printf("\tCPU %d\n", j);
		}
	}
}

void *worker(void *p) {
	int num=*(int *)p;
	fprintf(stderr, "starting thread %d\n", num);
	fprintf(stderr, "ending thread %d\n", num);
	return(NULL);
}

int main(int argc, char** argv, char** envp) {
	const int cpu_num=sysconf(_SC_NPROCESSORS_ONLN);
	const int num=10;
	pthread_t threads[num];
	pthread_attr_t attrs[num];
	cpu_set_t cpu_sets[num];
	int ids[num];
	void* rets[num];

	TRACE("started");
	for (int i=0; i<num; i++) {
		ids[i]=i;
		CPU_ZERO(cpu_sets + i);
		CPU_SET(i % cpu_num, cpu_sets + i);
		print_cpu_set(cpu_sets + i);
		CHECK_ZERO_ERRNO(pthread_attr_init(attrs + i));
		CHECK_ZERO_ERRNO(pthread_attr_setaffinity_np(attrs + i, sizeof(cpu_set_t), cpu_sets + i));
		CHECK_ZERO_ERRNO(pthread_create(threads + i, attrs + i, worker, ids + i));
	}
	TRACE("created threads");
	for (int i=0; i<num; i++) {
		CHECK_ZERO_ERRNO(pthread_join(threads[i], rets + i));
	}
	TRACE("end");
	return EXIT_SUCCESS;
}
