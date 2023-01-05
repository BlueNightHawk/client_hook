D// Emulates paranoia's hacked opengl32.dll
#include "PlatformHeaders.h"
#include "Platform.h"
#include "mathlib.h"
#include "vector.h"
#include "cdll_int.h"
#include "kbutton.h"
#include "funcptrs.h"
#include "Exports.h"

#include <filesystem>
#include <vector>

#include "SDL2/SDL.h"

#include "MinHook.h"
#include "Utils.hpp"

#include "SDL2/SDL_opengl.h"

#include <algorithm>

cl_enginefuncs_s gEngfuncs;

typedef const GLubyte*(APIENTRY* pglGetString)(GLenum name);
typedef void(APIENTRY* pglDepthRange)(GLclampd zNear, GLclampd zFar);
typedef void(APIENTRY* pglClear)(GLbitfield mask);

pglGetString ORIG_glGetString;
pglDepthRange ORIG_glDepthRange;
pglClear ORIG_glClear;

extern "C" {
void DLLEXPORT InitializePlugin(cl_enginefunc_t* pEnginefuncs, dllfuncs_t* clfuncs, int iVersion);
void DLLEXPORT HUD_Shutdown_Pre();
}

void HookParanoia();

void DLLEXPORT InitializePlugin(cl_enginefunc_t* pEnginefuncs, dllfuncs_t* clfuncs, int iVersion)
{
	memcpy(&gEngfuncs, pEnginefuncs, sizeof(cl_enginefunc_t));

	MH_Initialize();
	HookParanoia();
}

void DLLEXPORT HUD_Shutdown_Pre()
{
	MH_Uninitialize();
}

void* GetAnyGLFuncAddress(const char* name)
{
	void* p = (void*)wglGetProcAddress(name);
	if (p == 0 ||
		(p == (void*)0x1) || (p == (void*)0x2) || (p == (void*)0x3) ||
		(p == (void*)-1))
	{
		HMODULE module = LoadLibraryA("opengl32.dll");
		p = (void*)GetProcAddress(module, name);
	}

	return p;
}


const GLubyte* APIENTRY sys_glGetString(GLenum name)
{
	if (name == 0x1F03) // GL_EXTENSIONS
	{
		static char extstring[64 * 1024];
		strcpy(extstring, (const char*)(*ORIG_glGetString)(name));
		strcat(extstring, " PARANOIA_HACKS_V1");

		// remove GL_EXT_paletted_texture externsion
		char* pext = strstr(extstring, "GL_EXT_paletted_texture");
		if (pext)
		{
			pext[0] = 'A';
		}

		return (GLubyte*)extstring;
	}
	return (*ORIG_glGetString)(name);
}


void APIENTRY sys_glDepthRange(GLclampd zNear, GLclampd zFar)
{
	// buz
	if (zNear == 0 && (zFar == 0.5 || zFar == 1))
		zFar = 0.8;
	(*ORIG_glDepthRange)(zNear, zFar);
}

void APIENTRY sys_glClear(GLbitfield mask)
{
	// buz: clear also stencil buffer
	int hasStencil;
	glGetIntegerv(GL_STENCIL_BITS, &hasStencil);
	if (hasStencil)
	{
		mask |= GL_STENCIL_BUFFER_BIT;
		glClearStencil(0);
		//	glStencilMask(0); // dont write stencil by default
	}

	(*ORIG_glClear)(mask);
}

//-----------------------------------------------------------------------------
// Hook opengl32.dll
//-----------------------------------------------------------------------------
void HookParanoia()
{
	// Get SDL functions
	ORIG_glGetString = (pglGetString)GetAnyGLFuncAddress("glGetString");
	ORIG_glDepthRange = (pglDepthRange)GetAnyGLFuncAddress("glDepthRange");
	ORIG_glClear = (pglClear)GetAnyGLFuncAddress("glClear");

	if (ORIG_glGetString && ORIG_glDepthRange && ORIG_glClear)
		gEngfuncs.Con_DPrintf("[opengl32.dll] Hook successful! Paranoia hacks are enabled.\n");
	else
	{
		gEngfuncs.Con_DPrintf("[opengl32.dll] Hook failed! Paranoia hacks are disabled.\n");
		return;
	}

	if (ORIG_glGetString)
	{
		void* pglGetString = (void*)ORIG_glGetString;
		MH_CreateHook(pglGetString, (void*)sys_glGetString, (void**)&ORIG_glGetString);
		MH_EnableHook(pglGetString);
	}

	if (ORIG_glDepthRange)
	{
		void* pglDepthRange = (void*)ORIG_glDepthRange;
		MH_CreateHook(pglDepthRange, (void*)sys_glDepthRange, (void**)&ORIG_glDepthRange);
		MH_EnableHook(pglDepthRange);
	}

	if (ORIG_glClear)
	{
		void* pglClear = (void*)ORIG_glClear;
		MH_CreateHook(pglClear, (void*)sys_glClear, (void**)&ORIG_glClear);
		MH_EnableHook(pglClear);
	}
}