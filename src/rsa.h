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

#ifndef RSA_H
#define RSA_H

#include "networkmessage.h"

class RSA
{
    public:
        RSA() = default;

        // non-copyable
        RSA(const RSA&) = delete;
        RSA& operator=(const RSA&) = delete;

        void loadPEM();
        bool decrypt(NetworkMessage& msg);

    private:
        void decrypt(char* msg);
        CryptoPP::RSA::PrivateKey pk;
        CryptoPP::AutoSeededRandomPool prng;
};

extern RSA g_RSA;

#endif
