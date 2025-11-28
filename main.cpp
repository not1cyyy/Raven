#include <iostream>
#include <windows.h>
#include <shlobj.h>
#include <filesystem>
#include <fstream>
#include <vector>
#include <algorithm>
#include <optional>

namespace fs = std::filesystem;

fs::path getStellarBladeSave() {
    char path[MAX_PATH];

    bool found = SUCCEEDED(SHGetFolderPathA(NULL, CSIDL_LOCAL_APPDATA, NULL, 0, path));
    if (found){
        fs::path localAppData = path;

        return localAppData / "SB" / "Saved" / "SaveGames" / "StellarBladeSave00.sav";
    }

    return {};
}

std::optional<std::ptrdiff_t> findSequence(const std::vector<unsigned char>& buffer) {
    const std::vector<unsigned char> targetBytes = {
        0x53, 0x42, 0x53, 0x61, 0x76, 0x65, 0x47, 0x61,
        0x6D, 0x65, 0x44, 0x61, 0x74, 0x61, 0x5F, 0x4F,
        0x6E, 0x6C, 0x69, 0x6E, 0x65, 0x41, 0x63, 0x63,
        0x6F, 0x75, 0x6E, 0x74, 0x49, 0x6E, 0x66, 0x6F
    }; // SBSaveGameData_OnlineAccountInfo

    auto it = std::search(buffer.begin(), buffer.end(),
                          targetBytes.begin(), targetBytes.end());

    std::cout << "[INFO] Reading save file..." << std::endl;
    if (it != buffer.end()) {
        std::ptrdiff_t index = std::distance(buffer.begin(), it);
        std::cout << "[INFO] Found target byte sequence at offset: 0x"
                  << std::hex << index << std::dec << std::endl;
        return index;
    } else {
        std::cout << "[ERROR] Target byte sequence not found in the save file." << std::endl;
        return std::nullopt;
    }
}

std::vector<unsigned char> readFile(const fs::path& path) {
    std::ifstream file(path, std::ios::binary);
    if (!file.is_open()) throw std::runtime_error("Failed to open " + path.string());
    return {std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>()};
}

void writeFile(const fs::path& path, const std::vector<unsigned char>& data){
    std::ofstream out(path, std::ios::binary);
    if (!out.is_open()) throw std::runtime_error("Failed to write " + path.string());
    out.write(reinterpret_cast<const char*>(data.data()), data.size());
    std::cout << "[INFO] Overwriting bytes..." << std::endl;
    std::cout << "[INFO] Wrote necessary bytes to " << path.string() << std::endl;
}
fs::path findFirstSavInCurrentDir(){
    fs::path currentDir = fs::current_path();
    for (auto& entry : fs::directory_iterator(currentDir)){
        if (entry.is_regular_file() && entry.path().extension() == ".sav"){
            return entry.path();
        }
    }
    return {}; // empty path if none found 
}

void patchBuffer(std::vector<unsigned char>& targetBuffer, const std::vector<unsigned char>& sourceBuffer, std::ptrdiff_t offsetTarget, std::ptrdiff_t offsetSource){
    if (offsetTarget + sourceBuffer.size() > targetBuffer.size()){
        targetBuffer.resize(offsetTarget + sourceBuffer.size());
    }
    std::copy(sourceBuffer.begin() + offsetSource, sourceBuffer.end(), targetBuffer.begin() + offsetTarget);
}

void patchStellarBladeSave(){
    auto saveFile = getStellarBladeSave();
    auto buffer = readFile(saveFile);
    auto offsetOpt = findSequence(buffer);
    if (!offsetOpt) throw std::runtime_error("[ERROR] Target byte sequence not found in source save file.");
    auto offset = *offsetOpt;

    std::vector<unsigned char> importantBytes(buffer.begin() + offset, buffer.end());

    auto targetSav = findFirstSavInCurrentDir();
    if (targetSav.empty()) throw std::runtime_error("[ERROR] No .sav file found in current directory");

    auto savBuffer = readFile(targetSav);
    auto savOffsetOpt = findSequence(savBuffer);
    if (!savOffsetOpt) throw std::runtime_error("[ERROR] Target byte sequence not found in target save file.");
    auto savOffset = *savOffsetOpt;

    patchBuffer(savBuffer, importantBytes, savOffset, 0);
    writeFile(targetSav, savBuffer);

    fs::rename(targetSav, targetSav.parent_path() / "StellarBladeSave00.sav");
}
int main() {
    try{
        patchStellarBladeSave();
        std::cout << "[SUCCESS] Patched .sav file. \n" << std::endl;

    } catch(const std::exception& e){
        std::cerr << "[ERROR] " << e.what() << std::endl;
        return 1;
    } 
}  