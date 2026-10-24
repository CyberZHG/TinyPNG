#ifndef TINYPNG_DEFLATE_H
#define TINYPNG_DEFLATE_H

#include <cstdint>
#include <vector>

namespace tiny_png {

    std::vector<std::uint8_t> deflate_encode(const std::uint8_t* data, std::size_t len);
    std::vector<std::uint8_t> deflate_decode(const std::vector<std::uint8_t>& data);

}

#endif //TINYPNG_DEFLATE_H