#ifndef TINYPNG_HUFFMAN_H
#define TINYPNG_HUFFMAN_H

#include "tiny_png/bit_stream.h"
#include "tiny_png/lz77.h"

namespace tiny_png {

    BitStream huffman_encode(const std::vector<LZ77Seq>& seqs);
    std::vector<LZ77Seq> huffman_decode(BitStream& bits);

}

#endif //TINYPNG_HUFFMAN_H