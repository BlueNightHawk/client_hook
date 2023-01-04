#pragma once

#define NLHOOK_VERSION 1

#include "PlatformHeaders.h"
#include "Platform.h"
#include "mathlib.h"
#include "vector.h"
#include "cdll_int.h"

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
typedef void (*_pfn_HUD_TxferLocalOverrides)(struct entity_state_s* , const struct clientdata_s*);
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

// NLUI Defs
typedef int (*_pfn_NLUI_Initialize)(cl_enginefunc_t*, class IFileSystem*);

// Loaded from external dlls

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


typedef void (*_pfn_InitializePlugin)(cl_enginefunc_t* pEnginefuncs, dllfuncs_t* clfuncs, int iVersion);

typedef struct plugindll_s
{
	HMODULE hDllHandle;

	bool IsHandleValid();

	void InitDll(const char* pszName);
	void InitExports();
	void InitExportsPre();
	void InitExportsPost();

	void Free();

	dllfuncs_t preCall;
	dllfuncs_t postCall;
} plugindll_t;

namespace PluginManager
{
void InitPlugins();
void AddToList(char* pszName);
void FreePlugins();
namespace PluginFuncsPre
{
// From hl_weapons
void HUD_PostRunCmd(struct local_state_s* from, struct local_state_s* to, struct usercmd_s* cmd, int runfuncs, double time, unsigned int random_seed);

// From cdll_int
int Initialize(cl_enginefunc_t* pEnginefuncs, int iVersion);
int  HUD_VidInit(void);
void HUD_Init(void);
int HUD_Redraw(float flTime, int intermission);
int HUD_UpdateClientData(client_data_t* cdata, float flTime);
void HUD_Reset(void);
void HUD_PlayerMove(struct playermove_s* ppmove, int server);
void HUD_PlayerMoveInit(struct playermove_s* ppmove);
char HUD_PlayerMoveTexture(char* name);
int  HUD_ConnectionlessPacket(const struct netadr_s* net_from, const char* args, char* response_buffer, int* response_buffer_size);
int  HUD_GetHullBounds(int hullnumber, float* mins, float* maxs);
void HUD_Frame(double time);
void HUD_VoiceStatus(int entindex, qboolean bTalking);
void HUD_DirectorMessage(int iSize, void* pbuf);
void HUD_ChatInputPosition(int* x, int* y);

// From demo
void Demo_ReadBuffer(int size, unsigned char* buffer);

// From entity
int HUD_AddEntity(int type, struct cl_entity_s* ent, const char* modelname);
void HUD_CreateEntities(void);
void HUD_StudioEvent(const struct mstudioevent_s* event, const struct cl_entity_s* entity);
void HUD_TxferLocalOverrides(struct entity_state_s* state, const struct clientdata_s* client);
void HUD_ProcessPlayerState(struct entity_state_s* dst, const struct entity_state_s* src);
void HUD_TxferPredictionData(struct entity_state_s* ps, const struct entity_state_s* pps, struct clientdata_s* pcd, const struct clientdata_s* ppcd, struct weapon_data_s* wd, const struct weapon_data_s* pwd);
void HUD_TempEntUpdate(double frametime, double client_time, double cl_gravity, struct tempent_s** ppTempEntFree, struct tempent_s** ppTempEntActive, int (*Callback_AddVisibleEntity)(struct cl_entity_s* pEntity), void (*Callback_TempEntPlaySound)(struct tempent_s* pTemp, float damp));
struct cl_entity_s* HUD_GetUserEntity(int index);

// From in_camera
void CAM_Think(void);
int CL_IsThirdPerson(void);
void CL_CameraOffset(float* ofs);

// From input
struct kbutton_s KB_Find(const char* name);
void CL_CreateMove(float frametime, struct usercmd_s* cmd, int active);
void HUD_Shutdown(void);
int HUD_Key_Event(int eventcode, int keynum, const char* pszCurrentBinding);

// From inputw32
void IN_ActivateMouse(void);
void IN_DeactivateMouse(void);
void IN_MouseEvent(int mstate);
void IN_Accumulate(void);
void IN_ClearStates(void);

// From tri
void HUD_DrawNormalTriangles(void);
void HUD_DrawTransparentTriangles(void);

// From view
void V_CalcRefdef(struct ref_params_s* pparams);

// From GameStudioModelRenderer
int HUD_GetStudioModelInterface(int version, struct r_studio_interface_s** ppinterface, struct engine_studio_api_s* pstudio);
};
namespace PluginFuncsPost
{
// From hl_weapons
void HUD_PostRunCmd(struct local_state_s* from, struct local_state_s* to, struct usercmd_s* cmd, int runfuncs, double time, unsigned int random_seed);

// From cdll_int
int Initialize(cl_enginefunc_t* pEnginefuncs, int iVersion);
int HUD_VidInit(void);
void HUD_Init(void);
int HUD_Redraw(float flTime, int intermission);
int HUD_UpdateClientData(client_data_t* cdata, float flTime);
void HUD_Reset(void);
void HUD_PlayerMove(struct playermove_s* ppmove, int server);
void HUD_PlayerMoveInit(struct playermove_s* ppmove);
char HUD_PlayerMoveTexture(char* name);
int HUD_ConnectionlessPacket(const struct netadr_s* net_from, const char* args, char* response_buffer, int* response_buffer_size);
int HUD_GetHullBounds(int hullnumber, float* mins, float* maxs);
void HUD_Frame(double time);
void HUD_VoiceStatus(int entindex, qboolean bTalking);
void HUD_DirectorMessage(int iSize, void* pbuf);
void HUD_ChatInputPosition(int* x, int* y);

// From demo
void Demo_ReadBuffer(int size, unsigned char* buffer);

// From entity
int HUD_AddEntity(int type, struct cl_entity_s* ent, const char* modelname);
void HUD_CreateEntities(void);
void HUD_StudioEvent(const struct mstudioevent_s* event, const struct cl_entity_s* entity);
void HUD_TxferLocalOverrides(struct entity_state_s* state, const struct clientdata_s* client);
void HUD_ProcessPlayerState(struct entity_state_s* dst, const struct entity_state_s* src);
void HUD_TxferPredictionData(struct entity_state_s* ps, const struct entity_state_s* pps, struct clientdata_s* pcd, const struct clientdata_s* ppcd, struct weapon_data_s* wd, const struct weapon_data_s* pwd);
void HUD_TempEntUpdate(double frametime, double client_time, double cl_gravity, struct tempent_s** ppTempEntFree, struct tempent_s** ppTempEntActive, int (*Callback_AddVisibleEntity)(struct cl_entity_s* pEntity), void (*Callback_TempEntPlaySound)(struct tempent_s* pTemp, float damp));
struct cl_entity_s* HUD_GetUserEntity(int index);

// From in_camera
void CAM_Think(void);
int CL_IsThirdPerson(void);
void CL_CameraOffset(float* ofs);

// From input
struct kbutton_s KB_Find(const char* name);
void CL_CreateMove(float frametime, struct usercmd_s* cmd, int active);
void HUD_Shutdown(void);
int HUD_Key_Event(int eventcode, int keynum, const char* pszCurrentBinding);

// From inputw32
void IN_ActivateMouse(void);
void IN_DeactivateMouse(void);
void IN_MouseEvent(int mstate);
void IN_Accumulate(void);
void IN_ClearStates(void);

// From tri
void HUD_DrawNormalTriangles(void);
void HUD_DrawTransparentTriangles(void);

// From view
void V_CalcRefdef(struct ref_params_s* pparams);

// From GameStudioModelRenderer
int HUD_GetStudioModelInterface(int version, struct r_studio_interface_s** ppinterface, struct engine_studio_api_s* pstudio);
};
};