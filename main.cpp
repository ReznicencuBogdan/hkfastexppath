// Win32.cpp : Defines the entry point for the application.
//

#include <iostream>
#include <vector>
#include <algorithm>
#include <stdlib.h>
#include <stdio.h>

#include <shlobj.h>
#include <exdisp.h>
#include <shlwapi.h>
#include <comutil.h> // _bstr_t 
#include <CommCtrl.h>
#include <wininet.h> // CoInternetParseUrl

#pragma comment(lib, "Urlmon.lib")
#pragma comment(lib, "shlwapi.lib")
#pragma comment(lib, "comsuppwd.lib")
#pragma comment(lib, "ole32.lib")
#pragma comment(lib, "shell32.lib")
#pragma comment(lib, "propsys.lib")


class SHELLDATA
{
public:
	using WSTRING = std::wstring;
	SHELLDATA(HWND _WINDOW, WSTRING&& _PATH)
	 :
	 	WINDOW{_WINDOW},
	 	PATH{std::move(_PATH)} 
	{}
public:
	HWND WINDOW;
	WSTRING PATH;
};


extern "C" __declspec(dllexport) 
int 
WINAPI
WriteLastExplorerPathBuffer(wchar_t *output_buffer, int max_byte_count)
{
	IShellWindows* pIShellWindows;
	int max_wide_count = max_byte_count / 2;

	if (!SUCCEEDED(CoCreateInstance(CLSID_ShellWindows, NULL, CLSCTX_ALL, IID_IShellWindows, (void**)&pIShellWindows)))
	{
		return 0;
	}

	IDispatch* pIDispatch;
	VARIANT v;
	V_VT(&v) = VT_I4;

	HWND hwndShell = HWND_DESKTOP;

	//Bottom -> TopMost
	std::vector<SHELLDATA> listOfActiveShells;
	listOfActiveShells.reserve(20);

	for (V_I4(&v) = 0; pIShellWindows->Item(v, &pIDispatch) == S_OK; V_I4(&v)++)
	{
		IWebBrowserApp* pIWebBrowserApp;
		if (SUCCEEDED(pIDispatch->QueryInterface(IID_IWebBrowserApp, (void**)&pIWebBrowserApp)))
		{
			if (SUCCEEDED(pIWebBrowserApp->get_HWND((LONG_PTR*)&hwndShell)))
			{
				if( /*IsIconic(hwndShell) ||*/ !IsWindowVisible(hwndShell) )
				{
					continue;
				}

				BSTR bstrPath;
				pIWebBrowserApp->get_LocationURL(&bstrPath);

				WCHAR szDecodedUrl[INTERNET_MAX_URL_LENGTH];
				DWORD cchDecodedUrl = INTERNET_MAX_URL_LENGTH;
				PWCHAR pwBstrPath = _bstr_t(bstrPath);

				HRESULT hr = CoInternetParseUrl(
					pwBstrPath, 
					PARSE_PATH_FROM_URL,
					URL_UNESCAPE,
					szDecodedUrl, 
					INTERNET_MAX_URL_LENGTH,
					&cchDecodedUrl,
					0
				);

				SysFreeString(bstrPath);

				if (!SUCCEEDED(hr) || wcslen(szDecodedUrl) < 1)
				{
			    	continue;
			    }

				//
				listOfActiveShells.emplace_back(hwndShell, std::wstring(szDecodedUrl));
			}
			pIWebBrowserApp->Release();
		}
		pIDispatch->Release();
	}
	pIShellWindows->Release();


	//Iterate through windows TopMost -> Bottom
	HWND window = GetTopWindow(GetDesktopWindow());

	if (listOfActiveShells.size() == 0)
	{
		return 0;
	}

	do
	{
		const auto& it = std::find_if(std::begin(listOfActiveShells), std::end(listOfActiveShells), 
			[&window](const SHELLDATA& item)
			{
				return item.WINDOW == window;
			});

		if(it != std::end(listOfActiveShells))
		{
			if(it->PATH.length() >= max_wide_count + 3)
			{
				return 0;
			}

			if (std::swprintf(output_buffer, max_wide_count, L"\"%s\"", it->PATH.c_str()) < 0)
			{
				return 0;
			}

		   return 1;
		}
	}
	while (window = GetWindow(window, GW_HWNDNEXT));

	return 0;
}


BOOL WINAPI DllMain(
    HINSTANCE hinstDLL,  // handle to DLL module
    DWORD fdwReason,     // reason for calling function
    LPVOID lpReserved )  // reserved
{
    // Perform actions based on the reason for calling.
    switch( fdwReason ) 
    { 
        case DLL_PROCESS_ATTACH:
         CoInitializeEx(NULL, COINIT_MULTITHREADED); // not recommened ?
         DisableThreadLibraryCalls(hinstDLL);
         // Initialize once for each new process.
         // Return FALSE to fail DLL load.
            break;

        case DLL_THREAD_ATTACH:
         // Do thread-specific initialization.
            break;

        case DLL_THREAD_DETACH:
         // Do thread-specific cleanup.
            break;

        case DLL_PROCESS_DETACH:
         // Perform any necessary cleanup.
            break;
    }
    return TRUE;  // Successful DLL_PROCESS_ATTACH.
}



/*
* cmake --build . --config Release
*/