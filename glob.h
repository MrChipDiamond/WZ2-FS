#pragma once
#include<Windows.h>
typedef struct _settings
{
	bool b_menuEnable = false;
	bool b_debugEnable = true;


} settings;

typedef struct _offsets
{
	uintptr_t unlock_offsets;
	uintptr_t clantag_offset;
	uintptr_t logonstatus;

} offsets;

typedef struct _global_vars
{
	DWORD mwprocID;
	DWORD vgprocID;
	HWND hWind;
	HANDLE hProc;
	uintptr_t baseModule;

	offsets goffsets;
	settings msettings;

} global_vars;

extern global_vars* g_vars;