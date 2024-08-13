#pragma once
#include "vec.h"
#include "classes.h"


struct GfxSceneHudOutlineInfo
{
	unsigned int color;
	float scopeStencil;
	bool drawOccludedPixels;
	bool drawNonOccludedPixels;
	bool fill;
	bool useAlternateColor;
	bool forSpectator;
	bool specialActive;
	unsigned char renderMode;
	unsigned char lineWidth;
	unsigned __int8 temperatureSet;
	unsigned int mapEntLookup;
	float temperatureBase;
	float temperatureScale;
	float characterEVOffset;
};

struct GfxSceneEntityMutableShaderData
{
	char pd[0x88];
	GfxSceneHudOutlineInfo hudOutlineInfo;
	float eyeSensorPupilSize;
};

struct cpose_t
{
	unsigned __int8 eType;
	unsigned __int8 cullIn;
	unsigned __int8 isPosePrecise;
	unsigned __int8 entOriginSet;
	volatile int mdaoCullIn;
	unsigned __int8 ragdollFlags;
	unsigned __int8 isEntityPose;
	unsigned __int8 hasStaleEntityPose;
	unsigned __int8 mayhemInstanceId;
	bool isMayhem;
	bool reusePreviousSkelPose;
	int ragdollHandle;
	int killcamRagdollHandle;
	int physicsId;
	vec3_t actualOrigin;

};

typedef __int64(__fastcall* R_AddDObjToScene_t)(
	DObj* obj,
	cpose_t* pose,
	unsigned int entnum,
	unsigned int renderFlags,
	GfxSceneEntityMutableShaderData* sceneEntityMutableShaderData,
	const vec3_t* lightingOrigin,
	float materialTime);

extern R_AddDObjToScene_t AddDObjToScene;
extern __int64 R_AddDObjToScene(
	DObj* obj,
	cpose_t* pose,
	unsigned int entnum,
	unsigned int renderFlags,
	GfxSceneEntityMutableShaderData* sceneEntityMutableShaderData,
	const vec3_t* lightingOrigin,
	float materialTime);


typedef void(__fastcall* R_AddViewmodelDObjToScene_t)( DObj* obj,  cpose_t* pose, unsigned int entnum, unsigned int renderFlags,  GfxSceneEntityMutableShaderData* entityMutableShaderData,  vec3_t* lightingOrigin, float materialTime, int markableViewmodel);
inline R_AddViewmodelDObjToScene_t AddViewmodelDObjToScene;
 extern void  R_AddViewmodelDObjToScene( DObj* obj,  cpose_t* pose, unsigned int entnum, unsigned int renderFlags,  GfxSceneEntityMutableShaderData* entityMutableShaderData,  vec3_t* lightingOrigin, float materialTime, int markableViewmodel);

 inline __int64 s_LUI_DataBinding_Get_CrosshairColororig = NULL;
 extern int s_LUI_DataBinding_Get_CrosshairColorhk(int a1);