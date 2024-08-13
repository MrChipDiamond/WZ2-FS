#include"unlock.h"
#include"memory.h"
#include <thread>
#include "xor.hpp"











int GetLogonStatus(int status) {
	return reinterpret_cast<int(*)(int)>(g_vars->goffsets.logonstatus)(status);
}

void init_UnlockAll()
{
	g_vars = new global_vars();
	g_vars->baseModule = bMode;

	g_vars->goffsets.logonstatus = Scanner::Scanner(g_vars->baseModule + 0x3000000, (g_vars->baseModule + 0x3000000) + (iSize - 0x3000000), ("E8 ? ? ? ? 83 F8 02 0F 84 ? ? ? ? 48 89"), xorstr("LogonStatus"), false).resolve_call();


	while (GetLogonStatus(0) != 2)
	{
		std::this_thread::sleep_for(
			std::chrono::milliseconds(5));

	}

	g_vars->goffsets.unlock_offsets = Scanner::Scanner(g_vars->baseModule + 0x500000, (g_vars->baseModule + 0x500000) + (iSize - 0x500000), ("33 FF 48 8D 15 ? ? ? ? 48 89 05 ? ? ? ? 48 8D 0D"), (xorstr("unlock_offsets ")), false).getaddr();

}


void unlockAll()
{
	uintptr_t num = (g_vars->goffsets.unlock_offsets + 0xC);
	int num2 = memory::readMemory<int>(num);

	uintptr_t unlock_base = num + num2 + 4 - g_vars->baseModule;
	uintptr_t numP = (g_vars->baseModule + unlock_base + 0x60);

	memcpy((BYTE*)numP, (BYTE*)("\x48\x83\xC4\x08\x48\x8B\x5C\x24\x30\x48\x8B\x74\x24\x38\x48\x83\xC4\x20\x5F\x48\xC7\xC0\x01\x00\x00\x00\xC3"), 28);
	memory::writeMemory<uintptr_t>(g_vars->baseModule + unlock_base + 8, numP);
	memory::writeMemory<uintptr_t>(g_vars->baseModule + unlock_base, g_vars->baseModule + unlock_base);
}
