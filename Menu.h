#pragma once
#include "stdafx.h"
#include"sdk.h"



#pragma pack(push,1)
struct Blueprint {
	__int8 camo;
	char __pad1[11];
	__int16 sticker[3];
	char blueprintName[20];
	__int8 weapon;
	char __pad2[56];
	int lootItemID;
};
struct Blueprint_info {
	__int8 camo;
	__int16 sticker[3];
	char blueprintName[20];
	__int8 weapon;
	int lootItemID;
};



class BP
{
private:
	uintptr_t structStart = sig_offsets::BlueprintDataBuffer + 0x16;
	const int STRUCT_SIZE = 520;

	void WriteBlueprint(Blueprint_info& bp, int idx);
	int GetCamo(const char* camo);
	const char* Normalize(const char*, int idx);
	void debugPrint(int weapon, int idx);
public:
	void Save(const char* name, int camo, int weapon, int idx);
	void SaveAll(std::vector<const char*> names, std::vector<const char*> camos);
	~BP() {};
};





extern bool b_menu_open;

ImFont* GetFont(int i);

union VariableUnion {
	int intValue;
	unsigned int uintValue;
	float floatValue;
	unsigned int stringValue;
	const float* vectorValue;
	const char* codePosValue;
	unsigned __int64 scriptCodePosValue;
	unsigned int pointerValue;
	unsigned int entityOffset;
};


struct VariableValue {
	VariableUnion u;
	int type;
};


struct scrContext_t {
	char __padding0000[0xB9C0];
	VariableValue* top;
};

typedef void(__fastcall* GetDvarInt)(scrContext_t* scrContext);
inline GetDvarInt GetDvarInt_orig = 0;

namespace g_menu
{

	extern void GScr_GetDvarInt_Hook(scrContext_t* scrContext);
	extern	int aimbotKeyMode;
	
	extern D3D12_GPU_DESCRIPTOR_HANDLE gpu_handle;
	extern D3D12_CPU_DESCRIPTOR_HANDLE cpu_handle;
	extern D3D12_GPU_DESCRIPTOR_HANDLE testbild;
	extern bool b_menu_open;
	extern std::string str_config_name;

	void menu();

	extern char* weaponname_internal;
	extern char* weaponname;
	extern char* weaponname_alt;
}


extern ImFont* main_font1;
extern ImFont* main_font2;
extern ImFont* main_font3;
extern ImFont* main_font4;
extern ImFont* main_font5;
extern ImFont* main_font6;
extern ImFont* main_font7;
extern ImFont* main_font8;

extern void init_Font_List();


inline const char* dvar = "dvar dont exist";
template<typename T> inline void Cbuff(const char* dvarName, T value)
{

	dvar_s* dvar = sdk::Dvar_FindVarByNameF(dvarName);
	if (dvar)
	{
		if (dvar->flags > 0)
		{
			dvar->flags = 0;
		}
		switch (dvar->type)
		{
		case DvarType::DVAR_TYPE_BOOL:
		{

			sdk::Dvar_SetBool_Internal(dvar, value);
			break;
		}
		case DvarType::DVAR_TYPE_FLOAT:
		{

			sdk::Dvar_SetFloat_InternalF(dvar, value);
			break;
		}
		case DvarType::DVAR_TYPE_INT:
		{
			sdk::Dvar_SetInt_InternalF(dvar, value);
			break;
		}
	
		default:
			break;
		}

	}
	else
	{
		ImGui::OpenPopup("dvar");
	}
}