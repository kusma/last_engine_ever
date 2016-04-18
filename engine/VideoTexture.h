#ifndef VIDEOTEXTURE_H
#define VIDEOTEXTURE_H

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <d3d9.h>
#include <string>

extern IDirect3DDevice9 *device;

#include "Texture.h"
#include <decore.h>

#define MAGIC "KUSMAMPG"

namespace engine {
	class VideoTexture : public Texture {
	public:
		VideoTexture(std::string filename) : Texture(), fp(0), pixeldata(0) {
			fp = file_open(filename.c_str());
			if (!fp) throw std::string("file not found");

			char magic[8];
			file_read((void*)&magic, 1, 8, fp);
			if (memcmp(magic, MAGIC, strlen(MAGIC)) != 0) throw std::string("not a KPG-file.");

			static unsigned handle = 1;
			this->handle = handle;

			file_read((void*)&width, sizeof(unsigned), 1, fp);
			file_read((void*)&height, sizeof(unsigned), 1, fp);
			file_read((void*)&frames, sizeof(unsigned), 1, fp);
			file_read((void*)&fps, sizeof(float), 1, fp);

			DEC_PARAM dec_param;
			DEC_SET dec_set;
			DEC_MEM_REQS dec_mem_reqs;

			dec_param.x_dim = width;
			dec_param.y_dim = height;
			dec_param.output_format = DEC_RGB32;
			dec_param.time_incr = 0;
			dec_set.postproc_level = 0;

			if (decore(handle, DEC_OPT_MEMORY_REQS, &dec_param, &dec_mem_reqs) != DEC_OK) throw std::string("failed to init decoder. (1)");

			dec_param.buffers.mp4_edged_ref_buffers =	malloc(dec_mem_reqs.mp4_edged_ref_buffers_size);
			dec_param.buffers.mp4_edged_for_buffers =	malloc(dec_mem_reqs.mp4_edged_for_buffers_size);
			dec_param.buffers.mp4_display_buffers =		malloc(dec_mem_reqs.mp4_display_buffers_size);
			dec_param.buffers.mp4_state =				malloc(dec_mem_reqs.mp4_state_size);
			dec_param.buffers.mp4_tables =				malloc(dec_mem_reqs.mp4_tables_size);
			dec_param.buffers.mp4_stream =				malloc(dec_mem_reqs.mp4_stream_size);
			memset(dec_param.buffers.mp4_state, 0, dec_mem_reqs.mp4_state_size);
			memset(dec_param.buffers.mp4_tables, 0, dec_mem_reqs.mp4_tables_size);
			memset(dec_param.buffers.mp4_stream, 0, dec_mem_reqs.mp4_stream_size);

			if (decore(handle, DEC_OPT_INIT, &dec_param, NULL) != DEC_OK) throw std::string("failed to init decoder. (2)");
			if (decore(handle, DEC_OPT_SETOUT, &dec_param, NULL) != DEC_OK) throw std::string("failed to init decoder. (3)");
			if (decore(handle, DEC_OPT_SETPP, &dec_set, NULL) != DEC_OK) throw std::string("failed to init decoder. (4)");

			start_offset = file_tell(fp);
			current_frame = 0;

			pixeldata = (unsigned *)malloc(sizeof(unsigned) * width * height);
			if (!pixeldata) throw std::string("out of memory");

			if (device->CreateTexture(width, height, 0, D3DUSAGE_AUTOGENMIPMAP, D3DFMT_X8R8G8B8, D3DPOOL_MANAGED, &texture, 0) != D3D_OK)
				throw std::string("failed to create video-texture.");

			handle++;
		}

		~VideoTexture() {
			if (fp) file_close(fp);
			if (pixeldata) free(pixeldata);
		}

		void update(float time) {
			static unsigned char buffer[1024*1024];
			int size;
			int frame;

			time = float(fmod(time, float(frames) / fps));
			frame = unsigned(time * fps);

			if (frame == current_frame ) return;


			while (frame != current_frame) {
				if (current_frame > frames) {
					file_seek(fp, start_offset, SEEK_SET);
					current_frame = 0;
				} else current_frame++;

				file_read(&size, sizeof(int), 1, fp);
				file_read(buffer, size, 1, fp);

				DEC_FRAME dec_frame;
				dec_frame.bmp = pixeldata;
				dec_frame.bitstream = buffer;
				dec_frame.length = size;
				dec_frame.render_flag = 1;

				if (decore(handle, 0, &dec_frame, 0) != DEC_OK) throw std::string("error in stream!!");
			}

			D3DLOCKED_RECT rect;
			if (texture->LockRect(0, &rect, 0, 0) == D3D_OK) {
				int counter;
				int w, p;
				char *source;
				char *dest;

				source = (char*)pixeldata;
				dest = (char*)rect.pBits;

				w = width * sizeof(unsigned);
				p = rect.Pitch;

				counter = height;
				while (counter--) {
					memcpy(dest, source, w);
					source += w;
					dest += p;
				}

				texture->UnlockRect(0);
				texture->GenerateMipSubLevels();
			}

		}

	private:
		file *fp;
		unsigned handle;

		unsigned width, height;
		unsigned frames;

		unsigned current_frame;
		unsigned start_offset;

		unsigned *pixeldata;

		float fps;
	};
}

#undef MAGIC

#endif // RENDERTEXTURE_H
