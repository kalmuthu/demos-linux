/*
	This file is part of the linuxapi project.
	Copyright (C) 2011, 2012 Mark Veltzer <mark.veltzer@gmail.com>

	The linuxapi package is free software; you can redistribute it and/or
	modify it under the terms of the GNU Lesser General Public
	License as published by the Free Software Foundation; either
	version 2.1 of the License, or (at your option) any later version.

	The linuxapi package is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
	Lesser General Public License for more details.

	You should have received a copy of the GNU Lesser General Public
	License along with the GNU C Library; if not, write to the Free
	Software Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA
	02111-1307 USA.
*/

#include<stdio.h> // for printf(3), fflush(3), fgets(3), perror(3)
#include<sys/types.h> // for open(2)
#include<sys/stat.h> // for open(2)
#include<fcntl.h> // for open(2)
#include<stdlib.h> // for atoi(3), EXIT_FAILURE, EXIT_SUCCESS, exit(3)
#include<unistd.h> // for close(3)
#include<sys/ioctl.h> // for ioctl(2)

#include<us_helper.h> // our own helper

#include"shared.h" // for ioctl numbers

/*
 * A basic test which opens the device, runs one ioctl with 0 or user given
 * cmd and no data and then closes the device
 */
int main(int argc, char **argv, char **envp) {
	// file to be used
	const char *filename = "/dev/demo";
	// file descriptor
	int fd;
	// size of strings
	const unsigned int len = 256;
	// two strings to get the data
	char s1[len];
	char s2[len];

	CHECK_NOT_M1(fd = open(filename, O_RDWR));
	while(true) {
		// read two numbers from the user
		printf("Please enter a number 1:");
		fflush(stdout);
		if (fgets(s1, len, stdin) != s1) {
			perror("problem with fgets");
			exit(EXIT_FAILURE);
		}
		printf("Please enter a number 2:");
		fflush(stdout);
		if (fgets(s2, len, stdin) != s2) {
			perror("problem with fgets");
			exit(EXIT_FAILURE);
		}
		buffer b;
		b.u1 = atoi(s1);
		b.u2 = atoi(s2);
		b.d1 = atoi(s1);
		b.d2 = atoi(s2);
		klog_clear();
		CHECK_NOT_M1(ioctl(fd, IOCTL_DIV_DOOPS, &b));
		klog_show_clear();
		printf("unsigned long long results:\n");
		printf("div is %llu - should be %llu\n", b.udiv, b.u1 / b.u2);
		printf("mul is %llu - should be %llu\n", b.umul, b.u1 * b.u2);
		printf("add is %llu - should be %llu\n", b.uadd, b.u1 + b.u2);
		printf("sub is %llu - should be %llu\n", b.usub, b.u1 - b.u2);
		printf("long long results:\n");
		printf("div is %lld - should be %lld\n", b.ddiv, b.u1 / b.u2);
		printf("mul is %lld - should be %lld\n", b.dmul, b.u1 * b.u2);
		printf("add is %lld - should be %lld\n", b.dadd, b.u1 + b.u2);
		printf("sub is %lld - should be %lld\n", b.dsub, b.u1 - b.u2);
	}
	CHECK_NOT_M1(close(fd));
	return EXIT_SUCCESS;
}
