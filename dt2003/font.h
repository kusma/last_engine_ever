#ifndef _FONT_H
#define _FONT_H

#define FONTWD 461
#define FONTHT 10
#define FONTSIZE FONTWD*FONTHT
#define FONTCHARCOUNT 26*2 + 10 + 4
#define FONTSPACESIZE 6
#define FONTCHARWD 6

const unsigned char alphabet[FONTCHARCOUNT+1] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789-_=+";

struct FontChar {
	FontChar(int wd, int ht) {
		this->size = wd*ht;
		this->wd = wd;
		this->ht = ht;
		this->pixels = new unsigned char[size];
	}
	~FontChar() {
		delete[] pixels;
	}
	int wd;
	int ht;
	int size;
	unsigned char *pixels;
};

class Font {
	char fn[32];
	unsigned int *screen;
	FontChar *font[FONTCHARCOUNT];
	short charmap[256]; 
public:
	unsigned int colour;

	Font(unsigned int *ascreen, char *fn);
	~Font();
	void load();
	void textout(char* text, int x, int y);
};

#endif //ndef _FONT_H