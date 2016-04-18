#ifndef RESOURCE_PROXY_H
#define RESOURCE_PROXY_H

#include <map>
#include <string>
#include <assert.h>

namespace engine {

	template<typename T>
	class ResourceProxy {
	public:
		~ResourceProxy() {
			std::map<T*, int>::iterator iter = ref_count.begin();
			for (; iter!=ref_count.end(); iter++) {
				T*	obj   = iter->first;
				int count = iter->second;
				for (int i=0; i<count; i++) release(obj);
			}
		}

		T* get_resource(std::string filename) {
			T* obj = filename_map_static[filename];

			if (obj != NULL && ref_count_static[obj] > 0) {
				add_ref(obj);
			} else {
				try {
					obj = read_from_file(filename);
				} catch (std::string reason) {
					throw std::string("Failed to load ")+filename+std::string("\n")+reason;
				}
				if (obj) insert_resource(filename, obj);
			}
			return obj;
		}

		void insert_resource(std::string filename, T *t) {
			filename_map_static[filename] = t;
			add_ref(t);
		}

		void release(T* obj) {
			assert(ref_count[obj] > 0);
			assert(ref_count_static[obj] > 0);
			--ref_count[obj];
			if (--ref_count_static[obj] == 0) delete obj;
		}

		void add_ref(T* obj) {
			ref_count[obj]++;
			ref_count_static[obj]++;
		}

	private:
		virtual T* read_from_file(std::string filename) = 0;
		std::map<T*, int> ref_count;
		static std::map<T*, int> ref_count_static;
		static std::map<std::string, T*> filename_map_static;
	};

}

#endif // RESOURCE_PROXY_H
