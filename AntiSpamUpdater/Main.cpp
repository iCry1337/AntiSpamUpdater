#include <filesystem>
#include <iostream>
#include <string>
#include <exception>
#include <Windows.h>
#include <tchar.h>
#include <urlmon.h>

using namespace std;
namespace fs = std::filesystem;

#pragma comment (lib,"urlmon.lib")

std::wstring GetStringValueFromHKLM(const std::wstring& regSubKey, const std::wstring regValue) {
	size_t bufferSize = 0xFFF;
	std::wstring valueBuffer;
	valueBuffer.resize(bufferSize);
	auto cbData = static_cast<DWORD>(bufferSize * sizeof(wchar_t));
	auto rc = RegGetValueW(
		HKEY_LOCAL_MACHINE,
		regSubKey.c_str(),
		regValue.c_str(),
		RRF_RT_REG_SZ,
		nullptr,
		static_cast<void*>(valueBuffer.data()),
		&cbData
	);

	while (rc == ERROR_MORE_DATA)
	{
		cbData /= sizeof(wchar_t);
		if (cbData > static_cast<DWORD>(bufferSize))
		{
			bufferSize = static_cast<size_t>(cbData);
		}
		else
		{
			bufferSize *= 2;
			cbData = static_cast<DWORD>(bufferSize * sizeof(wchar_t));
		}
		valueBuffer.resize(bufferSize);
		rc = RegGetValueW(
			HKEY_LOCAL_MACHINE,
			regSubKey.c_str(),
			regValue.c_str(),
			RRF_RT_REG_SZ,
			nullptr,
			static_cast<void*>(valueBuffer.data()),
			&cbData
		);
	}

	if (rc == ERROR_SUCCESS)
	{
		cbData /= sizeof(wchar_t);
		valueBuffer.resize(static_cast<size_t>(cbData - 1));
		return valueBuffer;
	}
	else
	{
		throw std::runtime_error("Windows System Fehler Code: " + std::to_string(rc));
	}
}

int main()
{
	std::wstring regSubKey;
	regSubKey = L"SOFTWARE\\WOW6432Node\\Blizzard Entertainment\\World of Warcraft\\";

	std::wstring regValue (L"InstallPath");
	std::wstring classic = L"_classic_";
	std::wstring retail = L"_retail_";
	std::wstring valueFromRegistry;
	std::wstring download_url = L"https://gist.githubusercontent.com/thewheat/bb67f632950c7feaf4b8a2f3febbd98a/raw/02feb16f6fac5edf8e6df7e287dbb08b53cc38c1/Test.txt";
	

	try
	{
		valueFromRegistry = GetStringValueFromHKLM(regSubKey, regValue);
	}
	catch (std::exception& e)
	{
		std::cerr << e.what();
	}

	if (valueFromRegistry.find(classic) != wstring::npos)
	{
		std::wcout << "Die Classic Version wird nicht supported. Bitte starte vorher deine Retail Version\n" << std::endl;
		std::getchar();
		std::exit(0);
	}

	std::wstring neuePfadFurWTF = valueFromRegistry + L"WTF\\Account\\";
	std::wstring savepath = neuePfadFurWTF;
	if (valueFromRegistry.find(retail) != wstring::npos)
	{
		for (const auto& entry : fs::directory_iterator(neuePfadFurWTF))
			std::wcout << "Debug : " << entry.path() << std::endl;

		std::wcout << "\n" << std::endl;
	}

	URLDownloadToFile(NULL, download_url.c_str(), savepath.c_str(), 0, NULL);

	std::wcout << "Zuletzt benutzt WoW Version : " << valueFromRegistry << "\n" << std::endl;
	std::getchar();
}