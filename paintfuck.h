#pragma once

#include "engine/RenderTexture.h"

/* w & h are const in .cpp.
must be the same as the rendertexture */

/* example:
 	engine::RenderTexture render_texture(64, 64);
	InitPaintfuck();


	...

	loop
	{

		... draw scene in rendertexture ...

		DrawPaintfuck(render_texture); 
	}

	FreePaintfuck();

*/

void InitPaintfuck();

void DrawPaintfuck(engine::RenderTexture* renderTexture); // renderTexture to copy
void FreePaintfuck();