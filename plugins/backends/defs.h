#pragma once

#define NLHOOK_VERSION 1

#include "PlatformHeaders.h"

#include <map>
#include <vector>
#include <string>
#include <algorithm>
#include <cmath>
#include <unordered_map>
#include <filesystem>

// add headers that you want to pre-compile here
#include "Platform.h"
#include "mathlib.h"
#include "vector.h"
#include "cdll_int.h"
#include "Exports.h"

#include "cvardef.h"

#ifndef NOHOOK
#include "Utils.hpp"
#include "MinHook.h"
#include "reGS_patterns.hpp"
#endif

#ifndef NOGL
#include "SDL2/SDL_opengl.h"

inline void* GetAnyGLFuncAddress(const char* name)
{
	void* p = nullptr;
#ifdef USING_GLEXT
	p = (void*)wglGetProcAddress(name);
	if (p == 0 ||
		(p == (void*)0x1) || (p == (void*)0x2) || (p == (void*)0x3) ||
		(p == (void*)-1))
	{
#endif
		HMODULE module = GetModuleHandleA("opengl32.dll");
		if (module != 0)
			p = (void*)GetProcAddress(module, name);
#ifdef USING_GLEXT
	}
#endif
	return p;
}


#endif

#define PLUGINFUNCS extern "C"

#define DEFINE_MAIN \
BOOL APIENTRY DllMain(HMODULE hModule,\
	DWORD ul_reason_for_call,\
	LPVOID lpReserved)\
{\
	return TRUE;\
}

#ifdef CLFUNCS
// From hl_weapons
typedef void (*_pfn_HUD_PostRunCmd)(struct local_state_s*, struct local_state_s*, struct usercmd_s*, int, double, unsigned int);

// From cdll_int
typedef int (*_pfn_Initialize)(cl_enginefunc_t*, int);
typedef int (*_pfn_HUD_VidInit)(void);
typedef void (*_pfn_HUD_Init)(void);
typedef int (*_pfn_HUD_Redraw)(float, int);
typedef int (*_pfn_HUD_UpdateClientData)(client_data_t*, float);
typedef void (*_pfn_HUD_Reset)(void);
typedef void (*_pfn_HUD_PlayerMove)(struct playermove_s*, int);
typedef void (*_pfn_HUD_PlayerMoveInit)(struct playermove_s*);
typedef char (*_pfn_HUD_PlayerMoveTexture)(char*);
typedef int (*_pfn_HUD_ConnectionlessPacket)(const struct netadr_s*, const char*, char*, int*);
typedef int (*_pfn_HUD_GetHullBounds)(int, float*, float*);
typedef void (*_pfn_HUD_Frame)(double);
typedef void (*_pfn_HUD_VoiceStatus)(int, qboolean);
typedef void (*_pfn_HUD_DirectorMessage)(int, void*);
typedef void (*_pfn_HUD_ChatInputPosition)(int*, int*);

// From demo
typedef void (*_pfn_Demo_ReadBuffer)(int, unsigned char*);

// From entity
typedef int (*_pfn_HUD_AddEntity)(int, struct cl_entity_s*, const char*);
typedef void (*_pfn_HUD_CreateEntities)(void);
typedef void (*_pfn_HUD_StudioEvent)(const struct mstudioevent_s*, const struct cl_entity_s*);
typedef void (*_pfn_HUD_TxferLocalOverrides)(struct entity_state_s*, const struct clientdata_s*);
typedef void (*_pfn_HUD_ProcessPlayerState)(struct entity_state_s*, const struct entity_state_s*);
typedef void (*_pfn_HUD_TxferPredictionData)(struct entity_state_s*, const struct entity_state_s*, struct clientdata_s*, const struct clientdata_s*, struct weapon_data_s*, const struct weapon_data_s*);
typedef void (*_pfn_HUD_TempEntUpdate)(double, double, double, struct tempent_s**, struct tempent_s**, int (*)(struct cl_entity_s*), void (*)(struct tempent_s*, float));
typedef struct cl_entity_s* (*_pfn_HUD_GetUserEntity)(int index);

// From in_camera
typedef void (*_pfn_CAM_Think)(void);
typedef int (*_pfn_CL_IsThirdPerson)(void);
typedef void (*_pfn_CL_CameraOffset)(float*);

// From input
typedef struct kbutton_s* (*_pfn_KB_Find)(const char*);
typedef void (*_pfn_CL_CreateMove)(float, struct usercmd_s*, int);
typedef void (*_pfn_HUD_Shutdown)(void);
typedef int (*_pfn_HUD_Key_Event)(int, int, const char*);

// From inputw32
typedef void (*_pfn_IN_ActivateMouse)(void);
typedef void (*_pfn_IN_DeactivateMouse)(void);
typedef void (*_pfn_IN_MouseEvent)(int);
typedef void (*_pfn_IN_Accumulate)(void);
typedef void (*_pfn_IN_ClearStates)(void);

// From tri
typedef void (*_pfn_HUD_DrawNormalTriangles)(void);
typedef void (*_pfn_HUD_DrawTransparentTriangles)(void);

// From view
typedef void (*_pfn_V_CalcRefdef)(struct ref_params_s*);

// From GameStudioModelRenderer
typedef int (*_pfn_HUD_GetStudioModelInterface)(int, struct r_studio_interface_s**, struct engine_studio_api_s*);

// Plugin Specific
typedef void (*_pfn_InitializePlugin)(cl_enginefunc_t* pEnginefuncs, struct dllfuncs_s* clfuncs, int iVersion);
typedef void (*_pfn_DLL_SwapWindow)(void);

typedef struct dllfuncs_s
{
	// From hl_weapons
	_pfn_HUD_PostRunCmd pHUD_PostRunCmd;

	// From cdll_int
	_pfn_Initialize pInitialize;
	_pfn_HUD_VidInit pHUD_VidInit;
	_pfn_HUD_Init pHUD_Init;
	_pfn_HUD_Redraw pHUD_Redraw;
	_pfn_HUD_UpdateClientData pHUD_UpdateClientData;
	_pfn_HUD_Reset pHUD_Reset;
	_pfn_HUD_PlayerMove pHUD_PlayerMove;
	_pfn_HUD_PlayerMoveInit pHUD_PlayerMoveInit;
	_pfn_HUD_PlayerMoveTexture pHUD_PlayerMoveTexture;
	_pfn_HUD_ConnectionlessPacket pHUD_ConnectionlessPacket;
	_pfn_HUD_GetHullBounds pHUD_GetHullBounds;
	_pfn_HUD_Frame pHUD_Frame;
	_pfn_HUD_VoiceStatus pHUD_VoiceStatus;
	_pfn_HUD_DirectorMessage pHUD_DirectorMessage;
	_pfn_HUD_ChatInputPosition pHUD_ChatInputPosition;

	// From demo
	_pfn_Demo_ReadBuffer pDemo_ReadBuffer;

	// From entity
	_pfn_HUD_AddEntity pHUD_AddEntity;
	_pfn_HUD_CreateEntities pHUD_CreateEntities;
	_pfn_HUD_StudioEvent pHUD_StudioEvent;
	_pfn_HUD_TxferLocalOverrides pHUD_TxferLocalOverrides;
	_pfn_HUD_ProcessPlayerState pHUD_ProcessPlayerState;
	_pfn_HUD_TxferPredictionData pHUD_TxferPredictionData;
	_pfn_HUD_TempEntUpdate pHUD_TempEntUpdate;
	_pfn_HUD_GetUserEntity pHUD_GetUserEntity;

	// From in_camera
	_pfn_CAM_Think pCAM_Think;
	_pfn_CL_IsThirdPerson pCL_IsThirdPerson;
	_pfn_CL_CameraOffset pCL_CameraOffset;

	// From input
	_pfn_KB_Find pKB_Find;
	_pfn_CL_CreateMove pCL_CreateMove;
	_pfn_HUD_Shutdown pHUD_Shutdown;
	_pfn_HUD_Key_Event pHUD_Key_Event;

	// From inputw32
	_pfn_IN_ActivateMouse pIN_ActivateMouse;
	_pfn_IN_DeactivateMouse pIN_DeactivateMouse;
	_pfn_IN_MouseEvent pIN_MouseEvent;
	_pfn_IN_Accumulate pIN_Accumulate;
	_pfn_IN_ClearStates pIN_ClearStates;

	// From tri
	_pfn_HUD_DrawNormalTriangles pHUD_DrawNormalTriangles;
	_pfn_HUD_DrawTransparentTriangles pHUD_DrawTransparentTriangles;

	// From view
	_pfn_V_CalcRefdef pV_CalcRefdef;

	// From GameStudioModelRenderer
	_pfn_HUD_GetStudioModelInterface pHUD_GetStudioModelInterface;
} dllfuncs_t;
#endif

inline cl_enginefuncs_s gEngfuncs;