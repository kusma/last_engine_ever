#ifndef UV_H
#define UV_H

namespace engine {

	class Uv {
	public:
		Uv() {}
		Uv( float u, float v ) : u(u), v(v) {}
		float u, v;
	};

}

#endif // UV_H
