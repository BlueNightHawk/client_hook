// MP3 FIX

#include "defs.h"

typedef void (*_VGuiWrap2_NotifyOfServerConnect)(const char* game, int IP_0, int port);

_VGuiWrap2_NotifyOfServerConnect ORIG_VGuiWrap2_NotifyOfServerConnect = NULL;

void VGuiWrap2_NotifyOfServerConnect(const char* game, int IP_0, int port)
{
	ORIG_VGuiWrap2_NotifyOfServerConnect("valve", IP_0, port);
}

bool MP3Hook()
{
	if (gEngfuncs.CheckParm("-nomp3fix", 0) != 0)
	{
		return false;
	}

	Utils utils = Utils::Utils(NULL, NULL, NULL);


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
