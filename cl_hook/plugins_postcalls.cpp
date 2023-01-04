#include "Platform.h"
#include "mathlib.h"
#include "vector.h"
#include "cdll_int.h"

#include "PlatformHeaders.h"
#include "Platform.h"
#include <filesystem>
#include "funcptrs.h"
#include <vector.h>
#include <string.h>

#include "kbutton.h"

extern std::vector<plugindll_t> g_Plugins;

using namespace PluginManager;

// From hl_weapons
void PluginFuncsPost::HUD_PostRunCmd(struct local_state_s* from, struct local_state_s* to, struct usercmd_s* cmd, int runfuncs, double time, unsigned int random_seed)
{
	for (size_t i = 0; i < g_Plugins.size(); i++)
	{
		auto p = &g_Plugins.at(i).postCall;

		if (!p->pHUD_PostRunCmd)
			continue;

		p->pHUD_PostRunCmd(from, to, cmd, runfuncs, time, random_seed);
	}
}

// From cdll_int
int PluginFuncsPost::Initialize(cl_enginefunc_t* pEnginefuncs, int iVersion)
{
	for (size_t i = 0; i < g_Plugins.size(); i++)
	{
		auto p = &g_Plugins.at(i).postCall;

		if (!p->pInitialize)
			continue;

		p->pInitialize(pEnginefuncs, iVersion);
	}
	return 1;
}

int PluginFuncsPost::HUD_VidInit(void)
{
	for (size_t i = 0; i < g_Plugins.size(); i++)
	{
		auto p = &g_Plugins.at(i).postCall;

		if (!p->pHUD_VidInit)
			continue;

		p->pHUD_VidInit();
	}
	return 1;
}

void PluginFuncsPost::HUD_Init(void)
{
	for (size_t i = 0; i < g_Plugins.size(); i++)
	{
		auto p = &g_Plugins.at(i).postCall;

		if (!p->pHUD_Init)
			continue;

		p->pHUD_Init();
	}
}

int PluginFuncsPost::HUD_Redraw(float flTime, int intermission)
{
	for (size_t i = 0; i < g_Plugins.size(); i++)
	{
		auto p = &g_Plugins.at(i).postCall;

		if (!p->pHUD_Redraw)
			continue;

		p->pHUD_Redraw(flTime, intermission);
	}
	return 1;
}

int PluginFuncsPost::HUD_UpdateClientData(client_data_t* cdata, float flTime)
{
	for (size_t i = 0; i < g_Plugins.size(); i++)
	{
		auto p = &g_Plugins.at(i).postCall;

		if (!p->pHUD_UpdateClientData)
			continue;

		p->pHUD_UpdateClientData(cdata, flTime);
	}
	return 1;
}

void PluginFuncsPost::HUD_Reset(void)
{
	for (size_t i = 0; i < g_Plugins.size(); i++)
	{
		auto p = &g_Plugins.at(i).postCall;

		if (!p->pHUD_Reset)
			continue;

		p->pHUD_Reset();
	}
}

void PluginFuncsPost::HUD_PlayerMove(struct playermove_s* ppmove, int server)
{
	for (size_t i = 0; i < g_Plugins.size(); i++)
	{
		auto p = &g_Plugins.at(i).postCall;

		if (!p->pHUD_PlayerMove)
			continue;

		p->pHUD_PlayerMove(ppmove, server);
	}
}

void PluginFuncsPost::HUD_PlayerMoveInit(struct playermove_s* ppmove)
{
	for (size_t i = 0; i < g_Plugins.size(); i++)
	{
		auto p = &g_Plugins.at(i).postCall;

		if (!p->pHUD_PlayerMove)
			continue;

		p->pHUD_PlayerMoveInit(ppmove);
	}
}

char PluginFuncsPost::HUD_PlayerMoveTexture(char* name)
{
	for (size_t i = 0; i < g_Plugins.size(); i++)
	{
		auto p = &g_Plugins.at(i).postCall;

		if (!p->pHUD_PlayerMoveTexture)
			continue;

		p->pHUD_PlayerMoveTexture(name);
	}

	return 0;
}

int PluginFuncsPost::HUD_ConnectionlessPacket(const struct netadr_s* net_from, const char* args, char* response_buffer, int* response_buffer_size)
{
	for (size_t i = 0; i < g_Plugins.size(); i++)
	{
		auto p = &g_Plugins.at(i).postCall;

		if (!p->pHUD_ConnectionlessPacket)
			continue;

		p->pHUD_ConnectionlessPacket(net_from, args, response_buffer, response_buffer_size);
	}
	return 1;
}

int PluginFuncsPost::HUD_GetHullBounds(int hullnumber, float* mins, float* maxs)
{
	for (size_t i = 0; i < g_Plugins.size(); i++)
	{
		auto p = &g_Plugins.at(i).postCall;

		if (!p->pHUD_GetHullBounds)
			continue;

		p->pHUD_GetHullBounds(hullnumber, mins, maxs);
	}

	return 0;
}

void PluginFuncsPost::HUD_Frame(double time)
{
	for (size_t i = 0; i < g_Plugins.size(); i++)
	{
		auto p = &g_Plugins.at(i).postCall;

		if (!p->pHUD_Frame)
			continue;

		p->pHUD_Frame(time);
	}
}

void PluginFuncsPost::HUD_VoiceStatus(int entindex, qboolean bTalking)
{
	for (size_t i = 0; i < g_Plugins.size(); i++)
	{
		auto p = &g_Plugins.at(i).postCall;

		if (!p->pHUD_VoiceStatus)
			continue;

		p->pHUD_VoiceStatus(entindex, bTalking);
	}
}

void PluginFuncsPost::HUD_DirectorMessage(int iSize, void* pbuf)
{
	for (size_t i = 0; i < g_Plugins.size(); i++)
	{
		auto p = &g_Plugins.at(i).postCall;

		if (!p->pHUD_DirectorMessage)
			continue;

		p->pHUD_DirectorMessage(iSize, pbuf);
	}
}
void PluginFuncsPost::HUD_ChatInputPosition(int* x, int* y)
{
	for (size_t i = 0; i < g_Plugins.size(); i++)
	{
		auto p = &g_Plugins.at(i).postCall;

		if (!p->pHUD_ChatInputPosition)
			continue;

		p->pHUD_ChatInputPosition(x, y);
	}
}

// From demo
void PluginFuncsPost::Demo_ReadBuffer(int size, unsigned char* buffer)
{
	for (size_t i = 0; i < g_Plugins.size(); i++)
	{
		auto p = &g_Plugins.at(i).postCall;

		if (!p->pDemo_ReadBuffer)
			continue;

		p->pDemo_ReadBuffer(size, buffer);
	}
}

// From entity
int PluginFuncsPost::HUD_AddEntity(int type, struct cl_entity_s* ent, const char* modelname)
{
	for (size_t i = 0; i < g_Plugins.size(); i++)
	{
		auto p = &g_Plugins.at(i).postCall;

		if (!p->pHUD_AddEntity)
			continue;

		p->pHUD_AddEntity(type, ent, modelname);
	}
	return 1;
}

void PluginFuncsPost::HUD_CreateEntities(void)
{
	for (size_t i = 0; i < g_Plugins.size(); i++)
	{
		auto p = &g_Plugins.at(i).postCall;

		if (!p->pHUD_CreateEntities)
			continue;

		p->pHUD_CreateEntities();
	}
}

void PluginFuncsPost::HUD_StudioEvent(const struct mstudioevent_s* event, const struct cl_entity_s* entity)
{
	for (size_t i = 0; i < g_Plugins.size(); i++)
	{
		auto p = &g_Plugins.at(i).postCall;

		if (!p->pHUD_StudioEvent)
			continue;

		p->pHUD_StudioEvent(event, entity);
	}
}

void PluginFuncsPost::HUD_TxferLocalOverrides(struct entity_state_s* state, const struct clientdata_s* client)
{
	for (size_t i = 0; i < g_Plugins.size(); i++)
	{
		auto p = &g_Plugins.at(i).postCall;

		if (!p->pHUD_TxferLocalOverrides)
			continue;

		p->pHUD_TxferLocalOverrides(state, client);
	}
}

void PluginFuncsPost::HUD_ProcessPlayerState(struct entity_state_s* dst, const struct entity_state_s* src)
{
	for (size_t i = 0; i < g_Plugins.size(); i++)
	{
		auto p = &g_Plugins.at(i).postCall;

		if (!p->pHUD_ProcessPlayerState)
			continue;

		p->pHUD_ProcessPlayerState(dst, src);
	}
}

void PluginFuncsPost::HUD_TxferPredictionData(struct entity_state_s* ps, const struct entity_state_s* pps, struct clientdata_s* pcd, const struct clientdata_s* ppcd, struct weapon_data_s* wd, const struct weapon_data_s* pwd)
{
	for (size_t i = 0; i < g_Plugins.size(); i++)
	{
		auto p = &g_Plugins.at(i).postCall;

		if (!p->pHUD_TxferPredictionData)
			continue;

		p->pHUD_TxferPredictionData(ps, pps, pcd, ppcd, wd, pwd);
	}
}

void PluginFuncsPost::HUD_TempEntUpdate(double frametime, double client_time, double cl_gravity, struct tempent_s** ppTempEntFree, struct tempent_s** ppTempEntActive, int (*Callback_AddVisibleEntity)(struct cl_entity_s* pEntity), void (*Callback_TempEntPlaySound)(struct tempent_s* pTemp, float damp))
{
	for (size_t i = 0; i < g_Plugins.size(); i++)
	{
		auto p = &g_Plugins.at(i).postCall;

		if (!p->pHUD_TempEntUpdate)
			continue;

		p->pHUD_TempEntUpdate(frametime, client_time, cl_gravity, ppTempEntFree, ppTempEntActive, Callback_AddVisibleEntity, Callback_TempEntPlaySound);
	}
}

struct cl_entity_s* PluginFuncsPost::HUD_GetUserEntity(int index)
{
	for (size_t i = 0; i < g_Plugins.size(); i++)
	{
		auto p = &g_Plugins.at(i).postCall;

		if (!p->pHUD_GetUserEntity)
			continue;

		p->pHUD_GetUserEntity(index);
	}
	return &cl_entity_t();
}

// From in_camera
void PluginFuncsPost::CAM_Think(void)
{
	for (size_t i = 0; i < g_Plugins.size(); i++)
	{
		auto p = &g_Plugins.at(i).postCall;

		if (!p->pCAM_Think)
			continue;

		p->pCAM_Think();
	}
}

int PluginFuncsPost::CL_IsThirdPerson(void)
{
	for (size_t i = 0; i < g_Plugins.size(); i++)
	{
		auto p = &g_Plugins.at(i).postCall;

		if (!p->pCL_IsThirdPerson)
			continue;

		p->pCL_IsThirdPerson();
	}
	return 0;
}

void PluginFuncsPost::CL_CameraOffset(float* ofs)
{
	for (size_t i = 0; i < g_Plugins.size(); i++)
	{
		auto p = &g_Plugins.at(i).postCall;

		if (!p->pCL_CameraOffset)
			continue;

		p->pCL_CameraOffset(ofs);
	}
}

// From input
struct kbutton_s PluginFuncsPost::KB_Find(const char* name)
{
	for (size_t i = 0; i < g_Plugins.size(); i++)
	{
		auto p = &g_Plugins.at(i).postCall;

		if (!p->pKB_Find)
			continue;

		p->pKB_Find(name);
	}

	return kbutton_t();
}

void PluginFuncsPost::CL_CreateMove(float frametime, struct usercmd_s* cmd, int active)
{
	for (size_t i = 0; i < g_Plugins.size(); i++)
	{
		auto p = &g_Plugins.at(i).postCall;

		if (!p->pCL_CreateMove)
			continue;

		p->pCL_CreateMove(frametime, cmd, active);
	}
}

void PluginFuncsPost::HUD_Shutdown(void)
{
	for (size_t i = 0; i < g_Plugins.size(); i++)
	{
		auto p = &g_Plugins.at(i).postCall;

		if (!p->pHUD_Shutdown)
			continue;

		p->pHUD_Shutdown();
	}
}

int PluginFuncsPost::HUD_Key_Event(int eventcode, int keynum, const char* pszCurrentBinding)
{
	for (size_t i = 0; i < g_Plugins.size(); i++)
	{
		auto p = &g_Plugins.at(i).postCall;

		if (!p->pHUD_Key_Event)
			continue;

		p->pHUD_Key_Event(eventcode, keynum, pszCurrentBinding);
	}
	return 1;
}

// From inputw32
void PluginFuncsPost::IN_ActivateMouse(void)
{
	for (size_t i = 0; i < g_Plugins.size(); i++)
	{
		auto p = &g_Plugins.at(i).postCall;

		if (!p->pIN_ActivateMouse)
			continue;

		p->pIN_ActivateMouse();
	}
}

void PluginFuncsPost::IN_DeactivateMouse(void)
{
	for (size_t i = 0; i < g_Plugins.size(); i++)
	{
		auto p = &g_Plugins.at(i).postCall;

		if (!p->pIN_DeactivateMouse)
			continue;

		p->pIN_DeactivateMouse();
	}
}

void PluginFuncsPost::IN_MouseEvent(int mstate)
{
	for (size_t i = 0; i < g_Plugins.size(); i++)
	{
		auto p = &g_Plugins.at(i).postCall;

		if (!p->pIN_MouseEvent)
			continue;

		p->pIN_MouseEvent(mstate);
	}
}

void PluginFuncsPost::IN_Accumulate(void)
{
	for (size_t i = 0; i < g_Plugins.size(); i++)
	{
		auto p = &g_Plugins.at(i).postCall;

		if (!p->pIN_Accumulate)
			continue;

		p->pIN_Accumulate();
	}
}

void PluginFuncsPost::IN_ClearStates(void)
{
	for (size_t i = 0; i < g_Plugins.size(); i++)
	{
		auto p = &g_Plugins.at(i).postCall;

		if (!p->pIN_ClearStates)
			continue;

		p->pIN_ClearStates();
	}
}


// From tri
void PluginFuncsPost::HUD_DrawNormalTriangles(void)
{
	for (size_t i = 0; i < g_Plugins.size(); i++)
	{
		auto p = &g_Plugins.at(i).postCall;

		if (!p->pHUD_DrawNormalTriangles)
			continue;

		p->pHUD_DrawNormalTriangles();
	}
}

void PluginFuncsPost::HUD_DrawTransparentTriangles(void)
{
	for (size_t i = 0; i < g_Plugins.size(); i++)
	{
		auto p = &g_Plugins.at(i).postCall;

		if (!p->pHUD_DrawTransparentTriangles)
			continue;

		p->pHUD_DrawTransparentTriangles();
	}
}

// From view
void PluginFuncsPost::V_CalcRefdef(struct ref_params_s* pparams)
{
	for (size_t i = 0; i < g_Plugins.size(); i++)
	{
		auto p = &g_Plugins.at(i).postCall;

		if (!p->pV_CalcRefdef)
			continue;

		p->pV_CalcRefdef(pparams);
	}
}

// From GameStudioModelRenderer
int PluginFuncsPost::HUD_GetStudioModelInterface(int version, struct r_studio_interface_s** ppinterface, struct engine_studio_api_s* pstudio)
{
	for (size_t i = 0; i < g_Plugins.size(); i++)
	{
		auto p = &g_Plugins.at(i).postCall;

		if (!p->pHUD_GetStudioModelInterface)
			continue;

		p->pHUD_GetStudioModelInterface(version, ppinterface, pstudio);
	}
	return 0;
}