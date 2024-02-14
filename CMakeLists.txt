# This file is part of the GCEmu Project.
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <https://www.gnu.org/licenses/>.

cmake_minimum_required(VERSION 3.25)
project(GCEmu)

if (WIN32)
    add_definitions(-D_WIN32_WINNT=0x0601)
endif()

add_subdirectory("${PROJECT_SOURCE_DIR}/src/loginserver" "${PROJECT_SOURCE_DIR}/build")
