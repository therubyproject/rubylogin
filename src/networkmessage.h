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

#ifndef NETWORKMESSAGE_H
#define NETWORKMESSAGE_H

static constexpr int32_t NETWORKMESSAGE_MAXSIZE = 24590;

class NetworkMessage
{
    public:
        using MsgSize_t = uint16_t;

        // Headers:
        // 2 bytes for unencrypted message size
        // 4 bytes for checksum
        // 2 bytes for encrypted message size
        static constexpr MsgSize_t INITIAL_BUFFER_POSITION = 8;
        enum { HEADER_LENGTH = 2 };
        enum { CHECKSUM_LENGTH = 4 };
        enum { XTEA_MULTIPLE = 8 };
        enum { MAX_BODY_LENGTH = NETWORKMESSAGE_MAXSIZE - HEADER_LENGTH - CHECKSUM_LENGTH - XTEA_MULTIPLE };
        enum { MAX_PROTOCOL_BODY_LENGTH = MAX_BODY_LENGTH - 10 };

        NetworkMessage() = default;

        void reset() {
            info = {};
        }

        // simply read functions for incoming message
        uint8_t getByte() {
            if (!canRead(1)) {
                return 0;
            }

            return buffer[info.position++];
        }

        uint8_t getPreviousByte() {
            return buffer[--info.position];
        }

        template<typename T>
        T get() {
            if (!canRead(sizeof(T))) {
                return 0;
            }

            T v;
            memcpy(&v, buffer + info.position, sizeof(T));
            info.position += sizeof(T);
            return v;
        }

        std::string getString(uint16_t stringLen = 0);

        // skips count unknown/unused bytes in an incoming message
        void skipBytes(int16_t count) {
            info.position += count;
        }

        // simply write functions for outgoing message
        void addByte(uint8_t value) {
            if (!canAdd(1)) {
                return;
            }

            buffer[info.position++] = value;
            info.length++;
        }

        template<typename T>
        void add(T value) {
            if (!canAdd(sizeof(T))) {
                return;
            }

            memcpy(buffer + info.position, &value, sizeof(T));
            info.position += sizeof(T);
            info.length += sizeof(T);
        }

        void addBytes(const char* bytes, size_t size);
        void addPaddingBytes(size_t n);

        void addString(const std::string& value);

        void addDouble(double value, uint8_t precision = 2);

        MsgSize_t getLength() const {
            return info.length;
        }

        void setLength(MsgSize_t newLength) {
            info.length = newLength;
        }

        MsgSize_t getBufferPosition() const {
            return info.position;
        }

        uint16_t getLengthHeader() const {
            return static_cast<uint16_t>(buffer[0] | buffer[1] << 8);
        }

        int32_t decodeHeader();

        bool isOverrun() const {
            return info.overrun;
        }

        uint8_t* getBuffer() {
            return buffer;
        }

        const uint8_t* getBuffer() const {
            return buffer;
        }

        uint8_t* getBodyBuffer() {
            info.position = 2;
            return buffer + HEADER_LENGTH;
        }

        static const uint8_t headerLength = 2;

    protected:
        bool canAdd(size_t size) const {
            return (size + info.position) < MAX_BODY_LENGTH;
        }

        bool canRead(int32_t size) {
            if ((info.position + size) > (info.length + 8) || size >= (NETWORKMESSAGE_MAXSIZE - info.position)) {
                info.overrun = true;
                return false;
            }
            return true;
        }

        struct NetworkMessageInfo {
            MsgSize_t length = 0;
            MsgSize_t position = INITIAL_BUFFER_POSITION;
            bool overrun = false;
        };

        NetworkMessageInfo info;
        uint8_t buffer[NETWORKMESSAGE_MAXSIZE];
};

#endif
