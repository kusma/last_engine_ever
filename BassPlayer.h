#ifndef BASSPLAYER_H
#define BASSPLAYER_H

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <string>

#include "TimeProvider.h"
#include "file/file.h"
#include "bass.h"

class BassPlayer : public TimeProvider {
public:
	BassPlayer(HWND win);
	~BassPlayer();

	void load( const std::string filename, double offset = 0.0 );
	void play();

	void update_spectrum();
	float sample_spectrum(float pos) {
		pos = min(max(pos, 0), 1);
		return spectrum[unsigned(pos * 255)];
	}

	double get_time();

private:
	file *fp;
	HSTREAM music_file;
	float spectrum[256];
	double offset;
};


#endif
