#ifndef TINYPNG_DEFLATE_H
#define TINYPNG_DEFLATE_H

#include "bit_stream.h"
#include "lz77.h"

namespace tiny_png {

    BitStream huffman_encode(const std::vector<LZ77Seq>& seqs);
    std::vector<LZ77Seq> huffman_decode(BitStream& bits);

}

#endif //TINYPNG_DEFLATE_H