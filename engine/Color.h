#ifndef COLOR_H
#define COLOR_H

namespace engine {

	class Color {
	public:
		Color(){}
		Color( float r, float g, float b, float a=1 ) : r(r), g(g), b(b), a(a) {}

		unsigned to_int() {
			float ca = max(min(a, 1.f), 0.f);
			float cr = max(min(r, 1.f), 0.f);
			float cg = max(min(g, 1.f), 0.f);
			float cb = max(min(b, 1.f), 0.f);
			return (unsigned(ca * 255) << 24) | (unsigned(cr * 255) << 16) | (unsigned(cg * 255) << 8) | unsigned(cb * 255);
		}

		float r, g, b, a;
	};

}

#endif //COLOR_H
