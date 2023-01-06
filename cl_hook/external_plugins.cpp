#include "PlatformHeaders.h"
#include "Platform.h"
#include "mathlib.h"
#include "vector.h"
#include "cdll_int.h"
#include "kbutton.h"
#include "funcptrs.h"

#include <filesystem>
#include <vector>

extern cl_enginefuncs_s gEngfuncs;
extern cl_enginefuncs_s gClientEngfuncs;
std::vector<plugindll_t> g_Plugins;
extern dllfuncs_s g_ClientFuncs;

#define GET_FUNC_PTR_PRE(name) p->p##name## = (_pfn_##name##)GetProcAddress(hDllHandle, #name"_Pre")
#define GET_FUNC_PTR_POST(name) p->p##name## = (_pfn_##name##)GetProcAddress(hDllHandle, #name"_Post")

void plugindll_s::InitDll(const char *pszName)
{
	SetDllDirectory((std::filesystem::current_path().string() + "/" + gEngfuncs.pfnGetGameDirectory() + "/cl_dlls/plugins").c_str());

	hDllHandle = LoadLibrary(pszName);
}

bool plugindll_s::IsHandleValid()
{
	if (!hDllHandle || hDllHandle == INVALID_HANDLE_VALUE)
	{
		return false;
	}
	return true;
}

void plugindll_s::InitExports()
{
	InitExportsPre();
	InitExportsPost();
}

void plugindll_s::Free()
{
	while(FreeLibrary(hDllHandle));
}

void plugindll_s::InitExportsPre()
{
	auto p = &preCall;

	// From hl_weapons
	GET_FUNC_PTR_PRE(HUD_PostRunCmd);

	// From cdll_int
	GET_FUNC_PTR_PRE(Initialize);
	GET_FUNC_PTR_PRE(HUD_VidInit);
	GET_FUNC_PTR_PRE(HUD_Init);
	GET_FUNC_PTR_PRE(HUD_Redraw);
	GET_FUNC_PTR_PRE(HUD_UpdateClientData);
	GET_FUNC_PTR_PRE(HUD_Reset);
	GET_FUNC_PTR_PRE(HUD_PlayerMove);
	GET_FUNC_PTR_PRE(HUD_PlayerMoveInit);
	GET_FUNC_PTR_PRE(HUD_PlayerMoveTexture);
	GET_FUNC_PTR_PRE(HUD_ConnectionlessPacket);
	GET_FUNC_PTR_PRE(HUD_GetHullBounds);
	GET_FUNC_PTR_PRE(HUD_Frame);
	GET_FUNC_PTR_PRE(HUD_VoiceStatus);
	GET_FUNC_PTR_PRE(HUD_DirectorMessage);
	GET_FUNC_PTR_PRE(HUD_ChatInputPosition);

	// From demo
	GET_FUNC_PTR_PRE(Demo_ReadBuffer);

	// From entity
	GET_FUNC_PTR_PRE(HUD_AddEntity);
	GET_FUNC_PTR_PRE(HUD_CreateEntities);
	GET_FUNC_PTR_PRE(HUD_StudioEvent);
	GET_FUNC_PTR_PRE(HUD_TxferLocalOverrides);
	GET_FUNC_PTR_PRE(HUD_ProcessPlayerState);
	GET_FUNC_PTR_PRE(HUD_TxferPredictionData);
	GET_FUNC_PTR_PRE(HUD_TempEntUpdate);
	GET_FUNC_PTR_PRE(HUD_GetUserEntity);

	// From in_camera
	GET_FUNC_PTR_PRE(CAM_Think);
	GET_FUNC_PTR_PRE(CL_IsThirdPerson);
	GET_FUNC_PTR_PRE(CL_CameraOffset);

	// From input
	GET_FUNC_PTR_PRE(KB_Find);
	GET_FUNC_PTR_PRE(CL_CreateMove);
	GET_FUNC_PTR_PRE(HUD_Shutdown);
	GET_FUNC_PTR_PRE(HUD_Key_Event);

	// From inputw32
	GET_FUNC_PTR_PRE(IN_ActivateMouse);
	GET_FUNC_PTR_PRE(IN_DeactivateMouse);
	GET_FUNC_PTR_PRE(IN_MouseEvent);
	GET_FUNC_PTR_PRE(IN_Accumulate);
	GET_FUNC_PTR_PRE(IN_ClearStates);

	// From tri
	GET_FUNC_PTR_PRE(HUD_DrawNormalTriangles);
	GET_FUNC_PTR_PRE(HUD_DrawTransparentTriangles);

	// From view
	GET_FUNC_PTR_PRE(V_CalcRefdef);

	// From GameStudioModelRenderer
	GET_FUNC_PTR_PRE(HUD_GetStudioModelInterface);

	pDLL_SwapWindow = (_pfn_DLL_SwapWindow)GetProcAddress(hDllHandle, "DLL_SwapWindow");
}

void plugindll_s::InitExportsPost()
{
	auto p = &postCall;

	// From hl_weapons
	GET_FUNC_PTR_POST(HUD_PostRunCmd);

	// From cdll_int
	GET_FUNC_PTR_POST(Initialize);
	GET_FUNC_PTR_POST(HUD_VidInit);
	GET_FUNC_PTR_POST(HUD_Init);
	GET_FUNC_PTR_POST(HUD_Redraw);
	GET_FUNC_PTR_POST(HUD_UpdateClientData);
	GET_FUNC_PTR_POST(HUD_Reset);
	GET_FUNC_PTR_POST(HUD_PlayerMove);
	GET_FUNC_PTR_POST(HUD_PlayerMoveInit);
	GET_FUNC_PTR_POST(HUD_PlayerMoveTexture);
	GET_FUNC_PTR_POST(HUD_ConnectionlessPacket);
	GET_FUNC_PTR_POST(HUD_GetHullBounds);
	GET_FUNC_PTR_POST(HUD_Frame);
	GET_FUNC_PTR_POST(HUD_VoiceStatus);
	GET_FUNC_PTR_POST(HUD_DirectorMessage);
	GET_FUNC_PTR_POST(HUD_ChatInputPosition);

	// From demo
	GET_FUNC_PTR_POST(Demo_ReadBuffer);

	// From entity
	GET_FUNC_PTR_POST(HUD_AddEntity);
	GET_FUNC_PTR_POST(HUD_CreateEntities);
	GET_FUNC_PTR_POST(HUD_StudioEvent);
	GET_FUNC_PTR_POST(HUD_TxferLocalOverrides);
	GET_FUNC_PTR_POST(HUD_ProcessPlayerState);
	GET_FUNC_PTR_POST(HUD_TxferPredictionData);
	GET_FUNC_PTR_POST(HUD_TempEntUpdate);
	GET_FUNC_PTR_POST(HUD_GetUserEntity);

	// From in_camera
	GET_FUNC_PTR_POST(CAM_Think);
	GET_FUNC_PTR_POST(CL_IsThirdPerson);
	GET_FUNC_PTR_POST(CL_CameraOffset);

	// From input
	GET_FUNC_PTR_POST(KB_Find);
	GET_FUNC_PTR_POST(CL_CreateMove);
	GET_FUNC_PTR_POST(HUD_Shutdown);
	GET_FUNC_PTR_POST(HUD_Key_Event);

	// From inputw32
	GET_FUNC_PTR_POST(IN_ActivateMouse);
	GET_FUNC_PTR_POST(IN_DeactivateMouse);
	GET_FUNC_PTR_POST(IN_MouseEvent);
	GET_FUNC_PTR_POST(IN_Accumulate);
	GET_FUNC_PTR_POST(IN_ClearStates);

	// From tri
	GET_FUNC_PTR_POST(HUD_DrawNormalTriangles);
	GET_FUNC_PTR_POST(HUD_DrawTransparentTriangles);

	// From view
	GET_FUNC_PTR_POST(V_CalcRefdef);

	// From GameStudioModelRenderer
	GET_FUNC_PTR_POST(HUD_GetStudioModelInterface);
}

void PluginManager::AddToList(char* pszName)
{
	_pfn_InitializePlugin InitializePlugin;
	auto pPlugin = &g_Plugins.emplace_back();

	pPlugin->InitDll(pszName);

	if (pPlugin->IsHandleValid())
	{
		InitializePlugin = (_pfn_InitializePlugin)GetProcAddress(pPlugin->hDllHandle, "InitializePlugin");

		InitializePlugin(&gClientEngfuncs, &g_ClientFuncs, NLHOOK_VERSION);

		pPlugin->InitExports();
	}
}

void PluginManager::InitPlugins()
{
	g_Plugins.clear();

	static char token[128];
	char *afile, *pfile;

	afile = pfile = (char*)gEngfuncs.COM_LoadFile("cl_dlls/nlui_hook.txt", 5, 0);

	if (!afile)
		return;

	while (pfile = gEngfuncs.COM_ParseFile(pfile, token))
	{
		if (!stricmp("plugin", token))
		{
			pfile = gEngfuncs.COM_ParseFile(pfile, token);
			AddToList(token);
		}
	}

	gEngfuncs.COM_FreeFile(afile);
}

void PluginManager::FreePlugins()
{
	for (size_t i = 0; i < g_Plugins.size(); i++)
	{
		g_Plugins[i].Free();
	}
	//g_Plugins.clear();
}