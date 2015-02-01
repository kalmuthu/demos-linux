/*
 * This file is part of the linuxapi package.
 * Copyright (C) 2011-2014 Mark Veltzer <mark.veltzer@gmail.com>
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
#include <stdio.h>	// for printf(3)
#include <stdlib.h>	// for EXIT_SUCCESS
#include <endian.h>	// for __BYTE_ORDER
#include <us_helper.h>	// for stringify()

/*
 * This example tells you on what type of endian machine you are.
 * This relies on a header file endian.h which provides the __BYTE_ORDER
 * macro.
 *
 * References:
 * http://stackoverflow.com/questions/2100331/c-macro-definition-to-determine-big-endian-or-little-endian-machine
 */

int main(int argc, char** argv, char** envp) {
	printf("__BYTE_ORDER is [%s]\n", stringify(__BYTE_ORDER));
#if __BYTE_ORDER == __LITTLE_ENDIAN
	printf("your system is __LITTLE_ENDIAN\n");
#elif __BYTE_ORDER == __BIG_ENDIAN
	printf("your system is __BIG_ENDIAN\n");
#else
#error "This is a weird system you have"
#endif
	return EXIT_SUCCESS;
}
