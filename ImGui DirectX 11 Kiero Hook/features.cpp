#include "features.h"

void features::no_spread()
{

	if (config::enable_no_spread && globals::local_controller)
		globals::local_controller->fields.bulletSpread = 0.f;

	/*if (globals::local_controller->fields.cam)*/

	if (globals::local_controller)
		globals::local_controller->fields.grounded = true;

}

void features::infinite_ammo()
{

	if (config::enable_infinite_ammo && globals::local_controller)
	{

		if (globals::local_controller->fields.items)
		{

			for (int i = 0; i < globals::local_controller->fields.items->max_length; i++)
			{

				auto item = globals::local_controller->fields.items->m_Items[i];

				if (!item)
					continue;

				item->fields.ammo.fields.offset = 0;
				item->fields.totalAmmo.fields.offset = 0;

				item->fields.ammo.fields.value = 9999;
				item->fields.totalAmmo.fields.value = 9999;

			}

		}

	}

}

void features::rapid_fire()
{

	if (globals::local_controller && config::enable_rapid_fire)
	{

		if (globals::local_controller->fields.items)
		{

			for (int i = 0; i < globals::local_controller->fields.items->max_length; i++)
			{

				auto item = globals::local_controller->fields.items->m_Items[i];

				if (!item)
					continue;

				item->fields.info->fields.useDelay = 0.1f;

			}

		}

	}

}

void MoveMouse(int x, int y, int width, int height)
{

	float ScreenCenterX = 1920 / 2;
	float ScreenCenterY = 1080 / 2;

	float TargetX = 0;
	float TargetY = 0;

	if (x != 0)
	{
		if (x > ScreenCenterX)
		{
			TargetX = -(ScreenCenterX - x);
			if (config::enable_smoothing)
				TargetX /= config::aimbot_smoothness;
			if (TargetX + ScreenCenterX > ScreenCenterX * 2) TargetX = 0;
		}

		if (x < ScreenCenterX)
		{
			TargetX = x - ScreenCenterX;
			if (config::enable_smoothing)
				TargetX /= config::aimbot_smoothness;
			if (TargetX + ScreenCenterX < 0) TargetX = 0;
		}
	}

	if (y != 0)
	{
		if (y > ScreenCenterY)
		{
			TargetY = -(ScreenCenterY - y);
			if (config::enable_smoothing)
				TargetY /= config::aimbot_smoothness;
			if (TargetY + ScreenCenterY > ScreenCenterY * 2) TargetY = 0;
		}

		if (y < ScreenCenterY)
		{
			TargetY = y - ScreenCenterY;
			if (config::enable_smoothing)
				TargetY /= config::aimbot_smoothness;
			if (TargetY + ScreenCenterY < 0) TargetY = 0;
		}
	}

	mouse_event(MOUSEEVENTF_MOVE, static_cast<DWORD>(TargetX), static_cast<DWORD>(TargetY), NULL, NULL);

}


void features::aimbot()
{

	struct target
	{
		float fov;
		PlayerController_o* player;
		ImVec2 pos;
		UnityEngine_Vector3_o world_pos;
	};

	auto calc_fov = [&](ImVec2 pos)
	{
		static auto screen_center = ImVec2(ImGui::GetIO().DisplaySize.x / 2.f, ImGui::GetIO().DisplaySize.y / 2.f);
		return pos.distance(screen_center);
	};

	if (globals::local_controller && config::enable_aimbot && config::aimbot_keybind.enabled)
	{

		std::vector<target> targets;

		for (const auto& player : globals::players)
		{

			ImVec2 screen;

			if (!il2cpp::world_to_screen(player->fields.lastNonLocalPos, screen))
				continue;

			targets.push_back({calc_fov(screen), player, screen, player->fields.lastNonLocalPos});

		}

		if (targets.size() > 0)
		{

			auto compare_target = [&](target t1, target t2)
			{
				return t1.fov < t2.fov;
			};

			std::sort(targets.begin(), targets.end(), compare_target);

			target target = targets.at(0);
			static auto screen_center = ImVec2(ImGui::GetIO().DisplaySize.x / 2.f, ImGui::GetIO().DisplaySize.y / 2.f);

			if (target.fov > config::aimbot_fov)
				return;

			MoveMouse(target.pos.x, target.pos.y, screen_center.x, screen_center.y);

			if (config::aimbot_type == 1)
			{
				Sleep(20);
				MoveMouse(-target.pos.x, -target.pos.y, screen_center.x, screen_center.y);
			}

			aimbot_pos = target.world_pos;

		}

	}

}