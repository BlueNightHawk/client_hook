#define CLFUNCS
#include "defs.h"
#include "funcptrs.h"
#include "SDL2/SDL.h"
#include "kbutton.h"
#include "r_studioint.h"

#include "interface.h"
#include "FileSystem.h"
#include "filesystem_utils.h"

#include "APIProxy.h"

CSysModule* hClientDLL;

HMODULE LoadClient(const char* pszDllName)
{
	g_pFileSystem->GetLocalCopy(pszDllName);
	hClientDLL = Sys_LoadModule(pszDllName);

	return (HMODULE)hClientDLL;
}

void UnloadClient()
{
	Sys_UnloadModule(hClientDLL);
}
