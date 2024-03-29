#
# SPDX-FileCopyrightText: 2009 Johns Hopkins University (JHU), All Rights Reserved.
# SPDX-FileCopyrightText: 2018-2023 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
# SPDX-License-Identifier: LicenseRef-CISST

# --- begin cisst license - do not edit ---
#
# This software is provided "as is" under an open source license, with no warranty.
# The complete license can be found in LICENSES/LicenseRef-CISST.txt.
#
# --- end cisst license ---

set(QNX_VERSION 7.0.0)
set(QNX_PROCESSOR x86_64)

set(QNX_HOST_HINT "C:/qnx700/host/win32/x86")
set(QNX_TARGET_HINT "C:/qnx700/target/qnx6")

set(CMAKE_C_FLAGS "-Vgcc_ntox86_64" CACHE STRING "qcc c flags" FORCE)
set(CMAKE_CXX_FLAGS "-Vgcc_ntox86_64 -lang-c++" CACHE STRING "qcc cxx flags" FORCE)

include("${CMAKE_CURRENT_LIST_DIR}/Toolchain-QNX700-common.cmake")
