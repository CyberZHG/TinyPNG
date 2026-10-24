#ifndef TINYPNG_HUFFMAN_H
#define TINYPNG_HUFFMAN_H

#include "bit_stream.h"
#include "lz77.h"

namespace tiny_png {

    BitStream huffman_encode(const std::vector<LZ77Seq>& seqs);
    std::vector<LZ77Seq> huffman_decode(const BitStream& bits);

}

#endif //TINYPNG_HUFFMAN_H