#include "paintfuck.h"

#include "engine/Vector.h"
#include "engine/Matrix.h"
#include "engine/Texture.h"
#include "engine/RenderTarget.h"
#include "engine/RenderTexture.h"
#include "engine/Image.h"
#include "engine/TextureProxy.h"
#include <d3dx9math.h>

const int w = 64;
const int h = 64;

struct brush
{
	char* filename;
	engine::Texture* texture;
	engine::Image* image;
};

/* how i made these fucking brushes for ugly rendering(tm) :
	10 draw a fucking crap on photoshop
	20 apply watercolor
	30 goto 10
	*/
static brush brushes[] = {
/*	{ "spot.png", NULL, NULL }, */
	{ "spot1.png", NULL, NULL },
	{ "spot2.png", NULL, NULL },
	{ "spot3.png", NULL, NULL },
	{ "spot4.png", NULL, NULL },
	{ NULL, NULL, NULL } };
static int* brushesTab;
static float* anglesTab;

static int numBrushes = -1;
static engine::RenderTexture* dataTexture;
static engine::TextureProxy* texture_proxy;

void InitPaintfuck()
{
	texture_proxy = new engine::TextureProxy();
	while (brushes[++numBrushes].filename != NULL);

	for (int brush = 0; brush < numBrushes; brush++)
	{
		brushes[brush].texture = texture_proxy->get_resource(brushes[brush].filename);
		brushes[brush].image = new engine::Image(brushes[brush].texture, 3.0f);
	}

	brushesTab = new int[w*h];
	anglesTab = new float[w*h];

	for (int i = 0; i < w*h;i++)
	{
		brushesTab[i] = rand()%numBrushes;
		anglesTab[i] = 3.14159f*2*(rand()%360)/360.f;
	}

	// ok i shouldn't have used a rendertexture here maybe
	// but since it had the surface locked already..
	dataTexture = new engine::RenderTexture(w, h, 0, D3DPOOL_SYSTEMMEM);
}

void FreePaintfuck()
{
	delete dataTexture;
	delete brushesTab;
	delete anglesTab;
	for (int i = 0; i < numBrushes; i++)
		delete brushes[i].image;
	delete texture_proxy;
}

void DrawPaintfuck(engine::RenderTexture* renderTexture)
{
		D3DLOCKED_RECT rectum;

		device->GetRenderTargetData(renderTexture->get_surface(), dataTexture->get_surface());
		dataTexture->get_surface()->LockRect(&rectum, NULL, D3DLOCK_READONLY);

		device->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
		device->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TFACTOR);
		device->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_TEXTURE);
		device->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_MIRROR);
		device->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_MIRROR);
		device->SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_DISABLE);

		device->SetRenderState(D3DRS_TEXTUREFACTOR, 0xffffffff);
		device->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);

		device->SetRenderState(D3DRS_COLORVERTEX, true);


		engine::Matrix identity;
		identity.identity();

		D3DXMATRIX projection;
		D3DXMatrixOrthoLH(&projection, w, h, -1, 1);
		device->SetTransform(D3DTS_PROJECTION, &projection);
		engine::Matrix view;
		view.identity();
		float s = 1.0f;
		view.scale(engine::Vector(s,s,s));
		device->SetTransform(D3DTS_VIEW, (D3DMATRIX*)&identity);
		device->SetTransform(D3DTS_WORLD, (D3DMATRIX*)&identity);
		device->SetRenderState(D3DRS_ZWRITEENABLE, false);
		device->SetRenderState(D3DRS_ZENABLE, false);

		device->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

		int srcblend = D3DBLEND_SRCALPHA;
		int dstblend = D3DBLEND_INVSRCCOLOR;
		device->SetRenderState(D3DRS_ALPHABLENDENABLE, true);
		device->SetRenderState(D3DRS_SRCBLEND, srcblend);
		device->SetRenderState(D3DRS_DESTBLEND, dstblend);

		device->SetFVF(VERTEX2D_TYPE);
		device->SetStreamSource(0, brushes[0].image->get_vertex_buffer(), 0, sizeof(engine::Vertex2D));

		int* buf = (int*)rectum.pBits;

		D3DXMATRIX zeroTex, resetTex, rotateTex;
		D3DXMatrixIdentity(&zeroTex);
		D3DXMatrixIdentity(&resetTex);
		D3DMATRIX* zTex=(D3DMATRIX*)&zeroTex;
		D3DMATRIX* rTex=(D3DMATRIX*)&resetTex;
		zTex->_31 = -0.5f;
		zTex->_32 = -0.5f;
		rTex->_31 = 0.5f;
		rTex->_32 = 0.5f;

		static float fff=0;
	//	fff+=0.0333f;
		float space=2.f;
		engine::Matrix tex;
		const int step = 2;
		for (int x = 0; x < w; x+=step) {
			for (int y = 0; y < h; y+=step) {
				int r = x+(w-y)*w;
				if (buf[r] != 0) {
					float ang=anglesTab[r];
					D3DXMatrixRotationZ(&rotateTex, ang+fff);
					rotateTex = zeroTex * rotateTex * resetTex;

					device->SetTextureStageState( 0, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_COUNT2 );
					device->SetTransform(D3DTS_TEXTURE0, &rotateTex);

					identity.identity();
					identity.translate(engine::Vector((x-w/2)/s, (y-h/2)/s, 0));

					device->SetRenderState(D3DRS_TEXTUREFACTOR, buf[r] | 0xff000000);
					device->SetTexture(0, brushes[brushesTab[r]].texture->texture);

					device->SetTransform(D3DTS_WORLD, (D3DMATRIX*)&identity);
					device->DrawPrimitive(D3DPT_TRIANGLEFAN, 0, 2);
				}
			}
		}
		device->SetTextureStageState( 0, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_DISABLE );

		dataTexture->get_surface()->UnlockRect();
		device->SetRenderState(D3DRS_ALPHABLENDENABLE, false);

		device->SetRenderState(D3DRS_ZWRITEENABLE, true);
		device->SetRenderState(D3DRS_ZENABLE, true);
}