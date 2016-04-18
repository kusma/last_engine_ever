#ifndef _CURVE_H_
#define _CURVE_H_

#include <vector>

namespace engine {
	template <typename T, typename KeyType>
	class Curve {
	protected:
		Curve() {};
	public:
		Curve(std::vector<KeyType> &input) : keys(input) {};
		Curve(const KeyType *input, unsigned count) : keys(input, input + count) {};
		~Curve(){};

		void add_key(const float time, T key) {
			keys.push_back( KeyType( time, key ) );
		}

		T get_value(float time);

	protected:
		// find the key-number for a given time, optimize this one later!!!
		// (remembering last used key should be sufficient)
		int get_key_number(const float time) {
			int size = (int)keys.size();
			for (int i = 0; i < size; i++) {
				int next = i + 1;
				if (next > size) next = 0;
				if ((keys[i].time <= time) && (keys[next].time > time)) return i;
			}
			return 0;
		}
		std::vector<KeyType> keys;
	};
}

#endif // _CURVE_H_