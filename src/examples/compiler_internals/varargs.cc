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

#include<firstinclude.h>
#include<stdio.h> // for vprintf(3)
#include<stdlib.h> // for EXIT_SUCCESS

/*
* This example shows how to use the __builtin_ varargs
* of gcc...
*
* More info could be gotten about these builtins using 'info gcc'.
*
* Notes:
* - this is not portable because other compilers do not have these
* '__builtin_*' macros. If you want something portable use the 'stdarg'
* header and it's macros which are provided by the libc library.
* - We don't need any special header files (we are using compiler built ins
* which are something akin to "sizeof").
* - We use the __attribute__ feature to make sure the compiler checks that
* we are passing the arguments right (gcc feature).
* - Notice we can only put the __attribute__ feature on function declaration
* (that is why we have the funny declaration after the definition).
* - these are all *compiler builtins*. They are not taken from any header
* files. Compare this to using the glibc wrappers in a similar example
* under glibc.
*/

int trace(const char *fmt, ...) {
	extern char *program_invocation_short_name;

	printf("%s: ", program_invocation_short_name);
	__builtin_va_list args;
	__builtin_va_start(args, fmt);
	int ret;
	ret = vprintf(fmt, args);
	__builtin_va_end(args);
	return(ret);
}

int trace(const char *fmt, ...) __attribute__((format(printf, 1, 2)));

int main(int argc,char** argv,char** envp) {
	printf("sizeof(__builtin_va_list) is [%d]\n",sizeof(__builtin_va_list));
	trace("%s %d %f\n", "Hello", 5, 3.14);
	// the next line will produce a compile time error (passing int as string...)
	//trace("%s %s %f\n","Hello",5,3.14);
	return EXIT_SUCCESS;
}
