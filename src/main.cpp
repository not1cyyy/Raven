#include <iostream>
#include <filesystem>

#include "file_io.h"
#include "patcher.h"
#include "signature.h"

namespace fs = std::filesystem;

int main() {
    try {
        auto saveFile = getStellarBladeSave();
        auto buffer = readFile(saveFile);

        auto offset = findSequence(buffer, SB_SIGNATURE);
        if (offset < 0)
            throw std::runtime_error("[ERROR] Signature not found in original save file.");

        std::vector<unsigned char> importantBytes(buffer.begin() + offset, buffer.end());

        auto targetSav = findFirstSavInCurrentDir();
        if (targetSav.empty())
            throw std::runtime_error("[ERROR] No .sav file found in current directory");

        auto savBuffer = readFile(targetSav);

        auto savOffset = findSequence(savBuffer, SB_SIGNATURE);
        if (savOffset < 0)
            throw std::runtime_error("[ERROR] Signature not found in target file.");

        patchBuffer(savBuffer, importantBytes, savOffset);
        writeFile(targetSav, savBuffer);

        fs::rename(targetSav, targetSav.parent_path() / "StellarBladeSave00.sav");

        std::cout << "[SUCCESS] Successfully patched the .sav file." << std::endl;

    } catch(const std::exception& e) {
        std::cerr << "[ERROR] " << e.what() << std::endl;
        return 1;
    }
}
