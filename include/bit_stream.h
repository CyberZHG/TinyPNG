#ifndef TINYPNG_BIT_STREAM_H
#define TINYPNG_BIT_STREAM_H

#include <cstdint>
#include <vector>

namespace tiny_png {

    class BitStream {
    public:
        BitStream();

        [[nodiscard]] const std::vector<uint8_t> &getBytes() const {
            return _buffer;
        }

        void writeInt(std::int32_t value, std::int32_t numBits);

        void rewind();
        bool nextBit();

        [[nodiscard]] std::string toString() const;

    private:
        std::size_t _index;
        std::uint8_t _pos;
        std::vector<std::uint8_t> _buffer;

        std::size_t _readIndex;
    };

}

#endif //TINYPNG_BIT_STREAM_H