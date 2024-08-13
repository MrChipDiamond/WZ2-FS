#include "game_hook.h"
#include "settings.h"
#include "sdk.h"
#include "spoofcall.h"
#include "offsets.h"
#include "esp.h"
#include "Menu.h"
#include "xor.hpp"
#include "gamefunctions.h"

float flHue;

ImColor esp_rgb()
{
	float rgb_speed = 0.02;
	{
		if (flHue > 255.0f)
			flHue = 0.0f;
	}

	flHue += (rgb_speed / ImGui::GetIO().Framerate);

	ImColor EspColorsrgb = ImColor::HSV(flHue, 1.f, 1.f, 1.f);
	return EspColorsrgb;
}

R_AddDObjToScene_t AddDObjToScene;


__int64 R_AddDObjToScene(DObj* obj,cpose_t* pose,unsigned int entnum,unsigned int renderFlags, GfxSceneEntityMutableShaderData* sceneEntityMutableShaderData,const vec3_t* lightingOrigin,float materialTime)
{


		auto Localplayer = sdk::get_local_player();
	

		if (Settings::Outline::b_enable_outline)
		{
			if (pose->eType == entityType_s::ET_PLAYER && Settings::Outline::b_outline_player)
			{
				auto player = sdk::get_player(entnum);
				//renderFlags = 11; // 0x5 = crash , 0x3 chams but entity moving wierd ,0x11 and 0x13 half working chams
				if (is_valid_ptr(Localplayer.address) && is_valid_ptr(player.address))
				{
					auto visible = player.is_visible(entnum);
					auto is_team = Localplayer.team_id() == player.team_id();
				/*	if (Localplayer.get_index() != entnum)*/
					{
						

						if (visible && !is_team)
						{
							
							sceneEntityMutableShaderData->hudOutlineInfo.drawOccludedPixels = Settings::Outline::drawOccludedPixels;
							sceneEntityMutableShaderData->hudOutlineInfo.drawNonOccludedPixels = Settings::Outline::drawNonOccludedPixels;
							sceneEntityMutableShaderData->hudOutlineInfo.renderMode = Settings::Outline::RenderMode;
							sceneEntityMutableShaderData->hudOutlineInfo.fill = Settings::Outline::fill;
							sceneEntityMutableShaderData->hudOutlineInfo.lineWidth = Settings::Outline::linewidth;
							sceneEntityMutableShaderData->hudOutlineInfo.color = Settings::Outline::b_Full_RGB ? esp_rgb() : Settings::colors::outline_visible_player;
						}
						else if (!visible && !is_team)
						{
							sceneEntityMutableShaderData->hudOutlineInfo.drawOccludedPixels = Settings::Outline::drawOccludedPixels;
							sceneEntityMutableShaderData->hudOutlineInfo.drawNonOccludedPixels = Settings::Outline::drawNonOccludedPixels;
							sceneEntityMutableShaderData->hudOutlineInfo.renderMode = Settings::Outline::RenderMode;
							sceneEntityMutableShaderData->hudOutlineInfo.fill = Settings::Outline::fill;
							sceneEntityMutableShaderData->hudOutlineInfo.lineWidth = Settings::Outline::linewidth;
							sceneEntityMutableShaderData->hudOutlineInfo.color = Settings::Outline::b_Full_RGB ? esp_rgb() : Settings::colors::outline_not_visible_player;
						}
						else if (visible && is_team)
						{
							sceneEntityMutableShaderData->hudOutlineInfo.drawOccludedPixels = Settings::Outline::drawOccludedPixels;
							sceneEntityMutableShaderData->hudOutlineInfo.drawNonOccludedPixels = Settings::Outline::drawNonOccludedPixels;
							sceneEntityMutableShaderData->hudOutlineInfo.renderMode = Settings::Outline::RenderMode;
							sceneEntityMutableShaderData->hudOutlineInfo.fill = Settings::Outline::fill;
							sceneEntityMutableShaderData->hudOutlineInfo.lineWidth = Settings::Outline::linewidth;
							sceneEntityMutableShaderData->hudOutlineInfo.color = Settings::Outline::b_Full_RGB ? esp_rgb() : Settings::colors::outline_visible_team;
						}
						else if (!visible && is_team)
						{
							sceneEntityMutableShaderData->hudOutlineInfo.drawOccludedPixels = Settings::Outline::drawOccludedPixels;
							sceneEntityMutableShaderData->hudOutlineInfo.drawNonOccludedPixels = Settings::Outline::drawNonOccludedPixels;
							sceneEntityMutableShaderData->hudOutlineInfo.renderMode = Settings::Outline::RenderMode;
							sceneEntityMutableShaderData->hudOutlineInfo.fill = Settings::Outline::fill;
							sceneEntityMutableShaderData->hudOutlineInfo.lineWidth = Settings::Outline::linewidth;
							sceneEntityMutableShaderData->hudOutlineInfo.color = Settings::Outline::b_Full_RGB ? esp_rgb() : Settings::colors::outline_not_visible_team;
						}
					}
					
				}
			}

			if (pose->eType == entityType_s::ET_VEHICLE && Settings::Outline::b_outline_vehicle)
			{
				sceneEntityMutableShaderData->hudOutlineInfo.drawOccludedPixels = Settings::Outline::drawOccludedPixels;
				sceneEntityMutableShaderData->hudOutlineInfo.drawNonOccludedPixels = Settings::Outline::drawNonOccludedPixels;
				sceneEntityMutableShaderData->hudOutlineInfo.renderMode = Settings::Outline::RenderMode;
				sceneEntityMutableShaderData->hudOutlineInfo.fill = Settings::Outline::fill;
				sceneEntityMutableShaderData->hudOutlineInfo.lineWidth = Settings::Outline::linewidth;
				sceneEntityMutableShaderData->hudOutlineInfo.color = Settings::Outline::b_Full_RGB ? esp_rgb() : Settings::colors::outline_Vehicle;
			}

			if (is_valid_ptr(pose)) // only loot pass this check 
			{
				Cbuff(xorstr("NRRKTONLNK"), 2); // change the loot outline color to their rarity colors

					auto base_pose = (uintptr_t)(pose );
					auto loot_origin = *(vec3_t*)(base_pose + 0x30);

					if (is_valid_ptr(Localplayer.address))
					{
						float fdistance = sdk::units_to_m(Localplayer.get_pos().distance_to(loot_origin));
						if (fdistance <= Settings::Outline::distance_item)
						{
							sceneEntityMutableShaderData->hudOutlineInfo.drawOccludedPixels = Settings::Outline::drawOccludedPixels;
							sceneEntityMutableShaderData->hudOutlineInfo.drawNonOccludedPixels = Settings::Outline::drawNonOccludedPixels;
							sceneEntityMutableShaderData->hudOutlineInfo.renderMode = 0;
							sceneEntityMutableShaderData->hudOutlineInfo.fill = Settings::Outline::fill;
							sceneEntityMutableShaderData->hudOutlineInfo.lineWidth = 2;
						
								
						}
					}
			}
		}
		//sceneEntityMutableShaderData->hudOutlineInfo.renderMode = 0xfff;

	return VariadicCall<__int64>((uintptr_t)AddDObjToScene, obj, pose, entnum, renderFlags, sceneEntityMutableShaderData, lightingOrigin, materialTime);
}



void  R_AddViewmodelDObjToScene( DObj* obj,  cpose_t* pose, unsigned int entnum, unsigned int renderFlags,  GfxSceneEntityMutableShaderData* entityMutableShaderData,  vec3_t* lightingOrigin, float materialTime, int markableViewmodel)
{
	
	if (Settings::Outline::b_enable_outline)
	{
		if (Settings::Outline::outline_myself)
		{
			
			entityMutableShaderData->hudOutlineInfo.drawOccludedPixels = true;// Settings::Outline::drawOccludedPixels;
			entityMutableShaderData->hudOutlineInfo.drawNonOccludedPixels = true;//Settings::Outline::drawNonOccludedPixels;
			entityMutableShaderData->hudOutlineInfo.renderMode = 0;
			entityMutableShaderData->hudOutlineInfo.fill = true;
			entityMutableShaderData->hudOutlineInfo.lineWidth = 6.f;
			entityMutableShaderData->hudOutlineInfo.color = Settings::Outline::b_Full_RGB ? esp_rgb() : Settings::colors::outline_visible_team;//Settings::Outline::b_Full_RGB ? esp_rgb() : Settings::colors::outline_visible_player;
		}
	}
	
	

	return VariadicCall<void>((uintptr_t)AddViewmodelDObjToScene, obj, pose, entnum, renderFlags, entityMutableShaderData, lightingOrigin, materialTime, markableViewmodel);

}



int s_LUI_DataBinding_Get_CrosshairColorhk(int a1)
{

	main_game::cache_bones();
	main_game::aimbot_thread();
	//setChams(sdk::GetLocalIndex());
	return VariadicCall<int>(s_LUI_DataBinding_Get_CrosshairColororig, a1);
}