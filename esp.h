#pragma once
#include "stdafx.h"
#include "imgui/imgui.h"
#include "imgui/imgui_internal.h"
#include "sdk.h"
#include"offsets.h"
#include "spoofcall.h"
enum BONE_INDEX : BYTE
{
    BONE_POS_HELMET = 8,

    BONE_POS_HEAD = 7,
    BONE_POS_NECK = 6,
    BONE_POS_CHEST = 5,
    BONE_POS_MID = 4,
    BONE_POS_TUMMY = 3,
    BONE_POS_PELVIS = 2,

    BONE_POS_RIGHT_FOOT_1 = 21,
    BONE_POS_RIGHT_FOOT_2 = 22,
    BONE_POS_RIGHT_FOOT_3 = 23,
    BONE_POS_RIGHT_FOOT_4 = 24,

    BONE_POS_LEFT_FOOT_1 = 17,
    BONE_POS_LEFT_FOOT_2 = 18,
    BONE_POS_LEFT_FOOT_3 = 19,
    BONE_POS_LEFT_FOOT_4 = 20,

    BONE_POS_LEFT_HAND_1 = 13,
    BONE_POS_LEFT_HAND_2 = 14,
    BONE_POS_LEFT_HAND_3 = 15,
    BONE_POS_LEFT_HAND_4 = 16,

    BONE_POS_RIGHT_HAND_1 = 9,
    BONE_POS_RIGHT_HAND_2 = 10,
    BONE_POS_RIGHT_HAND_3 = 11,
    BONE_POS_RIGHT_HAND_4 = 12
};

static std::vector<std::pair<int, int>> skeleton = {
    {
        {BONE_POS_HELMET, BONE_POS_HEAD},
        {BONE_POS_HEAD, BONE_POS_NECK},
        {BONE_POS_NECK, BONE_POS_CHEST},
        {BONE_POS_CHEST, BONE_POS_MID },
        {BONE_POS_MID, BONE_POS_TUMMY},
        { BONE_POS_TUMMY, BONE_POS_LEFT_FOOT_1 },
        { BONE_POS_LEFT_FOOT_1, BONE_POS_LEFT_FOOT_2 },
        { BONE_POS_LEFT_FOOT_2, BONE_POS_LEFT_FOOT_3 },
        { BONE_POS_LEFT_FOOT_3, BONE_POS_LEFT_FOOT_4 },
        {BONE_POS_TUMMY, BONE_POS_RIGHT_FOOT_1},
        {BONE_POS_RIGHT_FOOT_1, BONE_POS_RIGHT_FOOT_2},
        {BONE_POS_RIGHT_FOOT_2, BONE_POS_RIGHT_FOOT_3},
        {BONE_POS_RIGHT_FOOT_3, BONE_POS_RIGHT_FOOT_4},
        {BONE_POS_NECK, BONE_POS_LEFT_HAND_1},
        {BONE_POS_LEFT_HAND_1, BONE_POS_LEFT_HAND_2},
        {BONE_POS_LEFT_HAND_2, BONE_POS_LEFT_HAND_3},
        {BONE_POS_LEFT_HAND_3, BONE_POS_LEFT_HAND_4},
        {BONE_POS_NECK, BONE_POS_RIGHT_HAND_1},
        {BONE_POS_RIGHT_HAND_1, BONE_POS_RIGHT_HAND_2},
        {BONE_POS_RIGHT_HAND_2, BONE_POS_RIGHT_HAND_3},
        {BONE_POS_RIGHT_HAND_3, BONE_POS_RIGHT_HAND_4},
    }
};



struct bonestest
{
    bonestest(int entnum) : entindex{ entnum } {}
    int entindex{};
    vec3_t bone1_3d[21];
    vec3_t bone2_3d[21];
    vec3_t aim_bone2[21];
    vec3_t aim_bone;
    
    bool drawn = false;
    bool processed = false;
};


struct bone_aim
{
    bone_aim(int entnum) : entindex{ entnum } {}
    int entindex{};
    vec3_t aim_bone;

    bool drawn = false;
    bool processed = false;
};

inline std::vector<bonestest> boneArray;
inline std::vector<bonestest> boneArray2;

namespace main_game
{
    void  aimbot_thread2();
    void aimbot_thread();
    void cache_bones_aim();
	void cache_bones();
	void init(ImFont* font);
	void esp_loop(ImFont* font);
}



struct vec3_int
{
    int x, y, z;
};

struct Weapon
{
    unsigned __int16 weaponIdx;
    unsigned __int16 stickerIndices[4];
    unsigned __int16 weaponClientLoadout;
    unsigned __int16 weaponOthers;
    unsigned __int8 weaponAttachments[13];
    unsigned __int8 attachmentVariationIndices[29];
    unsigned __int8 weaponCamo;
    unsigned __int8 weaponLootId;
    unsigned __int8 scopeVariation;
    unsigned __int8 visualAttachmentHighAddr;
};

struct LightSample
{
    bool bValid;
    unsigned __int8 value;
};

struct  UserCommandStateFlags
{};

/* 18122 */
struct usercmd_s
{
    unsigned __int64 buttons;
    UserCommandStateFlags stateFlags;
    int serverTime;
    int commandTime;
    int inputTime;
    vec3_int angles;
    Weapon weapon;
    Weapon offHand;
    char forwardmove;
    char rightmove;
    char pitchmove;
    char yawmove;
    unsigned __int16 meleeChargeEnt;
    unsigned __int8 packedControllerAim;
    char selectedLoc[2];
    char selectedAngle;
    char remoteControlAngles[2];
    char remoteControlMove[3];
    char mlgSelectedLoc[2];
    char mlgSelectedAngle;
    LightSample lightSample;
    __int16 avoidForward;
    __int16 avoidRight;
    unsigned __int8 upmove;
    unsigned __int8 downmove;
    bool inputFromGamepad;
    bool isAlternate;
    __int16 vehAngles[3];
    float vehOrgX;
    float vehOrgY;
    float vehOrgZ;
    float gunPitch;
    float gunYaw;
    float gunXOfs;
    float gunYOfs;
    float gunZOfs;
    float rollSensitivity;
    __int16 scriptedMeleeTarget;
    unsigned __int16 airburstMarkDistance;
    //UserCmdClientBits sightedClientsMask;
    //unsigned __int16 extrapolationMax;
    //unsigned __int16 interpolationMax;
};

enum StanceState : __int32
{
    CL_STANCE_STAND = 0x0,
    CL_STANCE_CROUCH = 0x1,
    CL_STANCE_PRONE = 0x2,
    CL_STANCE_SLIDE = 0x3,
    CL_STANCE_SLIDE_NOHOLD = 0x4,
};


struct HeldStanceInputStack
{
    StanceState heldStances[2];
    int stackIndex;
};
struct SecureCmdNumber
{
    int cmdNumber;
};
struct TurnRateCapInfo;
struct ClInputSequenceState;

struct ClInputSequenceVirtualAxisKeyState;

struct ClInputSequenceVirtualAxisKeyState;

struct SecureCLViewangles
{
    vec3_t clViewangles;
};

struct TurnRateCapInfo
{
};

struct ClInputSequenceState
{
};

struct ClInputSequenceVirtualAxisKeyState
{
};


struct ClActiveClient
{
    void* table; /*VFT*/
    bool usingAds;
    bool holdingBreath;
    bool wasHoldingBreath;
    bool isRemoteControlling;
    bool autoForward;
    int mouseDx[2];
    int mouseDy[2];
    float weightedDx[8];
    float weightedDy[8];
    int mouseWheelDelta;
    float mouseWheelDeltaSustainTime;
    int mouseIndex;
    bool stanceHeld;
    bool crouchToggle;
    bool crouchHoldPressed;
    bool crouchHoldReleased;
    bool ignoreStanceInput;
    bool wasSprinting;
    bool requireSlideRelease;
    bool standToProne;
    HeldStanceInputStack heldInputs;
    bool weaponForwardHoldStartedInAds;
    StanceState stance;
    StanceState stanceOnButtonDown;
    int stanceTime;
    Weapon cgameUserCmdWeapon;
    Weapon cgameUserCmdOffHand;
    int cgameUserCmdAlternate;
    float cgameFOVSensitivityScale;
    unsigned __int64 cgameExtraButtons;
    int cgameMountRegisterTime;
    UserCommandStateFlags cgameUserCmdStateFlags;
    TurnRateCapInfo turnRateCapInfos[5];
    ClInputSequenceState cmdSequenceState[7];
    ClInputSequenceVirtualAxisKeyState cmdSequenceAxisKeys[5];
    bool isDoubleAdsMountHeld;
    bool triggerSuperSprint;
    bool detonateOffhandPrimary;
    bool detonateOffhandSecondary;
    SecureCLViewangles clViewangles;
    int cmdInputAccumTimeMs;
    vec3_t mouseViewDelta;
    vec2_t accumulatedMouseDelta;
    int accumulatedInputTimer;
 /*  usercmd_s cmds[128];
    SecureCmdNumber cmdNumber;
    unsigned __int64 cmdAccumTimeUsec;
    unsigned __int64 cmdTimeUsec;
    int cmdTimeMsec;
    usercmd_s extrapCmd;
    int m_latencyDeviceState;
    unsigned int cmd_number_aab;
    unsigned int cmd_number_set_aab;
    unsigned int clviewangles_aab;
    unsigned int clviewangles_set_aab;
    bool mustNotActivatePing;
    int m_localClientNum;*/
};

extern void engine_renderer();







