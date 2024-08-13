#pragma once
//E8 ? ? ? ? F6 43 11 04 - resolve call 0x5DC1D20

//Material_Register(R_RegisterShader) : 0x5D4A6F0 E8 ? ? ? ? 4C 8B D8 48 8B 4C 24 ? 48 8D 05 ? ? ? ? 48 3B C8 72 6B RESOLVE CALL
//R_RegisterFont : 0x28AEFA0 //E8 ? ? ? ? 48 89 05 ? ? ? ? 8B 05 ? ? ? ? 83 RESOLVE CALL

//CL_DrawStretchPicPhysical : 0x3EB5B30 //E8 ? ? ? ? F3 44 0F 10 35 ? ? ? ? 41 0F 28 C1 48 89 6C 24 ? F3 41 0F 58 C1 48 89 74 24 ? 44 0F 28 EE F3 0F 11 7C 24 ? 45 0F 28 E1 F3 44 0F 11 74 24 ? F3 45 0F 58 E7 F3 44 0F 11 5C 24 ? 41
//CG_DrawRotatedPicPhysical : 0x459510  //E8 ? ? ? ? 44 0F 28 8C 24 ? ? ? ? 44 0F 28 84 24 ? ? ? ? 0F 28 BC 24 ? ? ? ? 0F 28 B4 24 ? ? ? ? 4C 8B B4 24 ? ? ? ? 48 8B BC 24

//R_TextWidth : 0x47AEDB4 //E8 ? ? ? ? F3 0F 10 5D ? 4C 8B C3 49 8B 4D 30 BA ? ? ? ? C7 44 24 ? ? ? ? ? 66 RESOLVE CALL

//R_AddCmdDrawText : 0x5D6ACE0 //E8 ? ? ? ? 48 8B 4D 20 48 33 CC E8 ? ? ? ? 4C 8D 9C 24 ? ? ? ? 49 8B 5B 48 41 RESOLVE CALL


////GetOriginForClosestCharacters index 4 //48 8D 05 ? ? ? ? 41 B8 ? ? ? ? 48 89 06 48 8D 96 : 0x451FEB0
//void __fastcall CgAntiLag::GetOriginForClosestCharacters(CgAntiLag* this, int entIndex, vec3_t* outOrigin) //gets entity origin.
//
////48 8D 05 ? ? ? ? 48 8B D9 48 89 01 33 D2 48 8D 05 ? ? ? ? C7 41 ? ? ? ? ? 48 89 01 [lea for vtable] GetWorldBonePos index 3. : 0x4BE40E0
//void __fastcall CgTargetAssist::GetWorldBonePos(CgTargetAssist* this, __int16 entNum, int boneIndex, vec3_t* outPos) //gets bone position.
//
//
////48 8D 2D ? ? ? ? 33 F6 0F 1F 80 ? ? ? ? 48 8B CB E8 [GetEntNum index 1] : 0x50112B0
//__int64 __fastcall CAssistTarget::GetEntNum(centity* this) //gets the index of the ent.
//
////48 8D 05 ? ? ? ? C7 43 ? ? ? ? ? 48 89 03 48 8B C3 C6 43 0B 01 C6 43 1D 01 C7 43 ? ? ? ? ? 48 83 C4 20 5B C3   CGMISSILE VTABLE
//entityState_t* __fastcall CgMissile::GetEntityState(CgMissile* this, const int entNum) INDEX 16 : 0x4A09F00
//bool __fastcall CgMissile::IsEntityValid(CgMissile * this, const int entNum) INDEX 18 : 0x4A1EA80
//bool __fastcall CgMissile::IsEntityClient(CgMissile * this, const int entNum) INDEX 19 : 0x4A149B0

//w2s 40 55 56 41 56 41 57 48 83 EC 38 48 8B 44 24 ? 44 8B F9 48 8D 0D ? ? ? ? 49 8B F1 49 8B E8 4C 8B F2

//4C 8D 2D ? ? ? ? F3 45 0F 10 B5 ? ? ? ? F3 45 0F 58 B5,
//scrPlace