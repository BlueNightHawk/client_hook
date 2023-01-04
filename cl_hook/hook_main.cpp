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

using namespace PluginManager;

cl_enginefunc_t gEngfuncs;

HMODULE hClient;

dllfuncs_s g_ClientFuncs;
dllfuncs_s g_OriginalClientFuncs;

#define GET_FUNC_PTR(name) g_ClientFuncs.p##name## = (_pfn_##name##)GetProcAddress(hClient, #name)

void InitClientDll(cl_enginefunc_t* pEnginefuncs)
{
	SetDllDirectory((std::filesystem::current_path().string() + "/" + pEnginefuncs->pfnGetGameDirectory() + "/cl_dlls").c_str());

	char dllname[128] = "mod_client.dll";
	static char token[128];
	char *afile, *pfile;

	afile = pfile = (char*)pEnginefuncs->COM_LoadFile("cl_dlls/nlui_hook.txt", 5, 0);

	if (!afile)
	{
		hClient = LoadLibrary(dllname);
		return;
	}
	while (pfile = pEnginefuncs->COM_ParseFile(pfile, token))
	{
		if (!stricmp("client", token))
		{
			pfile = pEnginefuncs->COM_ParseFile(pfile, token);
			sprintf(dllname, token);		
			break;
		}
	}

	pEnginefuncs->COM_FreeFile(afile);
	hClient = LoadLibrary(dllname);
}

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

	memcpy(&gEngfuncs, pEnginefuncs, sizeof(cl_enginefunc_t));

	if (!hClient || hClient == INVALID_HANDLE_VALUE)
	{
		return 0;
	}

	InitExports();

	InitPlugins();

	PluginFuncsPre::Initialize(pEnginefuncs, iVersion);
	g_ClientFuncs.pInitialize(pEnginefuncs, iVersion);
	PluginFuncsPost::Initialize(pEnginefuncs, iVersion);
	return 1;
}

int DLLEXPORT HUD_VidInit(void)
{
	PluginFuncsPre::HUD_VidInit();
	int result = g_ClientFuncs.pHUD_VidInit();
	PluginFuncsPost::HUD_VidInit();
	return result;
}

void DLLEXPORT HUD_Init(void)
{
	PluginFuncsPre::HUD_Init();
	g_ClientFuncs.pHUD_Init();
	PluginFuncsPost::HUD_Init();
}

int DLLEXPORT HUD_Redraw(float flTime, int intermission)
{
	PluginFuncsPre::HUD_Redraw(flTime, intermission);
	int result = g_ClientFuncs.pHUD_Redraw(flTime, intermission);
	PluginFuncsPost::HUD_Redraw(flTime, intermission);
	return result;
}

int DLLEXPORT HUD_UpdateClientData(client_data_t* cdata, float flTime)
{
	PluginFuncsPre::HUD_UpdateClientData(cdata, flTime);
	int result = g_ClientFuncs.pHUD_UpdateClientData(cdata, flTime);
	PluginFuncsPost::HUD_UpdateClientData(cdata, flTime);
	return result;
}

void DLLEXPORT HUD_Reset(void)
{
	PluginFuncsPre::HUD_Reset();
	g_ClientFuncs.pHUD_Reset();
	PluginFuncsPost::HUD_Reset();
}

void DLLEXPORT HUD_PlayerMove(struct playermove_s* ppmove, int server)
{
	PluginFuncsPre::HUD_PlayerMove(ppmove, server);
	g_ClientFuncs.pHUD_PlayerMove(ppmove, server);
	PluginFuncsPost::HUD_PlayerMove(ppmove, server);
}

void DLLEXPORT HUD_PlayerMoveInit(struct playermove_s* ppmove)
{
	PluginFuncsPre::HUD_PlayerMoveInit(ppmove);
	g_ClientFuncs.pHUD_PlayerMoveInit(ppmove);
	PluginFuncsPost::HUD_PlayerMoveInit(ppmove);
}

char DLLEXPORT HUD_PlayerMoveTexture(char* name)
{
	PluginFuncsPre::HUD_PlayerMoveTexture(name);
	auto result =  g_ClientFuncs.pHUD_PlayerMoveTexture(name);
	PluginFuncsPost::HUD_PlayerMoveTexture(name);
	return result;
}

int DLLEXPORT HUD_ConnectionlessPacket(const struct netadr_s* net_from, const char* args, char* response_buffer, int* response_buffer_size)
{
	PluginFuncsPre::HUD_ConnectionlessPacket(net_from, args, response_buffer, response_buffer_size);
	int result = g_ClientFuncs.pHUD_ConnectionlessPacket(net_from, args, response_buffer, response_buffer_size);
	PluginFuncsPost::HUD_ConnectionlessPacket(net_from, args, response_buffer, response_buffer_size);
	return result;
}

int DLLEXPORT HUD_GetHullBounds(int hullnumber, float* mins, float* maxs)
{
	PluginFuncsPre::HUD_GetHullBounds(hullnumber, mins, maxs);
	int result = g_ClientFuncs.pHUD_GetHullBounds(hullnumber, mins, maxs);
	PluginFuncsPost::HUD_GetHullBounds(hullnumber, mins, maxs);
	return result;
}

void DLLEXPORT HUD_Frame(double time)
{
	PluginFuncsPre::HUD_Frame(time);
	g_ClientFuncs.pHUD_Frame(time);
	PluginFuncsPost::HUD_Frame(time);
}

void DLLEXPORT HUD_VoiceStatus(int entindex, qboolean bTalking)
{
	PluginFuncsPre::HUD_VoiceStatus(entindex, bTalking);
	g_ClientFuncs.pHUD_VoiceStatus(entindex, bTalking);
	PluginFuncsPost::HUD_VoiceStatus(entindex, bTalking);
}

void DLLEXPORT HUD_DirectorMessage(int iSize, void* pbuf)
{
	PluginFuncsPre::HUD_DirectorMessage(iSize, pbuf);
	g_ClientFuncs.pHUD_DirectorMessage(iSize, pbuf);
	PluginFuncsPost::HUD_DirectorMessage(iSize, pbuf);
}

void DLLEXPORT HUD_ChatInputPosition(int* x, int* y)
{
	PluginFuncsPre::HUD_ChatInputPosition(x, y);
	g_ClientFuncs.pHUD_ChatInputPosition(x, y);
	PluginFuncsPost::HUD_ChatInputPosition(x, y);
}

// From demo
void DLLEXPORT Demo_ReadBuffer(int size, unsigned char* buffer)
{
	PluginFuncsPre::Demo_ReadBuffer(size, buffer);
	g_ClientFuncs.pDemo_ReadBuffer(size, buffer);
	PluginFuncsPost::Demo_ReadBuffer(size, buffer);
}

// From entity
int DLLEXPORT HUD_AddEntity(int type, struct cl_entity_s* ent, const char* modelname)
{
	PluginFuncsPre::HUD_AddEntity(type, ent, modelname);
	int result = g_ClientFuncs.pHUD_AddEntity(type, ent, modelname);
	PluginFuncsPost::HUD_AddEntity(type, ent, modelname);

	return result;
}

void DLLEXPORT HUD_CreateEntities(void)
{
	PluginFuncsPre::HUD_CreateEntities();
	g_ClientFuncs.pHUD_CreateEntities();
	PluginFuncsPost::HUD_CreateEntities();
}

void DLLEXPORT HUD_StudioEvent(const struct mstudioevent_s* event, const struct cl_entity_s* entity)
{
	PluginFuncsPre::HUD_StudioEvent(event, entity);
	g_ClientFuncs.pHUD_StudioEvent(event, entity);
	PluginFuncsPost::HUD_StudioEvent(event, entity);
}

void DLLEXPORT HUD_TxferLocalOverrides(struct entity_state_s* state, const struct clientdata_s* client)
{
	PluginFuncsPre::HUD_TxferLocalOverrides(state, client);
	g_ClientFuncs.pHUD_TxferLocalOverrides(state, client);
	PluginFuncsPost::HUD_TxferLocalOverrides(state, client);
}

void DLLEXPORT HUD_ProcessPlayerState(struct entity_state_s* dst, const struct entity_state_s* src)
{
	PluginFuncsPre::HUD_ProcessPlayerState(dst, src);
	g_ClientFuncs.pHUD_ProcessPlayerState(dst, src);
	PluginFuncsPost::HUD_ProcessPlayerState(dst, src);
}

void DLLEXPORT HUD_TxferPredictionData(struct entity_state_s* ps, const struct entity_state_s* pps, struct clientdata_s* pcd, const struct clientdata_s* ppcd, struct weapon_data_s* wd, const struct weapon_data_s* pwd)
{
	PluginFuncsPre::HUD_TxferPredictionData(ps, pps, pcd, ppcd, wd, pwd);
	g_ClientFuncs.pHUD_TxferPredictionData(ps, pps, pcd, ppcd, wd, pwd);
	PluginFuncsPost::HUD_TxferPredictionData(ps, pps, pcd, ppcd, wd, pwd);
}

void DLLEXPORT HUD_TempEntUpdate(double frametime, double client_time, double cl_gravity, struct tempent_s** ppTempEntFree, struct tempent_s** ppTempEntActive, int (*Callback_AddVisibleEntity)(struct cl_entity_s* pEntity), void (*Callback_TempEntPlaySound)(struct tempent_s* pTemp, float damp))
{
	PluginFuncsPre::HUD_TempEntUpdate(frametime, client_time, cl_gravity, ppTempEntFree, ppTempEntActive, Callback_AddVisibleEntity, Callback_TempEntPlaySound);
	g_ClientFuncs.pHUD_TempEntUpdate(frametime, client_time, cl_gravity, ppTempEntFree, ppTempEntActive, Callback_AddVisibleEntity, Callback_TempEntPlaySound);
	PluginFuncsPost::HUD_TempEntUpdate(frametime, client_time, cl_gravity, ppTempEntFree, ppTempEntActive, Callback_AddVisibleEntity, Callback_TempEntPlaySound);
}

struct cl_entity_s DLLEXPORT* HUD_GetUserEntity(int index)
{
	PluginFuncsPre::HUD_GetUserEntity(index);
	auto result = g_ClientFuncs.pHUD_GetUserEntity(index);
	PluginFuncsPost::HUD_GetUserEntity(index);
	return result;
}

// From in_camera
void DLLEXPORT CAM_Think(void)
{
	PluginFuncsPre::CAM_Think();
	g_ClientFuncs.pCAM_Think();
	PluginFuncsPost::CAM_Think();
}

int DLLEXPORT CL_IsThirdPerson(void)
{
	PluginFuncsPre::CL_IsThirdPerson();
	int result = g_ClientFuncs.pCL_IsThirdPerson();
	PluginFuncsPost::CL_IsThirdPerson();
	return result;
}

void DLLEXPORT CL_CameraOffset(float* ofs)
{
	PluginFuncsPre::CL_CameraOffset(ofs);
	g_ClientFuncs.pCL_CameraOffset(ofs);
	PluginFuncsPost::CL_CameraOffset(ofs);
}

// From input
struct kbutton_s DLLEXPORT* KB_Find(const char* name)
{
	PluginFuncsPre::KB_Find(name);
	auto result = g_ClientFuncs.pKB_Find(name);
	PluginFuncsPost::KB_Find(name);

	return result;
}

void DLLEXPORT CL_CreateMove(float frametime, struct usercmd_s* cmd, int active)
{
	PluginFuncsPre::CL_CreateMove(frametime, cmd, active);
	g_ClientFuncs.pCL_CreateMove(frametime, cmd, active);
	PluginFuncsPost::CL_CreateMove(frametime, cmd, active);
}

void DLLEXPORT HUD_Shutdown(void)
{
	PluginFuncsPre::HUD_Shutdown();
	g_ClientFuncs.pHUD_Shutdown();
	PluginFuncsPost::HUD_Shutdown();

	FreePlugins();

	FreeLibrary(hClient);
}

int DLLEXPORT HUD_Key_Event(int eventcode, int keynum, const char* pszCurrentBinding)
{
	PluginFuncsPre::HUD_Key_Event(eventcode, keynum, pszCurrentBinding);
	int result = g_ClientFuncs.pHUD_Key_Event(eventcode, keynum, pszCurrentBinding);
	PluginFuncsPost::HUD_Key_Event(eventcode, keynum, pszCurrentBinding);
	return result;
}

// From inputw32
void DLLEXPORT IN_ActivateMouse(void)
{
	PluginFuncsPre::IN_ActivateMouse();
	g_ClientFuncs.pIN_ActivateMouse();
	PluginFuncsPost::IN_ActivateMouse();
}

void DLLEXPORT IN_DeactivateMouse(void)
{
	PluginFuncsPre::IN_DeactivateMouse();
	g_ClientFuncs.pIN_DeactivateMouse();
	PluginFuncsPost::IN_DeactivateMouse();
}

void DLLEXPORT IN_MouseEvent(int mstate)
{
	PluginFuncsPre::IN_MouseEvent(mstate);
	g_ClientFuncs.pIN_MouseEvent(mstate);
	PluginFuncsPost::IN_MouseEvent(mstate);
}

void DLLEXPORT IN_Accumulate(void)
{
	PluginFuncsPre::IN_Accumulate();
	g_ClientFuncs.pIN_Accumulate();
	PluginFuncsPost::IN_Accumulate();
}

void DLLEXPORT IN_ClearStates(void)
{
	PluginFuncsPre::IN_ClearStates();
	g_ClientFuncs.pIN_ClearStates();
	PluginFuncsPost::IN_ClearStates();
}

// From tri
void DLLEXPORT HUD_DrawNormalTriangles(void)
{
	PluginFuncsPre::HUD_DrawNormalTriangles();
	g_ClientFuncs.pHUD_DrawNormalTriangles();
	PluginFuncsPost::HUD_DrawNormalTriangles();
}

void DLLEXPORT HUD_DrawTransparentTriangles(void)
{
	PluginFuncsPre::HUD_DrawTransparentTriangles();
	g_ClientFuncs.pHUD_DrawTransparentTriangles();
	PluginFuncsPost::HUD_DrawTransparentTriangles();
}

// From view
void DLLEXPORT V_CalcRefdef(struct ref_params_s* pparams)
{
	PluginFuncsPre::V_CalcRefdef(pparams);
	g_ClientFuncs.pV_CalcRefdef(pparams);
	PluginFuncsPost::V_CalcRefdef(pparams);
}

// From GameStudioModelRenderer
int DLLEXPORT HUD_GetStudioModelInterface(int version, struct r_studio_interface_s** ppinterface, struct engine_studio_api_s* pstudio)
{
	PluginFuncsPre::HUD_GetStudioModelInterface(version, ppinterface, pstudio);
	int result = g_ClientFuncs.pHUD_GetStudioModelInterface(version, ppinterface, pstudio);
	PluginFuncsPost::HUD_GetStudioModelInterface(version, ppinterface, pstudio);

	return result;
}