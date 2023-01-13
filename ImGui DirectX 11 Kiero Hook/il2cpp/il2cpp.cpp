#include "il2cpp.h"

#define ASSERT(x) { MessageBoxA(NULL, NULL, x, NULL); exit(0); }

bool il2cpp::sanity()
{

	if (!gameassembly)
		ASSERT("Failed to get GameAssembly.dll.")
	if (!resolve_call)
		ASSERT("Failed to get resolve call.")
	if (!get_domain)
		ASSERT("Failed to get get domain.")
	if (!thread_attach)
		ASSERT("Failed to get thread attach")

	size_t assembly_count = 0;
	auto assemblies = get_assemblies(get_domain(), &assembly_count);

	for (int i = 0; i < assembly_count; i++)
	{

		if (!assemblies[i])
			continue;

		assembly_pair assembly{};
		assembly.name = assemblies[i]->m_aName.m_pName;
		assembly.assembly = assemblies[i];

		if (std::string(assembly.name) == "Assembly-CSharp")
			il2cpp::assembly = assemblies[i];

		std::cout << "il2cpp | found assembly: " << assembly.name << "\n";

		il2cpp::assemblies.push_back(assembly);

	}

	if (!assembly)
		ASSERT("Failed to find assembly.")

	return true;

}