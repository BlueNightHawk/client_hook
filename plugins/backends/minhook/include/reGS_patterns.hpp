// BunnymodXT contributors & SmileyAG provided patterns.

#ifdef REGS_PATTERNS_HPP_RECURSE_GUARD
#error Recursive header files inclusion detected in reGS_patterns.hpp
#else //REGS_PATTERNS_HPP_RECURSE_GUARD

#define REGS_PATTERNS_HPP_RECURSE_GUARD

#ifndef REGS_PATTERNS_HPP_GUARD
#define REGS_PATTERNS_HPP_GUARD
#pragma once

#include "patterns.hpp"
#include "MemUtils.h"

namespace patterns
{
namespace engine
{
PATTERNS(AdjustSubRect,
	"HL-SteamPipe-8684",
	"55 8B EC 83 EC 10 53 8B 5D ?? 56 57 53");

PATTERNS(build_number,
	"HL-SteamPipe-8684",
	"55 8B EC 83 EC 08 A1 ?? ?? ?? ?? 56 33 F6 85 C0 0F 85 9B 00 00 00 53 33 DB 8B 04 9D ?? ?? ?? ?? 8B 0D ?? ?? ?? ?? 6A 03 50 51 E8");

PATTERNS(BoxFilter3x3,
	"HL-SteamPipe-8684",
	"55 8B EC 83 EC 1C 8B 45 ?? 53 56 57 8D 78 ??");

PATTERNS(BuildGammaTable,
	"HL-SteamPipe-8684",
	"55 8B EC 83 EC 14 D9 45 ??");

PATTERNS(BUsesSDLInput,
	"HL-SteamPipe-8684",
	"A0 ?? ?? ?? ?? 53 84 C0 0F 85");

PATTERNS(CBaseUI__HideGameUI,
	"HL-SteamPipe-8684",
	"56 8B F1 8B 0D ?? ?? ?? ?? 8B 01 FF 50 ?? 8B 0D ?? ?? ?? ?? 8B 11 FF 52 ?? FF 15");

PATTERNS(Cbuf_AddText,
	"HL-SteamPipe-8684",
	"55 8B EC 8B 45 08 68 ?? ?? ?? ?? 50 E8 ?? ?? ?? ?? 83 C4 08 5D C3 90 90 90 90 90 90 90 90 90 90 55 8B EC 56 57 8B 7D 08 57 E8");

PATTERNS(Cbuf_Execute,
	"HL-SteamPipe-8684",
	"6A 01 68 ?? ?? ?? ?? E8 ?? ?? ?? ?? 6A 01 6A 00 68 ?? ?? ?? ?? E8 ?? ?? ?? ?? 83 C4 14 C3");

PATTERNS(Cbuf_InsertText,
	"HL-SteamPipe-8684",
	"55 8B EC 8B 45 08 6A 00 68 ?? ?? ?? ?? 50 E8 ?? ?? ?? ?? 83 C4 0C 5D C3");

PATTERNS(CL_CheckGameDirectory,
	"HL-SteamPipe-8684",
	"55 8B EC 81 EC 04 01 00 00 56 8B 75");

PATTERNS(CL_DemoAPIRecording,
	"HL-SteamPipe-8684",
	"8B 0D ?? ?? ?? ?? 33 C0 85 C9 0F 95 C0 C3 90 90 8B 0D ?? ?? ?? ?? 33 C0 85 C9 0F 95 C0 C3 90 90 8B");

PATTERNS(CL_EmitEntities,
	"HL-SteamPipe-8684",
	"A1 ?? ?? ?? ?? C7 05 ?? ?? ?? ?? 00 00 00 00 83 F8 05");

PATTERNS(CL_Move,
	"HL-SteamPipe-8684",
	"55 8B EC 81 EC 78 08 00 00 A1 ?? ?? ?? ?? 53 56 57 33 FF 3B C7 0F 84 49 06 00 00 83 F8 01 0F 84 40 06 00 00 83 F8 02 0F 84 37 06 00 00");

PATTERNS(CL_ParseServerInfo,
	"HL-SteamPipe-8684",
	"55 8B EC 81 EC 18 01 00 00 D9 05");

PATTERNS(CL_Record_f,
	"HL-SteamPipe-8684",
	"55 8B EC 81 EC 18 03 00 00 53 56 57 E8 ?? ?? ?? ?? 83 F8 02 7D 14 68 ?? ?? ?? ?? E8");

PATTERNS(CL_RecordHUDCommand,
	"HL-SteamPipe-8684",
	"55 8B EC 83 EC 4C C6 45 B4 03 E8 ?? ?? ?? ?? D8 25 ?? ?? ?? ?? 51 D9 1C 24 FF 15 ?? ?? ?? ?? A1");

PATTERNS(CL_Stop_f,
	"HL-SteamPipe-8684",
	"55 8B EC 81 EC 18 02 00 00 A1 ?? ?? ?? ?? 56 83 F8 01 0F 85 ?? 02 00 00 A1 ?? ?? ?? ?? 33 F6 3B");

PATTERNS(CL_TakeSnapshot_f,
	"HL-SteamPipe-8684",
	"55 8B EC 81 EC 80 00 00 00 A1 ?? ?? ?? ?? 85 C0");

PATTERNS(ClientDLL_CheckStudioInterface,
	"HL-SteamPipe-8684",
	"55 8B EC E8 ?? ?? ?? ?? A1 ?? ?? ?? ?? 85 C0");

PATTERNS(ClientDLL_Init,
	"HL-SteamPipe-8684",
	"55 8B EC 81 EC 00 02 00 00 68");

PATTERNS(Cmd_AddMallocCommand,
	"HL-SteamPipe-8684",
	"55 8B EC 56 57 8B 7D 08 57 E8 ?? ?? ?? ?? 8A 08 83 C4 04 84 C9 74 12 57 68 ?? ?? ?? ?? E8 ?? ?? ?? ?? 83 C4 08 5F 5E 5D C3 8B 35");

PATTERNS(Cmd_Exec_f,
	"HL-SteamPipe-8684",
	"55 8B EC 81 EC ?? ?? ?? ?? 53 56 57 E8 ?? ?? ?? ?? 83 F8 02 74 14 68 ?? ?? ?? ?? E8 ?? ?? ?? ?? 83 C4 04 5F 5E 5B 8B E5 5D C3 6A 01 E8 ?? ?? ?? ?? 8B F0 83 C4 04 85 F6 0F 84 ?? ?? ?? ?? 68");

PATTERNS(Cmd_ExecuteStringWithPrivilegeCheck,
	"HL-SteamPipe-8684",
	"55 8B EC 83 EC 14 8B 45 0C 53 56 57 8B 7D 08 A3 ?? ?? ?? ?? 57 E8 ?? ?? ?? ?? 83 C4 04 E8 ?? ?? ?? ?? 85 C0");

PATTERNS(Cmd_FindCmd,
	"HL-SteamPipe-8684",
	"55 8B EC 56 8B 35 ?? ?? ?? ?? 85 F6 57 74 ?? 8B 7D ?? 8B 46 ?? 50 57 E8 ?? ?? ?? ?? 83 C4 08 85 C0 74 ?? 8B 36 85 F6 75 ?? 5F 33 C0 5E 5D C3 8B C6");

PATTERNS(ComputeScaledSize,
	"HL-SteamPipe-8684",
	"55 8B EC 51 A1 ?? ?? ?? ?? 56 85 C0 74 ?? 8B 45 ??");

PATTERNS(Cvar_DirectSet,
	"HL-SteamPipe-8684",
	"55 8B EC 81 EC ?? ?? ?? ?? 56 8B 75 0C 57 8B 7D 08 85 FF 0F 84 ?? ?? ?? ?? 85 F6 0F 84 ?? ?? ?? ?? ?? 47 08");

PATTERNS(Cvar_FindVar,
	"HL-SteamPipe-8684",
	"55 8B EC 8D 45 08 56 50 FF ?? ?? ?? ?? ?? 8B 35 ?? ?? ?? ?? 83 C4 04 85 F6 74 1A 8B 0E 8B 55 08 51 52 E8");

PATTERNS(Cvar_RegisterVariable,
	"HL-SteamPipe-8684",
	"55 8B EC 83 EC 14 53 56 8B 75 08 57 8B 06 50 E8 ?? ?? ?? ?? 83 C4 04 85 C0 74 17 8B 0E 51 68");

PATTERNS(DispatchDirectUserMsg,
	"HL-SteamPipe-8684",
	"55 8B EC 53 56 8B 35 ?? ?? ?? ?? 33 DB");

PATTERNS(DrawCrosshair,
	"HL-SteamPipe-8684",
	"55 8B EC A1 ?? ?? ?? ?? 85 C0 74 5A 8B 0D ?? ?? ?? ?? 8B 15 ?? ?? ?? ?? 51 8B 0D");

PATTERNS(Draw_Frame,
	"HL-SteamPipe-8684",
	"55 8B EC 83 EC 14 DB 45 ?? 53");

PATTERNS(Draw_FillRGBA,
	"HL-SteamPipe-8684",
	"55 8B EC 83 EC 08 8D 45 ?? 8D 4D ?? 50 8D 55 ?? 51 8D 45 ?? 52 8D 4D ?? 50 8D 55 ?? 51 8D 45 ?? 52 8D 4D ?? 50 51 FF 15 ?? ?? ?? ?? 83 C4 20 68 E1 0D 00 00 FF 15 ?? ?? ?? ?? 68 E2 0B 00 00 FF 15 ?? ?? ?? ?? 68 00 00 04 46 68 00 22 00 00 68 00 23 00 00 FF 15 ?? ?? ?? ?? 6A 01");

PATTERNS(Draw_String,
	"HL-SteamPipe-8684",
	"55 8B EC 56 57 E8 ?? ?? ?? ?? 8B 4D ??");

PATTERNS(EmitWaterPolys,
	"HL-SteamPipe-8684",
	"55 8B EC 83 EC 20 56 8B 75 ?? 33 D2");

PATTERNS(ED_ParseEdict,
	"HL-SteamPipe-8684",
	"55 8B EC 81 EC 10 01 00 00 A1 ?? ?? ?? ?? 56 8B 75");

PATTERNS(GL_Bind,
	"HL-SteamPipe-8684",
	"55 8B EC 8B 45 ?? 8B 0D ?? ?? ?? ?? 56");

PATTERNS(GL_ResampleTexture,
	"HL-SteamPipe-8684",
	"55 8B EC B8 14 20 00 00 E8 ?? ?? ?? ?? 8B 45 ??");

PATTERNS(GL_ResampleAlphaTexture,
	"HL-SteamPipe-8684",
	"55 8B EC 81 EC 14 08 00 00");

PATTERNS(GL_Upload32,
	"HL-SteamPipe-8684",
	"55 8B EC 83 EC 14 53 56 8D 45 ??");

PATTERNS(GL_Upload16,
	"HL-SteamPipe-8684",
	"55 8B EC 51 8B 45 ?? 8B 4D ?? 53");

PATTERNS(Host_AutoSave_f,
	"HL-SteamPipe-8684",
	"A1 ?? ?? ?? ?? B9 01 00 00 00 3B C1 0F 85 9F 00 00 00 A1 ?? ?? ?? ?? 85 C0 75 10 68 ?? ?? ?? ?? E8 ?? ?? ?? ?? 83 C4 04 33 C0 C3 39 0D");

PATTERNS(Host_Changelevel2_f,
	"HL-SteamPipe-8684",
	"55 8B EC 81 EC C4 00 00 00 53 56 33 DB 57 89 5D FC C7 05 ?? ?? ?? ?? 04 00 00 00 E8 ?? ?? ?? ?? 83 F8 02 7D 14 68 ?? ?? ?? ?? E8 ?? ?? ?? ?? 83 C4 04 5F 5E 5B 8B E5 5D C3 39 1D ?? ?? ?? ?? 0F 84 7F");

PATTERNS(Host_Frame,
	"HL-SteamPipe-8684",
	"55 8B EC 83 EC 18 56 57 33 FF");

PATTERNS(Host_FilterTime,
	"HL-SteamPipe-8684",
	"55 8B EC 83 EC 08 D9 05 ?? ?? ?? ?? D8 1D ?? ?? ?? ?? DF E0 25 00 41 00 00 75 3F E8 ?? ?? ?? ?? 85 C0 75 09 A1");

PATTERNS(Host_Loadgame_f,
	"HL-SteamPipe-8684",
	"83 3D ?? ?? ?? ?? 01 75 37 E8 ?? ?? ?? ?? 83 F8 02 74 0E 68 ?? ?? ?? ?? E8 ?? ?? ?? ?? 83 C4 04");

PATTERNS(Host_Reload_f,
	"HL-SteamPipe-8684",
	"55 8B EC 81 EC 04 01 00 00 A1 ?? ?? ?? ?? 85 C0 75 65 A1 ?? ?? ?? ?? 85 C0 74 5C 83 3D");

PATTERNS(Host_Tell_f,
	"HL-SteamPipe-8684",
	"55 8B EC 83 EC 40 A1 ?? ?? ?? ?? 56 83 F8 01 75 0A E8 ?? ?? ?? ?? 5E 8B E5 5D C3 E8 ?? ?? ?? ?? 83 F8 03 0F 8C ?? ?? ?? ?? E8");

PATTERNS(IntersectWRect,
	"HL-SteamPipe-8684",
	"55 8B EC 83 EC 10 53 8B 5D ?? 56 57 85 DB");

PATTERNS(IsFlippedViewModel,
	"HL-SteamPipe-8684",
	"A1 ?? ?? ?? ?? 85 C0 74 ?? D9 40");

PATTERNS(Key_Event,
	"HL-SteamPipe-8684",
	"55 8B EC 81 EC 00 04 00 00 8B 45 08 56 3D 00 01 00 00 57 0F 8D 8D 03 00 00 8D 45 0C 8D 4D 08 50");

PATTERNS(LoadThisDll,
	"HL-SteamPipe-8684",
	"55 8B EC 53 8B 5D ?? 57 53 E8 ?? ?? ?? ?? 8B F8");

PATTERNS(MiddleOfSV_ReadClientMessage,
	"HL-SteamPipe-8684",
	"56 E8 ?? ?? ?? ?? 8B 86 9C 4B 00 00 89 35 ?? ?? ?? ?? A3 ?? ?? ?? ?? C7 86 44 25 00 00 FF FF FF FF C7 05");

PATTERNS(MiddleOfSV_RunCmd,
	"HL-SteamPipe-8684",
	"8B 88 90 00 00 00 89 4A 40 8B 15 ?? ?? ?? ?? 83 C2 38 52 68 ?? ?? ?? ?? E8 ?? ?? ?? ?? 83 C4 08 A1");

PATTERNS(Mod_LeafPVS,
	"HL-SteamPipe-8684",
	"55 8B EC 8B 55 0C 8B 45 08 8B 8A 8C 00 00 00 3B C1 75 07 B8 ?? ?? ?? ?? 5D C3 56 8B 35 ?? ?? ?? ?? 85 F6 5E");

PATTERNS(PF_GetPhysicsKeyValue,
	"HL-SteamPipe-8684",
	"55 8B EC 8B 45 08 50 E8 ?? ?? ?? ?? 83 C4 04 83 F8 01 7C 33 3B 05 ?? ?? ?? ?? 7F 2B 8D 0C 80 C1 E1 08 03 C8 8D 14 48 A1");

PATTERNS(PF_traceline_DLL,
	"HL-SteamPipe-8684",
	"55 8B EC 8B 45 14 85 C0 75 05 A1 ?? ?? ?? ?? 8B 4D 0C 8B 55 08 56 50 8B 45 10 50 51 52 E8 ?? ?? ?? ?? D9 05");

PATTERNS(PM_PlayerTrace,
	"HL-SteamPipe-8684",
	"55 8B EC 83 EC 44 8B 45 18 56 57 6A 00 50 A1 ?? ?? ?? ?? 8B 90 4C 02 00 00 8D 88 50 02 00 00 8B 45 14 51 8B 4D 10 52 8B 55 0C 50 51 8D 45 BC 52 50 E8");

PATTERNS(RandomFloat,
	"HL-SteamPipe-8684",
	"55 8B EC 51 8D 45 0C 8D 4D 08 50 51 FF 15 ?? ?? ?? ?? 83 C4 08 E8");

PATTERNS(RandomLong,
	"HL-SteamPipe-8684",
	"55 8B EC 53 8D 45 0C 56 8D 4D 08 50 51 FF 15 ?? ?? ?? ?? 8B 75 0C");

PATTERNS(R_Clear,
	"HL-SteamPipe-8684",
	"8B 15 ?? ?? ?? ?? 33 C0 83 FA 01 0F 9F C0 50 E8 ?? ?? ?? ?? D9 05 ?? ?? ?? ?? DC 1D ?? ?? ?? ?? 83 C4 04 DF E0");

PATTERNS(R_DrawEntitiesOnList,
	"HL-SteamPipe-8684",
	"D9 05 ?? ?? ?? ?? D8 1D ?? ?? ?? ?? 56 DF E0 F6 C4 44 0F 8B ?? ?? ?? ?? 8B 0D");

PATTERNS(R_DrawParticles,
	"HL-SteamPipe-8684",
	"55 8B EC 83 EC 40 A1 ?? ?? ?? ?? 53 56 50");

PATTERNS(R_DrawSequentialPoly,
	"HL-SteamPipe-8684",
	"55 8B EC 51 A1 ?? ?? ?? ?? 53 56 57 83 B8 F8 02 00 00 01 75 63 E8 ?? ?? ?? ?? 68 03 03 00 00 68 02 03 00 00");

PATTERNS(R_DrawSkyChain,
	"HL-SteamPipe-8684",
	"55 8B EC 81 EC 00 03 00 00 E8 ?? ?? ?? ?? 85 C0 0F 85 ?? ?? ?? ?? A3 ?? ?? ?? ?? 8B 45 08");

PATTERNS(R_DrawSkyBox,
	"HL-SteamPipe-8684",
	"55 8B EC 83 EC 1C A1 ?? ?? ?? ?? 53 56");

PATTERNS(R_DrawViewModel,
	"HL-SteamPipe-8684",
	"55 8B EC 83 EC 50 D9 05 ?? ?? ?? ?? D8 1D ?? ?? ?? ?? 56 57 33 FF C7 45");

PATTERNS(R_DrawWorld,
	"HL-SteamPipe-8684",
	"55 8B EC 81 EC B8 0B 00 00");

PATTERNS(R_GLStudioDrawPoints,
	"HL-SteamPipe-8684",
	"55 8B EC 83 EC 48 A1 ?? ?? ?? ?? 8B 0D ?? ?? ?? ?? 53 56 8B 70");

PATTERNS(R_LoadSkys,
	"HL-SteamPipe-8684",
	"55 8B EC 83 EC 6C A1 ?? ?? ?? ?? 56 57 33 FF");

PATTERNS(R_SetFrustum,
	"HL-SteamPipe-8684",
	"55 8B EC 83 EC 08 DB 05");

PATTERNS(R_StudioCalcAttachments,
	"HL-SteamPipe-8684",
	"8B 0D ?? ?? ?? ?? 83 B9 ?? ?? ?? ?? 04");

PATTERNS(R_StudioDrawModel,
	"HL-SteamPipe-8684",
	"55 8B EC 81 EC 78 01 00 00");

PATTERNS(R_StudioRenderModel,
	"HL-SteamPipe-8684",
	"A1 ?? ?? ?? ?? 8B 0D ?? ?? ?? ?? 8B 15 ?? ?? ?? ?? A3 ?? ?? ?? ?? A1 ?? ?? ?? ?? 89 0D ?? ?? ?? ?? 89 15");

PATTERNS(R_StudioSetupBones,
	"HL-SteamPipe-8684",
	"55 8B EC 83 EC 48 A1 ?? ?? ?? ?? 8B 0D ?? ?? ?? ?? 53 56 8B 90");

PATTERNS(S_StartDynamicSound,
	"HL-SteamPipe-8684",
	"55 8B EC 83 EC 48 A1 ?? ?? ?? ?? 53");

PATTERNS(S_FindName,
	"HL-SteamPipe-8684",
	"55 8B EC 53 56 8B 75 ?? 33 DB 85 F6");

PATTERNS(SCR_BeginLoadingPlaque,
	"HL-SteamPipe-8684",
	"6A 01 E8 ?? ?? ?? ?? A1 ?? ?? ?? ?? 83 C4 04 83 F8 03 74 0A 83 F8 04 74 05 83 F8 05 75 ?? 83 3D ?? ?? ?? ?? 02 75 ?? 56 E8");

PATTERNS(SCR_DrawFPS,
	"HL-SteamPipe-8684",
	"D9 05 ?? ?? ?? ?? D8 1D ?? ?? ?? ?? DF E0 F6 C4 ?? ?? ?? DD 05 ?? ?? ?? ?? DC 1D ?? ?? ?? ?? DF E0 F6 C4 41 ?? ?? DD 05");

PATTERNS(SCR_DrawLoading,
	"HL-SteamPipe-8684",
	"A1 ?? ?? ?? ?? 85 C0 74 05 E9 ?? ?? FF FF C3 90");

PATTERNS(SCR_UpdateScreen,
	"HL-SteamPipe-8684",
	"55 8B EC 83 EC 10 A1 ?? ?? ?? ?? 56 33 F6 3B C6 0F 85 10 02 00 00 C7 05 ?? ?? ?? ?? 01 00 00 00 E8");

PATTERNS(SeedRandomNumberGenerator,
	"HL-SteamPipe-8684",
	"6A 00 E8 ?? ?? ?? ?? F7 D8 83 C4 04 3D E8 03 00 00 A3 ?? ?? ?? ?? 7E 08 F7 D8 A3 ?? ?? ?? ?? C3 3D 18 FC FF FF 7E 0A 2D 38 AD 53 01 A3 ?? ?? ?? ?? C3");

PATTERNS(SPR_Pointer,
	"HL-SteamPipe-8684",
	"55 8B EC 8B 45 ?? 8B 08 8B 81");

PATTERNS(ENGSPR_Set,
	"HL-SteamPipe-8684",
	"55 8B EC 83 EC 08 8D 45 ?? 8D 4D ?? 50 8D 55 ?? 51 8D 45 ?? 52 50");

PATTERNS(studioapi_GetCurrentEntity,
	"HL-SteamPipe-8684",
	"A1 ?? ?? ?? ?? C3 90 90 90 90 90 90 90 90 90 90 55 8B EC 8B 45 08 85 C0 7C 18");

PATTERNS(studioapi_StudioGetRotationMatrix,
	"HL-SteamPipe-8684",
	"B8 40 0A 79 02 C3 90 90 90 90 90 90 90 90 90 90");

PATTERNS(SV_AddLinksToPM_,
	"HL-SteamPipe-8684",
	"55 8B EC 83 EC 14 8B 4D 08 53 8B 5D 10 56 57 8B 79 1C 8D 41 18 3B F8 0F 84 ?? ?? ?? ?? 8B 47 04 83 C7 F8 89 45 EC 8B 87 ?? ?? ?? ?? 85 C0 74 35 8B 0D ?? ?? ?? ?? 85 C9 75 14 8B 0D");

PATTERNS(SV_Frame,
	"HL-SteamPipe-8684",
	"A1 ?? ?? ?? ?? 85 C0 74 ?? DD 05 ?? ?? ?? ?? A1 ?? ?? ?? ?? 8B 0D ?? ?? ?? ?? D9 1D ?? ?? ?? ?? A3 ?? ?? ?? ?? 89 0D");

PATTERNS(SV_SetMoveVars,
	"HL-SteamPipe-8684",
	"A1 ?? ?? ?? ?? 8B 0D ?? ?? ?? ?? 8B 15 ?? ?? ?? ?? A3 ?? ?? ?? ?? A1 ?? ?? ?? ?? 89 0D ?? ?? ?? ?? 8B 0D");

PATTERNS(SV_StudioSetupBones,
	"HL-SteamPipe-8684",
	"55 8B EC 81 EC 64 02 00 00 8B 4D ?? A1");

PATTERNS(SV_SpawnServer,
	"HL-SteamPipe-8684",
	"55 8B EC 81 EC 84 00 00 00 A1 ?? ?? ?? ?? 53 33 DB 56");

PATTERNS(SV_WriteEntitiesToClient,
	"HL-SteamPipe-8684",
	"55 8B EC B8 ?? ?? ?? ?? E8 ?? ?? ?? ?? 53 8B 5D 08 56 8B 35 ?? ?? ?? ?? 8B 43 68 8B 93");

PATTERNS(TextMessageDrawCharacter,
	"HL-SteamPipe-8684",
	"55 8B EC 8D 45 ?? 8D 4D ?? 50 8D 55 ?? 51 8D 45 ?? 52 8D 4D ?? 50 8D 55 ?? 51 52");

PATTERNS(V_FadeAlpha,
	"HL-SteamPipe-8684",
	"55 8B EC 83 EC 08 D9 05 ?? ?? ?? ?? DC 1D ?? ?? ?? ?? 8A 0D ?? ?? ?? ?? DF E0 F6 C4 05 7A 1C D9 05 ?? ?? ?? ?? DC 1D");

PATTERNS(ValidateWRect,
	"HL-SteamPipe-8684",
	"8B 54 24 ?? 31 C0 85 D2 74 ?? 8B 4A");

PATTERNS(VGUI2_DrawStringClient,
	"HL-SteamPipe-8684",
	"55 8B EC 8B 0D ?? ?? ?? ?? 53 56 57 8B 01 FF 50 ?? 8B 4D");

PATTERNS(VGUI2_DrawStringReverseClient,
	"HL-SteamPipe-8684",
	"55 8B EC 83 EC 0C 8B 0D ?? ?? ?? ?? 53 56 57 8B 01");

PATTERNS(VGUI2_ResetCurrentTexture,
	"HL-SteamPipe-8684",
	"C7 05 ?? ?? ?? ?? 00 00 00 00 C3 90 90 90 90 90 A1");

PATTERNS(VGuiWrap_Paint,
	"HL-SteamPipe-8684",
	"55 8B EC 83 EC 10 56 E8 ?? ?? ?? ?? 8B F0");

PATTERNS(VGuiWrap2_ConDPrintf,
	"HL-SteamPipe-8684",
	"55 8B EC A1 ?? ?? ?? ?? 85 C0 74 14 8B 55 08 8B 08 52 68 ?? ?? ?? ?? 50 FF 51 1C 83 C4 0C 5D C3 53 56 8B 75 08 57 8B FE 83 C9 FF 33 C0");

PATTERNS(VGuiWrap2_ConPrintf,
	"HL-SteamPipe-8684",
	"55 8B EC A1 ?? ?? ?? ?? 85 C0 74 14 8B 55 08 8B 08 52 68 ?? ?? ?? ?? 50 FF 51 18 83 C4 0C 5D C3 53 56 8B 75 08 57 8B FE 83 C9 FF 33 C0");

PATTERNS(VGuiWrap2_NotifyOfServerConnect,
	"HL-SteamPipe-8684",
	"55 8B EC 8B 0D ?? ?? ?? ?? 33 C0");

PATTERNS(VideoMode_GetCurrentVideoMode,
	"HL-SteamPipe-8684",
	"55 8B EC 8B 0D ?? ?? ?? ?? 8B 01 FF 50 ?? 85 C0");
}
}

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



#endif //REGS_PATTERNS_HPP_GUARD

#undef REGS_PATTERNS_HPP_RECURSE_GUARD
#endif //REGS_PATTERNS_HPP_RECURSE_GUARD