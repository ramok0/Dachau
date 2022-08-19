#include <iostream>
#include <Windows.h>
#include <vector>
#include <fstream>
#include <filesystem>
#include "nlohmann/json.hpp"

int get_directories_with_search_term(std::string path, std::vector<std::string>* buffer)
{
	for (auto& p : std::filesystem::directory_iterator(path)) {
		if (p.is_directory()) {
			std::string path = p.path().string();
			if (path.find("iscord") != std::string::npos) {
				buffer->push_back(path);
			}
		}

	}
	return 0;
}

int get_recurcive_directories_and_filter(std::string path, std::vector<std::string>* buffer) {
	for (auto& p : std::filesystem::recursive_directory_iterator(path)) {
		if (p.is_directory()) {
			if (p.path().string().find("discord_desktop_core") != std::string::npos) {
				if (p.path().parent_path().filename().string().find("discord_desktop_core") != std::string::npos) {
					buffer->push_back(p.path().string());
				}
			}
		}
	}

	return 0;
}

std::vector<std::string> getDiscordPaths(char* localAppdata) {
	std::vector<std::string> buffer = std::vector<std::string>();

	std::vector<std::string> DiscordDesktopCore = std::vector<std::string>();
	get_directories_with_search_term(std::string(localAppdata), &buffer);
	for (std::string path : buffer) {
		get_recurcive_directories_and_filter(path, &DiscordDesktopCore);
	}
	return DiscordDesktopCore;
}

bool readRegStringValue(HKEY hKey, LPCWSTR lpValueName, std::wstring* out) {
	WCHAR szBuffer[512];
	DWORD dwBufferSize = sizeof(szBuffer);
	ULONG nError;
	nError = RegQueryValueExW(hKey, lpValueName, 0, NULL, (LPBYTE)szBuffer, &dwBufferSize);
	if (nError != ERROR_SUCCESS) return false;
	std::wstring data = std::wstring(szBuffer);
	*out = data;
	return true;
}

void analysePackageJson(std::filesystem::path packageJsonPath) {
	if (std::filesystem::exists(packageJsonPath)) {
		std::ifstream packageJsonReader(packageJsonPath);
		if (packageJsonReader.is_open()) {
			nlohmann::json json;
			packageJsonReader >> json;
			if (json["main"] != nlohmann::detail::value_t::null && json["main"] != "index.js") {
				printf("Found a main file detouring\n");
				std::filesystem::path susPath = std::filesystem::path(std::string(json["main"]));
				if (std::filesystem::exists(susPath)) {
					std::filesystem::remove(susPath);
				}

				json["main"] = "index.js";
				packageJsonReader.close();
				std::ofstream packageJsonWriter(packageJsonPath);
				if (packageJsonWriter.is_open()) {
					packageJsonWriter << json;
					packageJsonWriter.close();
				}
			}
		}
	}
}

void anaylseIndexJs(std::filesystem::path indexJsPath) {
	if (std::filesystem::exists(indexJsPath)) {
		auto reader = std::ifstream(indexJsPath);
		std::string buffer;
		std::string fileContent;
		if (reader.is_open()) {
			while (std::getline(reader, buffer)) {
				fileContent += buffer;
			}
			buffer.clear();
			reader.close();

			auto originalContent = "module.exports = require('./core.asar');";
			if (fileContent != originalContent) {
				printf("\n%s might be infected, patching it\n\n", indexJsPath.string().c_str());
				auto writer = std::ofstream(indexJsPath);
				if (writer.is_open()) {
					writer << originalContent;
					writer.close();
				}
				else {
					printf("Cant write to write, unknown error\n");
				}
			}
		}
	}
}

char* getLocalappdata() {
	char* buffer;
	size_t size = sizeof(buffer);
	_dupenv_s(&buffer, &size, "LOCALAPPDATA");
	return buffer;
}

void initAnalyseDiscordPaths() {
	auto localappdata = getLocalappdata();
	printf("Found localappdata : %s\n", localappdata);
	std::vector<std::string> paths = getDiscordPaths(localappdata);
	for (auto path : paths) {
		printf("Analyzing folder...\n");
		std::filesystem::path discordPath = std::filesystem::path(path);
		std::filesystem::path packageJsonPath = std::filesystem::path(discordPath / "package.json");
		std::filesystem::path indexJsPath = std::filesystem::path(discordPath / "index.js");
		analysePackageJson(packageJsonPath);
		anaylseIndexJs(indexJsPath);
	}
}

bool openRegKeyWithReadWritePermisions(HKEY hKey, LPCWSTR subKey, HKEY* key) {
	LONG lRes = RegOpenKeyExW(hKey, subKey, 0, KEY_READ | KEY_SET_VALUE, key);

	return lRes == ERROR_SUCCESS;
}

bool regDelKey(HKEY hKey, LPCWSTR value) {
	LSTATUS status = RegDeleteValueW(hKey, value);
	return (status == ERROR_SUCCESS);
}

bool wStartsWith(std::wstring& str, const wchar_t* substr) {
	return str.rfind(substr, 0) == 0;
}

int main()
{
	printf("welcome\n");

	initAnalyseDiscordPaths();
	HKEY hKey{};

	if (openRegKeyWithReadWritePermisions(HKEY_CURRENT_USER, L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run", &hKey)) {
		std::wstring data{};
		if (readRegStringValue(hKey, L"Realtek HD Audio Universal Service", &data)) {
			if (wStartsWith(data, L"SecurityHealthSystray.exe")) {
				if (regDelKey(hKey, L"Realtek HD Audio Universal Service")) {
					printf("Removed autostart\n");
				}

				size_t pos = data.find(L"pythonw.exe");
				if (pos != std::wstring::npos) {
					std::wstring path = std::wstring(&data.data()[pos + strlen("pythonw.exe") + 1]);
					auto payloadPath = std::filesystem::path(path);
					if (std::filesystem::exists(payloadPath)) {
						std::filesystem::remove(payloadPath);
						printf("Removed startup payload from files\n");
					}
				}
			}
			else {
				wprintf(L"Unknown value : '%s'\n", data.c_str());
			}
		}
		else {
			printf("Failed to get value\n");
		}

	}	

	printf("cya, this tool was made by ramok xdxdxd proutent\n");

	return 0;
}