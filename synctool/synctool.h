#pragma once

#include <string>
#include <vector>

#include "bassplayer.h"
#include "engine/curve.h"

#ifdef SYNCTOOL
#include "synctoolwindow.h"
#endif

namespace sync {
	class Track {
	public:
#ifdef SYNCTOOL
		std::string name;
#endif
		engine::Curve<float, int> curve;
	};

	class Synctool {
	public:

		Synctool(std::string filename, BassPlayer *bassplayer);
		void pause();

	private:
		::BassPlayer *bassplayer;
		std::vector<Track> tracks;
#ifdef SYNCTOOL
		SynctoolWindow sync_win;
#endif
	};

}
