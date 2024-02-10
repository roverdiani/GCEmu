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

#ifndef GCEMU_OPCODEMAP_H
#define GCEMU_OPCODEMAP_H

#include <string>
#include <map>
#include "../../common/network/Packet.h"

struct OpcodeHandler
{
    std::string OpcodeName;
    void (*Handler)(Packet& recvPacket);
};

class OpcodeMap
{
public:
    OpcodeMap()
    {
        BuildOpcodeList();
    }

    ~OpcodeMap()
    {
        m_opcodeMap.clear();
    }

    void BuildOpcodeList();
    void StoreOpcode(uint16_t opcode, const std::string& name, void (*handler)(Packet& recvPacket))
    {
        OpcodeHandler& opHandler = m_opcodeMap[opcode];
        opHandler.OpcodeName = name;
        opHandler.Handler = handler;
    }

    OpcodeHandler const* LookupOpcode(uint16_t opcode) const
    {
        std::map<uint16_t, OpcodeHandler>::const_iterator itr = m_opcodeMap.find(opcode);
        if (itr != m_opcodeMap.end())
            return &itr->second;

        return nullptr;
    }

    std::string LookupOpcodeName(uint16_t opcode) const
    {
        if (OpcodeHandler const* op = LookupOpcode(opcode))
            return op->OpcodeName;

        return "Unknown opcode";
    }

    OpcodeHandler const& operator [](uint16_t opcode) const
    {
        std::map<uint16_t, OpcodeHandler>::const_iterator itr = m_opcodeMap.find(opcode);
        if (itr != m_opcodeMap.end())
            return itr->second;

        return EmptyHandler;
    }

    static OpcodeHandler const EmptyHandler;

private:
    std::map<uint16_t, OpcodeHandler> m_opcodeMap;
};

#endif //GCEMU_OPCODEMAP_H
