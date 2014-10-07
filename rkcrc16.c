/*-
 * Copyright (c) 2014 FUKAUMI Naoki.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <sys/mman.h>
#include <err.h>
#include <fcntl.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "rkcrc.h"

int
main(int argc, char *argv[])
{
	uint8_t *buf, tail[2];
	off_t size;
	int in, out;
	uint16_t crc;

	if (argc != 3) {
		fprintf(stderr, "usage: %s infile outfile\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	if ((in = open(argv[1], O_RDONLY)) == -1)
		err(EXIT_FAILURE, "%s", argv[1]);

	if ((size = lseek(in, 0, SEEK_END)) == -1)
		err(EXIT_FAILURE, "%s", argv[1]);

	if ((buf = mmap(NULL, size, PROT_READ, MAP_SHARED | MAP_FILE, in, 0))
	    == MAP_FAILED)
		err(EXIT_FAILURE, "%s", argv[1]);

	if ((out = open(argv[2], O_WRONLY | O_CREAT | O_TRUNC, 0644)) == -1)
		err(EXIT_FAILURE, "%s", argv[2]);

	crc = 0xffff;
	crc = rkcrc16(crc, buf, size);
	tail[0] = crc >> 8;
	tail[1] = crc & 0xff;

	write(out, buf, size);
	write(out, tail, sizeof(tail));

	close(out);
	close(in);

	return EXIT_SUCCESS;
}
