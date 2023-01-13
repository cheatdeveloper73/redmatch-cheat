#include "includes.h"
#include "hooks/hooks.h"
#include "il2cpp/il2cpp.h"
#include "globals.h"
#include "features.h"
#include "menu/menu.h"
extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

Present oPresent;
HWND window = NULL;
WNDPROC oWndProc;
ID3D11Device* pDevice = NULL;
ID3D11DeviceContext* pContext = NULL;
ID3D11RenderTargetView* mainRenderTargetView;

void InitImGui()
{
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags = ImGuiConfigFlags_NoMouseCursorChange;
	ImGui_ImplWin32_Init(window);
	ImGui_ImplDX11_Init(pDevice, pContext);
}

LRESULT __stdcall WndProc(const HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {

	if (true && ImGui_ImplWin32_WndProcHandler(hWnd, uMsg, wParam, lParam))
		return true;

	return CallWindowProc(oWndProc, hWnd, uMsg, wParam, lParam);
}

bool init = false;
HRESULT __stdcall hkPresent(IDXGISwapChain* pSwapChain, UINT SyncInterval, UINT Flags)
{
	if (!init)
	{
		if (SUCCEEDED(pSwapChain->GetDevice(__uuidof(ID3D11Device), (void**)&pDevice)))
		{
			pDevice->GetImmediateContext(&pContext);
			DXGI_SWAP_CHAIN_DESC sd;
			pSwapChain->GetDesc(&sd);
			window = sd.OutputWindow;
			ID3D11Texture2D* pBackBuffer;
			pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer);
			pDevice->CreateRenderTargetView(pBackBuffer, NULL, &mainRenderTargetView);
			pBackBuffer->Release();
			oWndProc = (WNDPROC)SetWindowLongPtr(window, GWLP_WNDPROC, (LONG_PTR)WndProc);
			InitImGui();
			menu.Initialize();
			init = true;
		}

		else
			return oPresent(pSwapChain, SyncInterval, Flags);
	}

	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	globals::local_controller = il2cpp::get_static_field_value<PlayerController_o*>(il2cpp::find_class("", "PlayerController"), "LocalInstance");

	if (GetAsyncKeyState(VK_INSERT) & 1)
		menu.opened = !menu.opened;

	if (globals::local_controller)
	{
		globals::local_controller->fields.antiCheat_lastFireTime = -1.f;
		globals::local_controller->fields.fireTimeDifference->fields._average_k__BackingField = 999.f;
		globals::local_controller->fields.fireTimeDifference->fields.sampleAccumulator = 999.f;
		auto samples = globals::local_controller->fields.fireTimeDifference->fields.samples;

		for (int i = 0; i < samples->fields._size; i++)
		{

			samples->fields._array->m_Items[i] = 999.f;

		}

	}

	menu.Render();

	features::no_spread();
	features::infinite_ammo();
	features::rapid_fire();

	globals::player_sync.lock();

	for (const auto& player : globals::players)
	{

		if (!player)
			continue;

		ImVec2 screen;

		if (!il2cpp::world_to_screen(player->fields.lastNonLocalPos, screen))
			continue;

		ImGui::GetBackgroundDrawList()->AddText(screen, IM_COL32_WHITE, il2cpp::to_string(player->fields._identity_k__BackingField->fields._Owner_k__BackingField->fields._name).c_str());

	}

	features::aimbot();

	globals::players.clear();

	globals::player_sync.unlock();

	if (config::show_fov)
		ImGui::GetBackgroundDrawList()->AddCircle(ImVec2(ImGui::GetIO().DisplaySize.x / 2.f, ImGui::GetIO().DisplaySize.y / 2.f), config::aimbot_fov, IM_COL32_WHITE);

	if (features::hits.size() > 0)
	{

		for (int i = 0; i < features::hits.size(); i++)
		{

			auto hit = features::hits.at(i);

			ImVec2 screen;

			if (!il2cpp::world_to_screen(hit.pos, screen))
			{
				features::hits.erase(features::hits.begin() + i);
				continue;
			}

			ImGui::GetBackgroundDrawList()->AddCircleFilled(screen, 3.f, IM_COL32(255, 0, 0, (GetTickCount64() - hit.time) * 255.f));

			if (hit.time + 1250.f < GetTickCount64())
				features::hits.erase(features::hits.begin() + i);

		}

	}

	ImGui::Render();

	pContext->OMSetRenderTargets(1, &mainRenderTargetView, NULL);
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
	return oPresent(pSwapChain, SyncInterval, Flags);
}

DWORD WINAPI MainThread(LPVOID lpReserved)
{
	bool init_hook = false;
	do
	{
		if (kiero::init(kiero::RenderType::D3D11) == kiero::Status::Success)
		{
			AllocConsole();
			SetConsoleTitleA("redhook (rellant.dev)");
			FILE* file;
			freopen_s(&file, "CONOUT$", "w", stdout);
			il2cpp::sanity();
			hooks::init();
			kiero::bind(8, (void**)& oPresent, hkPresent);
			std::cout << "initialized\n";
			init_hook = true;
		}
	} while (!init_hook);
	return TRUE;
}

BOOL WINAPI DllMain(HMODULE hMod, DWORD dwReason, LPVOID lpReserved)
{
	switch (dwReason)
	{
	case DLL_PROCESS_ATTACH:
		DisableThreadLibraryCalls(hMod);
		CreateThread(nullptr, 0, MainThread, hMod, 0, nullptr);
		break;
	case DLL_PROCESS_DETACH:
		kiero::shutdown();
		break;
	}
	return TRUE;
}