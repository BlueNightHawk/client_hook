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

#include "studio.h"
#include "com_model.h"
#include "r_studioint.h"

cl_enginefunc_t gEngfuncs;
cl_enginefunc_t gClientEngfuncs;

HMODULE hClient = 0;

dllfuncs_s g_ClientFuncs;
dllfuncs_s g_OriginalClientFuncs;

#define GET_FUNC_PTR(name) g_ClientFuncs.p##name## = (_pfn_##name##)GetProcAddress(hClient, #name)

void InitClientDll(cl_enginefunc_t* pEnginefuncs)
{
	//SetDllDirectory((std::filesystem::current_path().string() + "/" + pEnginefuncs->pfnGetGameDirectory() + "/cl_dlls").c_str());

	SetDllDirectory((std::filesystem::current_path().string() + "/" + "cryoffear/" + "cl_dlls").c_str());
	char dllname[128] = "mod_client.dll";
	hClient = LoadLibrary(dllname);

	if (!hClient)
	{
		MessageBox(0, "Client dll could not be loaded!", "Error", MB_OK);
	}
}

_pfn_HUD_PlayerMove pHUD_PlayerMove_0;
_pfn_HUD_PlayerMoveInit pHUD_PlayerMoveInit_0;

void InitExports()
{
	// From hl_weapons
	GET_FUNC_PTR(HUD_PostRunCmd);

	// From cdll_int
	GET_FUNC_PTR(Initialize);
	GET_FUNC_PTR(HUD_VidInit);
	GET_FUNC_PTR(HUD_Init);
	GET_FUNC_PTR(HUD_Redraw);
	GET_FUNC_PTR(HUD_UpdateClientData);
	GET_FUNC_PTR(HUD_Reset);
	GET_FUNC_PTR(HUD_PlayerMove);
	GET_FUNC_PTR(HUD_PlayerMoveInit);

	// COF specific
	pHUD_PlayerMove_0 = (_pfn_HUD_PlayerMove)GetProcAddress(hClient, "HUD_PlayerMove_0");
	pHUD_PlayerMoveInit_0 = (_pfn_HUD_PlayerMoveInit)GetProcAddress(hClient, "HUD_PlayerMoveInit_0");
	GET_FUNC_PTR(HUD_PlayerMoveTexture);
	GET_FUNC_PTR(HUD_ConnectionlessPacket);
	GET_FUNC_PTR(HUD_GetHullBounds);
	GET_FUNC_PTR(HUD_Frame);
	GET_FUNC_PTR(HUD_VoiceStatus);
	GET_FUNC_PTR(HUD_DirectorMessage);
	GET_FUNC_PTR(HUD_ChatInputPosition);

	// From demo
	GET_FUNC_PTR(Demo_ReadBuffer);

	// From entity
	GET_FUNC_PTR(HUD_AddEntity);
	GET_FUNC_PTR(HUD_CreateEntities);
	GET_FUNC_PTR(HUD_StudioEvent);
	GET_FUNC_PTR(HUD_TxferLocalOverrides);
	GET_FUNC_PTR(HUD_ProcessPlayerState);
	GET_FUNC_PTR(HUD_TxferPredictionData);
	GET_FUNC_PTR(HUD_TempEntUpdate);
	GET_FUNC_PTR(HUD_GetUserEntity);

	// From in_camera
	GET_FUNC_PTR(CAM_Think);
	GET_FUNC_PTR(CL_IsThirdPerson);
	GET_FUNC_PTR(CL_CameraOffset);

	// From input
	GET_FUNC_PTR(KB_Find);
	GET_FUNC_PTR(CL_CreateMove);
	GET_FUNC_PTR(HUD_Shutdown);
	GET_FUNC_PTR(HUD_Key_Event);

	// From inputw32
	GET_FUNC_PTR(IN_ActivateMouse);
	GET_FUNC_PTR(IN_DeactivateMouse);
	GET_FUNC_PTR(IN_MouseEvent);
	GET_FUNC_PTR(IN_Accumulate);
	GET_FUNC_PTR(IN_ClearStates);

	// From tri
	GET_FUNC_PTR(HUD_DrawNormalTriangles);
	GET_FUNC_PTR(HUD_DrawTransparentTriangles);

	// From view
	GET_FUNC_PTR(V_CalcRefdef);

	// From GameStudioModelRenderer
	GET_FUNC_PTR(HUD_GetStudioModelInterface);

	memcpy(&g_OriginalClientFuncs, &g_ClientFuncs, sizeof(dllfuncs_s));
}

void DLLEXPORT HUD_PostRunCmd(struct local_state_s* from, struct local_state_s* to, struct usercmd_s* cmd, int runfuncs, double time, unsigned int random_seed)
{
	g_ClientFuncs.pHUD_PostRunCmd(from, to, cmd, runfuncs, time, random_seed);
}

int DLLEXPORT Initialize(cl_enginefunc_t* pEnginefuncs, int iVersion)
{
	InitClientDll(pEnginefuncs);
	InitExports();

	if (g_ClientFuncs.pInitialize == nullptr)
	{
		MessageBox(0, "Client dll could not be loaded! Possibly encrypted.", "Error", MB_OK);
		return 0;
	}

	g_ClientFuncs.pInitialize(pEnginefuncs, iVersion);
	return 1;
}

int DLLEXPORT HUD_VidInit(void)
{
	int result = g_ClientFuncs.pHUD_VidInit();
	return result;
}

void DLLEXPORT HUD_Init(void)
{
	g_ClientFuncs.pHUD_Init();
}

int DLLEXPORT HUD_Redraw(float flTime, int intermission)
{
	int result = g_ClientFuncs.pHUD_Redraw(flTime, intermission);
	return result;
}

int DLLEXPORT HUD_UpdateClientData(client_data_t* cdata, float flTime)
{
	int result = g_ClientFuncs.pHUD_UpdateClientData(cdata, flTime);
	return result;
}

void DLLEXPORT HUD_Reset(void)
{
	g_ClientFuncs.pHUD_Reset();
}

void DLLEXPORT HUD_PlayerMove(struct playermove_s* ppmove, int server)
{
	g_ClientFuncs.pHUD_PlayerMove(ppmove, server);
}

void DLLEXPORT HUD_PlayerMoveInit(struct playermove_s* ppmove)
{
	g_ClientFuncs.pHUD_PlayerMoveInit(ppmove);
}

void DLLEXPORT HUD_PlayerMove_0(struct playermove_s* ppmove, int server)
{
	pHUD_PlayerMove_0(ppmove, server);
}

void DLLEXPORT HUD_PlayerMoveInit_0(struct playermove_s* ppmove)
{
	pHUD_PlayerMoveInit_0(ppmove);
}

char DLLEXPORT HUD_PlayerMoveTexture(char* name)
{
	auto result =  g_ClientFuncs.pHUD_PlayerMoveTexture(name);
	return result;
}

int DLLEXPORT HUD_ConnectionlessPacket(const struct netadr_s* net_from, const char* args, char* response_buffer, int* response_buffer_size)
{
	int result = g_ClientFuncs.pHUD_ConnectionlessPacket(net_from, args, response_buffer, response_buffer_size);
	return result;
}

int DLLEXPORT HUD_GetHullBounds(int hullnumber, float* mins, float* maxs)
{
	int result = g_ClientFuncs.pHUD_GetHullBounds(hullnumber, mins, maxs);
	return result;
}

void DLLEXPORT HUD_Frame(double time)
{
	g_ClientFuncs.pHUD_Frame(time);
}

void DLLEXPORT HUD_VoiceStatus(int entindex, qboolean bTalking)
{
	g_ClientFuncs.pHUD_VoiceStatus(entindex, bTalking);
}

void DLLEXPORT HUD_DirectorMessage(int iSize, void* pbuf)
{
	g_ClientFuncs.pHUD_DirectorMessage(iSize, pbuf);
}

void DLLEXPORT HUD_ChatInputPosition(int* x, int* y)
{
	g_ClientFuncs.pHUD_ChatInputPosition(x, y);
}

// From demo
void DLLEXPORT Demo_ReadBuffer(int size, unsigned char* buffer)
{
	g_ClientFuncs.pDemo_ReadBuffer(size, buffer);
}

// From entity
int DLLEXPORT HUD_AddEntity(int type, struct cl_entity_s* ent, const char* modelname)
{
	int result = g_ClientFuncs.pHUD_AddEntity(type, ent, modelname);
	return result;
}

void DLLEXPORT HUD_CreateEntities(void)
{
	g_ClientFuncs.pHUD_CreateEntities();
}

void DLLEXPORT HUD_StudioEvent(const struct mstudioevent_s* event, const struct cl_entity_s* entity)
{
	g_ClientFuncs.pHUD_StudioEvent(event, entity);
}

void DLLEXPORT HUD_TxferLocalOverrides(struct entity_state_s* state, const struct clientdata_s* client)
{
	g_ClientFuncs.pHUD_TxferLocalOverrides(state, client);
}

void DLLEXPORT HUD_ProcessPlayerState(struct entity_state_s* dst, const struct entity_state_s* src)
{
	g_ClientFuncs.pHUD_ProcessPlayerState(dst, src);
}

void DLLEXPORT HUD_TxferPredictionData(struct entity_state_s* ps, const struct entity_state_s* pps, struct clientdata_s* pcd, const struct clientdata_s* ppcd, struct weapon_data_s* wd, const struct weapon_data_s* pwd)
{
	g_ClientFuncs.pHUD_TxferPredictionData(ps, pps, pcd, ppcd, wd, pwd);
}

void DLLEXPORT HUD_TempEntUpdate(double frametime, double client_time, double cl_gravity, struct tempent_s** ppTempEntFree, struct tempent_s** ppTempEntActive, int (*Callback_AddVisibleEntity)(struct cl_entity_s* pEntity), void (*Callback_TempEntPlaySound)(struct tempent_s* pTemp, float damp))
{
	g_ClientFuncs.pHUD_TempEntUpdate(frametime, client_time, cl_gravity, ppTempEntFree, ppTempEntActive, Callback_AddVisibleEntity, Callback_TempEntPlaySound);
}

struct cl_entity_s DLLEXPORT* HUD_GetUserEntity(int index)
{
	auto result = g_ClientFuncs.pHUD_GetUserEntity(index);
	return result;
}

// From in_camera
void DLLEXPORT CAM_Think(void)
{
	g_ClientFuncs.pCAM_Think();
}

int DLLEXPORT CL_IsThirdPerson(void)
{
	int result = g_ClientFuncs.pCL_IsThirdPerson();
	return result;
}

void DLLEXPORT CL_CameraOffset(float* ofs)
{
	g_ClientFuncs.pCL_CameraOffset(ofs);
}

// From input
struct kbutton_s DLLEXPORT* KB_Find(const char* name)
{
	auto result = g_ClientFuncs.pKB_Find(name);

	return result;
}

void DLLEXPORT CL_CreateMove(float frametime, struct usercmd_s* cmd, int active)
{
	g_ClientFuncs.pCL_CreateMove(frametime, cmd, active);
}

void DLLEXPORT HUD_Shutdown(void)
{
	g_ClientFuncs.pHUD_Shutdown();
	FreeLibrary(hClient);
}

int DLLEXPORT HUD_Key_Event(int eventcode, int keynum, const char* pszCurrentBinding)
{
	int result = g_ClientFuncs.pHUD_Key_Event(eventcode, keynum, pszCurrentBinding);
	return result;
}

// From inputw32
void DLLEXPORT IN_ActivateMouse(void)
{
	g_ClientFuncs.pIN_ActivateMouse();
}

void DLLEXPORT IN_DeactivateMouse(void)
{
	g_ClientFuncs.pIN_DeactivateMouse();
}

void DLLEXPORT IN_MouseEvent(int mstate)
{
//	g_ClientFuncs.pIN_MouseEvent(mstate);
}

void DLLEXPORT IN_Accumulate(void)
{
	g_ClientFuncs.pIN_Accumulate();
}

void DLLEXPORT IN_ClearStates(void)
{
	g_ClientFuncs.pIN_ClearStates();
}

// From tri
void DLLEXPORT HUD_DrawNormalTriangles(void)
{
	g_ClientFuncs.pHUD_DrawNormalTriangles();
}

void DLLEXPORT HUD_DrawTransparentTriangles(void)
{
	g_ClientFuncs.pHUD_DrawTransparentTriangles();
}

// From view
void DLLEXPORT V_CalcRefdef(struct ref_params_s* pparams)
{
	g_ClientFuncs.pV_CalcRefdef(pparams);
}

// From GameStudioModelRenderer
int DLLEXPORT HUD_GetStudioModelInterface(int version, struct r_studio_interface_s** ppinterface, struct engine_studio_api_s* pstudio)
{	
	int result = g_ClientFuncs.pHUD_GetStudioModelInterface(version, ppinterface, pstudio);
	return result;
}