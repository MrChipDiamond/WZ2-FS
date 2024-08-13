#include "settings.h"
#include "xor.hpp"
#include "Menu.h"

namespace Settings {
	bool thirdperson = false;
	namespace menu {
		bool b_auto_show = false;
	}
	namespace esp {
		bool b_rainbow = false;
		 bool b_uav = true;
		 bool enable_auav = true;
		bool b_visible = false;
		bool b_visible_only =false;
		bool b_box = false;
		bool b_line = false;
		bool b_skeleton = false;
		bool b_names = false;
		bool b_weaponname = false;

		bool b_distance = false;
		bool b_fov = false;
		bool b_crosshair = false;
		bool b_friendly = false;
		bool b_radar = false;
		bool b_health = false;
		bool b_aim_point = false;
		bool b_warning = false;
		bool b_vehicle = false;
		int fov_size = 75; // 5 ~ 800
		int max_distance = 350; //5 ~ 1000
		int radar_zoom = 50;
		int box_index = 0;
		int line_choose = 0;
		float thickness = 1.f;
		bool watermark =true;
		float danger = 50.0f;
		float radar_getpos_x;
		float radar_getpos_y;
		std::vector<const char*> box_types = { ("2D Corner Box"),  ("2D Box"),("2D Black Filled"),("2D Filled")};
		std::vector<const char*> line_type = { "Middle", "Bottom" };
	}
	namespace itemesp
	{
		bool b_enable_LootESP = false;
		bool b_name = false;
		bool b_distance = false;
		int max_distance = 25; //5 ~ 1000

		//// items start here

		/// weapon 

		bool b_weapon_ar = true;
		bool b_weapon_pi = true;
		bool b_weapon_sm = true;
		bool b_weapon_sn = true;
		bool b_weapon_lm = true;


		// ammo

		bool b_ammo_ar = true;
		bool b_ammo_sm = true;
		bool b_ammo_sn = true;
		bool b_ammo_sg = true;


		//equipment
		bool b_flash = false;
		bool b_decoy = false;
		bool b_stim = false;
		bool b_grenade = false;
		bool b_semtex = false;
		bool b_snapshot = false;
		bool b_Concussion = false;
		bool b_C4 = false;
		bool b_Knife = false;
		bool b_Mine = false;
		bool b_Claymore = false;
		bool b_thermite = false;

		//missions

		bool b_Bounty = false;
		bool b_Scav = false;
		bool b_Most_Wanted = false;


		//others
		bool b_airstrike = false;
		bool b_Clusterstrike = false;
		bool b_UAV = false;
		bool b_CASH = false;
		bool b_Amor = false;
		bool b_Satchel = false;
		bool b_Gasmask = false;
		bool b_Armor_Box = false;
		bool b_Ammo_Box = false;
		bool b_Crate_normal = false;
		bool b_Crate_legendary = false;

		//color
		ImColor loot_color{ 1.f,1.f,1.f,1.f };
	}
	namespace aimbot
	{
		float recoil_slider = 0.f;
		bool visible_only = true;
		bool b_smooth = false;
		bool b_lock = false;
		bool b_recoil = false;
		bool b_spread = false;
		bool b_triggerbot = false;
		bool b_skip_knocked = false;
		bool b_prediction = true;
		bool b_target_bone = false;
		float f_bullet_speed = 2000; // 1 ~ 5000
		float f_min_closest = 7.f; // 5 ~ 50
		float f_speed = 2.5f; // 1 ~ 30
		float f_smooth = 2.5f; //1 ~ 30
		int i_bone = 1; //0 ~ 3
		int i_priority = 2; // 0 ~ 2
		
		 std::vector<const char*> Target_priority = { "Closest","FOV", "Closest and FOV"};
		std::vector<const char*> AimbotBones = { "Spine","Head", "Neck","Chest"  };
	}
	namespace misc {

		bool b_flash =false;
		float scale_recoil = 0;
		bool b_fov_scale = false;
		float f_fov_scale = 1.2f; // 1.2 ~ 2.0
		bool b_snow = true;
		int font_index = 7;
	}

	namespace Outline
	{

		bool b_Full_RGB = false;
		const char* outline_types[] = { ("Outline"),("Pixel"),"Heat","Shape","Chams"};
		bool b_outline_player = false;
		bool outline_myself = true;
		bool b_outline_item = false;
		bool b_outline_vehicle = false;
		bool b_enable_outline = false;
		bool drawOccludedPixels = true;
		bool drawNonOccludedPixels = true;
		 bool fill = true;
		 int RenderMode =	0;
		float linewidth = 1;
		float distance_item = 100.f;

	}

	namespace gamepad
	{
		 bool b_gamepad = false;
		 bool b_reload;
	}

	namespace colors
	{
		ImColor downed { 255,127,0 ,255};
		ImColor White = { 1.f,1.f,1.f,1.f };
		ImColor Black = { 0.f,0.f,0.f,1.f };
		ImColor Red = { 1.f,0.f,0.f,1.f };
		ImColor Green = { 0.f,1.f,0.f,1.f };
		ImColor Blue = { 0.f,0.f,1.f,1.f };
		ImColor outline_visible_player = Red;
		ImColor outline_not_visible_player = Red;
		ImColor outline_visible_team = Green;
		ImColor outline_not_visible_team = Green;

		ImColor outline_Item = Blue;
		ImColor outline_Vehicle = Green;
		ImColor visible_team = { 0.f, 0.f, 1.f, 1.f };
		ImColor not_visible_team = { 0.f, 0.75f, 1.f, 1.f };
		ImColor visible_enemy = Green;
		ImColor not_visible_enemy = { 1.f,0.f,0.f,1.f };
		ImColor radar_bg = { 1.f,1.f,1.f,0.f };
		ImColor radar_boarder = { 1.f,1.f,1.f,0.f };
	}

	BOOL WritePrivateProfileInt(LPCSTR lpAppName, LPCSTR lpKeyName, int nInteger, LPCSTR lpFileName) {
		char lpString[1024];
		sprintf(lpString, "%d", nInteger);
		return WritePrivateProfileStringA(lpAppName, lpKeyName, lpString, lpFileName);
	}
	BOOL WritePrivateProfileFloat(LPCSTR lpAppName, LPCSTR lpKeyName, float nInteger, LPCSTR lpFileName) {
		char lpString[1024];
		sprintf(lpString, "%f", nInteger);
		return WritePrivateProfileStringA(lpAppName, lpKeyName, lpString, lpFileName);
	}
	float GetPrivateProfileFloat(LPCSTR lpAppName, LPCSTR lpKeyName, FLOAT flDefault, LPCSTR lpFileName)
	{
		char szData[32];

		GetPrivateProfileStringA(lpAppName, lpKeyName, std::to_string(flDefault).c_str(), szData, 32, lpFileName);

		return (float)atof(szData);
	}

	void Save_Settings(std::string fileName) {
		char file_path[MAX_PATH];
		sprintf_s(file_path, xorstr("C:\\EliminationCheatsLite\\configs\\\\ % s % s"), fileName.c_str(), xorstr(".ini"));

		WritePrivateProfileInt(xorstr("esp"), "thickness", esp::thickness, file_path);
		WritePrivateProfileInt(xorstr("esp"), "line_choose", esp::line_choose, file_path);
		WritePrivateProfileInt(xorstr("esp"), "box_index", esp::box_index, file_path);
		WritePrivateProfileInt(xorstr("esp"), "b_uav", esp::b_uav, file_path);
		WritePrivateProfileInt(xorstr("esp"), "enable_auav", esp::enable_auav, file_path);
		WritePrivateProfileInt(xorstr("esp"), "b_box", esp::b_box, file_path);
		WritePrivateProfileInt(xorstr("esp"), "visible only", esp::b_visible_only, file_path);
		WritePrivateProfileInt(xorstr("esp"), "b_crosshair", esp::b_crosshair, file_path);
		WritePrivateProfileInt(xorstr("esp"), "b_distance", esp::b_distance, file_path);
		WritePrivateProfileInt(xorstr("esp"), "b_fov", esp::b_fov, file_path);
		WritePrivateProfileInt(xorstr("esp"), "b_friendly", esp::b_friendly, file_path);
		WritePrivateProfileInt(xorstr("esp"), "b_health", esp::b_health, file_path);
		WritePrivateProfileInt(xorstr("esp"), "b_line", esp::b_line, file_path);
		WritePrivateProfileInt(xorstr("esp"), "b_names", esp::b_names, file_path);
		WritePrivateProfileInt(xorstr("esp"), "b_radar", esp::b_radar, file_path);
		WritePrivateProfileInt(xorstr("esp"), "b_skeleton", esp::b_skeleton, file_path);
		WritePrivateProfileInt(xorstr("esp"), "b_visible", esp::b_visible, file_path);
		WritePrivateProfileInt(xorstr("esp"), "i_fov_size", esp::fov_size, file_path);
		WritePrivateProfileInt(xorstr("esp"), "i_max_distance", esp::max_distance, file_path);
		WritePrivateProfileInt(xorstr("esp"), "i_radar_zoom", esp::radar_zoom, file_path);
		WritePrivateProfileFloat(xorstr("esp"), "radar_getpos_x", esp::radar_getpos_x, file_path);
		WritePrivateProfileFloat(xorstr("esp"), "radar_getpos_y", esp::radar_getpos_y, file_path);
		WritePrivateProfileInt(xorstr("aimbot"), "visible_only", aimbot::visible_only, file_path);
		WritePrivateProfileInt(xorstr("aimbot"), "b_smooth", aimbot::b_smooth, file_path);
		WritePrivateProfileInt(xorstr("aimbot"), "b_lock", aimbot::b_lock, file_path);
		WritePrivateProfileInt(xorstr("aimbot"), "b_prediction", aimbot::b_prediction, file_path);
		WritePrivateProfileInt(xorstr("aimbot"), "b_recoil", aimbot::b_recoil, file_path);
		WritePrivateProfileInt(xorstr("aimbot"), "b_spread", aimbot::b_spread, file_path);
		WritePrivateProfileInt(xorstr("aimbot"), "b_triggerbot", aimbot::b_triggerbot, file_path);
		WritePrivateProfileInt(xorstr("aimbot"), "b_skip_knocked", aimbot::b_skip_knocked, file_path);
		WritePrivateProfileInt(xorstr("aimbot"), "b_target_bone", aimbot::b_target_bone, file_path);
		WritePrivateProfileFloat(xorstr("aimbot"), "f_bullet_speed", aimbot::f_bullet_speed, file_path);
		WritePrivateProfileFloat(xorstr("aimbot"), "f_min_closest", aimbot::f_min_closest, file_path);
		WritePrivateProfileFloat(xorstr("aimbot"), "f_smooth", aimbot::f_smooth, file_path);
		WritePrivateProfileFloat(xorstr("aimbot"), "f_speed", aimbot::f_speed, file_path);
		WritePrivateProfileInt(xorstr("aimbot"), "i_bone", aimbot::i_bone, file_path);
		WritePrivateProfileInt(xorstr("aimbot"), "i_priority", aimbot::i_priority, file_path);
		WritePrivateProfileInt(xorstr("misc"),"b_fov_scale", misc::b_fov_scale, file_path);
		WritePrivateProfileInt(xorstr("misc") ,"font_index", misc::font_index, file_path);
		WritePrivateProfileFloat(xorstr("misc"), "f_fov_scale", misc::f_fov_scale, file_path);
		WritePrivateProfileInt(xorstr("misc"), "snow ", misc::b_snow, file_path);
		WritePrivateProfileInt(xorstr("colors"), "not_visible_enemy", colors::not_visible_enemy, file_path);
		WritePrivateProfileInt(xorstr("colors"), "not_visible_team", colors::not_visible_team, file_path);
		WritePrivateProfileInt(xorstr("colors"), "visible_enemy", colors::visible_enemy, file_path);
		WritePrivateProfileInt(xorstr("colors"), "visible_team", colors::visible_team, file_path);
		WritePrivateProfileInt(xorstr("colors"), "outline_visible_player", colors::outline_visible_player, file_path);
		WritePrivateProfileInt(xorstr("colors"), "outline_not_visible_player", colors::outline_not_visible_player, file_path);
		WritePrivateProfileInt(xorstr("colors"), "outline_visible_team", colors::outline_visible_team, file_path);
		WritePrivateProfileInt(xorstr("colors"), "outline_not_visible_team", colors::outline_not_visible_team, file_path);






		//item esp
		//weapon
		WritePrivateProfileInt(xorstr("item"), "enable_loot", itemesp::b_enable_LootESP, file_path);
		WritePrivateProfileInt(xorstr("item"), "max_distance", itemesp::max_distance, file_path);
		WritePrivateProfileInt(xorstr("item"), "name", itemesp::b_name, file_path);
		WritePrivateProfileInt(xorstr("item"), "distance", itemesp::b_distance, file_path);
		WritePrivateProfileInt(xorstr("item"), "Assault_Rifle", itemesp::b_weapon_ar, file_path);
		WritePrivateProfileInt(xorstr("item"), "lmg", itemesp::b_weapon_lm, file_path);
		WritePrivateProfileInt(xorstr("item"), "smg", itemesp::b_weapon_sm, file_path);
		WritePrivateProfileInt(xorstr("item"), "pistol", itemesp::b_weapon_pi, file_path);
		WritePrivateProfileInt(xorstr("item"), "sniper", itemesp::b_weapon_sn, file_path);
		WritePrivateProfileInt(xorstr("item"), "ammo_ar", itemesp::b_ammo_ar, file_path);
		WritePrivateProfileInt(xorstr("item"), "ammo_smg", itemesp::b_ammo_sm, file_path);
		WritePrivateProfileInt(xorstr("item"), "ammo_shotgun", itemesp::b_ammo_sg, file_path);
		WritePrivateProfileInt(xorstr("item"), "ammo_sniper", itemesp::b_ammo_sn, file_path);
		WritePrivateProfileInt(xorstr("item"), "flash", itemesp::b_flash, file_path);
		WritePrivateProfileInt(xorstr("item"), "decoy", itemesp::b_decoy, file_path);
		WritePrivateProfileInt(xorstr("item"), "stim", itemesp::b_stim, file_path);
		WritePrivateProfileInt(xorstr("item"), "grenade", itemesp::b_grenade, file_path);
		WritePrivateProfileInt(xorstr("item"), "semtex", itemesp::b_semtex, file_path);
		WritePrivateProfileInt(xorstr("item"), "snapshot", itemesp::b_snapshot, file_path);
		WritePrivateProfileInt(xorstr("item"), "conccusion", itemesp::b_Concussion, file_path);
		WritePrivateProfileInt(xorstr("item"), "c4", itemesp::b_C4, file_path);
		WritePrivateProfileInt(xorstr("item"), "knife", itemesp::b_Knife, file_path);
		WritePrivateProfileInt(xorstr("item"), "mine", itemesp::b_Mine, file_path);
		WritePrivateProfileInt(xorstr("item"), "claymore", itemesp::b_Claymore, file_path);
		WritePrivateProfileInt(xorstr("item"), "airstrike", itemesp::b_airstrike, file_path);
		WritePrivateProfileInt(xorstr("item"), "cluster", itemesp::b_Clusterstrike, file_path);
		WritePrivateProfileInt(xorstr("item"), "uav", itemesp::b_UAV, file_path);
		WritePrivateProfileInt(xorstr("item"), "cash", itemesp::b_CASH, file_path);
		WritePrivateProfileInt(xorstr("item"), "amor", itemesp::b_Amor, file_path);
		WritePrivateProfileInt(xorstr("item"), "satchel", itemesp::b_Satchel, file_path);
		WritePrivateProfileInt(xorstr("item"), "gasmask", itemesp::b_Gasmask, file_path);
		WritePrivateProfileInt(xorstr("item"), "amorbox", itemesp::b_Amor, file_path);
		WritePrivateProfileInt(xorstr("item"), "ammobox", itemesp::b_Ammo_Box, file_path);
		WritePrivateProfileInt(xorstr("item"), "crate_normal", itemesp::b_Crate_normal, file_path);
		WritePrivateProfileInt(xorstr("item"), "crate_legendary", itemesp::b_Crate_legendary, file_path);


	}

	void Load_Settings(std::string fileName)
	{
		char file_path[MAX_PATH];
		sprintf_s(file_path, xorstr("C:\\EliminationCheatsLite\\configs\\%s%s"), fileName.c_str(), xorstr(".ini"));
		esp::thickness = GetPrivateProfileIntA("esp", "thickness", esp::thickness, file_path);
		misc::font_index = GetPrivateProfileIntA("misc", "font_index", misc::font_index, file_path);
		esp::line_choose = GetPrivateProfileIntA("esp", "line_choose", esp::line_choose, file_path);
		esp::box_index = GetPrivateProfileIntA("esp", "box_index", esp::box_index, file_path);
		esp::b_uav = GetPrivateProfileIntA("esp", "b_uav", esp::b_uav, file_path);
		esp::enable_auav = GetPrivateProfileIntA("esp", "enable_auav", esp::enable_auav, file_path);
		esp::b_visible_only = GetPrivateProfileIntA("esp", "visible only", esp::b_visible_only, file_path);
		esp::b_box = GetPrivateProfileIntA("esp", "b_box", esp::b_box, file_path);
		esp::b_crosshair = GetPrivateProfileIntA("esp", "b_crosshair", esp::b_crosshair, file_path);
		esp::b_distance = GetPrivateProfileIntA("esp", "b_distance", esp::b_distance, file_path);
		esp::b_fov = GetPrivateProfileIntA("esp", "b_fov", esp::b_fov, file_path);
		esp::b_friendly = GetPrivateProfileIntA("esp", "b_friendly", esp::b_friendly, file_path);
		esp::b_health = GetPrivateProfileIntA("esp", "b_health", esp::b_health, file_path);
		esp::b_line = GetPrivateProfileIntA("esp", "b_line", esp::b_line, file_path);
		esp::b_names = GetPrivateProfileIntA("esp", "b_names", esp::b_names, file_path);
		esp::b_radar = GetPrivateProfileIntA("esp", "b_radar", esp::b_radar, file_path);
		esp::b_skeleton = GetPrivateProfileIntA("esp", "b_skeleton", esp::b_skeleton, file_path);
		esp::b_visible = GetPrivateProfileIntA("esp", "b_visible", esp::b_visible, file_path);
		esp::fov_size = GetPrivateProfileIntA("esp", "i_fov_size", esp::fov_size, file_path);
		esp::max_distance = GetPrivateProfileIntA("esp", "i_max_distance", esp::max_distance, file_path);
		esp::radar_zoom = GetPrivateProfileIntA("esp", "i_radar_zoom", esp::radar_zoom, file_path);

		esp::radar_getpos_x = GetPrivateProfileIntA("esp", "radar_getpos_x", esp::radar_getpos_x, file_path);
		esp::radar_getpos_y = GetPrivateProfileIntA("esp", "radar_getpos_y", esp::radar_getpos_y, file_path);

		aimbot::visible_only = GetPrivateProfileIntA("aimbot", "visible_only", aimbot::visible_only, file_path);
		aimbot::b_smooth = GetPrivateProfileIntA("aimbot", "b_smooth", aimbot::b_smooth, file_path);
		aimbot::b_lock = GetPrivateProfileIntA("aimbot", "b_lock", aimbot::b_lock, file_path);
		aimbot::b_prediction = GetPrivateProfileIntA("aimbot", "b_prediction", aimbot::b_prediction, file_path);
		aimbot::b_recoil = GetPrivateProfileIntA("aimbot", "b_recoil", aimbot::b_recoil, file_path);
		aimbot::b_spread = GetPrivateProfileIntA("aimbot", "b_spread", aimbot::b_spread, file_path);
		aimbot::b_triggerbot = GetPrivateProfileIntA("aimbot", "b_triggerbot", aimbot::b_triggerbot, file_path);
		aimbot::b_skip_knocked = GetPrivateProfileIntA("aimbot", "b_skip_knocked", aimbot::b_skip_knocked, file_path);
		aimbot::b_target_bone = GetPrivateProfileIntA("aimbot", "b_target_bone", aimbot::b_target_bone, file_path);
		aimbot::f_bullet_speed = GetPrivateProfileFloat("aimbot", "f_bullet_speed", aimbot::f_bullet_speed, file_path);
		aimbot::f_min_closest = GetPrivateProfileFloat("aimbot", "f_min_closest", aimbot::f_min_closest, file_path);
		aimbot::f_smooth = GetPrivateProfileFloat("aimbot", "f_smooth", aimbot::f_smooth, file_path);
		aimbot::f_speed = GetPrivateProfileFloat("aimbot", "f_speed", aimbot::f_speed, file_path);
		aimbot::i_bone = GetPrivateProfileIntA("aimbot", "i_bone", aimbot::i_bone, file_path);
		aimbot::i_priority = GetPrivateProfileIntA("aimbot", "i_priority", aimbot::i_priority, file_path);
		misc::b_fov_scale = GetPrivateProfileIntA("misc", "b_fov_scale", misc::b_fov_scale, file_path);
		misc::f_fov_scale = GetPrivateProfileFloat("misc", "f_fov_scale", misc::f_fov_scale, file_path);
		misc::b_snow = GetPrivateProfileIntA("misc", "b_fov_scale", misc::b_snow, file_path);
		colors::not_visible_enemy = GetPrivateProfileIntA("colors", "not_visible_enemy", colors::not_visible_enemy, file_path);
		colors::not_visible_team = GetPrivateProfileIntA("colors", "not_visible_team", colors::not_visible_team, file_path);
		colors::visible_enemy = GetPrivateProfileIntA("colors", "visible_enemy", colors::visible_enemy, file_path);
		colors::visible_team = GetPrivateProfileIntA("colors", "visible_team", colors::visible_team, file_path);
		colors::outline_visible_player = GetPrivateProfileIntA("colors", "outline_visible_player", colors::outline_visible_player, file_path);
		colors::not_visible_enemy = GetPrivateProfileIntA("colors", "outline_not_visible_player", colors::outline_not_visible_player, file_path);
		colors::outline_visible_team = GetPrivateProfileIntA("colors", "outline_visible_team", colors::outline_visible_team, file_path);
		colors::outline_not_visible_team = GetPrivateProfileIntA("colors", "outline_not_visible_team", colors::outline_not_visible_team, file_path);



		//items
		itemesp::b_enable_LootESP = GetPrivateProfileIntA("item", "enable_loot", itemesp::b_enable_LootESP, file_path);
		itemesp::max_distance = GetPrivateProfileIntA("item", "max_distance", itemesp::max_distance, file_path);
		itemesp::b_name = GetPrivateProfileIntA("item", "name", itemesp::b_name, file_path);
		itemesp::b_distance = GetPrivateProfileIntA("item", "distance", itemesp::b_distance, file_path);
		itemesp::b_weapon_ar = GetPrivateProfileIntA("item", "Assault_Rifle", itemesp::b_weapon_ar, file_path);
		itemesp::b_weapon_lm = GetPrivateProfileIntA("item", "lmg", itemesp::b_weapon_lm, file_path);
		itemesp::b_weapon_sm = GetPrivateProfileIntA("item", "smg", itemesp::b_weapon_sm, file_path);
		itemesp::b_weapon_pi = GetPrivateProfileIntA("item", "pistol", itemesp::b_weapon_pi, file_path);
		itemesp::b_weapon_sn = GetPrivateProfileIntA("item", "sniper", itemesp::b_weapon_sn, file_path);
		itemesp::b_distance = GetPrivateProfileIntA("item", "ammo_ar", itemesp::b_ammo_ar, file_path);
		itemesp::b_ammo_sm = GetPrivateProfileIntA("item", "ammo_smg", itemesp::b_ammo_sm, file_path);
		itemesp::b_ammo_sg = GetPrivateProfileIntA("item", "ammo_shotgun", itemesp::b_ammo_sg, file_path);
		itemesp::b_ammo_sn = GetPrivateProfileIntA("item", "ammo_sniper", itemesp::b_ammo_sn, file_path);
		itemesp::b_flash = GetPrivateProfileIntA("item", "flash", itemesp::b_flash, file_path);
		itemesp::b_decoy = GetPrivateProfileIntA("item", "decoy", itemesp::b_decoy, file_path);
		itemesp::b_stim = GetPrivateProfileIntA("item", "stim", itemesp::b_stim, file_path);
		itemesp::b_grenade = GetPrivateProfileIntA("item", "grenade", itemesp::b_grenade, file_path);
		itemesp::b_semtex = GetPrivateProfileIntA("item", "semtex", itemesp::b_semtex, file_path);
		itemesp::b_snapshot = GetPrivateProfileIntA("item", "snapshot", itemesp::b_snapshot, file_path);
		itemesp::b_Concussion = GetPrivateProfileIntA("item", "conccusion", itemesp::b_Concussion, file_path);
		itemesp::b_C4 = GetPrivateProfileIntA("item", "c4", itemesp::b_C4, file_path);
		itemesp::b_Knife = GetPrivateProfileIntA("item", "knife", itemesp::b_Knife, file_path);
		itemesp::b_Mine = GetPrivateProfileIntA("item", "mine", itemesp::b_Mine, file_path);
		itemesp::b_Claymore = GetPrivateProfileIntA("item", "claymore", itemesp::b_Claymore, file_path);
		itemesp::b_airstrike = GetPrivateProfileIntA("item", "airstrike", itemesp::b_airstrike, file_path);
		itemesp::b_Clusterstrike = GetPrivateProfileIntA("item", "cluster", itemesp::b_Clusterstrike, file_path);
		itemesp::b_UAV = GetPrivateProfileIntA("item", "uav", itemesp::b_UAV, file_path);
		itemesp::b_CASH = GetPrivateProfileIntA("item", "cash", itemesp::b_CASH, file_path);
		itemesp::b_Amor = GetPrivateProfileIntA("item", "amor", itemesp::b_Amor, file_path);
		itemesp::b_Satchel = GetPrivateProfileIntA("item", "satchel", itemesp::b_Satchel, file_path);
		itemesp::b_Gasmask = GetPrivateProfileIntA("item", "gasmask", itemesp::b_Gasmask, file_path);
		itemesp::b_Amor = GetPrivateProfileIntA("item", "amorbox", itemesp::b_Amor, file_path);
		itemesp::b_Ammo_Box = GetPrivateProfileIntA("item", "ammobox", itemesp::b_Ammo_Box, file_path);
		itemesp::b_Crate_normal = GetPrivateProfileIntA("item", "crate_normal", itemesp::b_Crate_normal, file_path);
		itemesp::b_Crate_legendary = GetPrivateProfileIntA("item", "crate_legendary", itemesp::b_Crate_legendary, file_path);
		g_menu::str_config_name = fileName;
	};

	std::vector<std::string> GetList() {
		std::vector<std::string> configs;
		WIN32_FIND_DATA ffd;
		LPCSTR directory = xorstr("C:\\EliminationCheatsLite\\configs\\*");
		auto hFind = FindFirstFile(directory, &ffd);
		while (FindNextFile(hFind, &ffd))
		{
			if (!(ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
			{
				std::string file_name = ffd.cFileName;
				if (file_name.size() < 4) // .cfg
					continue;
				std::string end = file_name;
				end.erase(end.begin(), end.end() - 4);
				if (end != xorstr(".ini"))
					continue;
				file_name.erase(file_name.end() - 4, file_name.end());
				configs.push_back(file_name);
			}
		}
		return configs;
	}

	void Auto_Load() {
		if (!GetList().empty()) {
			Load_Settings(GetList().at(0));
		}
		else {
			CreateDirectoryA(xorstr("C:\\EliminationCheatsLite"), NULL);
			CreateDirectoryA(xorstr("C:\\EliminationCheatsLite\\configs"), NULL);
		}
	}

}

namespace weapon_info
{
	 int aimbottick = 0;
    const char* name;
    const char* alt_name;
    uintptr_t velocity;
    uintptr_t weapon_def;
    float bullet_speed;
}

namespace d3d12test
{
    ID3D12CommandQueue* d3d12CommandQueue2 = nullptr;
     ID3D12CommandQueue* d3d12CommandQueueoriginal = nullptr;
}

namespace MenuColors
{
	bool use_custom_theme = false;
	bool AntiAliasing = false;
	bool AntiAliasingLines = false;

	ImVec4 Color_Text = ImVec4(0.80f, 0.80f, 0.83f, 1.00f);
	ImVec4 Text_Disabled = ImVec4(0.24f, 0.23f, 0.29f, 1.00f);
	ImVec4 WindowBackround = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
	ImVec4 PopupBackround = ImVec4(0.07f, 0.07f, 0.09f, 1.00f);
	ImVec4 Border_Color = ImVec4(0.80f, 0.80f, 0.83f, 0.88f);
	ImVec4 Border_shadow = ImVec4(0.92f, 0.91f, 0.88f, 0.00f);
	ImVec4 Frame_Backround = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
	ImVec4 Frame_Backround_Hovered = ImVec4(0.24f, 0.23f, 0.29f, 1.00f);
	ImVec4 Frame_Backround_Active = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
	ImVec4 Titlebar_Backround = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
	ImVec4 TitleBgCollapsed = ImVec4(1.00f, 0.98f, 0.95f, 0.75f);
	ImVec4 TitleBgActive = ImVec4(0.07f, 0.07f, 0.09f, 1.00f);
	ImVec4 Menubar_Backround = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
	ImVec4 Scrollbar_Backround = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
	ImVec4 ScrollBar_Grab = ImVec4(0.80f, 0.80f, 0.83f, 0.31f);
	ImVec4 Scrollbar_Grab_Hovered = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
	ImVec4 Scrollbar_Grab_Active = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
	ImVec4 Check_Mark = ImVec4(0.80f, 0.80f, 0.83f, 0.31f);
	ImVec4 Slider_Grab = ImVec4(0.80f, 0.80f, 0.83f, 0.31f);
	ImVec4 Slider_grab_Active = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
	ImVec4 Button = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
	ImVec4 Button_Hovered = ImVec4(0.24f, 0.23f, 0.29f, 1.00f);
	ImVec4 Button_Active = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
	ImVec4 Header = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
	ImVec4 Header_Hovered = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
	ImVec4 Header_Active = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
	ImVec4 PlotLines = ImVec4(0.40f, 0.39f, 0.38f, 0.63f);
	ImVec4 PLotlines_Hovered = ImVec4(0.25f, 1.00f, 0.00f, 1.00f);
	ImVec4 Plotlines_Active = ImVec4(0.40f, 0.39f, 0.38f, 0.63f);
	ImVec4 PlotHistogram = ImVec4(0.25f, 1.00f, 0.00f, 1.00f);
	ImVec4 PlotHistogram_Active = ImVec4(0.25f, 1.00f, 0.00f, 0.43f);
	ImVec4 TextSelected_Backround = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
	ImVec4 ModalWindowDarkening = ImVec4(1.00f, 0.98f, 0.95f, 0.73f);




	float windowbordersize = 0.f;
	ImVec2 windowpadding(8, 8);
	float windowrounding = 10.0f;
	float childrounding;
	ImVec2 framepadding(5, 5);
	float framerounding = 6.0;
	float popuprounding = 3.0;
	ImVec2 itemspacing(8, 8);
	ImVec2 iteminnerspacing(8, 8);
	float indentspacing = 25.f;
	float scrollbarsize = 15.f;
	float scrollbarrounding = 9.0;
	float grabminsize = 5.0;
	float grabrounding = 6.0;
	ImVec2 windowtitleallignx(0.5f, 0.5f);



}