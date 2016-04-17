#ifndef _LINEAR_CURVE_H_
#define _LINEAR_CURVE_H_

#include "Curve.h"

namespace engine {
	template <typename T>
	class LinearKey {
	public:
		LinearKey() {}
		LinearKey(const float time, T data) : time(time), data(data) {}

		float time;
		T data;
	};

	template <typename T>
	class LinearCurve : public Curve<T, LinearKey<T> > {
	public:
		LinearCurve(std::vector<LinearKey<T> > &input) : Curve(input) {};
		LinearCurve(LinearKey<T> *input, unsigned count) : Curve<T, LinearKey<T> >(input, count) {};

		T get_value(float time) {
			if (keys.size() < 2) {
				if (keys.size() == 0) throw std::string("not enough keys");
				else return keys[0].data;
			}

			unsigned first_key = get_key_number(time);
			unsigned next_key = first_key + 1;
			if (next_key > unsigned(keys.size())) next_key = 0;

			T left = keys[first_key].data;
			T right = keys[next_key].data;
			float left_time = keys[first_key].time;
			float right_time = keys[next_key].time;

			float len = right_time - left_time;
			if (fabs(len) < 0.00001f) return left;
			float t = (time - left_time) / len;

			return left + (right - left) * t;
		}
	};
}

#endif // _LINEAR_CURVE_H_
