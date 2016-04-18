#pragma once

#include <vector>

template <typename T>
class Curve {
public:
	Curve() {};
	~Curve() {};

	void add_key(const float time, const T &value, const float t = 0, const float c = 0, const float b = 0);
	void calculate_tangents();
	T get_val(float time);

private:
	unsigned get_key_number(const float time) {
		unsigned size = unsigned(keys.size());
		for (unsigned i = 0; i < size; i++) {
			unsigned next = i + 1;
			if (next >= size) next = 0;
			if ((keys[i].time <= time) && (keys[next].time > time)) return i;
		}
		return 0;
	}

	class Key {
	public:
		Key(const float time, const T value, const float t = 0.f, const float c = 0.f, const float b = 0.f) : time(time), value(value), t(t), c(c), b(b) {}
		T interpolate(const Key &k, const float t);

		float time;
		T value, out_tangent, in_tangent;
		float t, c, b;
	};

	std::vector<Key> keys;
};

template <typename T>
void Curve<T>::add_key(const float time, const T &value, const float t, const float c, const float b) {
	keys.push_back(Key(time, value, t, c, b));
}

template <typename T>
void Curve<T>::calculate_tangents() {
	unsigned keys_size = unsigned(keys.size());
	if (keys_size < 2) return;

	// work on a pr segment basis
	for (unsigned i=0; i < keys_size - 1; i++) {
		Key &left = keys[i];
		Key &right = keys[i + 1];

		const float len = right.time - left.time;
		T d = right.value - left.value;

		float out_a = (1.f - left.t) * (1.f + left.c) * (1.f + left.b);
		float out_b = (1.f - left.t) * (1.f - left.c) * (1.f - left.b);

		if (i > 0) {
			const Key &prev = keys[i - 1];
			const float t = len / (right.time - prev.time);
			left.out_tangent = ((left.value - prev.value) * out_a + d * out_b) * t;
		} else left.out_tangent = d * out_b;


		float in_a = (1.f - left.t) * (1.f - left.c) * (1.f + left.b);
		float in_b = (1.f - left.t) * (1.f + left.c) * (1.f - left.b);

		if (i < keys_size - 2) {
			const Key &next = keys[i + 2];
			const float t = len / (next.time - left.time);
			right.in_tangent = ((next.value - right.value) * in_b + d * in_a) * t;
		} else right.in_tangent = d * in_a;
	}
}

template <typename T>
T Curve<T>::get_val(float time) {
	const unsigned key_count = unsigned(keys.size());

//	if (key_count == 0) return 0;
	if (key_count == 1) return keys[0].value; // if constant value

	unsigned first_key = get_key_number(time);
	unsigned next_key = first_key + 1;

	Key &left = keys[first_key];
	const Key &right = keys[next_key];

	if (time < left.time) return left.value;
	if (time > right.time) return keys[keys.size() - 1].value;

	float len = right.time - left.time;
	float t = (time - left.time) / len;

	return left.interpolate(right, t);
}

template <typename T>
T Curve<T>::Key::interpolate(const Key &k, const float t) {
	float t2 = t * t;
	float t3 = t * t2;
	float h2 = 3.f * t2 - t3 - t3;
	float h1 = 1.f - h2;
	float h4 = t3 - t2;
	float h3 = h4 - t2 + t;
	return value * h1 + k.value * h2 + out_tangent * h3 + k.in_tangent * h4;
}
