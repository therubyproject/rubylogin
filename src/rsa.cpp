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

#include "rsa.h"

RSA g_RSA;

void RSA::loadPEM()
{
    static const std::string header = "-----BEGIN RSA PRIVATE KEY-----";
    static const std::string footer = "-----END RSA PRIVATE KEY-----";

    std::ifstream file{"key.pem"};

    if (!file.is_open()) {
        throw std::runtime_error("Missing file.");
     }

    std::ostringstream oss;
    for (std::string line; std::getline(file, line); oss << line);
    std::string key = oss.str();

    if (key.substr(0, header.size()) != header) {
        throw std::runtime_error("Missing RSA private key header.");
    }

    if (key.substr(key.size() - footer.size(), footer.size()) != footer) {
        throw std::runtime_error("Missing RSA private key footer.");
    }

    key = key.substr(header.size(), key.size() - footer.size());

    CryptoPP::ByteQueue queue;
    CryptoPP::Base64Decoder decoder;
    decoder.Attach(new CryptoPP::Redirector(queue));
    decoder.Put(reinterpret_cast<const uint8_t*>(key.c_str()), key.size());
    decoder.MessageEnd();

    pk.BERDecodePrivateKey(queue, false, queue.MaxRetrievable());

    if (!pk.Validate(prng, 3)) {
        throw std::runtime_error("RSA private key is not valid.");
    }
}

bool RSA::decrypt(NetworkMessage& msg)
{
    if ((msg.getLength() - msg.getBufferPosition()) < 128) {
        return false;
    }

    decrypt(reinterpret_cast<char*>(msg.getBuffer()) + msg.getBufferPosition()); //does not break strict aliasing
    return msg.getByte() == 0;
}

void RSA::decrypt(char* msg)
{
    CryptoPP::Integer m{reinterpret_cast<uint8_t*>(msg), 128};
    auto c = pk.CalculateInverse(prng, m);
    c.Encode(reinterpret_cast<uint8_t*>(msg), 128);
}
