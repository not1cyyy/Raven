#pragma once
#include <filesystem>
#include <vector>

std::vector<unsigned char> readFile(const std::filesystem::path& path);
void writeFile(const std::filesystem::path& path, const std::vector<unsigned char>& data);

std::filesystem::path getStellarBladeSave();
std::filesystem::path findFirstSavInCurrentDir();
