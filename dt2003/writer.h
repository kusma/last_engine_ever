#ifndef WRITER_H
#define WRITER_H

#include "track.h"

struct Header {
	unsigned int ptncount;
	unsigned int trackcount;
	unsigned int maxptnlen;
	unsigned int totalrowcount;
	int noteoffs[MAXTRACKS];
};

bool writetd(char* fn, CTracks* t);
bool readtd(char* fn, CTracks* t);

#endif // WRITER_H