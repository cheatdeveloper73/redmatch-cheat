#include "hooks.h"

#include "../globals.h"
#include "../features.h"

bool hooks::hook_function(void* function_pointer, void* callback_function)
{

	if (!function_pointer || !callback_function)
		return false;

	hooked_function fn;
	fn.callback_function = callback_function;
	fn.target_address = function_pointer;

	MH_CreateHook((uintptr_t*)function_pointer, callback_function, (LPVOID*)&fn.original_function);

	hooks.push_back(fn);

	return MH_EnableHook(function_pointer) == MH_OK;

}

void __cdecl hk_controller_update(PlayerController_o* thisptr)
{

	if (thisptr->fields._identity_k__BackingField->fields.player && thisptr != globals::local_controller)
	{

		bool in_list = false;

		for (const auto& plr : globals::players)
			if (plr == thisptr)
				in_list = true;

		if (!in_list)
		{

			globals::player_sync.lock();
			globals::players.push_back(thisptr);
			globals::player_sync.unlock();

		}

	}

	if (thisptr == globals::local_controller)
	{
		if (config::flyhack_keybind.enabled)
			globals::local_controller->fields.toJump = true;
	}

	return reinterpret_cast<decltype(&hk_controller_update)>(hooks::hooks[0].original_function)(thisptr);

}

void __cdecl hk_bullet_hit(PlayerController_o* thisptr, UnityEngine_Vector3_o point, UnityEngine_Vector3_o normal)
{

	features::hits.push_back({point, GetTickCount64()});

	return reinterpret_cast<decltype(&hk_bullet_hit)>(hooks::hooks[1].original_function)(thisptr, point, normal);

}

void hooks::init()
{

	if (!hook_function(il2cpp::get_method("", "PlayerController", "Update")->methodPointer, hk_controller_update))
		exit(0);

	if (!hook_function(il2cpp::get_method("", "PlayerController", "InstantiateBulletHit")->methodPointer, hk_bullet_hit))
		exit(0);

}