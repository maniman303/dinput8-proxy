#include "pch.h"
#include "dinput8.h"
#include "logger.h"
#include "string_helper.h"
#include "dai_workaround.h"
#include <filesystem>
#include <dinput.h>
#include <olectl.h>

HMODULE dinput8_dll;

#define WRAPPER_FUNC(name) o##name = GetProcAddress(dinput8_dll, ###name)

#ifdef _M_AMD64
#pragma warning (disable: 4081)
#define STRINGIFY(name) #name
#define EXPORT_FUNCTION comment(linker, "/EXPORT:" __FUNCTION__ "=" __FUNCDNAME__)
#define WRAPPER_GENFUNC(name) \
    FARPROC o##name; \
    __declspec(dllexport) void WINAPI _##name() \
    { \
        __pragma(STRINGIFY(EXPORT_FUNCTION)); \
        o##name(); \
    }
#else
#define WRAPPER_GENFUNC(name) \
	FARPROC o##name; \
	__declspec(naked) void _##name() \
	{ \
		__asm jmp[o##name] \
	}
#endif

WRAPPER_GENFUNC(DirectInput8Create);
WRAPPER_GENFUNC(DllCanUnloadNow);
WRAPPER_GENFUNC(DllGetClassObject);
WRAPPER_GENFUNC(DllRegisterServer);
WRAPPER_GENFUNC(DllUnregisterServer);
WRAPPER_GENFUNC(GetdfDIJoystick);

static bool load_dinput8() {
	char systemPath[MAX_PATH];

	if (!GetSystemDirectoryA(systemPath, MAX_PATH)) {
		logger::info("Could not access System32 path.");
	}

	strcat_s(systemPath, "\\dinput8.dll");

	dinput8_dll = LoadLibraryA(systemPath);

	if (!dinput8_dll) {
		logger::info("Could not load org 'dinput8.dll' from '" + std::string(systemPath) + "'");

		char systemPathWow64[MAX_PATH];

		if (!GetSystemWow64DirectoryA(systemPathWow64, MAX_PATH)) {
			logger::info("Could not access SysWOW64 path.");
		}

		strcat_s(systemPathWow64, "\\dinput8.dll");

		dinput8_dll = LoadLibraryA(systemPathWow64);

		if (!dinput8_dll) {
			logger::info("Could not load org 'dinput8.dll' from '" + std::string(systemPathWow64) + "'");

			return false;
		}
	}

	WRAPPER_FUNC(DirectInput8Create);
	WRAPPER_FUNC(DllCanUnloadNow);
	WRAPPER_FUNC(DllGetClassObject);
	WRAPPER_FUNC(DllRegisterServer);
	WRAPPER_FUNC(DllUnregisterServer);
	WRAPPER_FUNC(GetdfDIJoystick);

	return true;
}

DWORD WINAPI load(LPVOID lpParam) {
	logger::init();

	if (!load_dinput8())
	{
		return 0;
	}

	auto path = std::filesystem::current_path();
	path.append("crypthook.dll");

	if (std::filesystem::exists(path))
	{
		logger::info("Loaded crypthook bypass.");
		LoadLibrary(L"crypthook");
	}
	else
	{
		logger::info("Could not find crypthook.dll file.");
	}

	char buf[MAX_PATH];
	GetModuleFileNameA(nullptr, buf, MAX_PATH);
	auto procFullPathString = std::string(buf);
	auto procPath = std::filesystem::path(procFullPathString);
	auto procFilename = procPath.filename().string();

	logger::info("Executable name: '" + procFilename + "'.");
	logger::info("Executable full path: '" + procFullPathString + "'.");

	if (to_lower(procFilename).compare("dragonageinquisition.exe") == 0)
	{
		daiworkaround::init();
	}

	return 0;
}