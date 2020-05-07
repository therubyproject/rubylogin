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

#ifndef XTEA_H
#define XTEA_H

#include "outputmessage.h"

class XTEA {
    public:
        XTEA() = default;
        ~XTEA() = default;

        // non-copyable
        XTEA(const XTEA&) = delete;
        XTEA& operator=(const XTEA&) = delete;

        void encrypt(uint32_t* key, OutputMessage& msg) const;
        bool decrypt(uint32_t* key, NetworkMessage& msg) const;

    private:
        uint32_t delta = 0x61C88647;
};

extern XTEA g_XTEA;

#endif
