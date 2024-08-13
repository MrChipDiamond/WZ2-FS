#include "vec.h"
#include <map>
#include "spoofcall.h"

//inline int currentPlayer = -1;

extern int currentPlayer ;

class CTargetPlayerManager {
public:
	//CTargetPlayerManager() {}
	bool isReactionTime = false;
	bool isTimerStart = false;
	bool isBlockPlayer(int time = -1);
	int reset = 0;
};

class CVelocityManager
{

public:
	struct VelocityData
	{
		vec3_t lastPos;
		vec3_t delta;
	};
	int updateTime = 1;
	bool bUpdateTick = false;
	std::map<DWORD, VelocityData> velocityMap;
public:
	//CVelocityManager() {}
	void Update(DWORD index, vec3_t vPos);
	vec3_t GetSpeed(DWORD index);
	void Tick();
};



class aim_assistclass
{	
	float init_center_dist = 10000.0f;
	float min_center_dist = init_center_dist;
	int bestEntityId{};

	vec3_t lock_position_angle = vec3_t();

	int cg_vieworigin_offset= 0x3b62c;
	int cg_viewangle_offset = 0x9fb7c;
	int ca_angle_offset = 0x8958;
	int ca_seed_offset = 0xb4;

	//__int64 ClientActivePtr = 0x41E46B0;

	uintptr_t cached_ClientActivePtr{};


	uint32_t xor_for_viewangle(uint64_t pointer, uint32_t value);
	vec3_t get_vieworigin();
	vec2_t get_viewangle();

	CVelocityManager velocityManager; 
	CTargetPlayerManager tpManager; 


	void initGamepad();
	bool is_key_down(int key);
	unsigned long get_bone_opt(int index);
	bool target_in_fov(vec3_t middle_position, vec3_t root_position, vec3_t position, int index);
	void aim_at(vec3_t dec);
public:
	//~aim_assist();
	void reset_lock();
	aim_assistclass();
	void aimbot();
	bool get_target_in_fov(vec2_t target_pos2, vec3_t target_pos, int index);

	void lock_on_target();
};

//extern std::unique_ptr<aim_assist> aim_assist_class;
inline aim_assistclass	aim_assist_class;

extern int currentPlayer;





