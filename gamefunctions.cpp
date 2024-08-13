#include"gamefunctions.h"






namespace GameFunctions
{
	bool G_Main_LocationalTrace( vec3_t start,  vec3_t end, int entnum)
	{
		auto playerstate = *(playerState_t**)(sig_offsets::p_playerstate);
		if (is_bad_ptr(playerstate))return 0;

		trace_t trace;
		VariadicCall<void>(sig_offsets::G_Main_LocationalTrace, &trace, &start, &end, playerstate->clienNum, 0x2806931, nullptr);//0x2806191 // 0x2001 // 0x2806591 //0x2806D21 //0x2806931 //0x2807191 //0x2004000 //0x2014011
		return (trace.hitId == entnum || trace.fraction >= 1.0f);
	}


	void G_Main_TraceCapsule(trace_t* results, vec3_t* begin, vec3_t* end, Bounds* bound, int skipEntity, int contentmask)
	{
		return VariadicCall<void>(sig_offsets::G_Main_TraceCapsule, results, begin, end, bound, skipEntity, contentmask);
	}

	bool  AIPhys_Trace(const vec3_t start, const vec3_t end,int entnum)
	{
		trace_t trace;

		 VariadicCall<void>(sig_offsets::AIPhys_Trace, &trace, &start, &end, 0, 0, 0x2806931);
		 return (trace.hitId == entnum || trace.fraction >= 1.0f);


	}

	void CG_PIP_EnableNightVision(int pipClient, double fadeInTime)
	{
		return VariadicCall<void>((g_data::base + 0x5C3B730), 0, 1);
	}


	bool WorldToScreenPosReal(vec3_t* pos, vec2_t* out)
	{
		return VariadicCall<bool>(sig_offsets::WordToScreenPosReal,(LocalClientNum_t)0, (uintptr_t*)sig_offsets::ScreenPlacement, pos, out);
	}


	entityState_t* GetEntityState(int entNum)
	{
		void** CgMissile = (void**)(sig_offsets::CgMissile);
			//sig_offsets::CgMissile);
		return VariadicCall<entityState_t*>((uintptr_t)CgMissile[16], (__int64)0, entNum);
	}


	snapshot_t* CG_SnapshotMP_GetNextSnap()
	{
		return VariadicCall<snapshot_t*>(sig_offsets::CG_NextSnapMP, (LocalClientNum_t)0);
	}

	uintptr_t GetClientState(int clientNum)
	{
		
		auto cached_snapshot_ptr = CG_SnapshotMP_GetNextSnap();
		

		if (cached_snapshot_ptr)
		{
			auto addr = (uintptr_t)(cached_snapshot_ptr + 0x3573c);
			auto ret = addr + (static_cast<uint64_t>(clientNum) * 0x1b4);
			return ret;
		}
		return NULL;
	}

	//
	playerState_t* GetPlayerState(const int entNum)
	{
		void** CgMissile = (void**)(sig_offsets::CgMissile);
		return VariadicCall<playerState_t*>((uintptr_t)CgMissile[79], (__int64)0, entNum);

	}

	uintptr_t GetClActiveClient()
	{

		auto addr = (GetClActiveClient_t)(sig_offsets::ClientActive);
		auto ret = spoof_call((void*)(sig_offsets::jmp_rbx), addr);
		return ret;
	}

	

		bool get_bone_by_id(int i, int bone_id, vec3_t* Out_bone_pos)
		{
			void** CgTargetAssist = (void**)(sig_offsets::CgTargetAssist);
			//auto addr = reinterpret_cast<bool(__fastcall*)(uint64_t, int, int, float*)>(CgTargetAssist[3]);
			float bone_out[3];


			if (VariadicCall<bool>((uintptr_t)CgTargetAssist[3], (uint64_t)0, i, bone_id, (float*)bone_out))
			{
				Out_bone_pos->x = bone_out[0];
				Out_bone_pos->y = bone_out[1];
				Out_bone_pos->z = bone_out[2];
				return true;
			}

			return false;

		


		}


		void set_viewangle(vec3_t* pos)
		{
			//"4C 8B ? 55 53 49 8D ? ? 48 81 EC ? ? ? ? F3 0F"

			auto addr = (CG_VehicleCam_SetClientViewAngles_t)(g_data::base + 0x4F36850);
			spoof_call((void*)(sig_offsets::jmp_rbx), addr, sdk::get_local_player().address, pos);

		}

		UICharacterState*  CL_UICharacter_GetState(unsigned int clientIndex)
		{
			return VariadicCall<UICharacterState*>((uintptr_t)(g_data::base + 0x46A8860), 0, clientIndex);
		}




		vec3_t  GetOriginForClosestCharacters(int entNum)
	{

		//48 8D 05 ? ? ? ? 41 B8 ? ? ? ? 48 89 06 48 8D 96
		void** CgAntiLag = (void**)(sig_offsets::CgAntiLag);
		auto addr = reinterpret_cast<void(__fastcall*)(__int64, int, vec3_t*)>(CgAntiLag[4]);
		vec3_t ret{};
		 spoof_call((void*)(sig_offsets::jmp_rbx), addr, (__int64)0, entNum, &ret);
		 return ret;
	}

	
	typedef void(__fastcall* CgHandler_GetEntityTeam)(__int64, DWORD*, unsigned int, unsigned int*);
	void GetEntityTeam(DWORD* teamOut, unsigned int entityNum)
	{
		void** CgHandler = (void**)(g_data::base + 0x5AB3C9E);
		auto addr = reinterpret_cast<void(__fastcall*)(__int64, DWORD*, unsigned int, unsigned int*)>(CgHandler[14]);
		static unsigned int entityNumOut;
		spoof_call((void*)(sig_offsets::jmp_rbx), addr, (__int64)0, teamOut, entityNum, &entityNumOut);
	}
	uintptr_t GetClientInfo()
	{
		auto addr = sig_offsets::Clientinfo;
		uintptr_t CI_offset = VariadicCall<uintptr_t>(addr, 300); // returns a offset lol
		uintptr_t cached_info = VariadicCall<uintptr_t>(addr, 0) - CI_offset;

		if (is_bad_ptr(cached_info))
			return 0;
		return cached_info;
	}

	uintptr_t GetCharacterinfo(int entnum)
	{
		auto vehiclesystem = (void**)(sig_offsets::Characterinfo);
		auto characterinfo = VariadicCall<uintptr_t>((uintptr_t)vehiclesystem[31], 0, 0, entnum);
		if (is_bad_ptr(characterinfo))return 0;
		return characterinfo;
	}



	 uintptr_t Com_DDL_LoadAsset(const char* fileName) {

		//40 53 48 83 EC 20 E8 ? ? ? ? 48 8B D8 48 85 C0 75 15 41 B8 ? ? ? ? 48 8D 15 ? ? ? ? 8D 48 01 E8 ? ? ? ? 48 8B 43 08
		uintptr_t address = g_data::base + 0x33fdf90;
		return VariadicCall<uintptr_t>(address,fileName);
	}

	 uintptr_t DDL_GetRootState(uintptr_t result, uintptr_t ddlDef) {


		//33 C0 C7 41 ?? ?? ?? ?? ?? 48 89 41 10 89 41 04 48 8B C1 48 89 51 18 C6 01 01 C3
		uintptr_t address = g_data::base + 0x6b7375f;
		return VariadicCall<uintptr_t>(address,result, ddlDef);

		
	}

	bool  CL_PlayerData_GetDDLBuffer(uintptr_t context, int controllerindex, int stats_source, unsigned int statsgroup) {
		//48 89 ? ? ? 48 89 ? ? ? 48 89 ? ? ? 57 48 83 EC ? 48 8B ? 49 63 ? 41 8B ? 48 63
		uintptr_t address = g_data::base + 0x42ca640;
		return VariadicCall<bool>(address,context, controllerindex, stats_source, statsgroup);
	}

	bool  DDL_MoveToName(uintptr_t fromState, uintptr_t toState, const char* name) {
		//45 33 ? E9 ? ? ? ? 48 33 ? E9 ? ? ? ? E9
		uintptr_t address = g_data::base + 0x6b74220;
		return VariadicCall<bool>(address,fromState, toState, name);
	}
	
	bool  DDL_SetInt(uintptr_t state, uintptr_t ddlContext, int val) {
		//E8 ? ? ? ? 41 88 B4 2F ? ? ? ? 
		uintptr_t address = g_data::base + 0x6b74660;
		return VariadicCall<bool>(address,state, ddlContext, val);
	}


	
	const char* UI_SafeTranslateString(const char* reference)
	{
		return VariadicCall<const char*>(g_data::base + 0x67904C0,reference);
	}

	const char* SL_ConvertToString(uintptr_t reference)
	{
		return VariadicCall<const char*>(g_data::base + 0x36F8690, reference);
	}


	//void IW8_DrawLine2(float x1, float y1, float x2, float y2, Engine_Color* color, float width)
	//{
	//	float h1, l1, l2, x, y, angle;
	//	h1 = y2 - y1;
	//	l1 = x2 - x1;
	//	l2 = sqrtf(l1 * l1 + h1 * h1);
	//	x = x1 + ((l1 - l2) / 2.0f);
	//	y = y1 + (h1 / 2.0f);
	//	angle = atanf(h1 / l1) * 57.29577951308232087f;

	//	VariadicCall<void>((uintptr_t)sig_offsets::R_AddCmdDrawStretchPicRotateXY, x, y, l2, y2, 0, 0, 0, 0, angle, 0, 0, color->toFloat(), materialWhite);
	//}


	
}



