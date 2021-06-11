#ifndef __BIT_OPERATION_H__
#define __BIT_OPERATION_H__

inline uint32_t rev32(uint32_t v)
{
    // https://graphics.stanford.edu/~seander/bithacks.html
    // swap odd and even bits
    v = ((v >> 1) & 0x55555555) | ((v & 0x55555555) << 1);
    // swap consecutive pairs
    v = ((v >> 2) & 0x33333333) | ((v & 0x33333333) << 2);
    // swap nibbles ...
    v = ((v >> 4) & 0x0F0F0F0F) | ((v & 0x0F0F0F0F) << 4);
    // swap bytes
    v = ((v >> 8) & 0x00FF00FF) | ((v & 0x00FF00FF) << 8);
    // swap 2-byte-long pairs
    v = ( v >> 16             ) | ( v               << 16);
    return v;
}



uint64_t rev64(void *v) {
    uint64_t x = (uint64_t) v;
    x = (x & 0x00000000FFFFFFFF) << 32 | (x & 0xFFFFFFFF00000000) >> 32;
    x = (x & 0x0000FFFF0000FFFF) << 16 | (x & 0xFFFF0000FFFF0000) >> 16;
    x = (x & 0x00FF00FF00FF00FF) << 8  | (x & 0xFF00FF00FF00FF00) >> 8;
    return x;
}

unsigned int _rotl(unsigned int value, int shift) {
    if ((shift &= 31) == 0)
        return value;
    return (value << shift) | (value >> (32 - shift));
}

unsigned int _rotr(unsigned int value, int shift) {
    if ((shift &= 31) == 0)
        return value;
    return (value >> shift) | (value << (32 - shift));
}





#endif /* __BIT_OPERATION_H__ */
