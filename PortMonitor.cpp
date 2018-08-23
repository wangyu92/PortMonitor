// https://stackoverflow.com/questions/15439740/segmentation-fault-on-enummonitors-winapi

#include "pch.h"

#include <exception>        // for std::exception
#include <iostream>         // for std::wcout, std::wcerr, std::endl
#include <sstream>          // for std::ostringstream
#include <stdexcept>        // for std::runtime_error
#include <vector>           // for std::vector
#include <windows.h>        // Win32 SDK main header
#include <winspool.h>       // for EnumMonitors()
using namespace std;

void ThrowOnApiFailure(const char* apiName, DWORD errorCode)
{
	ostringstream errorMessage;
	errorMessage << apiName << "() failed with error code " << errorCode;
	throw runtime_error(errorMessage.str());
}

void PrintMonitors()
{
	static const int kMonitorInfoLevel = 1; // for MONITOR_INFO_1

	// Ask output buffer size
	DWORD bufferSize = 0;
	DWORD infoCount = 0;
	::EnumMonitors(
		nullptr,
		kMonitorInfoLevel,
		nullptr,
		0,          // ask buffer size
		&bufferSize,
		&infoCount);
	DWORD error = ::GetLastError();
	if (error != ERROR_INSUFFICIENT_BUFFER)
	{
		ThrowOnApiFailure("EnumMonitors", error);
	}

	// Size output buffer
	vector<BYTE> buffer(bufferSize);

	// Fill buffer with monitor info
	if (!::EnumMonitors(
		nullptr,
		kMonitorInfoLevel,
		buffer.data(),
		buffer.size(),
		&bufferSize,
		&infoCount
	))
	{
		error = ::GetLastError();
		ThrowOnApiFailure("EnumMonitors", error);
	}

	// Print monitor info
	const MONITOR_INFO_1 * monitorInfo =
		reinterpret_cast<const MONITOR_INFO_1*>(buffer.data());
	for (DWORD i = 0; i < infoCount; i++)
	{
		wcout << monitorInfo[i].pName << endl;
	}
}

int main()
{
	try
	{
		PrintMonitors();
	}
	catch (const exception& e)
	{
		wcerr << "\n*** ERROR: " << e.what() << endl;
	}
}