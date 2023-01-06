// High Res Textures Plugin

#include "PlatformHeaders.h"
#include "Platform.h"
#include "mathlib.h"
#include "vector.h"
#include "cdll_int.h"
#include "../cl_hook/funcptrs.h"
#include "Exports.h"

#include "cvardef.h"

#include "Utils.hpp"
#include "MinHook.h"
#include "reGS_patterns.hpp"

#include "SDL2/SDL_opengl.h"

extern "C" {
void DLLEXPORT InitializePlugin(cl_enginefunc_t* pEnginefuncs, dllfuncs_t* clfuncs, int iVersion);
void DLLEXPORT HUD_Shutdown_Pre();
void DLLEXPORT HUD_Init_Post();
}

Utils utils = Utils::Utils(NULL, NULL, NULL);

#define TEX_TYPE_NONE 0
#define TEX_TYPE_ALPHA 1
#define TEX_TYPE_LUM 2
#define TEX_TYPE_ALPHA_GRADIENT 3
#define TEX_TYPE_RGBA 4
#define TEX_IS_ALPHA(type) ((type) == TEX_TYPE_ALPHA || (type) == TEX_TYPE_ALPHA_GRADIENT || (type) == TEX_TYPE_RGBA)

#define MAX_GLTEXTURES 4800

int texels;

typedef void (*_BuildGammaTable)(float g);
_BuildGammaTable ORIG_BuildGammaTable = NULL;

cvar_t *brightness, *texgamma;
byte texgammatable[256];

// FIXME: shorten these?
struct mspriteframe_t
{
	int width;
	int height;
	float up, down, left, right;
	int gl_texturenum;
};

typedef int (*_Draw_String)(int x, int y, char* str);
typedef int (*_GL_Bind)(int texnum);
typedef void (*_Draw_Frame)(mspriteframe_t* pFrame, int ix, int iy, const Rect* prcSubRect);
typedef void (*_BoxFilter3x3)(byte* out, byte* in, int w, int h, int x, int y);
typedef void (*_ComputeScaledSize)(int* wscale, int* hscale, int width, int height);
typedef void (*_GL_ResampleTexture)(unsigned int* in, int inwidth, int inheight, unsigned int* out, int outwidth, int outheight);
typedef void (*_GL_ResampleAlphaTexture)(byte* in, int inwidth, int inheight, byte* out, int outwidth, int outheight);
typedef void (*_GL_Upload32)(unsigned int* data, int width, int height, qboolean mipmap, int iType, int filter);
typedef void (*_GL_Upload16)(unsigned char* data, int width, int height, qboolean mipmap, int iType, unsigned char* pPal, int filter);


_Draw_String ORIG_Draw_String = NULL;
_GL_Bind ORIG_GL_Bind = NULL;
_Draw_Frame ORIG_Draw_Frame = NULL;
_BoxFilter3x3 ORIG_BoxFilter3x3 = NULL;
_ComputeScaledSize ORIG_ComputeScaledSize = NULL;
_GL_ResampleTexture ORIG_GL_ResampleTexture = NULL;
_GL_ResampleAlphaTexture ORIG_GL_ResampleAlphaTexture = NULL;
_GL_Upload32 ORIG_GL_Upload32 = NULL;
_GL_Upload16 ORIG_GL_Upload16 = NULL;

cvar_t* gl_spriteblend;
cvar_t* gl_dither;
cvar_t* gl_ansio;

typedef void (*_VideoMode_GetCurrentVideoMode)(int* wide, int* tall, int* bpp);
_VideoMode_GetCurrentVideoMode ORIG_VideoMode_GetCurrentVideoMode;

void VideoMode_GetCurrentVideoMode(int* wide, int* tall, int* bpp)
{
	ORIG_VideoMode_GetCurrentVideoMode(wide, tall, bpp);
}

cl_enginefuncs_s gEngfuncs;
dllfuncs_t gClientFuncs;

bool HWHook();

#define Find(future_name)                                                                                                                  \
	{                                                                                                                                      \
		auto f##future_name = utils.FindAsync(ORIG_##future_name, patterns::engine::future_name);                                          \
		auto pattern = f##future_name.get();                                                                                               \
		if (ORIG_##future_name)                                                                                                            \
		{                                                                                                                                  \
			gEngfuncs.Con_DPrintf("[hw dll] Found " #future_name " at %p (using the %s pattern).\n", ORIG_##future_name, pattern->name()); \
		}                                                                                                                                  \
	}

#define Hook(future_name)                                                                                                                  \
	{                                                                                                                                      \
		auto f##future_name = utils.FindAsync(ORIG_##future_name, patterns::engine::future_name);                                          \
		auto pattern = f##future_name.get();                                                                                               \
		if (ORIG_##future_name)                                                                                                            \
		{                                                                                                                                  \
			gEngfuncs.Con_DPrintf("[hw dll] Found " #future_name " at %p (using the %s pattern).\n", ORIG_##future_name, pattern->name()); \
			void* p##future_name = (void*)ORIG_##future_name;                                                                              \
			MH_CreateHook(p##future_name, (void*)##future_name, (void**)&ORIG_##future_name);                                              \
		}                                                                                                                                  \
		else                                                                                                                               \
		{                                                                                                                                  \
			gEngfuncs.Con_DPrintf("[hw dll] Could not find " #future_name ".\n");                                                          \
		}                                                                                                                                  \
	}

void ComputeScaledSize(int* wscale, int* hscale, int width, int height)
{
	ORIG_ComputeScaledSize(wscale, hscale, width, height);
}

void BoxFilter3x3(byte* out, byte* in, int w, int h, int x, int y)
{
	ORIG_BoxFilter3x3(out, in, w, h, x, y); // TODO: implement
}

void GL_MipMap(byte* in, int width, int height)
{
	byte* out = in;

	width <<= 2;
	height >>= 1;

	for (int i = 0; i < height; i++, in += width)
	{
		for (int j = 0; j < width; j += 8, out += 4, in += 8)
		{
			out[0] = (in[0] + in[4] + in[width + 0] + in[width + 4]) >> 2;
			out[1] = (in[1] + in[5] + in[width + 1] + in[width + 5]) >> 2;
			out[2] = (in[2] + in[6] + in[width + 2] + in[width + 6]) >> 2;
			out[3] = (in[3] + in[7] + in[width + 3] + in[width + 7]) >> 2;
		}
	}
}


void GL_ResampleTexture(unsigned int* in, int inwidth, int inheight, unsigned int* out, int outwidth, int outheight)
{
	ORIG_GL_ResampleTexture(in, inwidth, inheight, out, outwidth, outheight);
}

void GL_ResampleAlphaTexture(byte* in, int inwidth, int inheight, byte* out, int outwidth, int outheight)
{
	ORIG_GL_ResampleAlphaTexture(in, inwidth, inheight, out, outwidth, outheight);
}


int giTotalTextures;
int giTotalTexBytes;

void GL_Upload32(unsigned int* data, int width, int height, qboolean mipmap, int iType, int filter)
{
	static unsigned scaled[2048 * 1024]; // 2x from limit (HD textures support)

	int scaled_width, scaled_height;
	int iFormat = NULL, iComponent = NULL;

	int w, h, bpp;
	VideoMode_GetCurrentVideoMode(&w, &h, &bpp);

	int texbytes = width * height;

	giTotalTexBytes += texbytes;

	if (iType != TEX_TYPE_LUM)
		giTotalTexBytes += 2 * texbytes;

	giTotalTextures++;

	if (gl_spriteblend->value != 0.0f && TEX_IS_ALPHA(iType))
	{
		for (int i = 0; i < texbytes; i++)
		{
			if (!data[i])
				BoxFilter3x3(reinterpret_cast<byte*>(&data[i]), reinterpret_cast<byte*>(data), width, height, i % width, i / width);
		}
	}

	ComputeScaledSize(&scaled_width, &scaled_height, width, height);

	if (scaled_width * scaled_height > sizeof(scaled) / 4)
		gEngfuncs.Con_Printf("GL_LoadTexture: too big"); // Sys_Error

	switch (iType)
	{
	default:
		gEngfuncs.Con_Printf("GL_Upload32: Bad texture type"); // Sys_Error
		break;

	case TEX_TYPE_NONE:
		iFormat = GL_RGBA;

		if (bpp == 16)
			iComponent = GL_RGB8;
		else
			iComponent = GL_RGBA8;

		break;

	case TEX_TYPE_ALPHA:
	case TEX_TYPE_ALPHA_GRADIENT:
		iFormat = GL_RGBA;

		if (bpp == 16)
			iComponent = GL_RGBA4;
		else
			iComponent = GL_RGBA8;
		break;

	case TEX_TYPE_LUM:
		iFormat = GL_LUMINANCE;
		iComponent = GL_LUMINANCE8;
		break;

	case TEX_TYPE_RGBA:
		iFormat = GL_RGBA;

		if (bpp == 16)
			iComponent = GL_RGB5_A1;
		else
			iComponent = GL_RGBA8;

		break;
	}

	if (scaled_width == width && scaled_height == height)
	{
		if (!mipmap)
		{
			glTexImage2D(GL_TEXTURE_2D, GL_ZERO, iComponent, width, height, GL_ZERO, iFormat, GL_UNSIGNED_BYTE, data);
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filter);
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filter);
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, gl_ansio->value);
		}

		if (iType != TEX_TYPE_LUM)
			texbytes *= 4;

		memcpy(scaled, data, texbytes);
	}
	else
	{
		if (iType != TEX_TYPE_LUM)
			GL_ResampleTexture(data, width, height, scaled, scaled_width, scaled_height);
		else
			GL_ResampleAlphaTexture(reinterpret_cast<byte*>(data), width, height, reinterpret_cast<byte*>(scaled), scaled_width, scaled_height);
	}

	texels += scaled_width * scaled_height; // TODO: implement GL_Texels_f
	glTexImage2D(GL_TEXTURE_2D, GL_ZERO, iComponent, scaled_width, scaled_height, GL_ZERO, iFormat, GL_UNSIGNED_BYTE, scaled);

	if (mipmap)
	{
		int miplevel = 0;

		while (scaled_width > 1 || scaled_height > 1)
		{
			GL_MipMap(reinterpret_cast<byte*>(scaled), scaled_width, scaled_height);

			scaled_width >>= 1;
			scaled_height >>= 1;

			if (scaled_width < 1)
				scaled_width = 1;

			if (scaled_height < 1)
				scaled_height = 1;

			texels += scaled_width * scaled_height; // TODO: implement GL_Texels_f

			miplevel++;
			glTexImage2D(GL_TEXTURE_2D, miplevel, iComponent, scaled_width, scaled_height, GL_ZERO, iFormat, GL_UNSIGNED_BYTE, scaled);
		}
	}

	/* TODO: get gl_filter_min / gl_filer_max
	if (mipmap)
	{
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, gl_filter_min);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, gl_filter_max);
	}
	else
	*/
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filter);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filter);

	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, gl_ansio->value);
}

void GL_Upload16(unsigned char* data, int width, int height, qboolean mipmap, int iType, unsigned char* pPal, int filter)
{
	static unsigned trans[2560 * 1920];

	unsigned char *pb, *ppix;
	bool noalpha = true;
	int texturebytes = width * height;
	int p;

	if (texturebytes > sizeof(trans))
	{
		gEngfuncs.Con_Printf("Can't upload (%ix%i) texture, it's > 2560*1920 bytes\n", width, height);
		return;
	}

	if (iType != TEX_TYPE_LUM)
	{
		if (!pPal)
			return;

		for (int i = 0; i < 768; i++)
			pPal[i] = texgammatable[pPal[i]];
	}

	if (TEX_IS_ALPHA(iType))
	{
		if (iType == TEX_TYPE_ALPHA_GRADIENT)
		{
			for (int i = 0; i < texturebytes; i++)
			{
				p = data[i];
				pb = reinterpret_cast<byte*>(&trans[i]);

				pb[0] = pPal[765];
				pb[1] = pPal[766];
				pb[2] = pPal[767];
				pb[3] = p;

				noalpha = false;
			}
		}
		else if (iType == TEX_TYPE_RGBA)
		{
			for (int i = 0; i < texturebytes; i++)
			{
				p = data[i];
				pb = reinterpret_cast<byte*>(&trans[i]);

				pb[0] = pPal[p * 3 + 0];
				pb[1] = pPal[p * 3 + 1];
				pb[2] = pPal[p * 3 + 2];
				pb[3] = p;

				noalpha = false;
			}
		}
		else if (iType == TEX_TYPE_ALPHA)
		{
			for (int i = 0; i < texturebytes; i++)
			{
				p = data[i];
				pb = reinterpret_cast<byte*>(&trans[i]);

				if (p == 255)
				{
					pb[0] = pb[1] = pb[2] = pb[3] = NULL;
					noalpha = false;
				}
				else
				{
					pb[0] = pPal[p * 3 + 0];
					pb[1] = pPal[p * 3 + 1];
					pb[2] = pPal[p * 3 + 2];
					pb[3] = 255;
				}
			}
		}

		if (noalpha)
			iType = TEX_TYPE_NONE;
	}
	else if (iType == TEX_TYPE_NONE)
	{
		if (texturebytes & 3)
			gEngfuncs.Con_Printf("GL_Upload16: s&3\n"); // TODO: implement Sys_Error and replace Con_Printf with Sys_Error

		if (gl_dither->value != 0.0f)
		{
			for (int i = 0; i < texturebytes; i++)
			{
				p = data[i];
				pb = reinterpret_cast<byte*>(&trans[i]);
				ppix = &pPal[p * 3];

				pb[0] = ppix[0] |= (ppix[0] >> 6);
				pb[1] = ppix[1] |= (ppix[1] >> 6);
				pb[2] = ppix[2] |= (ppix[2] >> 6);
				pb[3] = 255;
			}
		}
		else
		{
			for (int i = 0; i < texturebytes; i++)
			{
				p = data[i];
				pb = reinterpret_cast<byte*>(&trans[i]);

				pb[0] = pPal[p * 3 + 0];
				pb[1] = pPal[p * 3 + 1];
				pb[2] = pPal[p * 3 + 2];
				pb[3] = 255;
			}
		}
	}
	else if (iType == TEX_TYPE_LUM)
		memcpy(trans, data, texturebytes);
	else
		gEngfuncs.Con_Printf("GL_Upload16: Bogus texture type!\n");

	GL_Upload32(trans, width, height, mipmap, iType, filter);
}


void BuildGammaTable(float g)
{
	int inf;
	float g1, g3;
	float original_g = g;

	if (g == 0.0)
		g = 2.5;

	g = 1.0 / g;
	g1 = texgamma->value * g;

	if (brightness->value <= 0.0)
		g3 = 0.125;
	else if (brightness->value > 1.0)
		g3 = 0.05;
	else
		g3 = 0.125 - (brightness->value * brightness->value) * 0.075;

	for (int i = 0; i < 256; i++)
	{
		inf = 255 * pow(i / 255.0, g1);

		if (inf < 0)
			inf = 0;

		if (inf > 255)
			inf = 255;

		texgammatable[i] = inf;
	}
	// ...
	// Further code is for other gamma tables. Not necessary for GL_Upload32/16.
	ORIG_BuildGammaTable(original_g);
}


bool HWHook();

void DLLEXPORT InitializePlugin(cl_enginefunc_t* pEnginefuncs, dllfuncs_t* clfuncs, int iVersion)
{
	memcpy(&gEngfuncs, pEnginefuncs, sizeof(cl_enginefunc_t));
	memcpy(&gClientFuncs, clfuncs, sizeof(cl_enginefunc_t));

	MH_Initialize();
}

void DLLEXPORT HUD_Init_Post()
{
	HWHook();
}

void DLLEXPORT HUD_Shutdown_Pre()
{
	MH_RemoveHook(BuildGammaTable);
	MH_RemoveHook(VideoMode_GetCurrentVideoMode);
	MH_RemoveHook(BoxFilter3x3);
	MH_RemoveHook(ComputeScaledSize);
	MH_RemoveHook(GL_ResampleTexture);
	MH_RemoveHook(GL_ResampleAlphaTexture);
	MH_RemoveHook(GL_Upload32);
	MH_RemoveHook(GL_Upload16);

	MH_Uninitialize();
}

void VideoMode_Hook()
{
	Hook(VideoMode_GetCurrentVideoMode);
}

void V_Hook()
{
	brightness = gEngfuncs.pfnGetCvarPointer("brightness");
	texgamma = gEngfuncs.pfnGetCvarPointer("texgamma");

	Hook(BuildGammaTable);
}

void GLHook()
{
	gl_spriteblend = gEngfuncs.pfnGetCvarPointer("gl_spriteblend");
	gl_dither = gEngfuncs.pfnGetCvarPointer("gl_dither");
	gl_ansio = gEngfuncs.pfnGetCvarPointer("gl_ansio");

	Hook(BoxFilter3x3);
	Hook(ComputeScaledSize);
	Hook(GL_ResampleTexture);
	Hook(GL_ResampleAlphaTexture);
	Hook(GL_Upload32);
	Hook(GL_Upload16);
}

bool HWHook()
{
	void* handle;
	void* base;
	size_t size;

	if (!MemUtils::GetModuleInfo(L"hw.dll", &handle, &base, &size))
		return false;

	utils = Utils::Utils(handle, base, size);

	V_Hook();
	VideoMode_Hook();
	GLHook();

	MH_EnableHook(MH_ALL_HOOKS);

	return true;
}