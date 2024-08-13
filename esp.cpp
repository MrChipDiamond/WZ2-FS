#include "stdafx.h"
#include "esp.h"
#include "sdk.h"
#include "aim.h"
#include "settings.h"
#include "Menu.h"
#include "offsets.h"
#include"INPUT.h"
#include "spoofcall.h"
#include "aimbot.h"
#include "gamefunctions.h"
#include "xor.hpp"
#include "classes.h"
bool once = false;
using Vector2 = vec2_t;

constexpr auto FL_NO_GULAG = 1u << 2;
constexpr auto FL_IN_DUCK = 1u << 3;
constexpr auto FL_IN_PRONE = 1u << 4;
constexpr auto FL_IN_ATTACK = 1u << 10;
constexpr auto FL_CAN_MOVE = 1u << 16;
constexpr auto FL_IS_DEAD = 1u << 17;
constexpr auto FL_IN_CLIMB = 1u << 24;
constexpr auto FL_IN_ZOOM = 1u << 25;
constexpr auto FL_IN_VEHICLE = 1u << 26;

#define MAX_ENTITIES 2047



float* Rect_T::toFloat()
{
	return (float*)this;
}
void Rect_T::getRect(float x, float y, float w, float h)
{
	this->lt_x = x; this->lt_y = y; this->rt_x = x + w; this->rt_y = y; this->rb_x = x + w; this->rb_y = y + h; this->lb_x = x; this->lb_y = y + h;
}


float* Engine_Color::toFloat()
{
	return (float*)this;
}

Engine_Color* getColorFromRGB(float r, float g, float b, float t)
{
	return new Engine_Color((1.f / 255.f) * r, (1.f / 255.f) * g, (1.f / 255.f) * b, (1.f / 255.f) * t);
}


Vector2 radar_rotate(const float x, const float y, float angle)
{
	angle = (float)(angle * (M_PI / 180.f));
	float cosTheta = (float)cos(angle);
	float sinTheta = (float)sin(angle);
	Vector2 returnVec;
	returnVec.x = cosTheta * x + sinTheta * y;
	returnVec.y = sinTheta * x - cosTheta * y;
	return returnVec;
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



void imgui_draw_cross()
{


	ImDrawList* draw_list = ImGui::GetBackgroundDrawList();

	ImVec2 winpos = ImGui::GetWindowPos();
	ImVec2 winsize = ImGui::GetWindowSize();

	draw_list->AddLine(ImVec2(winpos.x + winsize.x * 0.5f, winpos.y), ImVec2(winpos.x + winsize.x * 0.5f, winpos.y + winsize.y), ImColor(70, 70, 70, 255), 1.f);
	draw_list->AddLine(ImVec2(winpos.x, winpos.y + winsize.y * 0.5f), ImVec2(winpos.x + winsize.x, winpos.y + winsize.y * 0.5f), ImColor(70, 70, 70, 255), 1.f);

	draw_list->AddLine(ImVec2(winpos.x + winsize.x * 0.5f, winpos.y + winsize.y * 0.5f), ImVec2(winpos.x, winpos.y), ImColor(90, 90, 90, 255), 1.f);
	draw_list->AddLine(ImVec2(winpos.x + winsize.x * 0.5f, winpos.y + winsize.y * 0.5f), ImVec2(winpos.x + winsize.x, winpos.y), ImColor(90, 90, 90, 255), 1.f);
}

int RadarSize{};
int RadarRadius1{};





ImGuiWindowFlags flag_menu = ImGuiWindowFlags_::ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_::ImGuiWindowFlags_NoResize | ImGuiWindowFlags_::ImGuiWindowFlags_NoScrollbar;
ImGuiWindowFlags flag_no_menu = ImGuiWindowFlags_::ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_::ImGuiWindowFlags_NoResize | ImGuiWindowFlags_::ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_::ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_::ImGuiWindowFlags_NoMove;
void RadarGui(sdk::player_t local_entity, sdk::player_t entity, bool IsFriendly,  DWORD color)
{



	if (Settings::esp::b_radar)
	{
	
		ImGuiStyle& style = ImGui::GetStyle();
		style.WindowMinSize = ImVec2(230, 230);
		const float bor_size = style.WindowBorderSize;
		style.WindowBorderSize = 0.0f;


		style.WindowBorderSize = bor_size;
		style.WindowPadding = ImVec2(8, 8);
		style.WindowRounding = 10.0f;
		style.ChildRounding = 0.f;
		style.FramePadding = ImVec2(5, 5);
		style.FrameRounding = 6.0f;
		style.PopupRounding = 3.f;
		style.ItemSpacing = ImVec2(8, 8);
		style.ItemInnerSpacing = ImVec2(8, 6);
		style.IndentSpacing = 25.0f;
		style.ScrollbarSize = 15.0f;
		style.ScrollbarRounding = 9.0f;
		style.GrabMinSize = 5.0f;
		style.GrabRounding = 6.0f;
		style.WindowTitleAlign = { 0.5f,0.5f };






		style.Colors[ImGuiCol_WindowBg] = ImVec4(0.06f, 0.05f, 0.07f, 0.5f);

		if (g_menu::b_menu_open)
			ImGui::Begin("Radar", 0, flag_menu);
		else
			ImGui::Begin("Radar", 0, flag_no_menu);

		Settings::esp::radar_getpos_x = ImGui::GetWindowPos().x;
		Settings::esp::radar_getpos_y = ImGui::GetWindowPos().y;
		ImGui::SetWindowPos(ImVec2(Settings::esp::radar_getpos_x, Settings::esp::radar_getpos_y));

		ImGui::SetWindowSize(ImVec2(230, 230));
		RadarSize = 233;
		RadarRadius1 = RadarSize / 2;
		imgui_draw_cross();

		g_radar::draw_entity(local_entity, entity, IsFriendly, color, ImGui::GetWindowPos(), RadarRadius1);

		ImGui::End();
	}
}







int get_player_health(int i)
{
	uint64_t bgs = *(uint64_t*)(sig_offsets::name_array);

	if (is_valid_ptr(bgs))
	{
		sdk::name_t* info = (sdk::name_t*)(bgs + 0x4C70 + (i * 0xd0));
		
		 return info->health;

		
	}
	return 0;
}
void updateInput() noexcept
{
	if (g_menu::aimbotKeyMode == 0)
		keyPressed = aimbotkey.isDown();
	if (g_menu::aimbotKeyMode == 1 && aimbotkey.isPressed())
		keyPressed = !keyPressed;
}
namespace main_game
{


	void ui_header()
	{
		ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));
		ImGui::Begin("A", reinterpret_cast<bool*>(true), ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBringToFrontOnFocus);
		ImGui::SetWindowPos(ImVec2(0, 0), ImGuiCond_Always);
		ImGui::SetWindowSize(ImVec2(ImGui::GetIO().DisplaySize.x, ImGui::GetIO().DisplaySize.y), ImGuiCond_Always);
	}

	void ui_end()
	{
		ImGuiWindow* window = ImGui::GetCurrentWindow();
		window->DrawList->PushClipRectFullScreen();
		ImGui::End();
		ImGui::PopStyleColor();
	}
	bool b_panic = false;
	void init(ImFont* font)
	{

		ui_header();
		esp_loop(font);
		ui_end();
	}

	struct  XModel
	{
		const char* name;
		unsigned __int16 numsurfs;
		unsigned __int8 numLods;
		unsigned __int8 collLod;
		unsigned __int16 mdaoVolumeCount;
		unsigned __int8 shadowCutoffLod;
	
	};

	class CG_ClientModel_RuntimeData
	{
	public:
		XModel* models; //0x0000
		char pad_0008[184]; //0x0008
		uint16_t modelCount; //0x00C0
		int8_t hudOutlineEnabled; //0x00C2
		bool hudOutlineFill; //0x00C3
		uint32_t hudOutlineColor; //0x00C4
		int8_t hudOutlineLineWidth; //0x00C8
		int8_t hudOutlineRenderMode; //0x00C9
		char pad_00CA[102]; //0x00CA
		uint8_t eType; //0x0130
		char pad_0131[47]; //0x0131
		vec3_t vecOrigin; //0x0160
		char pad_016C[36]; //0x016C
		vec3_t vecOrigin2; //0x0190
		char pad_019C[180]; //0x019C
	}; //Size: 0x0250

	CG_ClientModel_RuntimeData* Get_loot_By_Id(uint32_t id)
	{
		auto addr = sig_offsets::loot_ptr;
		auto ret = addr + (static_cast<uint64_t>(id) * sizeof(CG_ClientModel_RuntimeData));
		return (CG_ClientModel_RuntimeData*)ret;
	}

	struct loot
	{

		auto get_Internal_name() -> char*;
		auto weapon_name() -> const char*;


		auto is_valid() -> bool;

		auto get_position()->vec3_t;

	};

	struct loot_definition_t
	{
		char internal_name[50];
		char name[50];
		bool* show = NULL;
	};

	auto loot::get_Internal_name() -> char*
	{
		auto ptr = *(uintptr_t*)(this + 0);
		if (is_bad_ptr(ptr))
			return NULL;

		return *(char**)(ptr);
	};

	auto loot::weapon_name() -> const char*
	{
		auto ptr = (Weapon*)(this + 0xCA);

		__int64* WeaponCompleteDefptr = (__int64*)(sig_offsets::weapon_definitions);

		char* weaponName = *(char**)(WeaponCompleteDefptr[ptr->weaponIdx] + 0x18); // WEAPON/internal name

		return GameFunctions::UI_SafeTranslateString(weaponName); // translate internal weapon name into "real names"
		
	};
	

	auto loot::is_valid() -> bool
	{
		return *(char*)(this + 0xC8) == 1 && *(char*)(this + 0xC9) == 1;
	};

	auto loot::get_position() -> vec3_t
	{
		return *(vec3_t*)(this + 0x168);
	};

	auto get_loot_by_id(const uint32_t id) -> loot*
	{
		return (loot*)(sig_offsets::loot_ptr) + (static_cast<uint64_t>(id) * 0x258);

	}




	const loot_definition_t definitions[] =
	{


		{ "accessory_money", "CASH", &Settings::itemesp::b_CASH },
		{ "loot_wm_armor_plate", "Amor", &Settings::itemesp::b_Amor },
		{ "lm_armor_vest_01", "Satchel", &Settings::itemesp::b_Satchel },
		{ "gasmask_a", "Gasmask", &Settings::itemesp::b_Gasmask },
		/*{ "art_loot_coin_specialist_01", "Specialist Token", &trr },*/
		{ "offhand_vm_supportbox_armor_br", "Armor Box", &Settings::itemesp::b_Armor_Box },
		{ "offhand_vm_supportbox_br", "Ammo Box", &Settings::itemesp::b_Ammo_Box },
		{ "loot_crate_01_br_legendary_01", "CRATE LEGENDARY", &Settings::itemesp::b_Crate_legendary },
		/*	{ "loot_crate_01_br_rebirth_reuseable","CRATE REBIRTH", &trr },*/
			{ "military_loot_crate", "CRATE MILITARY", &Settings::itemesp::b_Crate_normal },

			// Tablets
			// Mission
			{ "mission_tablet_01_scavenger", "Mission Scavenger", &Settings::itemesp::b_Scav},
			{ "mission_tablet_01_assassination", "Mission Most_Wanted", &Settings::itemesp::b_Most_Wanted },
			/*{ "mission_tablet_01_timedrun", "mission_tablet_01_timedrun", &trr },*/




			// Ammo
			{ "loot_ammo_pistol_smg", "SMG AMMO", &Settings::itemesp::b_ammo_sm },
			{ "ammo_box_ar", "AR AMMO", &Settings::itemesp::b_ammo_ar },
			{ "ammo_shotgun", "Shotgun AMMO", &Settings::itemesp::b_ammo_sg},
			{ "ammo_marksman_sniper", "SNIPER AMMO", &Settings::itemesp::b_ammo_sn},
			/*{ "ammo_rocket", "ROCKET AMMO", &trr },*/

			{ "wm_weapon_root_s4_ar", "Assault Rifle", &Settings::itemesp::b_weapon_ar},
			{ "wm_weapon_root_s4_pi", "Pistol", &Settings::itemesp::b_weapon_pi},
			{ "wm_weapon_root_s4_sm", "SMG", &Settings::itemesp::b_weapon_sm },
			{ "wm_weapon_root_s4_sn", "Sniper", &Settings::itemesp::b_weapon_sn },
			{ "wm_weapon_root_s4_lm", "LMG", &Settings::itemesp::b_weapon_lm },

			//Real wepon names
		//{"weapon_wm_ar_mike4", "mike4", &trr },
		//{"weapon_wm_sn_t9powersemi", "t9powersemi", &trr },
		//{"weapon_wm_ar_mcharlie", "mcharlie", &trr },
		//{"weapon_wm_sh_oscar12", "oscar12", &trr },
		//{"weapon_wm_sn_hdromeo", "hdromeo", &trr },

		//{"weapon_wm_sm_mpapa5", "mpapa5", &trr },
		//{"weapon_wm_sn_hdromeo", "hdromeo", &trr },
		//{"weapon_wm_stream_lmg_lima86", "lmg_lima86", &trr }, //bren
		//{"wm_weapon_stream_proxy_ar", "Bar", &trr }, //bar;
		//{"wm_weapon_stream_proxy_sn", "proxy_sn", &trr },
		//{"wm_weapon_stream_proxy_sm", "proxy_sm", &trr },
		//{"weapon_wm_sn_kilo98", "kilo98", &trr },
		//{"weapon_wm_pi_golf21", "golf21", &trr },
		//{"weapon_wm_ar_kilo433", "kilo433", &trr },
		//{"weapon_wm_ar_t9damage", "t9damage", &trr },

		//Equipment
		{"lm_offhand_wm_grenade_flash", "flash", &Settings::itemesp::b_flash },
		{"offhand_vm_grenade_decoy", "decoy", &Settings::itemesp::b_decoy },

		{"lm_heal_stim_syringe_01", "Stim", &Settings::itemesp::b_stim },
		{"offhand_wm_grenade_thermite_br", "Thermite", &Settings::itemesp::b_thermite },
		{"lm_offhand_wm_grenade_mike67", "Grenade", &Settings::itemesp::b_grenade},
		{"lm_offhand_wm_grenade_semtex", "Semtex", &Settings::itemesp::b_semtex},
		{"lm_offhand_wm_grenade_snapshot", "Snapshot", &Settings::itemesp::b_snapshot},
		{"lm_offhand_wm_grenade_concussion", "Concussion", &Settings::itemesp::b_Concussion },
		{"lm_offhand_wm_c4", "C4", &Settings::itemesp::b_C4 },
		{"weapon_wm_knife_offhand_thrown_electric", "Throwing Knife Electric", &Settings::itemesp::b_Knife },
		{"m_weapon_wm_knife_offhand_thrown_fire", "Throwing Knife Fire", &Settings::itemesp::b_Knife  },
		{ "lm_offhand_wm_at_mine", "Mine", &Settings::itemesp::b_Mine},
		{ "lm_weapon_wm_knife_offhand_thrown", "Throwing Knife", &Settings::itemesp::b_Knife  },
		{ "lm_offhand_wm_claymore", "Claymore", &Settings::itemesp::b_Claymore },



		//Unknown
	/*	{"lm_offhand_wm_tac_cover", "tac_cover", &trr },
		{"offhand_wm_jerrycan", "jerrycan", &trr },
		{"x2_buy_station_rig_skeleton", "buystation", &trr },
		{"veh8_mil_air_malfa_small_noscript", "veh8_mil_air", &trr },
		{"offhand_wm_trophy_system_br", "TrophySystem", &trr }*/
		{"perk_manual_01_dead_silence", "deadsilence", &Settings::itemesp::b_ammo_ar },



		//other strike,Uav ect

		{"wm_killstreak_handset_ch3_airstrike", "Airstrike", &Settings::itemesp::b_airstrike },
		{"killstreak_tablet_01_clusterstrike", "Clusterstrike", &Settings::itemesp::b_Clusterstrike },
		{"killstreak_tablet_01_uav", " UAV", &Settings::itemesp::b_UAV },

	};

	float fdistance{};
	void item_esp(ImFont* font)
	{

		auto in_game = sdk::in_game();

		if (!in_game)
		{
			return;
		}

	


		if (Settings::itemesp::b_enable_LootESP)
		{


			for (auto j = 0; j < 600; j++)
			{
				sdk::player_t Localplayer = sdk::get_local_player();
				if (is_bad_ptr(Localplayer.address))continue;

				ImColor loot_color{};
				float display_string2{};
				auto loot = get_loot_by_id(j);

				if (is_bad_ptr(loot))
					continue;

				if (!loot->is_valid())
					continue;

				auto name = loot->get_Internal_name();

				if (!strlen(name))
					continue;

				auto pos = loot->get_position();
				if (pos.is_Zero())continue;

				vec2_t ScreenPos2, BoxSize2;
				if (!sdk::w2s(pos, &ScreenPos2, &BoxSize2))continue;
				loot_color = Settings::itemesp::loot_color;
				fdistance = sdk::units_to_m(Localplayer.get_pos().distance_to(pos));
				if (fdistance > Settings::itemesp::max_distance)
					continue;

				for (auto k = 0; k < IM_ARRAYSIZE(definitions); k++)
				{
					auto* loot_def = &definitions[k];

					if (name && strstr(name, loot_def->internal_name))
					{
						if (loot_def->show != 0 && !*loot_def->show)
							continue;
						if (Settings::itemesp::b_name)
						{

						/*	if ("wm_weapon_root_s4_ar" == name || "wm_weapon_root_s4_pi" == name || "wm_weapon_root_s4_sm" == name || "wm_weapon_root_s4_sn" == name || "wm_weapon_root_s4_lm" == name)
							{

								g_draw::draw_sketch_edge_text(font, loot->weapon_name(), ImVec2(
									ScreenPos2.x + (BoxSize2.x / 2),
									ScreenPos2.y + BoxSize2.y + 5 + display_string2),
									18.0f,
									loot_color,
									true
								);
								
							}*/

							/* if ("wm_weapon_root_s4_ar"!= name || "wm_weapon_root_s4_pi" != name || "wm_weapon_root_s4_sm" != name || "wm_weapon_root_s4_sn" != name || "wm_weapon_root_s4_lm" != name)*/

							{

								 g_draw::draw_sketch_edge_text(font, loot_def->name, ImVec2(
									 ScreenPos2.x + (BoxSize2.x / 2),
									 ScreenPos2.y + BoxSize2.y + 5 + display_string2),
									 18.0f,
									 loot_color,
									 true
								 );
						
							}

			
						

						}

						if (Settings::itemesp::b_distance)
						{
							std::string distance_string("[ ");
							distance_string += std::to_string(fdistance);
							std::string::size_type end = distance_string.find('.');
							if (end != std::string::npos && (end + 2) <= (distance_string.length() - 1))
								distance_string.erase(distance_string.begin() + end + 2, distance_string.end());
							distance_string += "m ]";
							g_draw::draw_sketch_edge_text(font, distance_string, ImVec2(
								ScreenPos2.x + (BoxSize2.x / 2),
								ScreenPos2.y + BoxSize2.y + 5 + 20),
								18.0f,
								loot_color,
								true
							);
						}


					}

				}
			}
		}

	}


	void draw_bones1(vec2_t* bones_screenPos, long count, DWORD color)
	{
		long last_count = count - 1;
		for (long i = 0; i < last_count; ++i)
			g_draw::draw_line(ImVec2(bones_screenPos[i].x, bones_screenPos[i].y), ImVec2(bones_screenPos[i + 1].x, bones_screenPos[i + 1].y), color, Settings::esp::thickness);
	}



	void cache_bones()
	{
		auto ingame = sdk::in_game();



		if (!ingame)
		{
			boneArray.clear();
			return;
		}


		sdk::player_t local_player = sdk::get_local_player();
		auto local = local_player.address;
		if (is_bad_ptr(local))
			return;




		

		for (int i = 0; i < MAX_ENTITIES; ++i)
		{
			playerState_t* playerstate = *(playerState_t**)(sig_offsets::p_playerstate);
			entityState_t* entitystate = GameFunctions::GetEntityState(i);
			if (is_bad_ptr(entitystate)|| is_bad_ptr(playerstate))continue;
		
			if (sdk::GetLocalIndex() == i)continue;

			
			

		
			if (entitystate->eType == entityType_s::ET_PLAYER)
			{
				for (auto& ent : boneArray)
				{
					if (ent.drawn == false && ent.processed == false)
					{
						for (int k{}; k < skeleton.size(); ++k)
						{
							if (GameFunctions::get_bone_by_id(ent.entindex, skeleton[k].first, &ent.bone1_3d[k]) && GameFunctions::get_bone_by_id(ent.entindex, skeleton[k].second, &ent.bone2_3d[k]))
							{
								if (ent.bone1_3d[k].is_Zero() || ent.bone2_3d[k].is_Zero())break;
							}
						}

						ent.processed = true;
					}
				}

				for (int j{}; j < boneArray.size(); ++j)
				{
					if (boneArray[j].drawn == true && boneArray[j].processed == true)
					{
						boneArray.erase(boneArray.begin() + j);
					}
				}

			}
		}
	}

	void drawBone(vec3_t& bone1_3d, vec3_t& bone2_3d, vec3_t& cLocalPosInfo, uint32_t& color)
	{
		vec2_t bone1{};
		vec2_t bone2{};

		if (cLocalPosInfo.distance_to(bone1_3d) > 118) return;
		/*	if (cLocalPosInfo.distance_to(bone2_3d) > 118) return;*/
			/*if (cLocalPosInfo.distance_to(bone2_3d) > 39) return;*/

		if (sdk::WorldToScreen(bone1_3d, &bone1) && sdk::WorldToScreen(bone2_3d, &bone2))
		{
			if (bone1.is_Zero() && bone2.is_Zero())
				return;

			g_draw::draw_line(ImVec2(bone1.x, bone1.y), ImVec2(bone2.x, bone2.y), color, 1.f);
		}
	}

	void draw_bones(int index, vec3_t pos, uint32_t color)
	{
		if (boneArray.size() < 2048)
		{
			boneArray.emplace_back(bonestest(index));
		}
		else if (boneArray.size() <= 2048)
		{
			boneArray.clear();

		}

		for (auto& ent : boneArray)
		{
			if (ent.processed == true)
			{
				for (int k{}; k < skeleton.size(); ++k)
				{
					if (!ent.bone1_3d[k].is_Zero() || !ent.bone2_3d[k].is_Zero())
					{

						drawBone(ent.bone1_3d[k], ent.bone2_3d[k], pos, color);

					}
				}
				ent.drawn = true;
			}
		}
	}






	DWORD Local_Team_Id{};
	DWORD Entity_Team_Id{};
	vec3_t out{};

	__int64 WeaponStruct(entityState_t* entitystate)
	{

		short mapEntryId = entitystate->staticState.player.stowedWeaponHandle.m_mapEntryId;

		_int64* WeaponMap = (__int64*)(g_data::base + 0x151483D0);

		__int64 v7 = ((__int64)mapEntryId << 6) + *(__int64*)(*WeaponMap + 8); 

		return v7 + 2;
	}


	std::string  GetWeaponName(entityState_t* entitystate)
	{
		unsigned int weaponId = reinterpret_cast<Weapon_t*>(WeaponStruct(entitystate))->weaponIdx;

		__int64* WeaponCompleteDefptr = (__int64*)(sig_offsets::weapon_definitions);

		char* weaponName = *(char**)(WeaponCompleteDefptr[weaponId] + 0x18);

		return GameFunctions::UI_SafeTranslateString(weaponName); //E9 ? ? ? ? 48 8D 05 ? ? ? ? 48 83 C4 20 5B C3 CC B1
	}



	ImColor current_color{};


	void uatest()	
	{
		playerState_t* playerstate1 = *(playerState_t**)(sig_offsets::p_playerstate);

			if (is_bad_ptr(playerstate1))return;

			playerstate1->UAV = 0x2;
			playerstate1->radarEnabled = true;
			playerstate1->radarShowEnemyDirection = true;
	}
	void main_loop (ImFont* font)
	{

		if (!sdk::in_game())
		{
			boneArray.clear();
			return;
		}
	
		auto refdef = sdk::get_refdef();
		if (is_bad_ptr(refdef))return;

		
		sdk::player_t local_player = sdk::get_local_player();

		if (is_bad_ptr(local_player.address))
			return;

		if (Settings::aimbot::b_recoil)
		{
			sdk::no_recoil();
		}



		if (Settings::esp::b_crosshair)
			g_draw::draw_crosshair();

		if (Settings::esp::b_fov)
			g_draw::draw_fov(Settings::esp::fov_size);


	
		if (Settings::esp::b_visible)
		{
			g_data::visible_base = sdk::get_visible_base();
		}


		if (Settings::esp::b_warning)
		{
			//sdk::Danger(0);
		}

	

		for (int i = 0; i < MAX_ENTITIES; ++i)
		{
			float fdistance = 0;
			float display_string = 0;
	
			entityState_t* entitystate = GameFunctions::GetEntityState(i);

			if (is_bad_ptr(entitystate) /*|| is_bad_ptr(playerstate)*/)
				continue;

			if (entitystate->eType == entityType_s::ET_PLAYER) // player 
			{

				if (sdk::GetLocalIndex() == i )
					continue;
			
				sdk::player_t player = sdk::get_player(i);
				if (is_bad_ptr(player.address))
					continue;
					
				vec3_t pos = GameFunctions::GetOriginForClosestCharacters(i);

				if (pos.is_Zero())continue;
										
			auto is_friendly = player.team_id() == local_player.team_id();

				if (is_friendly && !Settings::esp::b_friendly)
					continue;

				auto health = get_player_health(i);
				auto is_visible = player.is_visible(i);



			fdistance = sdk::units_to_m(local_player.get_pos().distance_to(pos));

			if (fdistance >= Settings::esp::max_distance)
				continue;

			if (Settings::esp::b_visible && is_visible)
				current_color = is_friendly ? Settings::colors::visible_team : Settings::colors::visible_enemy;
			else
				current_color = is_friendly ? Settings::colors::not_visible_team : Settings::colors::not_visible_enemy;
			if (sdk::KNOCKED == player.get_stance() && !is_friendly)
				current_color = Settings::colors::downed;

			//if (Settings::esp::b_radar)
			//	RadarGui(local_player, player, is_friendly,  current_color);

			vec2_t ScreenPos{}, BoxSize{};

			if (!sdk::w2s(pos, &ScreenPos, &BoxSize))continue;

				if (Settings::esp::b_box)
				{
					switch (Settings::esp::box_index)
					{
					case 0:g_draw::draw_corned_box(ScreenPos, BoxSize, current_color, Settings::esp::thickness); break;
					case 1:g_draw::draw_box(ScreenPos.x, ScreenPos.y, BoxSize.x, BoxSize.y, current_color, Settings::esp::thickness); break;
					case 2:g_draw::fill_rectangle_black(ScreenPos.x, ScreenPos.y, BoxSize.x, BoxSize.y, current_color); break;
					case 3:g_draw::fill_rectangle(ScreenPos.x, ScreenPos.y, BoxSize.x, BoxSize.y, current_color); break;
					default:
						break;
					}

				}

			
				if (Settings::esp::b_line)
				{
					ImVec2 origin;


					switch (Settings::esp::line_choose)
					{
					case 0:
					{
						origin.x = refdef->Width / 2;
						origin.y = refdef->Height / 2;
						break;
					}
					case 1:
					{
						origin.x = refdef->Width / 2;
						origin.y = refdef->Height;
						break;

					}
					default:
						break;
					}

					g_draw::draw_line(origin, ImVec2(ScreenPos.x, ScreenPos.y), current_color, Settings::esp::thickness);
				}
				if (Settings::esp::b_names)
				{
					std::string name = sdk::get_player_name(i);
					g_draw::draw_sketch_edge_text(font, name, ImVec2(
						ScreenPos.x + (BoxSize.x / 2),
						ScreenPos.y + BoxSize.y + 5 + display_string),
						18.0f,
						current_color,
						true
					);

					display_string += 15;
				}

				if (Settings::esp::b_health)
				{
					g_draw::draw_health(health, pos);
				}

				if (Settings::esp::b_distance)
				{
					std::string distance_string("[ ");
					distance_string += std::to_string(fdistance);
					std::string::size_type end = distance_string.find('.');
					if (end != std::string::npos && (end + 2) <= (distance_string.length() - 1))
						distance_string.erase(distance_string.begin() + end + 2, distance_string.end());
					distance_string += "m ]";
					g_draw::draw_sketch_edge_text(font, distance_string, ImVec2(
						ScreenPos.x + (BoxSize.x / 2),
						ScreenPos.y + BoxSize.y + 5 + display_string),
						18.0f,
						current_color,
						true
					);

					display_string += 15;
				}

				if (Settings::esp::b_weaponname)
				{
					std::string name = GetWeaponName(entitystate);
					g_draw::draw_sketch_edge_text(font, name, ImVec2(
						ScreenPos.x + (BoxSize.x / 2),
						ScreenPos.y + BoxSize.y + 5 + display_string),
						18.0f,
						current_color,
						true
					);

				}
				if (Settings::esp::b_skeleton)
				{
					draw_bones(i, pos, current_color);
				}
			
			}
		

		}
		
	}





	



		






	
	bool outline_rarity = false;

	vec3_t bonepos{};
	vec2_t screenpos{};
	void aimbot_thread2()
	{
		aim_assistclass aim = aim_assistclass();
		auto ingame = sdk::in_game();


		if (!ingame)
		{
			sdk::clear_vel_map();
			once = false;
			return;
		}

	

	

		auto ref_def = sdk::get_refdef();
		if (is_bad_ptr(ref_def))
			return;
		sdk::player_t local_player = sdk::get_local_player();

		if (is_bad_ptr(local_player.address))
			return;
		playerState_t* playerstate = *(playerState_t**)(sig_offsets::p_playerstate);
		if (is_bad_ptr(playerstate))return;

	
		aim_assist_class.reset_lock();




		if (Settings::aimbot::visible_only)
		{
			g_data::visible_base = sdk::get_visible_base();
		}
	
		updateInput();

		for (int i = 0; i < 2048; ++i)
		{
		

			if (sdk::GetLocalIndex() == i) // 
				continue;
			float fdistance = 0;
			float display_string = 0;
			entityState_t* entitystate = GameFunctions::GetEntityState(i);

			if (is_bad_ptr(entitystate))
				continue;

		

			if (entitystate->eType == 1)
			{
				sdk::player_t player = sdk::get_player(i);
				

				if (is_bad_ptr(player.address) && i == currentPlayer)
				{
					currentPlayer = -1;
				}

				if (is_bad_ptr(player.address))
					continue;

				bool is_friendly = player.team_id() == local_player.team_id();;

			

			

				fdistance = sdk::units_to_m(local_player.get_pos().distance_to(player.get_pos()));



				if (fdistance > Settings::esp::max_distance)
					continue;

				auto is_visible = player.is_visible(i);

			
				if (Settings::aimbot::b_lock)
				{

					if ((Settings::aimbot::visible_only && !is_visible) || (is_bad_ptr(player.address) || (player.get_stance() == sdk::KNOCKED && Settings::aimbot::b_skip_knocked)))
					{
						if (i == currentPlayer)
						{
							currentPlayer = -1;
						}
					}

					if (is_friendly)
						continue;

					if (!GameFunctions::get_bone_by_id(i, sdk::BONE_POS_HEAD, &bonepos))
						continue;
					if (sdk::WorldToScreen(bonepos, &screenpos))
					{
						if(!aim_assist_class.get_target_in_fov(screenpos, bonepos, i))continue;
					}

				
						
				}
			}
		}
		if (!g_menu::b_menu_open && Settings::aimbot::b_lock)
			aim_assist_class.lock_on_target();
	}

	bool  FD_Trace(vec3_t start, vec3_t end, int entnum)
	{
		trace_t trace;
		VariadicCall<void>((g_data::base + 0x23D06B0), 0, &trace, start, end, 0, 0x2806931, 0);
		return(trace.hitId == entnum || trace.fraction == 1.f);
	}

	int SmartBoneSelection(int index)
	{
		vec3_t smartbone{};
		int ret{};
		
		vec3_t localplayer_head{};

		if (!GameFunctions::get_bone_by_id(sdk::GetLocalIndex(), sdk::BONE_POS_HEAD, &localplayer_head))
			return 0;

		// Set the priority
		if (GameFunctions::get_bone_by_id(index, sdk::BONE_POS_CHEST, &smartbone))
		{
			if (FD_Trace(sdk::get_camera_location(), smartbone, index))
			return sdk::BONE_POS_CHEST;

		}

		// if the selected bone is not visible loop trouh each bones and find the next best visible bone
		for (size_t bone_id = 0; bone_id <= 25; bone_id++)
		{
			if (GameFunctions::get_bone_by_id(index, bone_id, &smartbone))
			{
				if (FD_Trace(localplayer_head, smartbone, index))
				{
					ret = bone_id;
				}

			}

		}

		return ret; 

	}
		


	void aimbot_thread()
	{

		auto ingame = sdk::in_game();


		if (!ingame)
		{
			boneArray.clear();
			sdk::clear_vel_map();
			once = false;
			return;
		}

		auto ref_def = sdk::get_refdef();
		if (is_bad_ptr(ref_def))
			return;
		sdk::player_t local_player = sdk::get_local_player();

		if (is_bad_ptr(local_player.address))
			return;
	
		if (Settings::aimbot::visible_only)
		{
			g_data::visible_base = sdk::get_visible_base();
		}
		
		aim_assist::reset_lock();
		updateInput();
		sdk::start_tick();

		for (int i = 0; i < MAX_ENTITIES; ++i)
		{

		
			if (sdk::GetLocalIndex()== i)
				continue;

			float fdistance = 0;
			float display_string = 0;
			entityState_t* entitystate = GameFunctions::GetEntityState(i);
			if (is_bad_ptr(entitystate))continue;

			if (entitystate->eType == entityType_s::ET_PLAYER)
			{
				sdk::player_t player = sdk::get_player(i);
				if (is_bad_ptr(player.address))
					continue;
				if (Settings::esp::b_visible_only && !player.is_visible(i))
					continue;
				//bool is_friendly = player.team_id() == local_player.team_id();;

				if (Settings::aimbot::visible_only && !player.is_visible(i))
					continue;
			

				/*if (is_friendly)
					continue;*/

				fdistance = sdk::units_to_m(local_player.get_pos().distance_to(player.get_pos()));

				if (fdistance > Settings::esp::max_distance)
					continue;


				

				if (!Settings::aimbot::b_lock)continue;

					
						vec3_t current_bone;
						vec2_t bone_screen_pos;

						sdk::update_vel_map(sdk::local_index(), local_player.get_pos());

						
							if (!GameFunctions::get_bone_by_id(i, aim_assist::get_bone_opt(), &current_bone))
								continue;

							sdk::update_vel_map(i, current_bone);

							if (Settings::aimbot::b_prediction && fdistance > 4.f)
								current_bone = sdk::get_prediction(i, local_player.get_pos(), current_bone, player.get_pos());

							if (!sdk::WorldToScreen(current_bone, &bone_screen_pos))
								continue;

							if (player.get_stance() == sdk::KNOCKED && Settings::aimbot::b_skip_knocked)
								continue;


							switch (Settings::aimbot::i_priority)
							{
							case 0:
								aim_assist::get_closest_enemy(bone_screen_pos, fdistance);
								break;
							case 1:
								aim_assist::get_enemy_in_fov(bone_screen_pos);
								break;
							case 2:
								if (fdistance < Settings::aimbot::f_min_closest)
								{
									if (player.get_stance() == sdk::KNOCKED)
										continue;
									aim_assist::get_closest_enemy(bone_screen_pos, fdistance);
								}
								else {
									aim_assist::get_enemy_in_fov(bone_screen_pos);
								}
								break;
							}
						
						/*else
						{
							if (!once) {
								sdk::clear_vel_map();
								once = true;
							}

							sdk::update_vel_map(i, player.get_pos());

							current_bone = player.get_pos();

							if (Settings::aimbot::b_prediction && fdistance > 4.f)
								current_bone = sdk::get_prediction(i, local_player.get_pos(), current_bone);

							if (!sdk::head_to_screen(current_bone, &bone_screen_pos, player.get_stance()))
								continue;



							if (player.get_stance() == sdk::KNOCKED && Settings::aimbot::b_skip_knocked)
								continue;

							if (Settings::esp::b_visible && !player.is_visible())
								continue;

							if (is_friendly)
								continue;

							switch (Settings::aimbot::i_priority)
							{
							case 0:
								aim_assist::get_closest_enemy(bone_screen_pos, fdistance);
								break;
							case 1:
								aim_assist::get_enemy_in_fov(bone_screen_pos);
								break;
							case 2:
								if (fdistance < Settings::aimbot::f_min_closest)
								{
									if (player.get_stance() == sdk::KNOCKED)
										continue;
									aim_assist::get_closest_enemy(bone_screen_pos, fdistance);
								}
								else {
									aim_assist::get_enemy_in_fov(bone_screen_pos);
								}
								break;
							}

						}*/
					
				




			}


		}
		if (!g_menu::b_menu_open)
			aim_assist::is_aiming();

	}



		
	


	void esp_loop(ImFont* font)
	{
		
		//g_draw::draw_sketch_edge_text(font, "Ricocheat - MW19/WARZONE Edition", ImVec2(25, 25), 24, ImGui::ColorConvertFloat4ToU32(ImVec4(1, 1, 1, 1)), false);
	

		//item_esp(font);
		
		
	}
		
}







		
	
