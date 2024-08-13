#pragma once
#include"memory.h"
#include <thread>


inline std::ptrdiff_t bMode = (std::ptrdiff_t)memory::modinfo(memory::info::modbase);
inline std::ptrdiff_t iSize = (std::ptrdiff_t)memory::modinfo(memory::info::imagesize);

struct offsets_s
{
	uintptr_t unlock_offsets;
	uintptr_t clantag_offset;
	uintptr_t logonstatus;
	uintptr_t FindDvarByName;

};



struct global_vars
{
	DWORD mwprocID;
	DWORD vgprocID;
	HWND hWind;
	HANDLE hProc;
	uintptr_t baseModule;

	offsets_s goffsets;
};

inline global_vars* g_vars;









extern int GetLogonStatus(int status);

extern void init_UnlockAll();



extern void unlockAll();