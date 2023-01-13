#pragma once

#include "../il2cpp/il2cpp.h"
#include "../il2cpp.h"

#include "../kiero/minhook/include/MinHook.h"

#include <windows.h>
#include <vector>

struct hooked_function
{
	uintptr_t original_function;
	void* target_address;
	void* callback_function;
};

namespace hooks
{

	inline std::vector<hooked_function> hooks;

	void init();
	bool hook_function(void* function_pointer, void* callback_function);

}