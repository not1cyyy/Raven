#pragma once

#include <vector>
#include <cstddef>

std::ptrdiff_t findSequence(
    const std::vector<unsigned char>& buffer,
    const std::vector<unsigned char>& pattern
);

void patchBuffer(
    std::vector<unsigned char>& target,
    const std::vector<unsigned char>& source,
    std::size_t targetOffset
);
