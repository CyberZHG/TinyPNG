#ifndef TINYPNG_BIT_STREAM_H
#define TINYPNG_BIT_STREAM_H

#include <cstdint>
#include <vector>
#include <string>

namespace tiny_png {

    class BitStream {
    public:
        BitStream();

        [[nodiscard]] const std::vector<uint8_t>& getBytes() const {
            return _buffer;
        }
        template<typename InputIterator>
        void setBytes(InputIterator first, InputIterator last) {
            _buffer.assign(first, last);
            _index = _buffer.size();
            _pos = 0;
            _readIndex = 0;
        }

        void writeInt(std::int32_t value, std::int32_t numBits, bool msb);

        void rewind();
        bool nextBit();
        void alignToByte();
        std::int32_t readInt(std::int32_t numBits, bool msb);

        [[nodiscard]] std::string toString() const;

    private:
        std::size_t _index;
        std::uint8_t _pos;
        std::vector<std::uint8_t> _buffer;

        std::size_t _readIndex;
    };

}

#endif //TINYPNG_BIT_STREAM_H