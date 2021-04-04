// WarsmashWrapper.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "pch.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <wchar.h>
#include <iostream>
#include <Windows.h>

int WINAPI WinMain(
	HINSTANCE hInstance,     /* [input] handle to current instance */
	HINSTANCE hPrevInstance, /* [input] handle to previous instance */
	LPSTR lpCmdLine,         /* [input] pointer to command line */
	int nCmdShow             /* [input] show state of window */)
{
	// make sure we run from the RMS directory
	wchar_t myCmdLine[MAX_PATH];
	GetModuleFileName(NULL, myCmdLine, MAX_PATH);
	if (lpCmdLine != NULL) {
		//
		LPWSTR lpCmdDirectory = _wcsdup(myCmdLine);
		LPWSTR lpLastSlash = wcsrchr(lpCmdDirectory, '\\');
		if (lpLastSlash != NULL) {
			*lpLastSlash = '\0';
		}
		SetCurrentDirectory(lpCmdDirectory);
		free(lpCmdDirectory);
	}

	// additional information
	STARTUPINFO si;
	PROCESS_INFORMATION pi;

	// set the size of the structures
	ZeroMemory(&si, sizeof(si));
	si.cb = sizeof(si);
	ZeroMemory(&pi, sizeof(pi));

	wchar_t cmdArgs[MAX_PATH];
	if (lpCmdLine != NULL) {
		WCHAR lpwCmdLine[MAX_PATH];
		MultiByteToWideChar(CP_ACP, 0, lpCmdLine, -1, lpwCmdLine, MAX_PATH);
		wsprintf(cmdArgs, L"jre\\bin\\java.exe -jar warsmash.jar %s", lpwCmdLine);
	}
	else {
		wchar_t cmdArgsBase[] = L"jre\\bin\\java.exe -jar warsmash.jar";
		if (wcsncpy_s(cmdArgs, MAX_PATH, cmdArgsBase, MAX_PATH)) {
			std::cout << "Unable to generate fowarded argument list.\n";
			return FALSE;
		}
	}


	// Start the child process. 
	BOOL result = CreateProcess(TEXT("jre\\bin\\java.exe"),   // No module name (use command line)
		cmdArgs,        // Command line
		NULL,           // Process handle not inheritable
		NULL,           // Thread handle not inheritable
		FALSE,          // Set handle inheritance to FALSE
		NORMAL_PRIORITY_CLASS | CREATE_NO_WINDOW,              // No creation flags
		NULL,           // Use parent's environment block
		NULL,           // Use parent's starting directory 
		&si,            // Pointer to STARTUPINFO structure
		&pi            // Pointer to PROCESS_INFORMATION structure
	);

	if (!result) {
		// CreateProcess() failed
		// Get the error from the system
		LPVOID lpMsgBuf;
		DWORD dw = GetLastError();
		FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
			NULL, dw, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPTSTR)&lpMsgBuf, 0, NULL);

		// Display the error
		std::cout << lpMsgBuf;

		// Free resources created by the system
		LocalFree(lpMsgBuf);

		// We failed.
		return FALSE;
	}
	else
	{
		DWORD exitCode;
		// Successfully created the process.  Wait for it to finish.
		WaitForSingleObject(pi.hProcess, INFINITE);

		// Get the exit code.
		result = GetExitCodeProcess(pi.hProcess, &exitCode);

		// Close the handles.
		CloseHandle(pi.hProcess);
		CloseHandle(pi.hThread);

		if (!result)
		{
			// Could not get exit code.
			std::cout << "Executed command but couldn't get exit code.\n";
			return FALSE;
		}


		// We succeeded.
		return TRUE;
	}
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file