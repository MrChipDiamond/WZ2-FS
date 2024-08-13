#include "stdafx.h"
#include "Menu.h"
//#include imgui.h"
//#include "imgui/imgui_impl_win32.h"
//#include "imgui/imgui_impl_dx12.h"
#include "sdk.h"
#include "xor.hpp"
#include "settings.h"
#include "INPUT.h"
#include <imgui_internal.h>
#include"UnlockAllV2.h"




void hotkey(const char* label, KeyBind& key, float samelineOffset, const ImVec2& size)noexcept;


bool b_debug_open = false;


std::string fov_scale;

static auto vector_getter = [](void* vec, int idx, const char** out_text)
{
	auto& vector = *static_cast<std::vector<std::string>*>(vec);
	if (idx < 0 || idx >= static_cast<int>(vector.size())) { return false; }
	*out_text = vector.at(idx).c_str();
	return true;
};


enum MENUSETTING : unsigned int
{
	ESP,
	MISC,
	COLOR,
	SETTINGS
};

ImFont* main_font1 = nullptr;
ImFont* main_font2 = nullptr;
ImFont* main_font3 = nullptr;
ImFont* main_font4 = nullptr;
ImFont* main_font5 = nullptr;
ImFont* main_font6 = nullptr;
ImFont* main_font7 = nullptr;
ImFont* main_font8 = nullptr;


ImFont* GetFont(int index)
{
	switch (index)
	{
	case 0: return main_font1;
	case 1:return main_font2;
	case 2:return main_font3;
	case 3:return main_font4;
	case 4:return main_font5;
	case 5:return main_font6;
	case 6:return main_font7;
	case 7:return main_font8;
	default:
		break;
	}
}

extern void init_Font_List()
{

	ImGuiIO& io = ImGui::GetIO();

	main_font1 = io.Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\Arial.ttf", 18.0f);

	main_font2 = io.Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\Calibri.ttf", 18.0f);

	main_font3 = io.Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\Cambria.ttf", 18.0f);

	main_font4 = io.Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\Candara.ttf", 18.0f);

	main_font5 = io.Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\Corbel.ttf", 18.0f);

	main_font6 = io.Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\Segoe UI.ttf", 18.0f);

	main_font7 = io.Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\Tahoma.ttf", 18.0f);

	main_font8 = io.Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\Verdana.ttf", 18.0f);



}

void DrawRectRainbow()
{
	float rainbowSpeed = 0.001;
	static float staticHue = 0;
	ImDrawList* draw_list = ImGui::GetWindowDrawList();
	ImVec2 panelPos = ImGui::GetWindowPos();
	staticHue -= rainbowSpeed;
	if (staticHue < -1.f) staticHue += 1.f;
	for (int i = 0; i < 860; i++)
	{
		float hue = staticHue + (1.f / (float)860) * i;
		if (hue < 0.f) hue += 1.f;
		ImColor cRainbow = ImColor::HSV(hue, 1.f, 1.f);
		draw_list->AddRectFilled(ImVec2(panelPos.x + i, panelPos.y), ImVec2(panelPos.x + i + 1, panelPos.y), cRainbow);
	}
}
RECT rc = { 0 };

bool init_font = true;


namespace g_menu
{
	int aimbotKeyMode{ 0 };
	D3D12_GPU_DESCRIPTOR_HANDLE gpu_handle;
	D3D12_CPU_DESCRIPTOR_HANDLE cpu_handle;
	D3D12_GPU_DESCRIPTOR_HANDLE testbild;
	bool b_menu_open = true;
	bool b_render_meu = true;
	std::string str_config_name = "";


	const char* Fontname[]{ "Arial","Calibri","Cambria","Candara","Corbel","Segoe UI","Tahoma","Verdana" };


	char* weaponname_internal;
	char* weaponname;
	char* weaponname_alt;
	POINT mouse;

	bool create_snow = true;






	namespace Config
	{
		namespace Aimbot
		{
			static bool Enabled = false;


			static int AimbotBone = 0;

			static bool AimFov = false;
			static float AimFOVValue = 0;
			static float AimbotDistance = 0;
			static float AimbotSpeed = 0;
			static float AimbotSmooth = 0;
			static int AimKey = 0;

			static bool BulletPrediction = false;
			static bool VisibilityCheck = false;
		}
		//brauch ich 
		namespace Visuals
		{
			namespace Enemy
			{
				static bool Box;
				static bool Snaplines;
				static bool Distance;
				static bool Corner;
				static bool Healthbar;
				static bool Shieldbar;
				static bool KnockedPlayers;
				static bool Bloodhound;
				static bool Healthglow;
			}

			namespace Team
			{
				static bool Box;
				static bool Snaplines;
				static bool Distance;
				static bool Corner;
				static bool Healthbar;
				static bool Shieldbar;
				static bool KnockedPlayers;
				static bool Bloodhound;
				static bool Healthglow;
			}

			namespace Glow
			{
				static bool Items = false;
				static bool Health = false;
				static bool Bloodhound = false;
				static int Brightness = 1;
			}

			static float Boxes[3] = { 1.0f, 1.0f, 1.0f };
			static float Snaplines[3] = { 1.0f, 1.0f, 1.0f };
			static float Knocked[3] = { 1.0f, 1.0f, 1.0f };
			static float VisibleEnemy[3] = { 1.0f, 1.0f, 1.0f };
			static float TeamBox[3] = { 1.0f, 1.0f, 1.0f };
			static float TeamLine[3] = { 1.0f, 1.0f, 1.0f };
			static float TeamKnocked[3] = { 1.0f, 1.0f, 1.0f };
		}

		namespace Settings
		{
			static int GlowBrightness = 0;
			static float PlayerDistance = 0.0f;
			static float FOV = 0;
			static float AimDistance = 0.0f;
			static float AimSmooth = 0.0f;
			static float Box[3] = { 1.0f, 1.0f, 1.0f };
			static float Snaplines[3] = { 1.0f, 1.0f, 1.0f };
			static float Knocked[3] = { 1.0f, 1.0f, 1.0f };
			static float VisibleEnemy[3] = { 1.0f, 1.0f, 1.0f };
			static float TeamBox[3] = { 1.0f, 1.0f, 1.0f };
			static float TeamLine[3] = { 1.0f, 1.0f, 1.0f };
			static float TeamKnocked[3] = { 1.0f, 1.0f, 1.0f };
		}
	}







	void Render()
	{
		// Variables

		static auto width = ImGui::GetContentRegionAvailWidth();
		static auto height = ImGui::GetWindowHeight();
		static auto tab = 0;


		// Helper functions

		static auto Center = [](float padding) -> void
		{
			ImGui::SameLine((ImGui::GetContentRegionAvail().x / 2) - padding);
		};

		static auto Title = [](const char* format, float width = ImGui::GetContentRegionAvail().x) -> void
		{
			if (!format || width <= 0)
				return;

			ImGui::Spacing();
			ImGui::Spacing();
			ImGui::SameLine((ImGui::GetContentRegionAvail().x / 2) - (ImGui::CalcTextSize(format).x / 2));
			ImGui::Text(format);
			ImGui::Spacing();

			ImGui::PushID("##TitleSeparator_");
			ImGui::PushStyleColor(ImGuiCol_ChildBg, IM_COL32(0, 0, 0, 0));
			ImGui::BeginChild("_", ImVec2(width, 1));
			ImGui::Separator();
			ImGui::EndChild();
			ImGui::PopStyleColor();
			ImGui::PopID();

			ImGui::Spacing();
		};

		static auto GetBuffer = [](const char* format, ...) -> std::string
		{
			if (!format)
				return "";

			char buffer[512];

			va_list args;
			va_start(args, format);
			vsnprintf_s(buffer, sizeof(buffer), format, args);
			va_end(args);

			return std::string(buffer);
		};

		static auto SliderF = [](const char* format, float min, float max, float* value = 0, float width = ImGui::GetContentRegionAvail().x) -> void
		{
			if (!format || !value)
				return;

			ImGui::Spacing();
			ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 12);

			ImGui::PushItemWidth(width - 24);
			ImGui::PushID(format);
			ImGui::SliderFloat("##CustomSliderF_", value, min, max, GetBuffer("(%1.f %s)", *value, format).c_str());
			ImGui::PopID();
			ImGui::PopItemWidth();
		};

		static auto SliderI = [](const char* format, int min, int max, int* value = 0, float width = ImGui::GetContentRegionAvail().x) -> void
		{
			if (!format || !value)
				return;

			ImGui::Spacing();
			ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 12);

			ImGui::PushItemWidth(width - 24);
			ImGui::PushID(format);
			ImGui::SliderInt("##CustomSliderI_", value, min, max, GetBuffer("(%i %s)", *value, format).c_str());
			ImGui::PopID();
			ImGui::PopItemWidth();
		};

		static auto Combo = [](const char* format, std::vector<const char*> items, int* selected, float width = ImGui::GetContentRegionAvail().x) -> void
		{
			if (!format || !(items.size() > 1) || !selected)
				return;

			ImGui::Spacing();
			ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 12);

			ImGui::PushItemWidth(width - 24);
			ImGui::PushID(format);
			ImGui::Combo("##CustomCombo_", selected, items.data(), items.size());
			ImGui::PopID();
			ImGui::PopItemWidth();
		};

		static auto Interface = [](const char* format, ImVec2 size, bool border, bool padding, std::function<void()> fn) -> void
		{
			if (!format || !(size.x > 0 && size.y > 0))
				return;

			ImGui::PushID(format);

			if (padding)
				ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 12);

			ImGui::PushStyleColor(ImGuiCol_ChildBg, IM_COL32(28, 28, 28, 255));

			ImGui::BeginChild("##InterfaceChildTitle", !padding ? size : ImVec2(size.x - 24, size.y - 12), border);

			ImGui::PopStyleColor();

			ImGui::Spacing();
			ImGui::Spacing();
			ImGui::SameLine((ImGui::GetContentRegionAvail().x / 2) - (ImGui::CalcTextSize(format).x / 2));
			ImGui::Text(format);
			ImGui::Spacing();

			ImGui::Separator();

			ImGui::PushStyleColor(ImGuiCol_ChildBg, IM_COL32(24, 24, 24, 255));
			{
				ImGui::BeginChild("##InterfaceChild_", ImGui::GetContentRegionAvail(), false);
				fn();
				ImGui::EndChild();
			}
			ImGui::PopStyleColor();

			ImGui::EndChild();

			ImGui::PopID();
		};

		static auto Checkbox = [](const char* format, bool* value, const char* helpmarker = 0) -> void
		{
			ImGui::PushID("##CustomCheckbox_");

			ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 1);

			ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPosX() + 12, ImGui::GetCursorPosY() + 2));
			ImGui::Checkbox(format, value);

			ImGui::PopStyleVar();

			if (!(helpmarker == 0))
			{
				ImGui::SameLine();
				ImGui::TextColored(ImColor(255, 0, 0), "(?)");

				if (ImGui::IsItemHovered())
					ImGui::SetTooltip(helpmarker);
			}

			ImGui::Spacing();
			ImGui::PopID();
		};

		static auto ColorPicker = [](const char* str_id, float* value)
		{
			if (!str_id || !value)
				return;

			static const auto flags = ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoTooltip;

			ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 5);
			ImGui::ColorEdit3(str_id, value, flags);
			ImGui::Spacing();
		};

		// Draw

		ImGui::BeginChild("##Main", ImVec2(360, ImGui::GetContentRegionAvail().y), 0);
		{
			ImGui::Image((void*)g_menu::testbild.ptr, ImVec2(355, 135));

			ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 15);

			ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
			ImGui::PushStyleVar(ImGuiStyleVar_IndentSpacing, 0);
			ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 1);
			ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 0);

			ImGui::Spacing();

			ImGui::PushStyleColor(ImGuiCol_Border, IM_COL32(255, 0, 0, 255));

			/* Center(ImGui::GetContentRegionAvail().x / 2.85);

			 ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 24);
			 if (ImGui::Button("Aimbot", ImVec2(175, 33)))
				 tab = 0;

			 ImGui::Spacing();
			 ImGui::Spacing();*/

			Center(ImGui::GetContentRegionAvail().x / 2.85);

			ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 24);
			if (ImGui::Button("Visuals", ImVec2(175, 33)))
				tab = 0;

			ImGui::Spacing();
			ImGui::Spacing();

			/* Center(ImGui::GetContentRegionAvail().x / 2.85);

			 ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 24);
			 if (ImGui::Button("Item", ImVec2(175, 33)))
				 tab = 2;

			 ImGui::Spacing();
			 ImGui::Spacing();*/

			Center(ImGui::GetContentRegionAvail().x / 2.85);

			ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 24);
			if (ImGui::Button("Misc", ImVec2(175, 33)))
				tab = 1;

			ImGui::Spacing();
			ImGui::Spacing();

			Center(ImGui::GetContentRegionAvail().x / 2.85);

			ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 24);
			if (ImGui::Button("Settings", ImVec2(175, 33)))
				tab = 2;

			ImGui::PopStyleColor();

			ImGui::PopStyleVar(4);
		}

		ImGui::EndChild();

		ImGui::SameLine();

		ImGui::BeginChild("##RightSide", ImVec2(ImGui::GetContentRegionAvail().x, ImGui::GetContentRegionAvail().y), 1);
		{
			switch (tab)
			{

			case 0:	// Visuals
				Title("Visuals");
				Interface("ESP", ImVec2(ImGui::GetContentRegionAvail().x / 2, ImGui::GetContentRegionAvail().y), true, true, []() {

					Checkbox("Color Visibility", &Settings::esp::b_visible);
					Checkbox("Box", &Settings::esp::b_box);
					Checkbox("Health", &Settings::esp::b_health);
					Checkbox("Snapline", &Settings::esp::b_line);
					Checkbox("Bone", &Settings::esp::b_skeleton);
					Checkbox("Names", &Settings::esp::b_names);
					Checkbox("Distance", &Settings::esp::b_distance);
					Checkbox("Team", &Settings::esp::b_friendly);
					Checkbox("Radar", &Settings::esp::b_radar);


					});


				ImGui::SameLine();


				Interface("Option ", ImVec2(ImGui::GetContentRegionAvail().x, ImGui::GetContentRegionAvail().y), true, true, [&]() {
					Combo("Box Type", Settings::esp::box_types, &Settings::esp::box_index);
					Combo("Line Tye", Settings::esp::line_type, &Settings::esp::line_choose);
					SliderI("ESP Distance", 0, 500, &Settings::esp::max_distance);
					SliderF("ESP Thickness", 0, 5, &Settings::esp::thickness);
					SliderI("Radar Zoom", 0, 100, &Settings::esp::radar_zoom);
					ImGui::Text("Enemy Visible");
					ImGui::SameLine();
					ColorPicker("###Enemy Visible", (float*)&Settings::colors::visible_enemy);
					ImGui::Text("Enemy Not Visible");
					ImGui::SameLine();
					ColorPicker("###Enemy Not Visible", (float*)&Settings::colors::not_visible_enemy);
					ImGui::Text("Team Visible");
					ImGui::SameLine();
					ColorPicker("###Team Visible", (float*)&Settings::colors::visible_team);
					ImGui::Text("Team Not Visible");
					ImGui::SameLine();
					ColorPicker("####Team Not Visible", (float*)&Settings::colors::not_visible_team);
					});

				break;

			case 1:
				Title("Misc");

				Interface("Misc", ImVec2(ImGui::GetContentRegionAvail().x, ImGui::GetContentRegionAvail().y), true, true, []() {

					Checkbox("Crosshair", &Settings::esp::b_crosshair);
					Checkbox("No Recoil", &Settings::aimbot::b_recoil);
					Checkbox("Watermark", &Settings::esp::watermark);
					});



				break;

			case 2:	// Settings		  
				Title("Settings");

				Interface("Setting", ImVec2(ImGui::GetContentRegionAvail().x, ImGui::GetContentRegionAvail().y), true, true, [&]() {

					static std::string str_warn = xorstr("Config Must be at least 3 char length!");
					ImGui::Text(xorstr("Config:"));
					static char buff[128] = "";
					const auto width = ImGui::GetWindowWidth();
					ImGui::InputText(xorstr("##CONFIGNAME"), buff, 32);
					ImGui::SameLine();
					if (ImGui::Button(xorstr("New"), ImVec2(50, 25))) {
						if (std::string(buff).length() < 3) {
							ImGui::OpenPopup(xorstr("warn"));
							str_warn = xorstr("Config Must be at least 3 char length!");
						}
						else {
							Settings::Save_Settings(buff);
							std::string(buff).clear();
						}
					}
					auto vec_files = Settings::GetList();
					static auto i_config = 0;
					ImGui::Combo(xorstr("##CONFIG"), &i_config, vector_getter, static_cast<void*>(&vec_files), vec_files.size());
					if (ImGui::Button(xorstr("Save"), ImVec2(180, 25))) {
						if (!vec_files.empty()) {
							Settings::Save_Settings(vec_files[i_config]);
						}
					}
					ImGui::SameLine();
					if (ImGui::Button(xorstr("Load"), ImVec2(180, 25))) {
						if (!vec_files.empty()) {
							Settings::Load_Settings(vec_files[i_config]);
						}
					}
					if (ImGui::Button(xorstr("Delete"), ImVec2(180, 25)))
					{
						if (!vec_files.empty()) {

							auto str_config = xorstr("C:\\EliminationCheatsLite\\configs\\") + vec_files[i_config] + xorstr(".ini");
							if (remove(str_config.c_str()) != 0)
							{
								str_warn = xorstr("Could not delete the file!");
								ImGui::OpenPopup(xorstr("warn"));
							}
							str_config = xorstr("");
							str_config_name = xorstr("");
						}
					}
					if (ImGui::BeginPopup(xorstr("warn")))
					{
						ImGui::TextColored(ImVec4(1.f, 0.f, 0.f, 1.f), str_warn.c_str());
						ImGui::EndPopup();
					}

					ImGui::Spacing();


					});

				break;
			}
		}
		ImGui::EndChild();
	} // menu 




















	static int gameMode = 0;


	int SaveSlot = 0;
	int currentweapon = 0;
	char blueprint_name[500];
	char Loadoutname[500];
	int currentcamo = 0;


	char Blueprintname1[500];
	char Blueprintname2[500];
	char Blueprintname3[500];
	char Blueprintname4[500];





	void menu()
	{

		ImGui::StyleColorsDark();

		if (GetAsyncKeyState(VK_INSERT) & 0x1)
		{
			b_menu_open = !b_menu_open;
		}


		if (b_menu_open && screenshot::visuals)
		{
			static const char* camo_item[] = { "None", "Gold","Platinum","Polyatomic","Orion" };
			static const char* slot1 = "Select Item";
			static const char* slot2 = "Select Item";
			static const char* slot3 = "Select Item";
			static const char* slot4 = "Select Item";




			ImGui::SetNextWindowSize(ImVec2(450, 610));
			ImGui::PushFont(GetFont(6));
			ImGui::Begin("FORCE SAVE", &b_menu_open);

			if (ImGui::BeginTabBar("##bar1"))
			{
				if (ImGui::BeginTabItem("Main"))
				{
					ImGui::Text("Blueprints");

					ImGui::SetNextItemWidth(120);
					if (ImGui::BeginCombo("##camo1", slot1))
					{
						for (size_t i = 0; i < IM_ARRAYSIZE(camo_item); i++)
						{
							bool is_selected = (slot1 == camo_item[i]);
							if (ImGui::Selectable(camo_item[i], is_selected))
							{
								slot1 = camo_item[i];
							}
							if (is_selected)
							{
								ImGui::SetItemDefaultFocus();
							}

						}
						ImGui::EndCombo();

					}
					ImGui::SetNextItemWidth(120);
					ImGui::SameLine();
					ImGui::InputText("Blueprintname1", Blueprintname1, 500);
					ImGui::SetNextItemWidth(120);
					if (ImGui::BeginCombo("##camo2", slot2))
					{
						for (size_t i = 0; i < IM_ARRAYSIZE(camo_item); i++)
						{
							bool is_selected = (slot2 == camo_item[i]);
							if (ImGui::Selectable(camo_item[i], is_selected))
							{
								slot2 = camo_item[i];
							}
							if (is_selected)
							{
								ImGui::SetItemDefaultFocus();
							}

						}
						ImGui::EndCombo();

					}
					ImGui::SetNextItemWidth(120);
					ImGui::SameLine();
					ImGui::InputText("Blueprintname2", Blueprintname2, 500);
					ImGui::SetNextItemWidth(120);
					if (ImGui::BeginCombo("##camo3", slot3))
					{
						for (size_t i = 0; i < IM_ARRAYSIZE(camo_item); i++)
						{
							bool is_selected = (slot3 == camo_item[i]);
							if (ImGui::Selectable(camo_item[i], is_selected))
							{
								slot3 = camo_item[i];
							}
							if (is_selected)
							{
								ImGui::SetItemDefaultFocus();
							}

						}
						ImGui::EndCombo();

					}
					ImGui::SetNextItemWidth(120);
					ImGui::SameLine();
					ImGui::InputText("Blueprintname3", Blueprintname3, 500);
					ImGui::SetNextItemWidth(120);
					if (ImGui::BeginCombo("##camo4", slot4))
					{
						for (size_t i = 0; i < IM_ARRAYSIZE(camo_item); i++)
						{
							bool is_selected = (slot4 == camo_item[i]);
							if (ImGui::Selectable(camo_item[i], is_selected))
							{
								slot4 = camo_item[i];
							}
							if (is_selected)
							{
								ImGui::SetItemDefaultFocus();
							}

						}
						ImGui::EndCombo();
					}
					ImGui::SetNextItemWidth(120);
					ImGui::SameLine();
					ImGui::InputText("Blueprintname4", Blueprintname4, 500);




					if (ImGui::Button("Set Blueprint Data")) {

						std::vector<const char*> camos =
						{
							slot1,
							slot2,
							slot3,
							slot4
						};

						std::vector<const char*> BluePrintName =
						{
							Blueprintname1,
							Blueprintname2,
							Blueprintname3,
							Blueprintname4
						};

						BP bp;
						bp.SaveAll(BluePrintName, camos);

						memset(Blueprintname1, 0, 500);
						memset(Blueprintname2, 0, 500);
						memset(Blueprintname3, 0, 500);
						memset(Blueprintname4, 0, 500);



					}
					ImGui::SameLine();
					if (ImGui::Button("Save BluePrints")) {
						*(uintptr_t*)sig_offsets::s_blueprintsDirty = 1;

					}
					//ImGui::Combo("##ua", &v2.invidual_select, "Weapon\0Attachment\0Camo\0Bundle\0Equipment\0Killstreak\0Perk\0Operator\0Operator Equip\0Operator Skin\0Vehicle Skin\0Vehicle Horn\0Vehicle Track\0Feature\0");
					if (ImGui::Button("Unlock"))
					{
						UnlockAllV2 v2 = UnlockAllV2();
						v2.unlock_All();
					}


					if (ImGui::Button("Unlock Gunsmith"))
					{
						UnlockAllV2 v2 = UnlockAllV2();
						v2.Test();
					}

					ImGui::EndTabItem();

				}

			
			}




			ImGui::EndTabBar();
			ImGui::PopFont();
			ImGui::End();

		}


	}
}

const char*
BP::Normalize(const char* name, int idx)
{
	if (!strlen(name))
		name = "^9Force^6Save";

	char buffer[30]{};
	sprintf_s(buffer, "%03d%s", idx, name);
	return buffer;
}

int
BP::GetCamo(const char* camo)
{
	// fallback for if user doesnt select a camo
	if (strstr("Blueprint", camo)) return 0;

	std::unordered_map<const char*, int> map = {
		{"None", 0},
		{"Gold", 239},
		{"Platinum", 240},
		{"Polyatomic", 241},
		{"Orion", 242},
	};
	return map.at(camo);
}

void
BP::WriteBlueprint(Blueprint_info& bp, int idx)
{
	Blueprint* fromGame = reinterpret_cast<Blueprint*>(structStart + bp.weapon * STRUCT_SIZE + idx * STRUCT_SIZE / 5);
	if (is_valid_ptr(fromGame))
	{
		strncpy_s(fromGame->blueprintName, bp.blueprintName, 20);
		fromGame->weapon = bp.weapon;
		fromGame->camo = bp.camo;
		fromGame->lootItemID = 11354;
	}
}

void
BP::Save(const char* name, int camo, int weapon, int idx)
{
	Blueprint_info bp{};

	strncpy_s(bp.blueprintName, name, 20);
	bp.camo = camo;
	bp.weapon = weapon;
	WriteBlueprint(bp, idx);
}

void
BP::SaveAll(std::vector<const char*> names, std::vector<const char*> camos)
{
	for (int weapon = 0; weapon < 55 /*/ STRUCT_SIZE - 2*/; ++weapon)
	{
		for (int i = 0; i < 5; ++i)
		{
			int camo = GetCamo(camos[i]);
			const char* name = Normalize(names[i], i);
			Save(name, camo, weapon, i);
		}
	}
}



void hotkey(const char* label, KeyBind& key, float samelineOffset, const ImVec2& size) noexcept
{
	const auto id = ImGui::GetID(label);
	ImGui::PushID(label);

	ImGui::TextUnformatted(label);
	ImGui::SameLine(samelineOffset);

	if (ImGui::GetActiveID() == id) {
		ImGui::PushStyleColor(ImGuiCol_Button, ImGui::GetColorU32(ImGuiCol_ButtonActive));
		ImGui::Button("...", size);
		ImGui::PopStyleColor();

		ImGui::GetCurrentContext()->ActiveIdAllowOverlap = true;
		if ((!ImGui::IsItemHovered() && ImGui::GetIO().MouseClicked[0]) || key.setToPressedKey())
			ImGui::ClearActiveID();
	}
	else if (ImGui::Button(key.toString(), size)) {
		ImGui::SetActiveID(id, ImGui::GetCurrentWindow());
	}

	ImGui::PopID();
}