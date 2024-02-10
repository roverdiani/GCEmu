// This file is part of the GCEmu Project.
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.

#ifndef GCEMU_GENERATOR_H
#define GCEMU_GENERATOR_H

#include <cstdint>
#include <iostream>
#include <vector>
#include <random>
#include <mutex>

class Generator
{
public:
    // TODO: keep track of the randomDevice and maybe the generator, as it is costly to create new ones every time.

    static std::vector<uint8_t> GenerateIV(int32_t length = 8)
    {
        std::lock_guard<std::mutex> lock(m_generatorMutex);

        std::random_device randomDevice;
        std::mt19937 rng(randomDevice());
        std::uniform_int_distribution<uint32_t> uint_dist(0x00, 0xFF);

        std::vector<uint8_t> iv;
        uint8_t value = (uint8_t) uint_dist(rng);
        for (int32_t i = 0; i < 8; i++)
            iv.push_back(value);

        return iv;
    }

    static uint16_t GeneratePrefix()
    {
        std::lock_guard<std::mutex> lock(m_generatorMutex);

        std::random_device randomDevice;
        std::mt19937 rng(randomDevice());
        std::uniform_int_distribution<uint32_t> uint_dist(0x0000, 0xFFFF);

        return (uint16_t) uint_dist(rng);
    }

    static std::vector<uint8_t> GenerateKey(int32_t length = 8)
    {
        std::lock_guard<std::mutex> lock(m_generatorMutex);

        std::random_device randomDevice;
        std::mt19937 rng(randomDevice());
        std::uniform_int_distribution<uint32_t> uint_dist(0x00, 0xFF);

        std::vector<uint8_t> iv;
        for (int32_t i = 0; i < 8; i++)
            iv.push_back((uint8_t) uint_dist(rng));

        return iv;
    }

private:
    inline static std::mutex m_generatorMutex;
};

#endif //GCEMU_GENERATOR_H
