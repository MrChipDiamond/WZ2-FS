#include"aimbot.h"
#include <timeapi.h>
#include "settings.h"
#include "Gamepad.h"
#include "esp.h"
#include"offsets.h"
#include"gamefunctions.h"
#include "Menu.h"
# pragma comment(lib, "winmm.lib")

bool xbox_controller = false;
bool found_controller = false;
bool iskey_down = false;
constexpr float init_center_dist = 10000.0f;
float min_center_dist = init_center_dist;
POINT lock_position;
vec3_t lock_position_angle = vec3_t(0, 0, 0);
float reactionTime = 1.5f; //150ms
float numberOfPointToIgnore = 0.0191;
vec3_t g_AimPosition;
int currentPlayer = -1;
int old_target = -1;
double next_tick = 0.0;
int AimKey = 0; //0 ~ 2


vec2_t m_TimeToTarget;
vec2_t m_CurAimTime;
DWORD lastTick = 0;
void CVelocityManager::Update(DWORD index, vec3_t vPos)
{
	if (!bUpdateTick) return;
	velocityMap[index].delta = (vPos - velocityMap[index].lastPos);
	velocityMap[index].lastPos = vPos;
}
vec3_t CVelocityManager::GetSpeed(DWORD index)
{
	return velocityMap[index].delta;
}
void CVelocityManager::Tick()
{
	//inits new tick
	static DWORD lastTick = 0;
	DWORD t = timeGetTime();
	bUpdateTick = lastTick < t;
	if (bUpdateTick) lastTick = t + (DWORD)(updateTime * 10.f);
}

bool CTargetPlayerManager::isBlockPlayer(int time)
{
	//inits new tick
	DWORD t = timeGetTime();

	// Used to reset the timer
	if (time != -1)
	{
		lastTick = time;
		old_target = -1;
		currentPlayer = -1;
		return false;
	}

	if (old_target == -1 && currentPlayer == -1)
		return false;

	if (old_target == -1 && currentPlayer != -1)
	{
		lastTick = t + *(DWORD*)&reactionTime;
		old_target = currentPlayer;
		return true;
	}

	//Reset the last tick time
	if (currentPlayer != old_target)
	{
		isReactionTime = lastTick < t;
		if (isReactionTime)
		{
			lastTick = t + *(DWORD*)&reactionTime;

			old_target = currentPlayer;
		}
		return isReactionTime; // when this become TRUE means switch target is permitted
	}

	if (currentPlayer == old_target)
	{
		// If is the same player means the blocking is true
		lastTick = t + *(DWORD*)&reactionTime;
		return true;
	}

	return false;
}

aim_assistclass::aim_assistclass()
{

	tpManager = CTargetPlayerManager(); 
	velocityManager = CVelocityManager();

	initGamepad();



}



void onDeviceAttached(struct Gamepad_device* device, void* context) {

	if (device->vendorID != 0)
		found_controller = true;

	if (device->vendorID == 0x45E)
		xbox_controller = true;

}

void onDeviceRemoved(struct Gamepad_device* device, void* context) {
}

void onButtonDown(struct Gamepad_device* device, unsigned int buttonID, double timestamp, void* context) {
	//if (g_Vars->mSettings.b_aimbot)
		//if (buttonID == 6)
			//iskey_down = true;
			//LOGS("Button %u down on device %u at %f with context %p\n", buttonID, device->deviceID, timestamp, context);

	/*if (buttonID == 6)
		iskey_down = true;*/
}

void onButtonUp(struct Gamepad_device* device, unsigned int buttonID, double timestamp, void* context) {
	//if (g_Vars->mSettings.b_DebugEnable)
		//if (buttonID == 6)
			//iskey_down = false;
		/*	LOGS("Button %u up on device %u at %f with context %p\n", buttonID, device->deviceID, timestamp, context);

	if (buttonID == 6)
		iskey_down = false;*/
}

void onAxisMoved(struct Gamepad_device* device, unsigned int axisID, float value, float lastValue, double timestamp, void* context) {
	
}

void aim_assistclass::initGamepad()
{
	Gamepad_deviceAttachFunc(onDeviceAttached, (void*)0x1);
	Gamepad_deviceRemoveFunc(onDeviceRemoved, (void*)0x2);
	Gamepad_buttonDownFunc(onButtonDown, (void*)0x3);
	Gamepad_buttonUpFunc(onButtonUp, (void*)0x4);
	Gamepad_axisMoveFunc(onAxisMoved, (void*)0x5);
	Gamepad_init();
}

bool is_button_down()
{
	return ((iskey_down) ? 1 : 0);
}

bool aim_assistclass::is_key_down(int key)
{
	return ((GetAsyncKeyState(key) & 0x8000) ? 1 : 0);
}

unsigned long aim_assistclass::get_bone_opt(int index)
{
	switch (index)
	{
	case 0:
		return BONE_INDEX::BONE_POS_HEAD;
	case 1:
		return BONE_INDEX::BONE_POS_NECK;
	case 2:
		return BONE_INDEX::BONE_POS_CHEST;
	};
}

void aim_assistclass::reset_lock()
{
	min_center_dist = init_center_dist;
}

bool aim_assistclass::get_target_in_fov(vec2_t target_pos2, vec3_t target_pos, int index)
{
	auto refdef2 = sdk::get_refdef();

	if (is_bad_ptr(refdef2))
		return false;
	auto refdef = refdef2;
	float x = target_pos2.x - (refdef->Width / 2);
	float y = target_pos2.y - (refdef->Height / 2);
	float center_dis = sqrt(pow(y, 2) + pow(x, 2));
	if ((aim_assistclass::tpManager.isBlockPlayer() && currentPlayer == index) || currentPlayer == -1)
	{
		if (center_dis < min_center_dist && center_dis <= Settings::esp::fov_size)
		{
			bestEntityId = index;
			min_center_dist = center_dis;
			lock_position_angle = target_pos;
			return true;
		}
	}

	return false;
}

vec3_t aim_assistclass::get_vieworigin()
{
	auto cg_t = GameFunctions::GetClientInfo();
	if (!cg_t) return vec3_t();

	//ci_vieworigin_offset : 89 83 ? ? ? ? 8B 47 04 89 83 ? ? ? ? 8B 47 08 89 83 ? ? ? ? 48 8B 5C 24 ? 48 83 C4 20 5F C3 
	return *(vec3_t*)(cg_t + cg_vieworigin_offset);
}

vec2_t aim_assistclass::get_viewangle()
{
	vec2_t viewangle = vec2_t();

	auto addr =  GameFunctions::GetClientInfo();


	if (!addr) return vec2_t();

	//cg_t::SetViewModelTransform
	//F3 0F 10 81 ? ? ? ? 48 8D 54 24 ? F3 0F 10 99 ? ? ? ? 49 8B F1 48 8B BC 24 ? ? ? ? 48 8B D9 [ + 4 <int&>]
	viewangle.y = *(float*)(addr + cg_viewangle_offset);//标准化正常viewangle
	viewangle.x = 360.f - *(float*)(addr + cg_viewangle_offset + 4);

	if (viewangle.y > 85.0f)
		viewangle.y = viewangle.y - 360.0f;
	viewangle.x -= 270.f;

	//LOGS("client viewangle x: %lf, y: %lf", viewangle.x, viewangle.y);

	return viewangle;
}

uint32_t aim_assistclass::xor_for_viewangle(uint64_t pointer, uint32_t value)
{

	//input_seed_offset : 44 8B 81 ? ? ? ? 4C 8D 89 ? ? ? ? F3 ? ? ? 41 ? ? F3 ? ? ? ? ? ? ? 41 ? ?
	uint32_t xorvalue1 = *(uint32_t*)(cached_ClientActivePtr + ca_seed_offset);//random_seed_viewangle
	uint32_t xorvalue2 = pointer;
	uint32_t xorvalue = (((xorvalue1 ^ xorvalue2) + 2) * (xorvalue1 ^ xorvalue2)) ^ value;
	return xorvalue;
}

#define D3DX_PI    ((FLOAT)  3.1415927f)
float GetMagnitude(vec3_t vec)
{
	return sqrt(pow(vec.x, 2) + pow(vec.y, 2) + pow(vec.z, 2));
}

vec3_t NormalizeVec(vec3_t vec)
{
	float Length = GetMagnitude(vec);
	vec /= Length;
	return vec;
}
vec2_t calculate_angle2(vec3_t src, vec3_t dest, vec3_t ViewAxis[3])
{
	float smoothing = 8.0f;
	vec3_t angles(0, 0, 0);
	vec3_t aimAt = dest - src;
	vec3_t normalized = NormalizeVec(aimAt);
	angles = vec3_t();
	aimAt = normalized * ViewAxis[1];
	float yaw = (float)(asin(aimAt.x + aimAt.y + aimAt.z) * (180 / D3DX_PI));
	aimAt = normalized * ViewAxis[2];
	float pitch = (float)(-asin(aimAt.x + aimAt.y + aimAt.z) * (180 / D3DX_PI));

	angles.x = yaw;
	angles.y = pitch;
	angles.z = 0.0f;
	//LOGS("angle x: %s, y: %s", std::to_string(angles.x), std::to_string(angles.y));

	return angles;
}



vec2_t calculate_angle(vec3_t src, vec3_t dst) {
	vec2_t angles = vec2_t(0, 0);

	vec3_t delta = src - dst;
	float hyp = sqrtf(delta.x * delta.x + delta.y * delta.y);

	angles.x = std::atanf(delta.x / delta.y) * 57.2957795131f;
	angles.y = std::atanf(-delta.z / hyp) * -57.2957795131f;

	if (delta.y >= 0.0f)
		angles.x -= 180.0f;

	return angles;
}

bool aim_assistclass::target_in_fov(vec3_t middle_position, vec3_t root_position, vec3_t position, int index)
{
	middle_position.z += (root_position.z - position.z) / 2.0f;
	vec2_t delta = calculate_angle(get_vieworigin(), middle_position) - get_viewangle();
	if (delta.length() <= Settings::esp::fov_size);
	{
		Settings::esp::fov_size = delta.length();
		bestEntityId = index;
	}
}

float smoothing = 0.6f;
void aim_assistclass::aim_at(vec3_t dst)
{
	sdk::refdef_t* refdef = sdk::get_refdef();
	cached_ClientActivePtr = GameFunctions::GetClActiveClient();
	if (!cached_ClientActivePtr || !refdef)
		return;

	//LOGS_ADDR(cached_ClientActivePtr);

	//input_angle_offset : 44 8B 81 ? ? ? ? 4C 8D 89 ? ? ? ? F3 ? ? ? 41 ? ? F3 ? ? ? ? ? ? ? 41 ? ?
	uint32_t viewangle_y_xor = xor_for_viewangle(cached_ClientActivePtr + ca_angle_offset, *(uint32_t*)(cached_ClientActivePtr + ca_angle_offset));//内存angle可修改区域
	uint32_t viewangle_x_xor = xor_for_viewangle(cached_ClientActivePtr + ca_angle_offset + 0x4, *(uint32_t*)(cached_ClientActivePtr + ca_angle_offset + 0x4));

	float* viewangle_y = (float*)&viewangle_y_xor;
	float* viewangle_x = (float*)&viewangle_x_xor;

	//LOGS("viewangle x: %s, y: %s", std::to_string(*viewangle_x), std::to_string(*viewangle_y));

	vec2_t viewangle_ci = get_viewangle();

	vec3_t vieworgin = get_vieworigin();

	//vec3_t delta_angle = math::calculate_angle(get_vieworigin(), dst, sdk::refdef->view.axis);

	vec2_t delta_angle = calculate_angle2(vieworgin, dst, refdef->ViewAxis);

	float viewangle_y_new = *viewangle_y + (delta_angle.y - viewangle_ci.y);
	float viewangle_x_new = *viewangle_x - (delta_angle.x - viewangle_ci.x);

	//LOGS("viewangle new x: %s, y: %s", std::to_string(viewangle_x_new), std::to_string(viewangle_y_new));

	int32_t viewangle_y_new_xor = *(int32_t*)&viewangle_y_new;
	int32_t viewangle_x_new_xor = *(int32_t*)&viewangle_x_new;

	*(uint32_t*)(cached_ClientActivePtr + ca_angle_offset) = xor_for_viewangle(cached_ClientActivePtr + ca_angle_offset, viewangle_y_new_xor);
	*(uint32_t*)(cached_ClientActivePtr + ca_angle_offset + 0x4) = xor_for_viewangle(cached_ClientActivePtr + ca_angle_offset + 0x4, viewangle_x_new_xor);


	////////////////////////////////////////////////////

}


void aim_assistclass::lock_on_target()
{
	Gamepad_processEvents();
	if (iskey_down)
	{
	
		currentPlayer = bestEntityId;

		if (min_center_dist != init_center_dist && aim_assistclass::tpManager.isBlockPlayer())
		{
			aim_at(lock_position_angle);
		}
		else
		{
			currentPlayer = -1;
		}
	}
	else
	{
		aim_assistclass::tpManager.isBlockPlayer(0);
	}

	if (is_key_down(VK_RBUTTON))
	{
		currentPlayer = bestEntityId;

		if (min_center_dist != init_center_dist && aim_assistclass::tpManager.isBlockPlayer())
		{
			aim_at(lock_position_angle);
		}
		else
		{
			currentPlayer = -1;
		}
	}
	else
	{
		aim_assistclass::tpManager.isBlockPlayer(0);
	}
}