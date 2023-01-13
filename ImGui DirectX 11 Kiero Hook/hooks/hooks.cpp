#include "hooks.h"

#include "../globals.h"
#include "../features.h"

#define OCALL(x, y, ...) reinterpret_cast<decltype(&x)>(y)(...)

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

void __cdecl hk_ban(MyceliumPlayer_o* player)
{

	std::cout << "(ban) attempted to ban " << il2cpp::to_string(player->fields._name) << "\n";

	return;

}

void __cdecl hk_local_ban(MyceliumPlayer_o* player)
{

	std::cout << "(local ban) attempted to ban " << il2cpp::to_string(player->fields._name) << "\n";

	return;

}

bool __cdecl hk_is_banned(Steamworks_CSteamID_o player)
{

	//std::cout << "(is banned) checking if " << player.fields.m_SteamID << " is banned.\n";
	return false;

}

void __cdecl hk_ban_handler(Message_o* message, MyceliumPlayer_o* sender)
{

	std::cout << "got ban request from " << il2cpp::to_string(sender->fields._name) << " | content " << message->fields._Header_k__BackingField << "\n";

	return;

}

void __cdecl hk_bullet_hit(PlayerController_o* thisptr, UnityEngine_Vector3_o point, UnityEngine_Vector3_o normal)
{

	features::hits.push_back({point, GetTickCount64()});

	return reinterpret_cast<decltype(&hk_bullet_hit)>(hooks::hooks[1].original_function)(thisptr, point, normal);

}

bool __cdecl hk_get_grounded(PlayerController_o* thisptr, bool grounded, UnityEngine_Transform_o* transform)
{

	return reinterpret_cast<decltype(&hk_get_grounded)>(hooks::hooks[6].original_function)(thisptr, true, transform);

}

void __cdecl hk_fire_local(PlayerController_o* thisptr, UnityEngine_Vector3_array* bullet_directions)
{

	return reinterpret_cast<decltype(&hk_fire_local)>(hooks::hooks[7].original_function)(thisptr, bullet_directions);

}

bool __cdecl hk_has_dlc(uint32_t dlc)
{
	std::cout << "returning true for having " << dlc << " dlc!\n";
	return true;
}

void __cdecl hk_crash(void* thisptr)
{
	return;
}

void __cdecl hk_take_action(MyceliumPlayer_o* hacker, System_String_o* code)
{

	std::cout << "prevented action: " << il2cpp::to_string(code) << " on " << il2cpp::to_string(hacker->fields._name) << "\n";
	return;

}

void hooks::init()
{

	if (!hook_function(il2cpp::get_method("", "PlayerController", "Update")->methodPointer, hk_controller_update))
		exit(0);

	if (!hook_function(il2cpp::get_method("", "PlayerController", "InstantiateBulletHit")->methodPointer, hk_bullet_hit))
		exit(0);

	if (!hook_function(il2cpp::get_method("", "MyceliumBanManager", "LocalBan")->methodPointer, hk_local_ban))
		exit(0);

	if (!hook_function(il2cpp::get_method("", "MyceliumBanManager", "Ban")->methodPointer, hk_ban))
		exit(0);

	if (!hook_function(il2cpp::get_method("", "MyceliumBanManager", "IsBanned")->methodPointer, hk_is_banned))
		exit(0);

	if (!hook_function(il2cpp::get_method("", "MyceliumBanManager", "MessageHandler_RequestBan")->methodPointer, hk_ban_handler))
		exit(0);

	if (!hook_function(il2cpp::get_method("", "SayNoToHX", "Crash")->methodPointer, hk_crash))
		exit(0);

	if (!hook_function(il2cpp::get_method("", "AntiCheat", "TakeAction")->methodPointer, hk_take_action))
		exit(0);

}