#pragma once

#include "config.h"
#include <cstdint>
#include "il2cpp/il2cpp.h"
#include "il2cpp.h"
#include "globals.h"
#include <algorithm>

namespace features
{

	void no_spread();
	void infinite_ammo();
	void rapid_fire();
	void aimbot();

	inline UnityEngine_Vector3_o aimbot_pos;

	struct bullet_hit
	{
		UnityEngine_Vector3_o pos;
		uint64_t time;
	};

	inline std::vector<bullet_hit> hits{};

}