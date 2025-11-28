#include "patcher.h"
#include <algorithm>
#include <stdexcept>
#include <iostream>

std::ptrdiff_t findSequence(
    const std::vector<unsigned char>& buffer,
    const std::vector<unsigned char>& pattern
) {
    auto it = std::search(buffer.begin(), buffer.end(),
                          pattern.begin(), pattern.end());

    if (it == buffer.end())
        return -1;

    std::cout << "[INFO] Found sequence at offset: " << std::hex << std::distance(buffer.begin(), it) << std::endl;
    return std::distance(buffer.begin(), it);
}

void patchBuffer(
    std::vector<unsigned char>& target,
    const std::vector<unsigned char>& source,
    std::size_t targetOffset
) {
    if (targetOffset + source.size() > target.size()) {
        target.resize(targetOffset + source.size());
    }

    std::copy(source.begin(), source.end(),
              target.begin() + targetOffset);
}
