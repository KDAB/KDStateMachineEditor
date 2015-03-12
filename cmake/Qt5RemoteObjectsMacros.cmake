# This file is part of the KDAB State Machine Editor Library.
#
# Copyright (C) 2014-2015 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com.
# All rights reserved.
# Author: Kevin Funk <kevin.funk@kdab.com>
#
# Licensees holding valid commercial KDAB State Machine Editor Library
# licenses may use this file in accordance with the KDAB State Machine Editor
# Library License Agreement provided with the Software.
#
# This file may be distributed and/or modified under the terms of the
# GNU Lesser General Public License version 2.1 as published by the
# Free Software Foundation and appearing in the file LICENSE.LGPL.txt included.
#
# This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
# WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
#
# Contact info@kdab.com if any conditions of this licensing are not
# clear to you.

# Macros
# =========
#
# In some cases it can be necessary or useful to invoke the Qt build tools in a
# more-manual way. Several macros are available to add targets for such uses.
#
# ::
#
#   macro qt5_generate_repc(outfiles infile outputtype)
#         out: outfiles
#         in: infile outputtype
#         create C++ code from a list of .rep files. Per-directory preprocessor
#         definitions are also added.
#         infile should be a replicant template file (.rep)
#         outputtype specifies output file type, it can be one of SOURCE|REPLICA
#         Example usage: qt5_generate_repc(LIB_SRCS interface.rep SOURCE)
#           for generating interface_source.h and adding it to LIB_SRCS

get_target_property(_moc_executable Qt5::moc LOCATION)
set(_search_dirs "${_moc_executable}/..")

find_program(Qt5RemoteObjects_REPC_EXECUTABLE repc PATHS ${_search_dirs})
if(NOT Qt5RemoteObjects_REPC_EXECUTABLE)
    message(FATAL_ERROR "repc executable not found in dirs: ${_search_dirs} -- Check installation.")
endif()

macro(qt5_generate_repc outfiles infile outputtype)
    # get include dirs and flags
    get_filename_component(abs_infile ${infile} ABSOLUTE)
    get_filename_component(infile_name "${infile}" NAME)
    string(REPLACE ".rep" "" _infile_base ${infile_name})
    if(${outputtype} STREQUAL "SOURCE")
        set(_outfile_base "${_infile_base}_source")
        set(_repc_args -o source)
    else()
        set(_outfile_base "${_infile_base}_replica")
        set(_repc_args -o replica)
    endif()
    set(_outfile_header "${CMAKE_CURRENT_BINARY_DIR}/${_outfile_base}.h")
    add_custom_command(OUTPUT ${_outfile_header}
        DEPENDS ${abs_infile}
        COMMAND ${Qt5RemoteObjects_REPC_EXECUTABLE} ${abs_infile} ${_repc_args} ${_outfile_header}
        VERBATIM)
    set_source_files_properties(${_outfile_header} PROPERTIES GENERATED TRUE)

    qt5_get_moc_flags(_moc_flags)
    # Make sure we get the compiler flags from the Qt5::RemoteObjects target (for includes)
    string(REPLACE ";" ";-I" _includes "${Qt5RemoteObjects_INCLUDE_DIRS}")
    set(_includes "-I${_includes}")
    list(APPEND _moc_flags ${_includes})

    set(_moc_outfile "${CMAKE_CURRENT_BINARY_DIR}/moc_${_outfile_base}.cpp")
    qt5_create_moc_command(${_outfile_header} ${_moc_outfile} "${_moc_flags}" "" "")
    list(APPEND ${outfiles} "${_outfile_header}" ${_moc_outfile})
endmacro()
