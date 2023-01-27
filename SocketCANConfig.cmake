# Find Linux SocketCAN file for cmake
#
# Variables:
#   SOCKETCAN_INCLUDE_DIR  Include path for SocketCAN
#   SOCKETCAN_FOUND        TRUE if SocketCAN was found
#
# Author: Davide Rondini
# License: GPL 2.0 License
# SPDX-FileCopyrightText: 2023 Davide Rondini <info@drondini.com>
# SPDX-License-Identifier: GPL-2.0-or-later

if (SOCKETCAN_INCLUDE_DIR)
#  # Already in cache, be silent
  set(SOCKETCAN_FIND_QUIETLY TRUE)
endif (SOCKETCAN_INCLUDE_DIR)

if(NOT UNIX)
	set(SOCKETCAN_FOUND FALSE)
endif(NOT UNIX)

find_path(SOCKETCAN_INCLUDE_DIR linux/can.h
	${LINUX_KERNEL_HEADERS_DIR}
	/usr/include
)
message(STATUS "SocketCAN include path: ${SOCKETCAN_INCLUDE_DIR}")

if(SOCKETCAN_INCLUDE_DIR)
	set(SOCKETCAN_FOUND TRUE)
else(SOCKETCAN_INCLUDE_DIR)
  set(SOCKETCAN_FOUND FALSE)
endif(SOCKETCAN_INCLUDE_DIR)

if(SOCKETCAN_FOUND)
   if(NOT SOCKETCAN_FIND_QUIETLY)
      message(STATUS "Found SocketCAN")
   endif(NOT SOCKETCAN_FIND_QUIETLY)
else(SOCKETCAN_FOUND)
   if(SOCKETCAN_FIND_REQUIRED)
      message(STATUS "Looked for SocketCAN")
      message(FATAL_ERROR "Could NOT find SocketCAN library")
   endif(SOCKETCAN_FIND_REQUIRED)
endif(SOCKETCAN_FOUND)

mark_as_advanced(
  SOCKETCAN_INCLUDE_DIR
)

