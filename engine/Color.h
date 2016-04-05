#ifndef COLOR_H
#define COLOR_H

namespace engine {

	class Color {
	public:
		Color(){}
		Color( float r, float g, float b, float a=1 ) : r(r), g(g), b(b), a(a) {}
		float r, g, b, a;
	};

}

#endif //COLOR_H
