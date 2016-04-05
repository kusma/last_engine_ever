#include "std.h"


//guess it could be faster. could be a lot slower too, though.
void box(int x1, int y1, int x2, int y2, unsigned int colour, unsigned int *pixels) {
	int j = y1*WINWD;
	int i1, i2;
	for(;y1<y2;y1++,j+=WINWD) {
		for(i1=j+x1,i2=j+x2;i1<i2;i1++) pixels[i1] = colour;
	}
}