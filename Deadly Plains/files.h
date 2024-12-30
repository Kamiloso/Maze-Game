#pragma once

#include <string>

using namespace std;

void save_to_file(wstring file_name, string contents);
string read_from_file(wstring file_name); // reads only the first stream "burst", for example "Ala has a cat." -> "Ala"
