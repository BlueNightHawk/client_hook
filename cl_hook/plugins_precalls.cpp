#include "Platform.h"
#include "mathlib.h"
#include "vector.h"
#include "cdll_int.h"

#include "PlatformHeaders.h"
#include <filesystem>
#include "funcptrs.h"
#include <vector>
#include <string.h>
#include "kbutton.h"

extern std::vector<plugindll_t> g_Plugins;

using namespace PluginManager;

// From hl_weapons
void PluginFuncsPre::HUD_PostRunCmd(struct local_state_s* from, struct local_state_s* to, struct usercmd_s* cmd, int runfuncs, double time, unsigned int random_seed)
{
	for (size_t i = 0; i < g_Plugins.size(); i++)
	{
		auto p = &g_Plugins.at(i).preCall;

		if (!p->pHUD_PostRunCmd)
			continue;

		p->pHUD_PostRunCmd(from, to, cmd, runfuncs, time, random_seed);
	}
}

// From cdll_int
int PluginFuncsPre::Initialize(cl_enginefunc_t* pEnginefuncs, int iVersion)
{
	for (size_t i = 0; i < g_Plugins.size(); i++)
	{
		auto p = &g_Plugins.at(i).preCall;

		if (!p->pInitialize)
			continue;

		p->pInitialize(pEnginefuncs, iVersion);
	}
	return 1;
}

int PluginFuncsPre::HUD_VidInit(void)
{
	for (size_t i = 0; i < g_Plugins.size(); i++)
	{
		auto p = &g_Plugins.at(i).preCall;

		if (!p->pHUD_VidInit)
			continue;

		p->pHUD_VidInit();
	}
	return 1;
}

void PluginFuncsPre::HUD_Init(void)
{
	for (size_t i = 0; i < g_Plugins.size(); i++)
	{
		auto p = &g_Plugins.at(i).preCall;

		if (!p->pHUD_Init)
			continue;

		p->pHUD_Init();
	}
}

int PluginFuncsPre::HUD_Redraw(float flTime, int intermission)
{
	for (size_t i = 0; i < g_Plugins.size(); i++)
	{
		auto p = &g_Plugins.at(i).preCall;

		if (!p->pHUD_Redraw)
			continue;

		p->pHUD_Redraw(flTime, intermission);
	}
	return 1;
}

int PluginFuncsPre::HUD_UpdateClientData(client_data_t* cdata, float flTime)
{
	for (size_t i = 0; i < g_Plugins.size(); i++)
	{
		auto p = &g_Plugins.at(i).preCall;

		if (!p->pHUD_UpdateClientData)
			continue;

		p->pHUD_UpdateClientData(cdata, flTime);
	}
	return 1;
}

void PluginFuncsPre::HUD_Reset(void)
{
	for (size_t i = 0; i < g_Plugins.size(); i++)
	{
		auto p = &g_Plugins.at(i).preCall;

		if (!p->pHUD_Reset)
			continue;

		p->pHUD_Reset();
	}
}

void PluginFuncsPre::HUD_PlayerMove(struct playermove_s* ppmove, int server)
{
	for (size_t i = 0; i < g_Plugins.size(); i++)
	{
		auto p = &g_Plugins.at(i).preCall;

		if (!p->pHUD_PlayerMove)
			continue;

		p->pHUD_PlayerMove(ppmove, server);
	}
}

void PluginFuncsPre::HUD_PlayerMoveInit(struct playermove_s* ppmove)
{
	for (size_t i = 0; i < g_Plugins.size(); i++)
	{
		auto p = &g_Plugins.at(i).preCall;

		if (!p->pHUD_PlayerMove)
			continue;

		p->pHUD_PlayerMoveInit(ppmove);
	}
}

char PluginFuncsPre::HUD_PlayerMoveTexture(char* name)
{
	for (size_t i = 0; i < g_Plugins.size(); i++)
	{
		auto p = &g_Plugins.at(i).preCall;

		if (!p->pHUD_PlayerMoveTexture)
			continue;

		p->pHUD_PlayerMoveTexture(name);
	}

	return 0;
}

int PluginFuncsPre::HUD_ConnectionlessPacket(const struct netadr_s* net_from, const char* args, char* response_buffer, int* response_buffer_size)
{
	for (size_t i = 0; i < g_Plugins.size(); i++)
	{
		auto p = &g_Plugins.at(i).preCall;

		if (!p->pHUD_ConnectionlessPacket)
			continue;

		p->pHUD_ConnectionlessPacket(net_from, args, response_buffer, response_buffer_size);
	}
	return 1;
}

int PluginFuncsPre::HUD_GetHullBounds(int hullnumber, float* mins, float* maxs)
{
	for (size_t i = 0; i < g_Plugins.size(); i++)
	{
		auto p = &g_Plugins.at(i).preCall;

		if (!p->pHUD_GetHullBounds)
			continue;

		p->pHUD_GetHullBounds(hullnumber, mins, maxs);
	}

	return 0;
}

void PluginFuncsPre::HUD_Frame(double time)
{
	for (size_t i = 0; i < g_Plugins.size(); i++)
	{
		auto p = &g_Plugins.at(i).preCall;

		if (!p->pHUD_Frame)
			continue;

		p->pHUD_Frame(time);
	}
}

void PluginFuncsPre::HUD_VoiceStatus(int entindex, qboolean bTalking)
{
	for (size_t i = 0; i < g_Plugins.size(); i++)
	{
		auto p = &g_Plugins.at(i).preCall;

		if (!p->pHUD_VoiceStatus)
			continue;

		p->pHUD_VoiceStatus(entindex, bTalking);
	}
}

void PluginFuncsPre::HUD_DirectorMessage(int iSize, void* pbuf)
{
	for (size_t i = 0; i < g_Plugins.size(); i++)
	{
		auto p = &g_Plugins.at(i).preCall;

		if (!p->pHUD_DirectorMessage)
			continue;

		p->pHUD_DirectorMessage(iSize, pbuf);
	}
}
void PluginFuncsPre::HUD_ChatInputPosition(int* x, int* y)
{
	for (size_t i = 0; i < g_Plugins.size(); i++)
	{
		auto p = &g_Plugins.at(i).preCall;

		if (!p->pHUD_ChatInputPosition)
			continue;

		p->pHUD_ChatInputPosition(x, y);
	}
}

// From demo
void PluginFuncsPre::Demo_ReadBuffer(int size, unsigned char* buffer)
{
	for (size_t i = 0; i < g_Plugins.size(); i++)
	{
		auto p = &g_Plugins.at(i).preCall;

		if (!p->pDemo_ReadBuffer)
			continue;

		p->pDemo_ReadBuffer(size, buffer);
	}
}

// From entity
int PluginFuncsPre::HUD_AddEntity(int type, struct cl_entity_s* ent, const char* modelname)
{
	for (size_t i = 0; i < g_Plugins.size(); i++)
	{
		auto p = &g_Plugins.at(i).preCall;

		if (!p->pHUD_AddEntity)
			continue;

		p->pHUD_AddEntity(type, ent, modelname);
	}
	return 1;
}

void PluginFuncsPre::HUD_CreateEntities(void)
{
	for (size_t i = 0; i < g_Plugins.size(); i++)
	{
		auto p = &g_Plugins.at(i).preCall;

		if (!p->pHUD_CreateEntities)
			continue;

		p->pHUD_CreateEntities();
	}
}

void PluginFuncsPre::HUD_StudioEvent(const struct mstudioevent_s* event, const struct cl_entity_s* entity)
{
	for (size_t i = 0; i < g_Plugins.size(); i++)
	{
		auto p = &g_Plugins.at(i).preCall;

		if (!p->pHUD_StudioEvent)
			continue;

		p->pHUD_StudioEvent(event, entity);
	}
}

void PluginFuncsPre::HUD_TxferLocalOverrides(struct entity_state_s* state, const struct clientdata_s* client)
{
	for (size_t i = 0; i < g_Plugins.size(); i++)
	{
		auto p = &g_Plugins.at(i).preCall;

		if (!p->pHUD_TxferLocalOverrides)
			continue;

		p->pHUD_TxferLocalOverrides(state, client);
	}
}

void PluginFuncsPre::HUD_ProcessPlayerState(struct entity_state_s* dst, const struct entity_state_s* src)
{
	for (size_t i = 0; i < g_Plugins.size(); i++)
	{
		auto p = &g_Plugins.at(i).preCall;

		if (!p->pHUD_ProcessPlayerState)
			continue;

		p->pHUD_ProcessPlayerState(dst, src);
	}
}

void PluginFuncsPre::HUD_TxferPredictionData(struct entity_state_s* ps, const struct entity_state_s* pps, struct clientdata_s* pcd, const struct clientdata_s* ppcd, struct weapon_data_s* wd, const struct weapon_data_s* pwd)
{
	for (size_t i = 0; i < g_Plugins.size(); i++)
	{
		auto p = &g_Plugins.at(i).preCall;

		if (!p->pHUD_TxferPredictionData)
			continue;

		p->pHUD_TxferPredictionData(ps, pps, pcd, ppcd, wd, pwd);
	}
}

void PluginFuncsPre::HUD_TempEntUpdate(double frametime, double client_time, double cl_gravity, struct tempent_s** ppTempEntFree, struct tempent_s** ppTempEntActive, int (*Callback_AddVisibleEntity)(struct cl_entity_s* pEntity), void (*Callback_TempEntPlaySound)(struct tempent_s* pTemp, float damp))
{
	for (size_t i = 0; i < g_Plugins.size(); i++)
	{
		auto p = &g_Plugins.at(i).preCall;

		if (!p->pHUD_TempEntUpdate)
			continue;

		p->pHUD_TempEntUpdate(frametime, client_time, cl_gravity, ppTempEntFree, ppTempEntActive, Callback_AddVisibleEntity, Callback_TempEntPlaySound);
	}
}

struct cl_entity_s* PluginFuncsPre::HUD_GetUserEntity(int index)
{
	for (size_t i = 0; i < g_Plugins.size(); i++)
	{
		auto p = &g_Plugins.at(i).preCall;

		if (!p->pHUD_GetUserEntity)
			continue;

		p->pHUD_GetUserEntity(index);
	}
	return &cl_entity_t();
}

// From in_camera
void PluginFuncsPre::CAM_Think(void)
{
	for (size_t i = 0; i < g_Plugins.size(); i++)
	{
		auto p = &g_Plugins.at(i).preCall;

		if (!p->pCAM_Think)
			continue;

		p->pCAM_Think();
	}
}

int PluginFuncsPre::CL_IsThirdPerson(void)
{
	for (size_t i = 0; i < g_Plugins.size(); i++)
	{
		auto p = &g_Plugins.at(i).preCall;

		if (!p->pCL_IsThirdPerson)
			continue;

		p->pCL_IsThirdPerson();
	}
	return 0;
}

void PluginFuncsPre::CL_CameraOffset(float* ofs)
{
	for (size_t i = 0; i < g_Plugins.size(); i++)
	{
		auto p = &g_Plugins.at(i).preCall;

		if (!p->pCL_CameraOffset)
			continue;

		p->pCL_CameraOffset(ofs);
	}
}

// From input
struct kbutton_s PluginFuncsPre::KB_Find(const char* name)
{
	for (size_t i = 0; i < g_Plugins.size(); i++)
	{
		auto p = &g_Plugins.at(i).preCall;

		if (!p->pKB_Find)
			continue;

		p->pKB_Find(name);
	}

	return kbutton_t();
}

void PluginFuncsPre::CL_CreateMove(float frametime, struct usercmd_s* cmd, int active)
{
	for (size_t i = 0; i < g_Plugins.size(); i++)
	{
		auto p = &g_Plugins.at(i).preCall;

		if (!p->pCL_CreateMove)
			continue;

		p->pCL_CreateMove(frametime, cmd, active);
	}
}

void PluginFuncsPre::HUD_Shutdown(void)
{
	for (size_t i = 0; i < g_Plugins.size(); i++)
	{
		auto p = &g_Plugins.at(i).preCall;

		if (!p->pHUD_Shutdown)
			continue;

		p->pHUD_Shutdown();
	}
}

int PluginFuncsPre::HUD_Key_Event(int eventcode, int keynum, const char* pszCurrentBinding)
{
	for (size_t i = 0; i < g_Plugins.size(); i++)
	{
		auto p = &g_Plugins.at(i).preCall;

		if (!p->pHUD_Key_Event)
			continue;

		p->pHUD_Key_Event(eventcode, keynum, pszCurrentBinding);
	}
	return 1;
}

// From inputw32
void PluginFuncsPre::IN_ActivateMouse(void)
{
	for (size_t i = 0; i < g_Plugins.size(); i++)
	{
		auto p = &g_Plugins.at(i).preCall;

		if (!p->pIN_ActivateMouse)
			continue;

		p->pIN_ActivateMouse();
	}
}

void PluginFuncsPre::IN_DeactivateMouse(void)
{
	for (size_t i = 0; i < g_Plugins.size(); i++)
	{
		auto p = &g_Plugins.at(i).preCall;

		if (!p->pIN_DeactivateMouse)
			continue;

		p->pIN_DeactivateMouse();
	}
}

void PluginFuncsPre::IN_MouseEvent(int mstate)
{
	for (size_t i = 0; i < g_Plugins.size(); i++)
	{
		auto p = &g_Plugins.at(i).preCall;

		if (!p->pIN_MouseEvent)
			continue;

		p->pIN_MouseEvent(mstate);
	}
}

void PluginFuncsPre::IN_Accumulate(void)
{
	for (size_t i = 0; i < g_Plugins.size(); i++)
	{
		auto p = &g_Plugins.at(i).preCall;

		if (!p->pIN_Accumulate)
			continue;

		p->pIN_Accumulate();
	}
}

void PluginFuncsPre::IN_ClearStates(void)
{
	for (size_t i = 0; i < g_Plugins.size(); i++)
	{
		auto p = &g_Plugins.at(i).preCall;

		if (!p->pIN_ClearStates)
			continue;

		p->pIN_ClearStates();
	}
}


// From tri
void PluginFuncsPre::HUD_DrawNormalTriangles(void)
{
	for (size_t i = 0; i < g_Plugins.size(); i++)
	{
		auto p = &g_Plugins.at(i).preCall;

		if (!p->pHUD_DrawNormalTriangles)
			continue;

		p->pHUD_DrawNormalTriangles();
	}
}

void PluginFuncsPre::HUD_DrawTransparentTriangles(void)
{
	for (size_t i = 0; i < g_Plugins.size(); i++)
	{
		auto p = &g_Plugins.at(i).preCall;

		if (!p->pHUD_DrawTransparentTriangles)
			continue;

		p->pHUD_DrawTransparentTriangles();
	}
}

// From view
void PluginFuncsPre::V_CalcRefdef(struct ref_params_s* pparams)
{
	for (size_t i = 0; i < g_Plugins.size(); i++)
	{
		auto p = &g_Plugins.at(i).preCall;

		if (!p->pV_CalcRefdef)
			continue;

		p->pV_CalcRefdef(pparams);
	}
}

// From GameStudioModelRenderer
int PluginFuncsPre::HUD_GetStudioModelInterface(int version, struct r_studio_interface_s** ppinterface, struct engine_studio_api_s* pstudio)
{
	for (size_t i = 0; i < g_Plugins.size(); i++)
	{
		auto p = &g_Plugins.at(i).preCall;

		if (!p->pHUD_GetStudioModelInterface)
			continue;

		p->pHUD_GetStudioModelInterface(version, ppinterface, pstudio);
	}
	return 0;
}