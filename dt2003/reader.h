#ifndef READER_H
#define READER_H

#include "writer.h"
#include <vector>

class Syncreader {
protected:
	Header fh;
	
	std::vector<std::vector<bool> > interp;
	std::vector<std::vector<float> > data;
	std::vector<std::vector<int> > idata;
	std::vector<std::vector<bool> > trig;
	
	float frow;
	float drow;
	int irow;
	int lasttrig[TRACKCOUNT];

	bool read(char* fn);
	bool fill();
public:

	Syncreader();
	~Syncreader();

	bool load();
	float get(Tracks t);
	bool getbool(Tracks t);
	bool get_trig(Tracks t);
	void update(float row);
	void setrow(int row) { irow = frow = row; drow = 0.0f; };
};

#endif //READER_H