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

#ifndef OUTPUTMESSAGE_H
#define OUTPUTMESSAGE_H

#include "networkmessage.h"
#include "tools.h"

class OutputMessage : public NetworkMessage
{
    public:
        OutputMessage() = default;

        // non-copyable
        OutputMessage(const OutputMessage&) = delete;
        OutputMessage& operator=(const OutputMessage&) = delete;

        uint8_t* getOutputBuffer() {
            return buffer + outputBufferStart;
        }

        void writeMessageLength() {
            add_header(info.length);
        }

        void addCryptoHeader() {
            add_header(adlerChecksum(buffer + outputBufferStart, info.length));
            writeMessageLength();
        }

    protected:
        template <typename T>
        void add_header(T add) {
            assert(outputBufferStart >= sizeof(T));
            outputBufferStart -= sizeof(T);
            memcpy(buffer + outputBufferStart, &add, sizeof(T));
            //added header size to the message size
            info.length += sizeof(T);
        }

        MsgSize_t outputBufferStart = INITIAL_BUFFER_POSITION;
};


#endif
