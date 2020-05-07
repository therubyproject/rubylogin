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

#include "networkmessage.h"

int32_t NetworkMessage::decodeHeader()
{
    int32_t newSize = static_cast<int32_t>(buffer[0] | buffer[1] << 8);
    info.length = newSize;
    return info.length;
}

std::string NetworkMessage::getString(uint16_t stringLen/* = 0*/)
{
    if (stringLen == 0) {
        stringLen = get<uint16_t>();
    }

    if (!canRead(stringLen)) {
        return std::string();
    }

    char* v = reinterpret_cast<char*>(buffer) + info.position; //does not break strict aliasing
    info.position += stringLen;
    return std::string(v, stringLen);
}

void NetworkMessage::addString(const std::string& value)
{
    size_t stringLen = value.length();
    if (!canAdd(stringLen + 2) || stringLen > 8192) {
        return;
    }

    add<uint16_t>(stringLen);
    memcpy(buffer + info.position, value.c_str(), stringLen);
    info.position += stringLen;
    info.length += stringLen;
}

void NetworkMessage::addDouble(double value, uint8_t precision/* = 2*/)
{
    addByte(precision);
    add<uint32_t>((value * std::pow(static_cast<float>(10), precision)) + std::numeric_limits<int32_t>::max());
}

void NetworkMessage::addBytes(const char* bytes, size_t size)
{
    if (!canAdd(size) || size > 8192) {
        return;
    }

    memcpy(buffer + info.position, bytes, size);
    info.position += size;
    info.length += size;
}

void NetworkMessage::addPaddingBytes(size_t n)
{
    if (!canAdd(n)) {
        return;
    }

    memset(buffer + info.position, 0x33, n);
    info.length += n;
}

