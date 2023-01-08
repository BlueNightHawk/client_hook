#include "PlatformHeaders.h"
#include <string>

int RegRead(const char* valuename);
bool RegWrite(const char* valuename, int value);
bool RegCreate(const char* subkey);

// Read data from registry
BOOL readDwordValueRegistry(HKEY hKeyParent, LPCSTR subkey, LPCSTR valueName, DWORD* readData)
{
	HKEY hKey;
	DWORD Ret;
	// Check if the registry exists
	Ret = RegOpenKeyEx(
		hKeyParent,
		subkey,
		0,
		KEY_READ,
		&hKey);
	if (Ret == ERROR_SUCCESS)
	{
		DWORD data;
		DWORD len = sizeof(DWORD); // size of data
		Ret = RegQueryValueEx(
			hKey,
			valueName,
			NULL,
			NULL,
			(LPBYTE)(&data),
			&len);
		if (Ret == ERROR_SUCCESS)
		{
			RegCloseKey(hKey);
			(*readData) = data;
			return TRUE;
		}
		RegCloseKey(hKey);
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

int RegRead(const char* valuename)
{
	DWORD outvalue = 0;
	readDwordValueRegistry(HKEY_CURRENT_USER, "Software\\Valve\\Half-Life\\Settings", valuename, &outvalue);
	return (int)outvalue;
}

BOOL WriteInRegistry(HKEY hKeyParent, LPCSTR subkey, LPCSTR valueName, DWORD data)
{
	DWORD Ret; // use to check status
	HKEY hKey; // key
	// Open the key
	Ret = RegOpenKeyEx(
		hKeyParent,
		subkey,
		0,
		KEY_WRITE,
		&hKey);
	if (Ret == ERROR_SUCCESS)
	{
		// Set the value in key
		if (ERROR_SUCCESS !=
			RegSetValueEx(
				hKey,
				valueName,
				0,
				REG_DWORD,
				reinterpret_cast<BYTE*>(&data),
				sizeof(data)))
		{
			RegCloseKey(hKey);
			return FALSE;
		}
		// close the key
		RegCloseKey(hKey);
		return TRUE;
	}
	return FALSE;
}

bool RegWrite(const char* valuename, int value)
{
	return WriteInRegistry(HKEY_CURRENT_USER, "Software\\Valve\\Half-Life\\Settings", valuename, (DWORD)value);
}

BOOL CreateRegistryKey(HKEY hKeyParent, LPCSTR subkey)
{
	DWORD dwDisposition; // It verify new key is created or open existing key
	HKEY hKey;
	DWORD Ret;
	Ret =
		RegCreateKeyEx(
			hKeyParent,
			subkey,
			0,
			NULL,
			REG_OPTION_NON_VOLATILE,
			KEY_ALL_ACCESS,
			NULL,
			&hKey,
			&dwDisposition);
	if (Ret != ERROR_SUCCESS)
	{
		printf("Error opening or creating new key\n");
		return FALSE;
	}
	RegCloseKey(hKey); // close the key
	return TRUE;
}

bool RegCreate(const char* subkey)
{
	return CreateRegistryKey(HKEY_CURRENT_USER, ((std::string)("Software\\Valve\\Half-Life\\Settings\\") + subkey).c_str());
}
