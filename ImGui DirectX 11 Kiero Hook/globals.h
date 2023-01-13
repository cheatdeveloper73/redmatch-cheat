#pragma once

#include "il2cpp.h"

#include <mutex>
#include <vector>

namespace globals
{

	inline PlayerController_o* local_controller = nullptr;

	inline std::vector<PlayerController_o*> players{};
	inline std::mutex player_sync{};

}