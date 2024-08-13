#include "sdk.h"
#include "defs.h"
#include "offsets.h"
#include "xor.hpp"
#include "imgui/imgui_internal.h"
#include "lazyimporter.h"
#include "settings.h"
#include"classes.h"
#include "Menu.h"
#include "spoofcall.h"
#include "gamefunctions.h"



#define	PITCH				0		// up / down
#define	YAW					1		// left / right
#define	ROLL				2	
namespace process
{
	HWND hwnd;

	BOOL CALLBACK EnumWindowCallBack(HWND hWnd, LPARAM lParam)
	{
		DWORD dwPid = 0;
		GetWindowThreadProcessId(hWnd, &dwPid);
		if (dwPid == lParam)
		{
			hwnd = hWnd;
			return FALSE;
		}
		return TRUE;
	}

	HWND get_process_window()
	{
		if (hwnd)
			return hwnd;

		EnumWindows(EnumWindowCallBack, GetCurrentProcessId());

		if (hwnd == NULL)
			Exit();

		return hwnd;
	}
}


void AngleVectors(const vec3_t angles, vec3_t forward, vec3_t right, vec3_t up) {
    float       angle;
    static float        sr, sp, sy, cr, cp, cy;
    // static to help MS compiler fp bugs

    angle = angles[YAW] * (M_PI * 2 / 360);
    sy = sin(angle);
    cy = cos(angle);
    angle = angles[PITCH] * (M_PI * 2 / 360);
    sp = sin(angle);
    cp = cos(angle);
    angle = angles[ROLL] * (M_PI * 2 / 360);
    sr = sin(angle);
    cr = cos(angle);

    if (!forward.is_Zero())
    {
        forward[0] = cp * cy;
        forward[1] = cp * sy;
        forward[2] = -sp;
    }
    if (!right.is_Zero())
    {
        right[0] = (-1 * sr * sp * cy + -1 * cr * -sy);
        right[1] = (-1 * sr * sp * sy + -1 * cr * cy);
        right[2] = -1 * sr * cp;
    }
    if (!up.is_Zero())
    {
        up[0] = (cr * sp * cy + -sr * -sy);
        up[1] = (cr * sp * sy + -sr * cy);
        up[2] = cr * cp;
    }
}

namespace sdk
{
    const DWORD nTickTime = 64;//64 ms
    bool bUpdateTick = false;
    std::map<DWORD, velocityInfo_t> velocityMap;

    vec3_t PlayerViewPoint(float Line, vec3_t Angle, vec3_t ClientPos)
    {
        vec3_t Trail;

        AngleVectors(Angle, Trail, {}, {});
        Trail[2] = -Trail[2]; // same as 0.0f

        vec3_t Pos;
       

        ClientPos[0] += (Trail[0] * Line);
        ClientPos[1] += (Trail[1] * Line);
        ClientPos[2] += (Trail[2] * Line);

        return ClientPos;
    }


    vec3_t get_camera_location()
    {
        vec3_t camera{};
        uintptr_t camera_base = *(uintptr_t*)(sig_offsets::camera_base);

        float x = *(float*)(camera_base + 0x108);
        float y = *(float*)(camera_base + 0x118);
        float z = *(float*)(camera_base + 0x128);

        return camera = {x,y,z};
    }

    bool WorldToScreen(const vec3_t& WorldPos, vec2_t* ScreenPos)
    {
        vec3_t ViewOrig = get_camera_location();
        auto refdef = sdk::get_refdef();
        if (is_bad_ptr(refdef))return false;
        vec3_t vLocal, vTransform;
        vLocal = WorldPos - ViewOrig;
        // get our dot products from viewAxis
        vTransform.x = vLocal.dot(refdef->ViewAxis[1]);
        vTransform.y = vLocal.dot(refdef->ViewAxis[2]);
        vTransform.z = vLocal.dot(refdef->ViewAxis[0]);
        // make sure it is in front of us
        if (vTransform.z < 0.01f)
            return false;
        ScreenPos->x = ((refdef->Width / 2) * (1 - (vTransform.x / refdef->FovX / vTransform.z)));
        ScreenPos->y = ((refdef->Height / 2) * (1 - (vTransform.y / refdef->FovY / vTransform.z)));

      /*  if (ScreenPos->x < 1 || ScreenPos->y < 1 || (ScreenPos->x > refdef->Width) || (ScreenPos->y > refdef->Height)) {
            return false;
        }*/

        return true;
    }

    float units_to_m(float units) {
        return units * 0.0254;
    }

    float m_to_units(float meters) {
        return 0.0254 / meters;
    }

    float xangle(const vec3_t& LocalPos, const vec3_t& WorldPos)
    {
        float dl = sqrt((WorldPos.x - LocalPos.x) * (WorldPos.x - LocalPos.x) + (WorldPos.y - LocalPos.y) * (WorldPos.y - LocalPos.y));

        if (dl == 0.0f)
            dl = 1.0f;

        float dl2 = abs(WorldPos.x - LocalPos.x);
        float teta = ((180.0f / M_PI) * acos(dl2 / dl));

        if (WorldPos.x < LocalPos.x)
            teta = 180.0f - teta;

        if (WorldPos.y < LocalPos.y)
            teta = teta * -1.0f;

        if (teta > 180.0f)
            teta = (360.0f - teta) * (-1.0f);

        if (teta < -180.0f)
            teta = (360.0f + teta);

        return teta;
    }

    void rotation_point_alpha(float x, float y, float z, float alpha, vec3_t* outVec3)
    {




        if (!g_data::dx9rot)
        {
          

        }
        typedef LinearTransform* (WINAPI* t_D3DXMatrixRotationY)(LinearTransform* pOut, FLOAT Angle);
        static t_D3DXMatrixRotationY D3DXMatrixRotationY = NULL;
        if (D3DXMatrixRotationY == NULL)
            D3DXMatrixRotationY = (t_D3DXMatrixRotationY)GetProcAddress(g_data::dx9rot, xorstr("D3DXMatrixRotationY"));

        typedef Vec4* (WINAPI* t_D3DXVec3Transform)(Vec4* pOut, CONST Vec4* pV, CONST LinearTransform* pM);
        static t_D3DXVec3Transform D3DXVec4Transform = NULL;
        if (D3DXVec4Transform == NULL)
            D3DXVec4Transform = (t_D3DXVec3Transform)GetProcAddress(g_data::dx9rot, xorstr("D3DXVec4Transform"));

        Matrix4x4 rot1;
        Vec4 vec = { x, z, y, 1.0f };
        D3DXMatrixRotationY((LinearTransform*)&rot1, alpha * M_PI / 180.0f);
        D3DXVec4Transform((Vec4*)&vec, (const Vec4*)&vec, (const LinearTransform*)&rot1);

        outVec3->x = vec.x;
        outVec3->y = vec.z;
        outVec3->z = vec.y;
    };

    //typedef bool(__fastcall* w2s_t)(int localClientNum, void* scrPlace,vec3_t* worldPos, vec2_t* outScreenPos);
    //bool w2s_function(int localClientNum,   vec3_t* worldPos, vec2_t* outScreenPos)
    //{
    //    auto source_place = (uintptr_t)(g_data::base + 0x1E68AEA0);
    //    auto addr = (uintptr_t)(g_data::base + 0x46b6f70);

    //    auto ret = spoof_call((void*)(g_data::base + offsets::jmp_rbx), (w2s_t)addr, 0, (void*)&source_place, worldPos, outScreenPos);
    //}

    bool w2s(const vec3_t& WorldPos, vec2_t* ScreenPos, vec2_t* BoxSize)
    {
        auto ViewOrig = get_camera_location();

        float angleX = xangle(ViewOrig, WorldPos);

        vec3_t posl, posr;

        rotation_point_alpha(-16.0f, 0.0f, 65.0f, -angleX + 90.0f, &posl);
        rotation_point_alpha(16.0f, 0.0f, -5.0f, -angleX + 90.0f, &posr);

        vec3_t vposl, vposr;

        vposl.x = WorldPos.x + posl.x;
        vposl.y = WorldPos.y + posl.y;
        vposl.z = WorldPos.z + posl.z;

        vposr.x = WorldPos.x + posr.x;
        vposr.y = WorldPos.y + posr.y;
        vposr.z = WorldPos.z + posr.z;

        vec2_t screenPosl, screenPosr;

        if (!WorldToScreen(vposl, &screenPosl)|| !WorldToScreen(vposr, &screenPosr))
        {
            return false;
        }

        BoxSize->x = abs(screenPosr.x - screenPosl.x);
        BoxSize->y = abs(screenPosl.y - screenPosr.y);

        ScreenPos->x = screenPosr.x - BoxSize->x;
        ScreenPos->y = screenPosl.y;

        return true;
    }

    bool head_to_screen(vec3_t pos, vec2_t* pos_out, int stance)
    {
        vec2_t w2s_head;

        pos.z += 55.f;
        if (!WorldToScreen(pos, &w2s_head))
            return false;

        else if (stance == sdk::CROUNCH)
        {
            pos.z -= 25.f;
            if (!WorldToScreen(pos, &w2s_head))
                return false;
        }
        else if (stance == sdk::KNOCKED)
        {
            pos.z -= 28.f;
            if (!WorldToScreen(pos, &w2s_head))
                return false;
        }
        else if (stance == sdk::PRONE)
        {
            pos.z -= 50.f;
            if (!WorldToScreen(pos, &w2s_head))
                return false;
        }

        pos_out->x = w2s_head.x;
        pos_out->y = w2s_head.y;

        return true;
    }

    bool bones_to_screen(vec3_t* BonePosArray, vec2_t* ScreenPosArray, const long Count)
    {
        for (long i = 0; i < Count; ++i)
        {
            if (!WorldToScreen(BonePosArray[i], &ScreenPosArray[i]))
                return false;
        }
        return true;
    }

    bool is_valid_bone(vec3_t origin, vec3_t* bone, const long Count)
    {
        for (long i = 0; i < Count; ++i)
        {
            if (bone[i].distance_to(bone[i + 1]) > 41 && origin.distance_to(bone[i]) > 200) //118 ~= 3metres, 39~= 1metre
                return false;
        }
        return true;
    }

    bool in_game()
    {
        return *(int*)(sig_offsets::gamemode) > 1;
    }

  

    player_t get_player(int i)
    {
       /* uint64_t decryptedPtr = decryption::get_client_info();

        if (is_valid_ptr(decryptedPtr))
        {
            uint64_t client_info = decryption::get_client_info_base();

            if (is_valid_ptr(client_info))
            {
                return player_t(client_info + (i * offsets::player::size));
            }
        }
        return player_t(NULL);*/
         auto characterinfo = GameFunctions::GetCharacterinfo(i);
        if (is_valid_ptr(characterinfo))
        {
            return player_t(characterinfo);
        }

        return player_t(NULL);
    }

    player_t get_local_player()
    {
       /* uint64_t cg_t = GameFunctions::GetClientInfo();
        if (is_bad_ptr(cg_t))
            return player_t(NULL);

        auto local_index = *(uintptr_t*)(cg_t + sig_offsets::local_index);

        if (is_bad_ptr(local_index))
            return player_t(NULL);

        auto index = *(int*)(local_index + 0x204)*/;


		int localnum = sdk::GetLocalIndex();
        
        return get_player(localnum);
    }

    int local_index()
    {

        uint64_t cg_t = GameFunctions::GetClientInfo();

        if (is_valid_ptr(cg_t))
        {
            auto local_index = *(uintptr_t*)(cg_t + sig_offsets::local_index);
            if (is_bad_ptr(local_index))return 0;
            return *(int*)(local_index + 0x204);
        }
        return 0;
    }

    name_t* get_name_ptr(int i) // real clientinfo
    {
        uint64_t bgs = *(uint64_t*)(sig_offsets::name_array);

        if (bgs)
        {
            name_t* pClientInfo = (name_t*)(bgs + 0x4C70 + ((i + i * 8) << 4));

            if (pClientInfo)
            {
                return pClientInfo;
            }
        }
        return 0;
    }



    refdef_t* get_refdef()
    {
        uint32_t crypt_0 = *(uint32_t*)(sig_offsets::Refdef);
        uint32_t crypt_1 = *(uint32_t*)(sig_offsets::Refdef + 0x4);
        uint32_t crypt_2 = *(uint32_t*)(sig_offsets::Refdef + 0x8);
        // lower 32 bits
        uint32_t entry_1 = (uint32_t)(sig_offsets::Refdef);
        uint32_t entry_2 = (uint32_t)(sig_offsets::Refdef + 0x4);
        // decryption
        uint32_t _low = entry_1 ^ crypt_2;
        uint32_t _high = entry_2 ^ crypt_2;
        uint32_t low_bit = crypt_0 ^ _low * (_low + 2);
        uint32_t high_bit = crypt_1 ^ _high * (_high + 2);
        refdef_t* refdef = (refdef_t*)(((uint64_t)high_bit << 32) + low_bit);
        if (is_bad_ptr(refdef))return nullptr;

        return refdef;
    }

    vec3_t get_camera_pos()
    {
        vec3_t pos = vec3_t{};

        auto camera_ptr = *(uint64_t*)(sig_offsets::camera_base);

        if (is_bad_ptr(camera_ptr))return pos;


        pos = *(vec3_t*)(camera_ptr + 0x1E8);
        return pos;
    }

    std::string get_player_name(int i)
    {
        uint64_t bgs = *(uint64_t*)(sig_offsets::name_array);

        if (bgs)
        {
            name_t* clientInfo_ptr = (name_t*)(bgs + 0x4C70 + (i * 0xD0));
        

            int length = strlen(clientInfo_ptr->name);
            for (int j = 0; j < length; ++j)
            {
                char ch = clientInfo_ptr->name[j];
                bool is_english = ch >= 0 && ch <= 127;
                if (!is_english)
                    return xorstr("Player");
            }
            return clientInfo_ptr->name;
        }
        return xorstr("Player");
    }

    bool IsVisible(vec3_t start, vec3_t end, int entitynum)
    {
        return  GameFunctions::G_Main_LocationalTrace(start, end, entitynum);
    }

    bool IsVisible2(vec3_t start, vec3_t end, int entitynum)
    {
        return  GameFunctions::AIPhys_Trace(start, end, entitynum);
    }

    bool Trace(vec3_t start, vec3_t end, int entitynum)
    {


        return false;
    }
  

    int GetLocalIndex()
    {

        playerState_t* LocalPlayer = *(playerState_t**)(sig_offsets::p_playerstate);
        if (is_bad_ptr(LocalPlayer))
            return 0;

        return  LocalPlayer->clienNum;


    }












    static float valuesRecoilBackup[962][60];
    static  float valuesSpreadBackup[962][22];
    bool save_recoil = true;
    bool save_spread = true;


    bool Updated()
    {
        BYTE m_checkUpdate[2] = { 0x74, 0x1D };

        for (int count{ 0 }; count < 2; ++count)
        {
            if (((BYTE*)(g_data::base + 0x42B9039))[count] == m_checkUpdate[count])
                return true;
        }

        return false;
    }


    void no_recoil()
    {
        uint64_t cl_info = GameFunctions::GetClientInfo();

        if (cl_info)
        {
            
            // up, down
            uint64_t r12 = cl_info;
            r12 += sig_offsets::Recoil;
            uint64_t rsi = r12 + 0x4;
            DWORD edx = *(uint64_t*)(r12 + 0xC);
            DWORD ecx = (DWORD)r12;
            ecx ^= edx;
            DWORD eax = (DWORD)((uint64_t)ecx + 0x2);
            eax *= ecx;
            ecx = (DWORD)rsi;
            ecx ^= edx;
            DWORD udZero = eax;
            //left, right
            eax = (DWORD)((uint64_t)ecx + 0x2);
            eax *= ecx;
            DWORD lrZero = eax;
            *(DWORD*)(r12) = udZero;
            *(DWORD*)(rsi) = lrZero;
        }
    }



   
	void enable_uav()
	{
        auto localplayer = sdk::get_local_player();
        if (!localplayer.address)return;
        
        //playerState_t* player = GameFunctions::GetPlayerState(index);
        CgSnapshotMP* nextSnap = (CgSnapshotMP*)GameFunctions::CG_SnapshotMP_GetNextSnap(); // CgSnapshotMP cannot be modified?
		if (sdk::in_game())
		{
			if (nextSnap != 0)
			{
           
			
			}
		}
	}

	int get_player_health(int i)
	{
		uint64_t bgs = *(uint64_t*)(sig_offsets::name_array);

		if (is_valid_ptr(bgs))
		{
			name_t* pClientInfo = (name_t*)(bgs + 0x4C70 + (i * 0xD0));

			if (is_valid_ptr(pClientInfo))
			{
				return pClientInfo->health;
			}
		}
		return 0;
	}
	

	void start_tick()
	{
		static DWORD lastTick = 0;
		DWORD t = GetTickCount64();
		bUpdateTick = lastTick < t;

		if (bUpdateTick)
			lastTick = t + nTickTime;
	}

	void update_vel_map(int index, vec3_t vPos)
	{
		if (!bUpdateTick)
			return;

		velocityMap[index].delta = vPos - velocityMap[index].lastPos;
		velocityMap[index].lastPos = vPos;
	}

	void clear_vel_map()
	{
		if (!velocityMap.empty()) { velocityMap.clear(); }
	}

	vec3_t get_speed(int index)
	{
		return velocityMap[index].delta;
	}

	vec3_t get_prediction(int index, vec3_t source, vec3_t destination)
	{
		auto local_velocity = get_speed(local_index());
		auto target_velocity = get_speed(index);

		const auto distance = source.distance_to(destination);
		const auto travel_time = distance / Settings::aimbot::f_bullet_speed;
		auto pred_destination = destination + (target_velocity - local_velocity) * travel_time;
		pred_destination.z += 0.5f * m_to_units(9.8f) * travel_time * travel_time;

		return pred_destination;
	}

	vec3_t get_prediction(int index, vec3_t source, vec3_t bone, vec3_t position)
	{
		auto local_velocity = get_speed(local_index());
		auto target_velocity = get_speed(index);

		const auto distance = source.distance_to(position);
		const auto travel_time = distance / Settings::aimbot::f_bullet_speed;
		auto pred_destination = bone + (target_velocity - local_velocity) * travel_time;
		pred_destination.z += 0.5f * m_to_units(9.8f) * (travel_time * travel_time);

		return pred_destination;
	}

	int get_client_count()
	{
	/*	auto cl_info = decryption::get_client_info();
		if (is_valid_ptr(cl_info))
		{
			auto client_ptr = *(uint64_t*)(cl_info + offsets::local_index);
			if (is_valid_ptr(client_ptr))
			{
				return *(int*)(client_ptr + 0x1C);
			}
		}

		return 0;*/
	}

	Result MidnightSolver(float a, float b, float c)
	{
		Result res;

		double subsquare = b * b - 4 * a * c;

		if (subsquare < 0)
		{
			res.hasResult = false;
			return res;
		}
		else
		{
			res.hasResult = true,
				res.a = (float)((-b + sqrt(subsquare)) / (2 * a));
			res.b = (float)((-b - sqrt(subsquare)) / (2 * a));
		}
		return res;
	}

	vec3_t prediction_solver(vec3_t local_pos, vec3_t position, int index, float bullet_speed)
	{
		vec3_t aimPosition = vec3_t();
		auto target_speed = get_speed(index);

		local_pos -= position;

		float a = (target_speed.x * target_speed.x) + (target_speed.y * target_speed.y) + (target_speed.z * target_speed.z) - ((bullet_speed * bullet_speed) * 100);
		float b = (-2 * local_pos.x * target_speed.x) + (-2 * local_pos.y * target_speed.y) + (-2 * local_pos.z * target_speed.z);
		float c = (local_pos.x * local_pos.x) + (local_pos.y * local_pos.y) + (local_pos.z * local_pos.z);

		local_pos += position;

		Result r = MidnightSolver(a, b, c);

		if (r.a >= 0 && !(r.b >= 0 && r.b < r.a))
		{
			aimPosition = position + target_speed * r.a;
		}
		else if (r.b >= 0)
		{
			aimPosition = position + target_speed * r.b;
		}

		return aimPosition;

	}

	uint64_t get_visible_base()
	{
		for (int32_t j = 4000; j >= 0; --j)
		{
			uint64_t n_index = (j + (j << 2)) << 0x6;
			uint64_t vis_base = *(uint64_t*)(sig_offsets::distribute);

			if (is_bad_ptr(vis_base))
				continue;

			uint64_t vis_base_ptr = vis_base + n_index;
			uint64_t cmp_function = *(uint64_t*)(vis_base_ptr + 0x90);

			if (is_bad_ptr(cmp_function))
				continue;

			uint64_t about_visible = sig_offsets::visible;
			if (cmp_function == about_visible)
			{
				return vis_base_ptr;
			}
		}
		return NULL;
	}

	// player class methods

	int player_t::team_id() {

		if (is_bad_ptr(address))return 0;
		return *(int*)((uintptr_t)address + sig_offsets::player_team);
	}

	int player_t::get_stance() {

		auto ret = *(int*)((uintptr_t)address + sig_offsets::stance);
		return ret;
	}

	vec3_t player_t::get_pos()
	{
		if (is_bad_ptr(address))return {};
		auto local_pos_ptr = *(uintptr_t*)((uintptr_t)address + sig_offsets::position);

		if (is_bad_ptr(local_pos_ptr))return{};
		else
			return *(vec3_t*)(local_pos_ptr + 0x40);
		return vec3_t{};
        
	}

	float player_t::get_rotation()
	{
		if (is_bad_ptr(address))return 0;
		auto local_pos_ptr = *(uintptr_t*)((uintptr_t)address + sig_offsets::position);

		if (is_bad_ptr(local_pos_ptr))return 0;

		auto rotation = *(float*)(local_pos_ptr + 0x58);

		if (rotation < 0)
			rotation = 360.0f - (rotation * -1);

		rotation += 90.0f;

		if (rotation >= 360.0f)
			rotation = rotation - 360.0f;

		return rotation;
	}

	int player_t::weapon_index()
	{
		if (is_bad_ptr(this->address))return 0;

		return *(int*)(this->address +0x1243);

	}

	uint32_t player_t::get_index( )
	{

  //      auto cl_info = GameFunctions::GetClientInfo();
		//if (is_valid_ptr(cl_info))
		//{
  //          auto addr = g_data::base + 0x4e64c60;
  //          uintptr_t CI_offset = VariadicCall<uintptr_t>(addr, 300);
		//	//auto cl_info_base = GameFunctions::GetCharacterinfo(0);
		//	if (is_valid_ptr(CI_offset))
		//	{
		//		return ((uintptr_t)address - CI_offset) / offsets::player::size;
		//	}
		//	return 0;
		//}


        
      /*      auto cl_info_base = GameFunctions::GetCharacterinfo(index);
            if (is_valid_ptr(cl_info_base))
            {
                return ((uintptr_t)address - cl_info_base) / sig_offsets::player_size;
            }
            return 0;*/
        return 0;
        
	}

	bool player_t::is_visible(int entnum)
	{
		if (is_valid_ptr(g_data::visible_base))
		{
			uint64_t VisibleList = *(uint64_t*)(g_data::visible_base + 0x108);
			if (is_bad_ptr(VisibleList))
				return false;

			uint64_t rdx = VisibleList + (entnum * 9 + 0x14E) * 8;
			if (is_bad_ptr(rdx))
				return false;

			DWORD VisibleFlags = (rdx + 0x10) ^ *(DWORD*)(rdx + 0x14);
			if (is_bad_ptr(VisibleFlags))
				return false;

			DWORD v511 = VisibleFlags * (VisibleFlags + 2);
			if (is_bad_ptr(v511))
				return false;

			BYTE VisibleFlags1 = *(DWORD*)(rdx + 0x10) ^ v511 ^ BYTE1(v511);
			if (VisibleFlags1 == 3) {
				return true;
			}
		}
		return false;
	}

  

    typedef void(__fastcall* Clan_tag)(int controller, const char* clantag);
    void CL_PlayerData_SetCustomClanTag(const char* clanTag)
    {
        //40 53 48 83 EC 30 48 8B 05 ? ? ? ? 48 33 C4 48 89 44 24 ? 4C 8B CA 41 B8 ? ? ? ? 8B 15 ? ? ? ? 8B D9

        auto addr = (Clan_tag)(g_data::base + 0x4291890);
        spoof_call((void*)(sig_offsets::jmp_rbx), addr, 0, clanTag);
    }

    typedef dvar_s* (__fastcall* Dvar_FindVarByName)(const char* dvarName);
    dvar_s* Dvar_FindVarByNameF(const char* dvarName)
    {
        auto addr = (Dvar_FindVarByName)(sig_offsets::FindDvarByName);
        return spoof_call((void*)(sig_offsets::jmp_rbx), addr, dvarName);

    }




    typedef uintptr_t(__fastcall* Dvar_SetFloat_Internal)(dvar_s* a1, float a2);
    uintptr_t Dvar_SetFloat_InternalF(dvar_s* a1, float a2)
    {
        auto addr = (Dvar_SetFloat_Internal)(sig_offsets::DvarSetFloat);
        return spoof_call((void*)(sig_offsets::jmp_rbx), addr, a1, a2);

    }



    typedef   std::ptrdiff_t(__fastcall* Dvar_SetBool_Internal_t)(dvar_s* a1, bool a2);
    std::ptrdiff_t Dvar_SetBool_Internal(dvar_s* a1, bool a2)
    {
        // "48 8B ? 48 83 EC ? 80 79 09 ? 4C 8B"

        auto addr = (Dvar_SetBool_Internal_t)(sig_offsets::DvarSetBool);
        return spoof_call((void*)(sig_offsets::jmp_rbx), addr, a1, a2);
    }
    typedef  uintptr_t(__fastcall* Dvar_SetInt_Internal)(dvar_s* a1, unsigned int a2);
    uintptr_t Dvar_SetInt_InternalF(dvar_s* a1, unsigned int a2)
    {
        //"40 ? 48 81 EC ? ? ? ? 48 8B ? ? ? ? ? 48 33 ? 48 89 ? ? ? ? ? ? 0F B6 ? ? 48 8B"
        auto addr = (Dvar_SetInt_Internal)(sig_offsets::DvarSetInt);
        return spoof_call((void*)(sig_offsets::jmp_rbx), addr, a1, a2);
    }

    uintptr_t Dvar_SetVec4_internal(dvar_s* a, float a1,float a2,float a3,float a4)
    {
        return reinterpret_cast<uintptr_t(__fastcall*)(dvar_s * a, float a1, float a2, float a3, float a4)>(g_data::base + 0x3843E20)(a,a1, a2,a3,a4);
    }

 


    

    //bool check_valid_bones2(unsigned long i, vec3_t origem)
    //{
    //    vec3_t header_to_bladder[6], right_foot_to_bladder[5], left_foot_to_bladder[5], right_hand[5], left_hand[5];
    //    vec2_t screen_header_to_bladder[6], screen_right_foot_to_bladder[5], screen_left_foot_to_bladder[5], screen_right_hand[5], screen_left_hand[5]; screen_left_hand[5];

    //    if (sdk::get_bone_by_player_index(i, sdk::BONE_POS_HEAD, &header_to_bladder[0]) &&
    //        sdk::get_bone_by_player_index(i, sdk::BONE_POS_NECK, &header_to_bladder[1]) &&
    //        sdk::get_bone_by_player_index(i, sdk::BONE_POS_CHEST, &header_to_bladder[2]) &&
    //        sdk::get_bone_by_player_index(i, sdk::BONE_POS_MID, &header_to_bladder[3]) &&
    //        sdk::get_bone_by_player_index(i, sdk::BONE_POS_TUMMY, &header_to_bladder[4]) &&
    //        sdk::get_bone_by_player_index(i, sdk::BONE_POS_PELVIS, &header_to_bladder[5]) &&
    //        sdk::get_bone_by_player_index(i, sdk::BONE_POS_RIGHT_FOOT_1, &right_foot_to_bladder[1]) &&
    //        sdk::get_bone_by_player_index(i, sdk::BONE_POS_RIGHT_FOOT_2, &right_foot_to_bladder[2]) &&
    //        sdk::get_bone_by_player_index(i, sdk::BONE_POS_RIGHT_FOOT_3, &right_foot_to_bladder[3]) &&
    //        sdk::get_bone_by_player_index(i, sdk::BONE_POS_RIGHT_FOOT_4, &right_foot_to_bladder[4]) &&
    //        sdk::get_bone_by_player_index(i, sdk::BONE_POS_LEFT_FOOT_1, &left_foot_to_bladder[1]) &&
    //        sdk::get_bone_by_player_index(i, sdk::BONE_POS_LEFT_FOOT_2, &left_foot_to_bladder[2]) &&
    //        sdk::get_bone_by_player_index(i, sdk::BONE_POS_LEFT_FOOT_3, &left_foot_to_bladder[3]) &&
    //        sdk::get_bone_by_player_index(i, sdk::BONE_POS_LEFT_FOOT_4, &left_foot_to_bladder[4]) &&
    //        sdk::get_bone_by_player_index(i, sdk::BONE_POS_LEFT_HAND_1, &right_hand[1]) &&
    //        sdk::get_bone_by_player_index(i, sdk::BONE_POS_LEFT_HAND_2, &right_hand[2]) &&
    //        sdk::get_bone_by_player_index(i, sdk::BONE_POS_LEFT_HAND_3, &right_hand[3]) &&
    //        sdk::get_bone_by_player_index(i, sdk::BONE_POS_LEFT_HAND_4, &right_hand[4]) &&
    //        sdk::get_bone_by_player_index(i, sdk::BONE_POS_RIGHT_HAND_1, &left_hand[1]) &&
    //        sdk::get_bone_by_player_index(i, sdk::BONE_POS_RIGHT_HAND_2, &left_hand[2]) &&
    //        sdk::get_bone_by_player_index(i, sdk::BONE_POS_RIGHT_HAND_3, &left_hand[3]) &&
    //        sdk::get_bone_by_player_index(i, sdk::BONE_POS_RIGHT_HAND_4, &left_hand[4]))
    //    {
    //        return true;
    //        //if (!sdk::bones_to_screen(header_to_bladder, screen_header_to_bladder, 6))
    //        //	return false;

    //        //if (!sdk::bones_to_screen(right_foot_to_bladder, screen_right_foot_to_bladder, 5))
    //        //	return false;

    //        //if (!sdk::bones_to_screen(left_foot_to_bladder, screen_left_foot_to_bladder, 5))
    //        //	return false;

    //        //if (!sdk::bones_to_screen(right_hand, screen_right_hand, 5))
    //        //	return false;

    //        //if (!sdk::bones_to_screen(left_hand, screen_left_hand, 5))
    //        //	return false;


    //    }
    //    else
    //    {
    //        return false;
    //    }

    //    return true;
    //}

    //Address of signature = ModernWarfare.exe + 0x009BEDEE
    //    "\xFF\x23\x2C\x00\xB3", "xxx?x"
       /* "FF 23 2C ? B3"*/


    int saved = 0;
    int i_index;
    bool reset_danger = false;
   void Danger(int max_entities)
    {
       auto localplayer = get_local_player();
       auto playerstate = *(playerState_t**)(sig_offsets::p_playerstate);

       if (is_bad_ptr(playerstate))return;
        reset_danger = true;
        for (i_index = 0; i_index < 2048; i_index++)
        {
            
            if (reset_danger)
            {
                saved = 0;
                reset_danger = false;
            }
            entityState_t* entitystate = GameFunctions::GetEntityState(i_index);
            
        
            if (entitystate->eType == entityType_s::ET_PLAYER)
            {
                auto player = get_player(i_index);
              
            

                if (playerstate->clienNum == i_index)continue;
                if (is_bad_ptr(player.address) || is_bad_ptr(localplayer.address))continue;
                

               vec3_t outpos= GameFunctions::GetOriginForClosestCharacters(i_index);
                auto entitypos = outpos;

                bool is_team = localplayer.team_id() == player.team_id();

              
                    if (is_team)continue;
                    auto distance = sdk::units_to_m(localplayer.get_pos().distance_to(entitypos));


                    if (distance <= Settings::esp::danger)
                    {

                        saved++;

                    }
               

                
            }

              
        }
        if (saved > 0)
        {
            auto refdef = sdk::get_refdef();
            if (is_bad_ptr(refdef))return;

            g_draw::draw_sketch_edge_text(GetFont(0), "WARNING " + std::string(std::to_string(saved)) + " player nearby!", ImVec2(refdef->Width / 2, refdef->Height / 4), 30.f, ImColor(255, 0, 0, 255), true);
        }
      
   }
      
       




}
namespace decryption
{


}




uintptr_t GetModuleBaseAddress(const std::string modName)
{
  
     DWORD procId = GetCurrentProcessId();
    uintptr_t modBaseAddr = 0;
    HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE , procId);
    if (hSnap != INVALID_HANDLE_VALUE)
    {
        MODULEENTRY32 modEntry;
        modEntry.dwSize = sizeof(modEntry);
        if (Module32First(hSnap, &modEntry))
        {
            do
            {
                if (!_stricmp(modEntry.szModule, modName.c_str()))
                {
                    modBaseAddr = (uintptr_t)modEntry.modBaseAddr;
                    break;
                }
            } while (Module32Next(hSnap, &modEntry));
        }
    }
    CloseHandle(hSnap);
    return modBaseAddr;
}
  




namespace g_data
{
    HANDLE hMainThread; 
	uintptr_t base;
	uintptr_t peb;
	HWND hWind;
	uintptr_t visible_base;
	FARPROC Targetbitblt;
	FARPROC TargetStretchbitblt;
	FARPROC Target_NtGdiBitBlt;
    uintptr_t target_dvar;
    uintptr_t target_dvardecr;
	sdk::refdef_t refdef2;
	HMODULE dx9rot;
	void init()
	{
        hMainThread = GetCurrentThread();
		base = (uintptr_t)(iat(GetModuleHandleA).get()("cod.exe"));
		dx9rot = LoadLibraryA(xorstr("d3dx9_43.dll"));
		Targetbitblt = iat(GetProcAddress).get()(iat(GetModuleHandleA).get()("Gdi32.dll"), "BitBlt");
		Target_NtGdiBitBlt = iat(GetProcAddress).get()(iat(GetModuleHandleA).get()("win32u.dll"), "NtGdiBitBlt");
		TargetStretchbitblt = iat(GetProcAddress).get()(iat(GetModuleHandleA).get()("win32u.dll"), "NtGdiStretchBlt");
       
		hWind = process::get_process_window();

		peb = __readgsqword(0x60);
	}
}
namespace menu_setting
{
	int Tab = 0;
}
namespace globals
{
    const char* weaponname{};
    bool is_local_client;
    uintptr_t gfx;
    bool critical_render;
    int counter = 0;
    vec3_t playerpos;
    vec3_t playerpos1;
    vec3_t playerpos2;
    vec3_t playerpos3;
    vec3_t playerpos4;
    vec3_t playerpos5;
    Vec2int viewangle;
    __int64 func;
	bool b_in_game = false;
	bool local_is_alive = false;
	bool is_aiming = false;
	int max_player_count = -1;

    namespace custom
    {
        vec3_t testangle;
        int localClientNum;
         unsigned int clientModelIdx;
        bool enabled;
        int colorIdx;
        bool fill;
    }

	/*const char* aim_lock_point[] = { "Spine", "Head", "Neck", "Chest" };
	const char* aim_lock_key[] = { "LButton", "RButton", "LShift" };
	const char* aim_priority_opt[] = { "Only by Closest", "Only in FOV", "Closest and FOV" };
	const const char* unlock_opt[] = { "Unlock CW Camos", "Unlock MW Camos" };*/

	uintptr_t local_ptr;
	uintptr_t player_ptr;
	uintptr_t refdef_ptr;
	vec3_t local_pos;
	uintptr_t clientinfo;
	uintptr_t clientinfobase;
     uintptr_t cg_entites;
     float test;
}

namespace colors
{
	ImColor Color{ 1.f,1.f,1.f,1.f };
	ImColor VisibleColorTeam{ 0.f, 0.f, 1.f, 1.f };
	ImColor NotVisibleColorTeam{ 0.f, 0.75f, 1.f, 1.f };
	ImColor VisibleColorEnemy{ 1.f, 1.f, 0.f, 1.f };
	ImColor NotVisibleColorEnemy{ 1.f,0.f,0.f,1.f };
	ImColor radar_bg_color{ 1.f,1.f,1.f,0.f };
	ImColor radar_boarder_color{ 1.f,1.f,1.f,0.f };
}

namespace screenshot
{
	bool		visuals = true;
	bool* pDrawEnabled = nullptr;
	int	screenshot_counter = 0;
	uint32_t	bit_blt_log = 0;
	const char* bit_blt_fail;
	uintptr_t	bit_blt_anotherlog;
	uint32_t	GdiStretchBlt_log = 0;
	const char* GdiStretchBlt_fail;
	uintptr_t	GdiStretchBlt_anotherlog;
	uintptr_t	texture_copy_log = 0;
	uintptr_t	virtualqueryaddr = 0;
}

namespace g_draw
{
	void draw_line(const ImVec2& from, const ImVec2& to, uint32_t color, float thickness)
	{
        auto window = ImGui::GetWindowDrawList();

        window->AddLine(from, to, color, thickness);
        
	
		/*float a = (color >> 24) & 0xff;
		float r = (color >> 16) & 0xff;
		float g = (color >> 8) & 0xff;
		float b = (color) & 0xff;*/
		
	}

	void draw_box(const float x, const float y, const float width, const float height, const uint32_t color, float thickness)
	{
		draw_line(ImVec2(x, y), ImVec2(x + width, y), color, thickness);
		draw_line(ImVec2(x, y), ImVec2(x, y + height), color, thickness);
		draw_line(ImVec2(x, y + height), ImVec2(x + width, y + height), color, thickness);
		draw_line(ImVec2(x + width, y), ImVec2(x + width, y + height), color, thickness);
	}

	void draw_corned_box(const vec2_t& rect, const vec2_t& size, uint32_t color, float thickness)
	{
		size.x - 5;
		const float lineW = (size.x / 5);
		const float lineH = (size.y / 6);
		const float lineT = 1;

		//outline
		draw_line(ImVec2(rect.x - lineT, rect.y - lineT), ImVec2(rect.x + lineW, rect.y - lineT), color, thickness); //top left
		draw_line(ImVec2(rect.x - lineT, rect.y - lineT), ImVec2(rect.x - lineT, rect.y + lineH), color, thickness);
		draw_line(ImVec2(rect.x - lineT, rect.y + size.y - lineH), ImVec2(rect.x - lineT, rect.y + size.y + lineT), color, thickness); //bot left
		draw_line(ImVec2(rect.x - lineT, rect.y + size.y + lineT), ImVec2(rect.x + lineW, rect.y + size.y + lineT), color, thickness);
		draw_line(ImVec2(rect.x + size.x - lineW, rect.y - lineT), ImVec2(rect.x + size.x + lineT, rect.y - lineT), color, thickness); // top right
		draw_line(ImVec2(rect.x + size.x + lineT, rect.y - lineT), ImVec2(rect.x + size.x + lineT, rect.y + lineH), color, thickness);
		draw_line(ImVec2(rect.x + size.x + lineT, rect.y + size.y - lineH), ImVec2(rect.x + size.x + lineT, rect.y + size.y + lineT), color, thickness); // bot right
		draw_line(ImVec2(rect.x + size.x - lineW, rect.y + size.y + lineT), ImVec2(rect.x + size.x + lineT, rect.y + size.y + lineT), color, thickness);

		//inline
		draw_line(ImVec2(rect.x, rect.y), ImVec2(rect.x, rect.y + lineH), color, thickness);//top left
		draw_line(ImVec2(rect.x, rect.y), ImVec2(rect.x + lineW, rect.y), color, thickness);
		draw_line(ImVec2(rect.x + size.x - lineW, rect.y), ImVec2(rect.x + size.x, rect.y), color, thickness); //top right
		draw_line(ImVec2(rect.x + size.x, rect.y), ImVec2(rect.x + size.x, rect.y + lineH), color, thickness);
		draw_line(ImVec2(rect.x, rect.y + size.y - lineH), ImVec2(rect.x, rect.y + size.y), color, thickness); //bot left
		draw_line(ImVec2(rect.x, rect.y + size.y), ImVec2(rect.x + lineW, rect.y + size.y), color, thickness);
		draw_line(ImVec2(rect.x + size.x - lineW, rect.y + size.y), ImVec2(rect.x + size.x, rect.y + size.y), color, thickness);//bot right
		draw_line(ImVec2(rect.x + size.x, rect.y + size.y - lineH), ImVec2(rect.x + size.x, rect.y + size.y), color, thickness);

	}

	void fill_rectangle_black(const float x, const float y, const float width, const float hight, const uint32_t color)
	{
		const float end_y = y + hight;


        auto black_color = ImGui::ColorConvertFloat4ToU32({ 0.f, 0.f, 0.f, .25f });
		for (float curr_y = y; curr_y < end_y; ++curr_y)
		{
			draw_line(ImVec2(x, curr_y), ImVec2(x + width, curr_y), black_color, Settings::esp::thickness);
		}

        draw_line(ImVec2(x, y), ImVec2(x + width, y), color, Settings::esp::thickness);
        draw_line(ImVec2(x, y), ImVec2(x, y + hight), color, Settings::esp::thickness);
        draw_line(ImVec2(x, y + hight), ImVec2(x + width, y + hight), color, Settings::esp::thickness);
        draw_line(ImVec2(x + width, y), ImVec2(x + width, y + hight), color, Settings::esp::thickness);


	}

    void fill_rectangle(const float x, const float y, const float width, const float hight, const uint32_t color)
    {
        const float end_y = y + hight;

   

        auto black_color = ImGui::ColorConvertFloat4ToU32({ 0.f, 0.f, 0.f, 1.f });
        for (float curr_y = y; curr_y < end_y; ++curr_y)
        {
            draw_line(ImVec2(x, curr_y), ImVec2(x + width, curr_y), color, 5.f);
        }


    }

	void draw_circle(const ImVec2& position, float radius, uint32_t color, float thickness)
	{
		float step = (float)M_PI * 2.0f / thickness;
		for (float a = 0; a < (M_PI * 2.0f); a += step)
		{
			draw_line(
				ImVec2(radius * cosf(a) + position.x, radius * sinf(a) + position.y),
				ImVec2(radius * cosf(a + step) + position.x, radius * sinf(a + step) + position.y),
				color,
				1.5f
			);
		}
	}

	void draw_sketch_edge_text(ImFont* pFont, const std::string& text, const ImVec2& pos, float size, uint32_t color, bool center, uint32_t EdgeColor)
	{
		constexpr float fStrokeVal1 = 1.0f;
		ImGuiWindow* window = ImGui::GetCurrentWindow();

		float Edge_a = (EdgeColor >> 24) & 0xff;
		float Edge_r = (EdgeColor >> 16) & 0xff;
		float Edge_g = (EdgeColor >> 8) & 0xff;
		float Edge_b = (EdgeColor) & 0xff;
		std::stringstream steam(text);
		std::string line;
		float y = 0.0f;
		int i = 0;
		while (std::getline(steam, line))
		{
			ImVec2 textSize = pFont->CalcTextSizeA(size, FLT_MAX, 0.0f, line.c_str());
			if (center)
			{
				window->DrawList->AddText(pFont, size, ImVec2((pos.x - textSize.x / 2.0f) - fStrokeVal1, pos.y + textSize.y * i), ImGui::GetColorU32(ImVec4(Edge_r / 255, Edge_g / 255, Edge_b / 255, Edge_a / 255)), line.c_str());
				window->DrawList->AddText(pFont, size, ImVec2((pos.x - textSize.x / 2.0f) + fStrokeVal1, pos.y + textSize.y * i), ImGui::GetColorU32(ImVec4(Edge_r / 255, Edge_g / 255, Edge_b / 255, Edge_a / 255)), line.c_str());
				window->DrawList->AddText(pFont, size, ImVec2((pos.x - textSize.x / 2.0f), (pos.y + textSize.y * i) - fStrokeVal1), ImGui::GetColorU32(ImVec4(Edge_r / 255, Edge_g / 255, Edge_b / 255, Edge_a / 255)), line.c_str());
				window->DrawList->AddText(pFont, size, ImVec2((pos.x - textSize.x / 2.0f), (pos.y + textSize.y * i) + fStrokeVal1), ImGui::GetColorU32(ImVec4(Edge_r / 255, Edge_g / 255, Edge_b / 255, Edge_a / 255)), line.c_str());
				window->DrawList->AddText(pFont, size, ImVec2(pos.x - textSize.x / 2.0f, pos.y + textSize.y * i), color, line.c_str());
			}
			else
			{
				window->DrawList->AddText(pFont, size, ImVec2((pos.x) - fStrokeVal1, (pos.y + textSize.y * i)), ImGui::GetColorU32(ImVec4(Edge_r / 255, Edge_g / 255, Edge_b / 255, Edge_a / 255)), line.c_str());
				window->DrawList->AddText(pFont, size, ImVec2((pos.x) + fStrokeVal1, (pos.y + textSize.y * i)), ImGui::GetColorU32(ImVec4(Edge_r / 255, Edge_g / 255, Edge_b / 255, Edge_a / 255)), line.c_str());
				window->DrawList->AddText(pFont, size, ImVec2((pos.x), (pos.y + textSize.y * i) - fStrokeVal1), ImGui::GetColorU32(ImVec4(Edge_r / 255, Edge_g / 255, Edge_b / 255, Edge_a / 255)), line.c_str());
				window->DrawList->AddText(pFont, size, ImVec2((pos.x), (pos.y + textSize.y * i) + fStrokeVal1), ImGui::GetColorU32(ImVec4(Edge_r / 255, Edge_g / 255, Edge_b / 255, Edge_a / 255)), line.c_str());
				window->DrawList->AddText(pFont, size, ImVec2(pos.x, pos.y + textSize.y * i), color, line.c_str());
			}
			y = pos.y + textSize.y * (i + 1);
			i++;
		}
	}


	void draw_crosshair()
	{
        auto refdef = sdk::get_refdef();
        if (is_bad_ptr(refdef))return;

		constexpr long crosshair_size = 10.0f;

		ImVec2 center = ImVec2(refdef->Width / 2, refdef->Height / 2);

		g_draw::draw_line(ImVec2((center.x), (center.y) - crosshair_size), ImVec2((center.x), (center.y) + crosshair_size), 0xFFFFFFFF, 3.f);
		g_draw::draw_line(ImVec2((center.x) - crosshair_size, (center.y)), ImVec2((center.x) + crosshair_size, (center.y)), 0xFFFFFFFF, 3.f);
	}

	void draw_fov(const float aimbot_fov)
	{
        auto refdef = sdk::get_refdef();
        if (is_bad_ptr(refdef))return;
		ImVec2 center = ImVec2(refdef->Width / 2,refdef->Height / 2);

		g_draw::draw_circle(center, aimbot_fov, 0xFFFFFFFF, 100.0f);
	}

	void draw_bones(vec2_t* bones_screenPos, long count, DWORD color)
	{
		long last_count = count - 1;
        for (long i = 0; i < last_count; ++i)
           // GameFunctions::IW8_DrawLine(bones_screenPos[i].x, bones_screenPos[i].y, bones_screenPos[i + 1].x, bones_screenPos[i + 1].y,redColor,1.f);
			g_draw::draw_line(ImVec2(bones_screenPos[i].x, bones_screenPos[i].y), ImVec2(bones_screenPos[i + 1].x, bones_screenPos[i + 1].y), color, Settings::esp::thickness);
	}


	void draw_bones(unsigned long i, DWORD color)
	{
    
		vec3_t header_to_bladder[6], right_foot_to_bladder[5], left_foot_to_bladder[5], right_hand[5], left_hand[5];
        vec2_t screen_header_to_bladder[6], screen_right_foot_to_bladder[5], screen_left_foot_to_bladder[5], screen_right_hand[5], screen_left_hand[5];

		if (GameFunctions::get_bone_by_id(i, sdk::BONE_POS_HEAD, &header_to_bladder[0]) &&
			GameFunctions::get_bone_by_id(i, sdk::BONE_POS_NECK, &header_to_bladder[1]) &&
			GameFunctions::get_bone_by_id(i, sdk::BONE_POS_CHEST, &header_to_bladder[2]) &&
			GameFunctions::get_bone_by_id(i, sdk::BONE_POS_MID, &header_to_bladder[3]) &&
			GameFunctions::get_bone_by_id(i, sdk::BONE_POS_TUMMY, &header_to_bladder[4]) &&
			GameFunctions::get_bone_by_id(i, sdk::BONE_POS_PELVIS, &header_to_bladder[5]) &&
			GameFunctions::get_bone_by_id(i, sdk::BONE_POS_RIGHT_FOOT_1, &right_foot_to_bladder[1]) &&
			GameFunctions::get_bone_by_id(i, sdk::BONE_POS_RIGHT_FOOT_2, &right_foot_to_bladder[2]) &&
			GameFunctions::get_bone_by_id(i, sdk::BONE_POS_RIGHT_FOOT_3, &right_foot_to_bladder[3]) &&
			GameFunctions::get_bone_by_id(i, sdk::BONE_POS_RIGHT_FOOT_4, &right_foot_to_bladder[4]) &&
			GameFunctions::get_bone_by_id(i, sdk::BONE_POS_LEFT_FOOT_1, &left_foot_to_bladder[1]) &&
			GameFunctions::get_bone_by_id(i, sdk::BONE_POS_LEFT_FOOT_2, &left_foot_to_bladder[2]) &&
			GameFunctions::get_bone_by_id(i, sdk::BONE_POS_LEFT_FOOT_3, &left_foot_to_bladder[3]) &&
			GameFunctions::get_bone_by_id(i, sdk::BONE_POS_LEFT_FOOT_4, &left_foot_to_bladder[4]) &&
			GameFunctions::get_bone_by_id(i, sdk::BONE_POS_LEFT_HAND_1, &right_hand[1]) &&
			GameFunctions::get_bone_by_id(i, sdk::BONE_POS_LEFT_HAND_2, &right_hand[2]) &&
			GameFunctions::get_bone_by_id(i, sdk::BONE_POS_LEFT_HAND_3, &right_hand[3]) &&
			GameFunctions::get_bone_by_id(i, sdk::BONE_POS_LEFT_HAND_4, &right_hand[4]) &&
			GameFunctions::get_bone_by_id(i, sdk::BONE_POS_RIGHT_HAND_1, &left_hand[1]) &&
			GameFunctions::get_bone_by_id(i, sdk::BONE_POS_RIGHT_HAND_2, &left_hand[2]) &&
			GameFunctions::get_bone_by_id(i, sdk::BONE_POS_RIGHT_HAND_3, &left_hand[3]) &&
			GameFunctions::get_bone_by_id(i, sdk::BONE_POS_RIGHT_HAND_4, &left_hand[4]))
		{
			right_foot_to_bladder[0] = header_to_bladder[5];
			left_foot_to_bladder[0] = header_to_bladder[5];
			right_hand[0] = header_to_bladder[3];
			left_hand[0] = header_to_bladder[3];

			

			if (!sdk::bones_to_screen(header_to_bladder, screen_header_to_bladder, 6))
				return;

			if (!sdk::bones_to_screen(right_foot_to_bladder, screen_right_foot_to_bladder, 5))
				return;

			if (!sdk::bones_to_screen(left_foot_to_bladder, screen_left_foot_to_bladder, 5))
				return;

			if (!sdk::bones_to_screen(right_hand, screen_right_hand, 5))
				return;

			if (!sdk::bones_to_screen(left_hand, screen_left_hand, 5))
				return;

          
			draw_bones(screen_header_to_bladder, 6, color);
			draw_bones(screen_right_foot_to_bladder, 5, color);
			draw_bones(screen_left_foot_to_bladder, 5, color);
			draw_bones(screen_right_hand, 5, color);
			draw_bones(screen_left_hand, 5, color);
		}
	}


	void draw_health(int i_health, vec3_t pos)
	{
		vec2_t bottom, top;

		if (!sdk::WorldToScreen(pos, &bottom))
			return;

		pos.z += 60;
		if (!sdk::WorldToScreen(pos, &top))
			return;

		top.y -= 5;
		auto height = top.y - bottom.y;
		auto width = height / 2.f;
		auto x = top.x - width / 1.8f;
		auto y = top.y;

		auto bar_width = max(0, min(127, i_health)) * (bottom.y - top.y) / 127.f;
		auto health = max(0, min(127, i_health));
		uint32_t color_health = ImGui::ColorConvertFloat4ToU32(ImVec4(0.00f, 1.000f, 0.000f, 1.000f));
        uint32_t outline = ImGui::ColorConvertFloat4ToU32(ImVec4(0.00f, 0.000f, 0.000f, 1.000f));

		g_draw::fill_rectangle(x, y, 5, 127 * (bottom.y - top.y) / 127.f, ImGui::ColorConvertFloat4ToU32(ImVec4(1.000f, 0.000f, 0.000f, 1.000f)));
		g_draw::fill_rectangle(x , y , 5, bar_width , color_health);
        g_draw::draw_box(x, y +1, 5, bar_width + 1, outline,2);

	}


}

namespace g_radar {

	float RadarPosX = 60;
	float RadarPosY = 45;
	long  RadarSize = 220;
	long  RadarRadius = RadarSize / 2;
	float RadarLineInterval = 1.0f;
	float BackgroundInterval = 5.0f;

	void show_radar_background()
	{
		/*RadarPosX = g_data::refdef->Width - RadarSize - 50;*/

		g_draw::fill_rectangle(
			RadarPosX - BackgroundInterval,
			RadarPosY - BackgroundInterval,
			RadarSize + (BackgroundInterval * 2),
			RadarSize + (BackgroundInterval * 2),
			/*0xFFFFFEFE*/
			colors::radar_boarder_color);

		g_draw::fill_rectangle(
			RadarPosX,
			RadarPosY,
			RadarSize,
			RadarSize,
			/*0xFFA8AAAA*/
			colors::radar_bg_color);

		g_draw::draw_line(
			ImVec2(RadarPosX, RadarPosY + RadarLineInterval),
			ImVec2(RadarPosX, RadarPosY + RadarSize - RadarLineInterval + 1),
			0xFF010000,
			1.3f);

		g_draw::draw_line(
			ImVec2(RadarPosX + RadarSize, RadarPosY + RadarLineInterval),
			ImVec2(RadarPosX + RadarSize, RadarPosY + RadarSize - RadarLineInterval + 1),
			0xFF010000,
			1.3f);

		g_draw::draw_line(
			ImVec2(RadarPosX, RadarPosY),
			ImVec2(RadarPosX + RadarSize - RadarLineInterval + 2, RadarPosY),
			0xFF010000,
			1.3f);

		g_draw::draw_line(
			ImVec2(RadarPosX, RadarPosY + RadarSize),
			ImVec2(RadarPosX + RadarSize - RadarLineInterval + 2, RadarPosY + RadarSize),
			0xFF010000,
			1.3f);


		g_draw::draw_line(
			ImVec2(RadarPosX + RadarRadius, RadarPosY + RadarLineInterval),
			ImVec2(RadarPosX + RadarRadius, RadarPosY + RadarSize - RadarLineInterval),
			0xFF010000,
			1.3f);

		g_draw::draw_line(
			ImVec2(RadarPosX, RadarPosY + RadarRadius),
			ImVec2(RadarPosX + RadarSize - RadarLineInterval, RadarPosY + RadarRadius),
			0xFF010000,
			1.3f);


		g_draw::draw_box(RadarPosX + RadarRadius - 3, RadarPosY + RadarRadius - 3, 5, 5, 0xFF000100, 1.0f);
		g_draw::fill_rectangle(RadarPosX + RadarRadius - 2, RadarPosY + RadarRadius - 2, 4, 4, 0xFFFFFFFF);
	}

	ImVec2 rotate(const ImVec2& center, const ImVec2& pos, float angle)
	{
		ImVec2 Return;
		angle *= -(M_PI / 180.0f);
		float cos_theta = cos(angle);
		float sin_theta = sin(angle);
		Return.x = (cos_theta * (pos[0] - center[0]) - sin_theta * (pos[1] - center[1])) + center[0];
		Return.y = (sin_theta * (pos[0] - center[0]) + cos_theta * (pos[1] - center[1])) + center[1];
		return Return;
	}

	vec2_t radar_rotate(const float x, const float y, float angle)
	{
		angle = (float)(angle * (M_PI / 180.f));
		float cosTheta = (float)cos(angle);
		float sinTheta = (float)sin(angle);
		vec2_t returnVec;
		returnVec.x = cosTheta * x + sinTheta * y;
		returnVec.y = sinTheta * x - cosTheta * y;
		return returnVec;
	}

	void draw_entity(const ImVec2& pos, float angle, DWORD color)
	{
		constexpr long up_offset = 7;
		constexpr long lr_offset = 5;

		for (int FillIndex = 0; FillIndex < 5; ++FillIndex)
		{
			ImVec2 up_pos(pos.x, pos.y - up_offset + FillIndex);
			ImVec2 left_pos(pos.x - lr_offset + FillIndex, pos.y + up_offset - FillIndex);
			ImVec2 right_pos(pos.x + lr_offset - FillIndex, pos.y + up_offset - FillIndex);

			ImVec2 p0 = rotate(pos, up_pos, angle);
			ImVec2 p1 = rotate(pos, left_pos, angle);
			ImVec2 p2 = rotate(pos, right_pos, angle);

			g_draw::draw_line(p0, p1, FillIndex == 0 ? 0xFF010000 : color, 1.0f);
			g_draw::draw_line(p1, p2, FillIndex == 0 ? 0xFF010000 : color, 1.0f);
			g_draw::draw_line(p2, p0, FillIndex == 0 ? 0xFF010000 : color, 1.0f);
		}
	}

	void draw_death_entity(const ImVec2& pos, DWORD color)
	{
		constexpr float line_radius = 5;
		ImVec2 p0(pos.x - line_radius, pos.y - line_radius);
		ImVec2 p1(pos.x + line_radius, pos.y + line_radius);
		ImVec2 p3(pos.x - line_radius, pos.y + line_radius);
		ImVec2 p4(pos.x + line_radius, pos.y - line_radius);
		g_draw::draw_line(p0, p1, color, 1.5f);
		g_draw::draw_line(p3, p4, color, 1.5f);
	}

	void draw_entity(sdk::player_t local_entity, sdk::player_t entity, bool IsFriendly,  DWORD color, ImVec2 window_pos, int RadarRadius1)
	{
		const float local_rotation = local_entity.get_rotation();
		float rotation = entity.get_rotation();

		rotation = rotation - local_rotation;

		if (rotation < 0)
			rotation = 360.0f - std::fabs(rotation);

		float x_distance = local_entity.get_pos().x - entity.get_pos().x;
		float y_distance = local_entity.get_pos().y - entity.get_pos().y;

		float zoom = Settings::esp::radar_zoom * 0.001f;

		x_distance *= zoom;
		y_distance *= zoom;

		vec2_t point_offset = radar_rotate(x_distance, y_distance, local_rotation);

		long positiveRadarRadius = RadarRadius1 - 5;
		long negaRadarRadius = (RadarRadius1 * -1) + 5;

		if (point_offset.x > positiveRadarRadius)
		{
			point_offset.x = positiveRadarRadius;
		}
		else if (point_offset.x < negaRadarRadius)
		{
			point_offset.x = negaRadarRadius;
		}

		if (point_offset.y > positiveRadarRadius)
		{
			point_offset.y = positiveRadarRadius;
		}
		else if (point_offset.y < negaRadarRadius)
		{
			point_offset.y = negaRadarRadius;
		}

		
			draw_entity(ImVec2(window_pos.x + RadarRadius1 + point_offset.x, window_pos.y + RadarRadius1 + point_offset.y), rotation, color);
		
		
		
	}
}
