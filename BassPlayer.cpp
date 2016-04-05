#include "BassPlayer.h"

BassPlayer::BassPlayer(HWND win) : music_file(0) {
	if (!BASS_Init(1, 44100, BASS_DEVICE_LATENCY, win, NULL)) throw std::string("could not init bass");
}

BassPlayer::~BassPlayer() {
	if (music_file) BASS_StreamFree( music_file );
	if (fp) file_close( fp );
	BASS_Free();
}

void BassPlayer::load(const std::string filename, double offset) {
	try {
		fp = file_open(filename.c_str());
		if (!fp) throw std::string("file not found");

#ifndef NDEBUG
		music_file = BASS_StreamCreateFile(1, fp->data, 0, fp->size, offset != 0.0 ? BASS_MP3_SETPOS : 0);
#else
		music_file = BASS_StreamCreateFile(1, fp->data, 0, fp->size, BASS_MP3_SETPOS);
#endif
		if (!music_file) {
			unsigned int error = BASS_ErrorGetCode();
			switch (error) {
			case BASS_ERROR_INIT:
				throw std::string("BASS_Init has not been successfully called. This should not happen at all.");
			case BASS_ERROR_NOTAVAIL:
				throw std::string("Only decoding streams (BASS_STREAM_DECODE) are allowed when using the \"no sound\" device.");
			case BASS_ERROR_ILLPARAM:
				throw std::string("Zero-length music file. File is proabelby corrupted.");
			case BASS_ERROR_FILEFORM:
				throw std::string("The file's format is not recognised/supported.");
			case BASS_ERROR_FORMAT:
				throw std::string("The sample format is not supported by the device/drivers. If the stream is more than stereo or the BASS_SAMPLE_FLOAT flag is used, it could be that they are not supported (ie. no WDM drivers).");
			case BASS_ERROR_SPEAKER:
				throw std::string("The device/drivers do not support the requested speaker(s), or you're attempting to assign a stereo stream to a mono speaker.");
			case BASS_ERROR_MEM:
				throw std::string("Insufficent memory.");
			case BASS_ERROR_NO3D:
				throw std::string("Couldn't initialize 3D support for the stream.");
			case BASS_ERROR_UNKNOWN:
			default:
				throw std::string("Unknown or undocumented BASS-error code");
			}
		}

		this->offset = offset;
	} catch (const std::string reason) {
		throw std::string("Failed to load ")+filename+std::string("\n")+reason;
	} catch(...) {
		throw std::string("Failed to load ")+filename+std::string("\nUnknown error");
	}
}

void BassPlayer::play() {
	if (!music_file) throw std::string("no music file loaded");
	BASS_Start();
	BASS_StreamPlay(music_file, 1, 0);
	if (offset != 0.0) BASS_ChannelSetPosition(music_file, unsigned(44100 * 2 * 2 * offset));
}

void BassPlayer::update_spectrum() {
	BASS_ChannelGetData(music_file, spectrum, BASS_DATA_FFT512);
}

double BassPlayer::get_time() {
	long long bytes_played = BASS_ChannelGetPosition(music_file);
	return bytes_played * (1.0 / (44100 * 2 * 2));
}
