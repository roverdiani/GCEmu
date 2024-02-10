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

#ifndef GCEMU_LOGINOPCODES_H
#define GCEMU_LOGINOPCODES_H

enum LoginOpcodes
{
    EVENT_HEART_BIT_NOT         = 0x0000,
    EVENT_ACCEPT_CONNECTION_NOT = 0x0001,
    ENU_VERIFY_ACCOUNT_REQ      = 0x0002,
    ENU_WAIT_TIME_NOT           = 0x0005,
};

#endif //GCEMU_LOGINOPCODES_H
