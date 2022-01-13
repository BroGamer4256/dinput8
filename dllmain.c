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

void LoadOriginalLibrary(HMODULE dll)
{
	_DllCanUnloadNow = (fn_DllCanUnloadNow)GetProcAddress(dll, "DllCanUnloadNow");
	_DllGetClassObject = (fn_DllGetClassObject)GetProcAddress(dll, "DllGetClassObject");
	_DllRegisterServer = (fn_DllRegisterServer)GetProcAddress(dll, "DllRegisterServer");
	_DllUnregisterServer = (fn_DllUnregisterServer)GetProcAddress(dll, "DllUnregisterServer");
	_DirectInput8Create = (fn_DirectInput8Create)GetProcAddress(dll, "DirectInput8Create");
}

#ifdef __cplusplus
extern "C" {
#endif

HRESULT DirectInput8Create(HINSTANCE hinst, DWORD dwVersion, REFIID riidltf, LPVOID* ppvOut, LPUNKNOWN punkOuter) { return _DirectInput8Create(hinst, dwVersion, riidltf, ppvOut, punkOuter); }
HRESULT DllRegisterServer() { return _DllRegisterServer(); }
HRESULT DllUnregisterServer() { return _DllUnregisterServer(); }
HRESULT DllCanUnloadNow() { return _DllCanUnloadNow(); }
HRESULT DllGetClassObject(REFCLSID rclsid, REFIID riid, LPVOID *ppv) { return _DllGetClassObject(rclsid, riid, ppv); }

#ifdef __cplusplus
}
#endif

static LONG OriginalLibraryLoaded = 0;
void Init()
{
	char buf[MAX_PATH];
	PWSTR szSystemPath;

	if (_InterlockedCompareExchange(&OriginalLibraryLoaded, 1, 0) != 0) return;
#ifdef __cplusplus
	SHGetKnownFolderPath(FOLDERID_System, 0, 0, &szSystemPath);
#else
    SHGetKnownFolderPath(&FOLDERID_System, 0, 0, &szSystemPath);
#endif
	wcstombs(buf, szSystemPath, MAX_PATH);
	strcat(buf, "\\dinput8.dll");

	LoadOriginalLibrary(LoadLibraryA(buf));
}

void Load()
{
	char originalDir[MAX_PATH];
	GetCurrentDirectoryA(MAX_PATH, originalDir);

	SetCurrentDirectoryA("plugins\\");
	WIN32_FIND_DATAA fd;
	HANDLE file = FindFirstFileA("*.dva", &fd);
	char dir[MAX_PATH];
	GetCurrentDirectoryA(MAX_PATH, dir);
	strcat(dir, "\\");
	if (file == INVALID_HANDLE_VALUE) return;

	do
	{
		if (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) continue;
		char filePath[MAX_PATH];
		strcpy(filePath, dir);

		char* dot = strrchr(fd.cFileName, '.');
		if (dot && !strcmp(dot, ".dva"))
		{
			strcat(filePath, fd.cFileName);
			if (LoadLibraryA(filePath) == 0)
			{
				MessageBoxA(0, strcat((char*)"Failed to load ", fd.cFileName), "", MB_ICONERROR);
			}
		}
	} while (FindNextFileA(file, &fd));
	SetCurrentDirectoryA(originalDir);
}

BOOL WINAPI DllMain(HMODULE mod, DWORD cause, void *ctx)
{
	if (cause != DLL_PROCESS_ATTACH) {
		return TRUE;
	}
	Init();
	Load();

	return TRUE;
}
