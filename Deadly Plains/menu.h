#pragma once

#include <iostream>
#include <string>
#include <vector>

using namespace std;

int main_menu();
unsigned int seed_menu();
void instructions_menu();
void phases_menu();
int pause_menu(int score);
void you_died_menu(int score, string info = "YOU DIED");
