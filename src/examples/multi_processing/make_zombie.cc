/*
 * This file is part of the linuxapi package.
 * Copyright (C) 2011-2013 Mark Veltzer <mark.veltzer@gmail.com>
 *
 * linuxapi is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * linuxapi is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with linuxapi. If not, see <http://www.gnu.org/licenses/>.
 */

#include <firstinclude.h>
#include <unistd.h>	// for fork(2), getpid(2), sleep(3), pipe(2)
#include <stdio.h>	// for printf(3)
#include <sys/types.h>	// for waitid(2), getpid(2)
#include <sys/wait.h>	// for waitid(2)
#include <stdlib.h>	// for exit(3)
#include <string.h>	// for strsignal(3)
#include <proc/readproc.h>	// for get_proc_stats(3)
#include <us_helper.h>	// for CHECK_ZERO(), CHECK_NOT_M1(), TRACE(), CHECK_1()
#include <multiproc_utils.h>	// for print_code(), print_status()
#include <process_utils.h>	// for set_process_name()

/*
 * This example demostrates how processes become zombies in Linux...
 *
 * How does a process become a zombie in Linux ?
 * - The parent creates the process.
 * - The process ends.
 * - The parent does not collect the processes return code using one of
 * the wait(2) family of functions.
 *
 * Note that if the parent dies then init(1) will actually serve as the
 * new parent for the child process (sort of adoption) and will clear the
 * child. So a zombie is always a process with a live parent (unless there
 * is a bug in init(1), ofcourse...).
 *
 * We show the status of the child in this application in two ways:
 * - using the ps command.
 * - using the libproc library.
 * - catting
 *
 * From the command line you can also see info about zombies using:
 * - ps(1) (done in this example).
 * - catting files from the /proc folder (done in two ways in this example).
 * - top(1)
 *
 * EXTRA_LINK_CMDS=pkg-config --libs libprocps
 */

// print the state of a process in 3 different ways...
static inline void print_state(pid_t pid) {
	my_system("ps --no-headers -o comm,state %d", pid);
	my_system("cat /proc/%d/status | grep State", pid);
	// the function get_proc_stats is declared by the procps headers
	// but does not exist in the procps shared object...
	/*
	 * proc_t myproc;
	 * get_proc_stats(pid,&myproc);
	 * printf("pid is %d, state is %c\n",pid, myproc.state);
	 */
}

int main(int argc, char** argv, char** envp) {
	int pipefd_c2p[2];
	CHECK_ZERO(pipe(pipefd_c2p));
	int pipefd_p2c[2];
	CHECK_ZERO(pipe(pipefd_p2c));

	pid_t child_pid=CHECK_NOT_M1(fork());
	// the child
	if (child_pid==0) {
		process_set_name("child");
		TRACE("this is the child");
		CHECK_ZERO(close(pipefd_c2p[0]));
		CHECK_ZERO(close(pipefd_p2c[1]));
		// tell the parent I am ready
		CHECK_1(write(pipefd_c2p[1], "r", 1));	// r is for ready
		// lets wait for the parent to tell us to die...
		char c;
		CHECK_1(read(pipefd_p2c[0], &c, 1));
		// now let the child die in order to become a zombie...
		return 0;
		// the parent
	} else {
		process_set_name("parent");
		TRACE("this is the parent");
		CHECK_ZERO(close(pipefd_c2p[1]));
		CHECK_ZERO(close(pipefd_p2c[0]));
		// wait for the child to become ready...
		char c;
		CHECK_1(read(pipefd_c2p[0], &c, 1));
		// print the state of the child (in 3 different ways...)
		print_state(child_pid);
		// tell the child it is ok to die...
		CHECK_1(write(pipefd_p2c[1], "d", 1));	// d is for die
		TRACE("going to sleep so that the child would become a zombie");
		TRACE("you can now see the child zombie on the command line");
		TRACE("using tools like top, ps, /proc and more");
		CHECK_ZERO(sleep(100));
		// print the state of the child (now he should be a zombie...).
		print_state(child_pid);
		// now lets take the return code from the child...
		siginfo_t info;
		CHECK_NOT_M1(waitid(P_PID, child_pid, &info, WEXITED | WSTOPPED | WCONTINUED));
		print_code(info.si_code);
		print_status(info.si_status);
	}
	return EXIT_SUCCESS;
}
