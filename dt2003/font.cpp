#include "std.h"
#include "font.h"

Font::Font(unsigned int *ascreen, char *afn) {
	screen = ascreen;
	strcpy(this->fn,afn);
	ZeroMemory(font,FONTCHARCOUNT*sizeof(FontChar *));
	colour = 0xFF00FF;
}

Font::~Font() {
	for(int i=0;i<FONTCHARCOUNT;i++) {
		if (font[i]) delete font[i];
	}
}

void Font::load() {
	int i=0;
	FILE *fp = fopen(fn,"rb");

	//read character start positions
	int *fontstart = new int[FONTCHARCOUNT+1]; //+1 for imaginary last char.
	unsigned char idx = 0;
	for(i=0;i<FONTWD;i++) {
		unsigned char moo; 
		fread(&moo,sizeof(unsigned char),1,fp);
		if(moo) {
			fontstart[idx] = i;
			idx++;
		}
	}

	//read characters into memory
	unsigned char* pixels = new unsigned char[FONTSIZE];
	for(i=0;i<FONTSIZE;i++) pixels[i] = 0;
	int banana = FONTSIZE;
	fread(pixels,sizeof(unsigned char),FONTSIZE,fp);

	fontstart[idx] = FONTWD + 1;

	/*for(i=0;i<2710;i++) {
		fread(pxls+i,1,1,fp);
	}*/
	for(i=0;i<idx;i++) {
		int wd = fontstart[i+1]-fontstart[i];
		font[i] = new FontChar(wd,FONTHT);
		for(int x=0;x<wd;x++) {
			int realx=x+fontstart[i];
			for(int y=0;y<FONTHT;y++) {
				font[i]->pixels[x+y*wd] = pixels[realx+y*FONTWD];
			}
		}
	}

	//make charactermap
	for(i=0;i<256;i++) charmap[i] = -1; //-1 means space.
	for(i=0;i<FONTCHARCOUNT;i++) {
		charmap[alphabet[i]] = i; //charmap['a'] will now tell the char index of 'a'.
	}
	delete[] fontstart;
	delete[] pixels;
}

void Font::textout(char* text, int x, int y) {
	int len = lstrlen(text);
	int xoffset = x;
	int yoffset;
	for(int pos=0;pos<len;pos++) {
		char chr = text[pos];
		int idx = charmap[chr];
		int prevxoff = xoffset;
		if(idx>=0) for(int _x=0;_x<font[idx]->wd;_x++,xoffset++) {
			yoffset = y;
			xoffset %= WINWD;
			for(int _y=0;_y<font[idx]->ht;_y++,yoffset++) {
				yoffset %= WINWD;
				unsigned char c = font[idx]->pixels[_x+_y*font[idx]->wd];

				if(c) {
					screen[xoffset+yoffset*WINWD] = colour; 
				}
			}
		} else {
			xoffset += FONTCHARWD; //unknown char adds a space.
		}
		xoffset = prevxoff+FONTCHARWD;
	}
}
