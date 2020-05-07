/**
 * Ruby Login
 * Copyright (C) 2020 Leandro Matheus de Oliveira Sarna
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#include "includes.h"

#include "tools.h"
#include "networkmessage.h"

static uint32_t circularShift(int bits, uint32_t value)
{
    return (value << bits) | (value >> (32 - bits));
}

static void processSHA1MessageBlock(const uint8_t* messageBlock, uint32_t* H)
{
    uint32_t W[80];
    for (int i = 0; i < 16; ++i) {
        const size_t offset = i << 2;
        W[i] = messageBlock[offset] << 24 | messageBlock[offset + 1] << 16 | messageBlock[offset + 2] << 8 | messageBlock[offset + 3];
    }

    for (int i = 16; i < 80; ++i) {
        W[i] = circularShift(1, W[i - 3] ^ W[i - 8] ^ W[i - 14] ^ W[i - 16]);
    }

    uint32_t A = H[0], B = H[1], C = H[2], D = H[3], E = H[4];

    for (int i = 0; i < 20; ++i) {
        const uint32_t tmp = circularShift(5, A) + ((B & C) | ((~B) & D)) + E + W[i] + 0x5A827999;
        E = D; D = C; C = circularShift(30, B); B = A; A = tmp;
    }

    for (int i = 20; i < 40; ++i) {
        const uint32_t tmp = circularShift(5, A) + (B ^ C ^ D) + E + W[i] + 0x6ED9EBA1;
        E = D; D = C; C = circularShift(30, B); B = A; A = tmp;
    }

    for (int i = 40; i < 60; ++i) {
        const uint32_t tmp = circularShift(5, A) + ((B & C) | (B & D) | (C & D)) + E + W[i] + 0x8F1BBCDC;
        E = D; D = C; C = circularShift(30, B); B = A; A = tmp;
    }

    for (int i = 60; i < 80; ++i) {
        const uint32_t tmp = circularShift(5, A) + (B ^ C ^ D) + E + W[i] + 0xCA62C1D6;
        E = D; D = C; C = circularShift(30, B); B = A; A = tmp;
    }

    H[0] += A;
    H[1] += B;
    H[2] += C;
    H[3] += D;
    H[4] += E;
}

std::string transformToSHA1(const std::string& input)
{
    uint32_t H[] = {
        0x67452301,
        0xEFCDAB89,
        0x98BADCFE,
        0x10325476,
        0xC3D2E1F0
    };

    uint8_t messageBlock[64];
    size_t index = 0;

    uint32_t length_low = 0;
    uint32_t length_high = 0;
    for (char ch : input) {
        messageBlock[index++] = ch;

        length_low += 8;
        if (length_low == 0) {
            length_high++;
        }

        if (index == 64) {
            processSHA1MessageBlock(messageBlock, H);
            index = 0;
        }
    }

    messageBlock[index++] = 0x80;

    if (index > 56) {
        while (index < 64) {
            messageBlock[index++] = 0;
        }

        processSHA1MessageBlock(messageBlock, H);
        index = 0;
    }

    while (index < 56) {
        messageBlock[index++] = 0;
    }

    messageBlock[56] = length_high >> 24;
    messageBlock[57] = length_high >> 16;
    messageBlock[58] = length_high >> 8;
    messageBlock[59] = length_high;

    messageBlock[60] = length_low >> 24;
    messageBlock[61] = length_low >> 16;
    messageBlock[62] = length_low >> 8;
    messageBlock[63] = length_low;

    processSHA1MessageBlock(messageBlock, H);

    char hexstring[41];
    static const char hexDigits[] = {"0123456789abcdef"};
    for (int hashByte = 20; --hashByte >= 0;) {
        const uint8_t byte = H[hashByte >> 2] >> (((3 - hashByte) & 3) << 3);
        index = hashByte << 1;
        hexstring[index] = hexDigits[byte >> 4];
        hexstring[index + 1] = hexDigits[byte & 15];
    }
    return std::string(hexstring, 40);
}

uint32_t adlerChecksum(const uint8_t* data, size_t length)
{
    if (length > NETWORKMESSAGE_MAXSIZE) {
        return 0;
    }

    const uint16_t adler = 65521;

    uint32_t a = 1, b = 0;

    while (length > 0) {
        size_t tmp = length > 5552 ? 5552 : length;
        length -= tmp;

        do {
            a += *data++;
            b += a;
        } while (--tmp);

        a %= adler;
        b %= adler;
    }

    return (b << 16) | a;
}
