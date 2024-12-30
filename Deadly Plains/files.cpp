#include <string>
#include <fstream>
#include <windows.h>

#include "files.h"

using namespace std;

// Converts wstring to string (can potentially break data if contains weird characters)
static string wstring_to_string(wstring wstr)
{
    string str = "";
    for (auto it = wstr.begin(); it != wstr.end(); ++it)
    {
        str += static_cast<char>(*it);
    }
    return str;
}

// Ensures directory exists
static bool ensure_directory_exists(const wstring& path)
{
    if (CreateDirectoryW(path.c_str(), NULL))
        return true;
    else
        return (GetLastError() == ERROR_ALREADY_EXISTS);
}

// Returns %appdata% path
static wstring get_appdata_path()
{
    wchar_t buffer[1024];
    DWORD bufferSize = sizeof(buffer) / sizeof(wchar_t);
    DWORD result = GetEnvironmentVariableW(L"APPDATA", buffer, bufferSize);

    if (result > 0 && result < bufferSize)
        return wstring(buffer) + L"\\";
    else
        return L"";
}

// Returns final MazeGame path
static wstring get_final_path()
{
    wstring path = get_appdata_path() + L"MazeGame\\";
    if (!ensure_directory_exists(path))
    {
        path = L".\\MazeGame\\";
        ensure_directory_exists(path);
    }
    return path;
}

// Saves data to file
void save_to_file(wstring file_name, string contents)
{
    wstring path = get_final_path() + file_name;
    wofstream file(path);
    if (file.is_open())
    {
        file << wstring(contents.begin(), contents.end());
        file.close();
    }
}

// Reads data from file
string read_from_file(wstring file_name)
{
    wstring path = get_final_path() + file_name;
    wifstream file(path);
    if (file.is_open())
    {
        wstring contents;
        file >> contents;
        file.close();
        return wstring_to_string(contents);
    }
    else return "";
}
