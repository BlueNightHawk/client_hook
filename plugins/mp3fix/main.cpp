// Example plugin : HL2 style viewbob and viewlag

#include "PlatformHeaders.h"
#include <Psapi.h>
#include "Platform.h"
#include "mathlib.h"
#include "vector.h"
#include "cdll_int.h"
#include "../cl_hook/funcptrs.h"
#include "Exports.h"

#include "cvardef.h"

#include <string>
#include "include/MinHook.h"
#include "SPTLib/Utils.hpp"
#include "SPTLib/patterns.hpp"
#include "SPTLib/MemUtils.h"
#include <filesystem>

extern cl_enginefunc_t gEngfuncs;
Utils utils = Utils::Utils(NULL, NULL, NULL);

typedef void (*_VGuiWrap2_NotifyOfServerConnect)(const char* game, int IP_0, int port);

_VGuiWrap2_NotifyOfServerConnect ORIG_VGuiWrap2_NotifyOfServerConnect = NULL;

void VGuiWrap2_NotifyOfServerConnect(const char* game, int IP_0, int port)
{
	ORIG_VGuiWrap2_NotifyOfServerConnect("valve", IP_0, port);
}


namespace patterns
{
namespace engine
{
PATTERNS(VGuiWrap2_NotifyOfServerConnect,
	"HL-SteamPipe-8684",
	"55 8B EC 8B 0D ?? ?? ?? ?? 33 C0");
}
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

bool HWHook()
{
	if (gEngfuncs.CheckParm("-nomp3fix", 0) != 0)
	{
		return false;
	}

	void* handle = nullptr;
	void* base = nullptr;
	size_t size = 0;

	if (!MemUtils::GetModuleInfo(L"hw.dll", &handle, &base, &size))
		return false;

	utils = Utils::Utils(handle, base, size);

	Hook(VGuiWrap2_NotifyOfServerConnect);

	MH_EnableHook(MH_ALL_HOOKS);

	return true;
}
