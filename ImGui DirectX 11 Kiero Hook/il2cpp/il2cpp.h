#pragma once

#include <iostream>
#include <windows.h>
#include <vector>
#include <string>

#include "../imgui/imgui.h"
#include "../il2cpp.h"
#include "structures.h"

namespace il2cpp
{

	struct assembly_pair
	{
		const char* name;
		il2cppAssembly* assembly;
	};

	struct Vector3
	{
		float x, y, z;
		Vector3(UnityEngine_Vector3_o ptr) { this->x = ptr.fields.x; this->y = ptr.fields.y; this->z = ptr.fields.z; };
		Vector3(float _x, float _y, float _z) { this->x = _x; this->y = _y; this->z = _z; };
	};

	inline uintptr_t gameassembly = (uintptr_t)GetModuleHandleA("GameAssembly.dll");
	inline il2cppAssembly* assembly = nullptr;

	inline std::vector<assembly_pair> assemblies{};
	
	template <class t>
	t resolve_export(const char* name)
	{

		return reinterpret_cast<t>(GetProcAddress((HMODULE)gameassembly, name));

	}

	inline auto resolve_call = resolve_export<uintptr_t(__fastcall*)(const char*)>("il2cpp_resolve_icall");
	inline auto get_domain = resolve_export<void*(__fastcall*)(void)>("il2cpp_domain_get");
	inline auto thread_attach = resolve_export<void*(__fastcall*)(void*)>("il2cpp_thread_attach");
	inline auto get_assemblies = resolve_export<il2cppAssembly**(__fastcall*)(void*, size_t*)>("il2cpp_domain_get_assemblies");
	inline auto class_from_name = resolve_export<il2cppClass*(__fastcall*)(il2cppImage*, const char*, const char*)>("il2cpp_class_from_name");
	inline auto method_from_name = resolve_export<MethodInfo*(__fastcall*)(il2cppClass*, const char*, int)>("il2cpp_class_get_method_from_name");
	inline auto field_from_name = resolve_export<il2cppFieldInfo*(__fastcall*)(il2cppClass*, const char*)>("il2cpp_class_get_field_from_name");
	inline auto static_field_get_value = resolve_export<void(__fastcall*)(il2cppFieldInfo*, void*)>("il2cpp_field_static_get_value");
	inline auto static_field_set_value = resolve_export<void(__fastcall*)(il2cppFieldInfo*, void*)>("il2cpp_field_static_set_value");

	template <typename ret, typename... _args>
	inline ret call_function(MethodInfo* method, _args... args)
	{
		typedef ret(*func)(_args...);
		func fn = (func)((void*)method->methodPointer);
		return fn(args...);
	}

	inline il2cppClass* find_class(const char* namespxce, const char* class_name)
	{
		return class_from_name(assembly->m_pImage, namespxce, class_name);
	}

	inline il2cppClass* find_class(il2cppAssembly* assembly, const char* namespxce, const char* class_name)
	{
		return class_from_name(assembly->m_pImage, namespxce, class_name);
	}

	inline MethodInfo* get_fn_ptr(il2cppClass* klass, const char* _method, int args_count = 0)
	{

		return method_from_name(klass, _method, args_count);

	}

	inline MethodInfo* get_method(const char* namespxce, const char* class_name, const char* method)
	{

		auto klass = find_class(namespxce, class_name);
		
		for (int i = 0; i < 16; i++)
		{

			auto info = method_from_name(klass, method, i);

			if (!info)
				continue;

			return info;

		}

		return nullptr;

	}

	inline MethodInfo* get_method(il2cppAssembly* assembly, const char* namespxce, const char* class_name, const char* method)
	{

		auto klass = find_class(assembly, namespxce, class_name);

		for (int i = 0; i < 16; i++)
		{

			auto info = method_from_name(klass, method, i);

			if (!info)
				continue;

			return info;

		}

		return nullptr;

	}

	inline std::string to_string(System_String_o* str)
	{

		if (!str)
			return "NULL";
		else
		{
			auto wstr = std::wstring((wchar_t*)(&str->fields.m_firstChar));
			return std::string(wstr.begin(), wstr.end());
		}

	}

	template <class type>
	inline type get_static_field_value(il2cppClass* klass, const char* field_name)
	{

		auto field = field_from_name(klass, field_name);

		type buffer;
		static_field_get_value(field, &buffer);
		return buffer;

	}

	template <class type>
	void set_static_field_value(il2cppClass* klass, const char* field_name, type val)
	{

		auto field = field_from_name(klass, field_name);
		static_field_set_value(field, &val);

	}

	inline UnityEngine_Camera_o* camera_get_main()
	{

		static auto camera_get_main = reinterpret_cast<UnityEngine_Camera_o*(__fastcall*)(void)>(resolve_call("UnityEngine.Camera::get_main()"));
		return camera_get_main();

	}

	inline bool world_to_screen(Vector3 world, ImVec2& out)
	{

		static il2cppAssembly* coremodule = nullptr;

		if (!coremodule)
			for (const auto& assembly : assemblies)
				if (std::string(assembly.name) == "UnityEngine.CoreModule")
					coremodule = assembly.assembly;

		static auto world_to_screen_point = reinterpret_cast<Vector3(__cdecl*)(UnityEngine_Camera_o*, Vector3)>(get_method(coremodule, "UnityEngine", "Camera", "WorldToScreenPoint")->methodPointer);

		UnityEngine_Camera_o* camera = camera_get_main();

		if (!camera)
			return false;

		Vector3 pos = world_to_screen_point(camera, world);

		if (pos.z < 0.f)
			return false;

		ImVec2 screen_size = ImGui::GetIO().DisplaySize;
		out = ImVec2{ pos.x, screen_size.y - pos.y };

		return true;

	}

	inline void transform_look_at(UnityEngine_Transform_o* transform, UnityEngine_Vector3_o pos)
	{

		static il2cppAssembly* coremodule = nullptr;

		if (!coremodule)
			for (const auto& assembly : assemblies)
				if (std::string(assembly.name) == "UnityEngine.CoreModule")
					coremodule = assembly.assembly;

		static auto transform_look_at = reinterpret_cast<void(__cdecl*)(UnityEngine_Transform_o*, UnityEngine_Vector3_o)>(il2cpp::gameassembly + 0xDDE2B0);

		return transform_look_at(transform, pos);

	}

	inline UnityEngine_Vector3_o rigidbody_get_velocity(UnityEngine_Rigidbody_o* body)
	{

		static il2cppAssembly* physicsmodule = nullptr;

		if (!physicsmodule)
			for (const auto& assembly : assemblies)
				if (std::string(assembly.name) == "UnityEngine.PhysicsModule")
					physicsmodule = assembly.assembly;

		static auto rigidbody_get_velocity = reinterpret_cast<UnityEngine_Vector3_o(__cdecl*)(UnityEngine_Rigidbody_o*)>(il2cpp::get_method(physicsmodule, "UnityEngine", "RigidBody", "get_velocity")->methodPointer);

		return rigidbody_get_velocity(body);

	}

	bool sanity();

}