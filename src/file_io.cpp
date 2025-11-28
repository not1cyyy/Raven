#include "file_io.h"
#include <fstream>
#include <stdexcept>
#include <windows.h>
#include <shlobj.h>
#include <iostream>

namespace fs = std::filesystem;

std::vector<unsigned char> readFile(const fs::path& path) {
    std::ifstream file(path, std::ios::binary);
    if (!file.is_open())
        throw std::runtime_error("Failed to open " + path.string());

    std::cout << "[INFO] Reading " << path << std::endl;
    return { std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>() };
}

void writeFile(const fs::path& path, const std::vector<unsigned char>& data) {
    std::ofstream out(path, std::ios::binary);
    if (!out.is_open())
        throw std::runtime_error("Failed to write to " + path.string());

    std::cout << "[INFO] Writing to " << path << std::endl;
    out.write(reinterpret_cast<const char*>(data.data()), data.size());
}

fs::path getStellarBladeSave() {
    char path[MAX_PATH];

    if (SUCCEEDED(SHGetFolderPathA(NULL, CSIDL_LOCAL_APPDATA, NULL, 0, path))) {
        fs::path localAppData = path;
        std::cout << "[INFO] Found local app data at " << localAppData << std::endl;
        return localAppData / "SB" / "Saved" / "SaveGames" / "StellarBladeSave00.sav";
    }

    return {};
}

fs::path findFirstSavInCurrentDir() {
    for (auto& entry : fs::directory_iterator(fs::current_path())) {
        if (entry.is_regular_file() && entry.path().extension() == ".sav")
            return entry.path();
    }
    return {};
}
