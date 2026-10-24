#include "bit_stream.h"

namespace tiny_png {

    BitStream::BitStream() : _index(0), _pos(0), _readIndex(0)  {
    }

    void BitStream::writeInt(std::int32_t value, std::int32_t numBits) {
        while (numBits > 0) {
            if (_index == _buffer.size()) {
                _buffer.push_back(0);
            }
            const std::int32_t bits = std::min(numBits, 8 - _pos);
            _buffer[_index] |= (value & ((1 << bits) - 1)) << _pos;
            if ((_pos += bits) >= 8) {
                _pos = 0;
                ++_index;
            }
            numBits -= bits;
            value >>= bits;
        }
    }

    void BitStream::rewind() {
        _readIndex = 0;
    }

    bool BitStream::nextBit() {
        const auto index = _readIndex >> 8;
        if (index >= _buffer.size()) {
            return false;
        }
        return _buffer[index] & (1 << (_readIndex++ & 7));
    }

    std::string BitStream::toString() const {
        std::string s;
        for (const auto& byte : _buffer) {
            for (size_t i = 0; i < 8; ++i) {
                if (byte & (1 << i)) {
                    s += '1';
                } else {
                    s += '0';
                }
            }
        }
        return s;
    }

}
