#include <shlobj.h>

typedef HRESULT(*fn_DirectInput8Create)(HINSTANCE hinst, DWORD dwVersion, REFIID riidltf, LPVOID* ppvOut, LPUNKNOWN punkOuter);
typedef HRESULT(*fn_DllCanUnloadNow)();
typedef HRESULT(*fn_DllGetClassObject)(REFCLSID rclsid, REFIID riid, LPVOID *ppv);
typedef HRESULT(*fn_DllRegisterServer)();
typedef HRESULT(*fn_DllUnregisterServer)();

fn_DirectInput8Create _DirectInput8Create;
fn_DllCanUnloadNow _DllCanUnloadNow;
fn_DllGetClassObject _DllGetClassObject;
fn_DllRegisterServer _DllRegisterServer;
fn_DllUnregisterServer _DllUnregisterServer;

#ifdef __cplusplus
extern "C" {
#endif

HRESULT DirectInput8Create(HINSTANCE hinst, DWORD dwVersion, REFIID riidltf, LPVOID* ppvOut, LPUNKNOWN punkOuter) { return _DirectInput8Create(hinst, dwVersion, riidltf, ppvOut, punkOuter); }
HRESULT DllRegisterServer() { return _DllRegisterServer(); }
HRESULT DllUnregisterServer() { return _DllUnregisterServer(); }
HRESULT DllCanUnloadNow() { return _DllCanUnloadNow(); }
#if __x86_64__
HRESULT DllGetClassObject(REFCLSID rclsid, REFIID riid, LPVOID *ppv) { return _DllGetClassObject(rclsid, riid, ppv); }
#else
STDAPI DllGetClassObject(REFCLSID rclsid, REFIID riid, LPVOID *ppv) { return _DllGetClassObject(rclsid, riid, ppv); }
#endif

#ifdef __cplusplus
}
#endif

char inited = 0;
BOOL WINAPI DllMain(HMODULE mod, DWORD cause, void *ctx)
{
	if (cause != DLL_PROCESS_ATTACH || inited == 1) {
		return TRUE;
	}
	inited = 1;

	char buf[MAX_PATH];
	PWSTR szSystemPath;

#ifdef __cplusplus
	SHGetKnownFolderPath(FOLDERID_System, 0, 0, &szSystemPath);
#else
	SHGetKnownFolderPath(&FOLDERID_System, 0, 0, &szSystemPath);
#endif
	wcstombs(buf, szSystemPath, MAX_PATH);
	strcat(buf, "\\dinput8.dll");

	HMODULE original = LoadLibraryA(buf);
	_DllCanUnloadNow = (fn_DllCanUnloadNow)GetProcAddress(original, "DllCanUnloadNow");
	_DllGetClassObject = (fn_DllGetClassObject)GetProcAddress(original, "DllGetClassObject");
	_DllRegisterServer = (fn_DllRegisterServer)GetProcAddress(original, "DllRegisterServer");
	_DllUnregisterServer = (fn_DllUnregisterServer)GetProcAddress(original, "DllUnregisterServer");
	_DirectInput8Create = (fn_DirectInput8Create)GetProcAddress(original, "DirectInput8Create");

	wchar_t baseDir[MAX_PATH];
	GetModuleFileNameW(0, baseDir, MAX_PATH);
	*wcsrchr(baseDir, '\\') = 0;
	SetCurrentDirectoryW(baseDir);

	WIN32_FIND_DATAW fd;
	HANDLE file = FindFirstFileW(L"plugins\\*.dll", &fd);
	if (file == INVALID_HANDLE_VALUE) return TRUE;

	do
	{
		if (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) continue;
		wchar_t filePath[MAX_PATH];
		wcscpy(filePath, baseDir);
		wcscat(filePath, L"\\plugins\\");
		wcscat(filePath, fd.cFileName);
		if (LoadLibraryW(filePath) == 0)
		{
			wcstombs(buf, fd.cFileName, MAX_PATH);
			MessageBoxA(0, strcat((char*)"Failed to load ", buf), "", MB_ICONERROR);
		}
	} while (FindNextFileW(file, &fd));

	return TRUE;
}
