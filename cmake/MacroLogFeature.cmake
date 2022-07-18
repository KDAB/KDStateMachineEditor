# This file defines the Feature Logging macros.
#
# MACRO_LOG_FEATURE(VAR FEATURE DESCRIPTION URL [REQUIRED [MIN_VERSION [COMMENTS]]])
#   Logs the information so that it can be displayed at the end
#   of the configure run
#   VAR : TRUE or FALSE, indicating whether the feature is supported
#   FEATURE: name of the feature, e.g. "libjpeg"
#   DESCRIPTION: description what this feature provides
#   URL: home page
#   REQUIRED: TRUE or FALSE, indicating whether the feature is required
#   MIN_VERSION: minimum version number. empty string if unneeded
#   COMMENTS: More info you may want to provide.  empty string if unnecessary
#
# MACRO_DISPLAY_FEATURE_LOG()
#   Call this to display the collected results.
#   Exits CMake with a FATAL error message if a required feature is missing
#
# Example:
#
# INCLUDE(MacroLogFeature)
#
# FIND_PACKAGE(JPEG)
# MACRO_LOG_FEATURE(JPEG_FOUND "libjpeg" "Support JPEG images" "http://www.ijg.org" TRUE "3.2a" "")
# ...
# MACRO_DISPLAY_FEATURE_LOG()

# SPDX-FileCopyrightText: 2006 Alexander Neundorf, <neundorf@kde.org>
# SPDX-FileCopyrightText: 2006 Allen Winter, <winter@kde.org>
# SPDX-FileCopyrightText: 2009 Sebastian Trueg, <trueg@kde.org>
# SPDX-License-Identifier: BSD-3-Clause

if(NOT _macroLogFeatureAlreadyIncluded)
    set(_file ${CMAKE_BINARY_DIR}/MissingRequirements.txt)
    if(EXISTS ${_file})
        file(REMOVE ${_file})
    endif(EXISTS ${_file})

    set(_file ${CMAKE_BINARY_DIR}/EnabledFeatures.txt)
    if(EXISTS ${_file})
        file(REMOVE ${_file})
    endif(EXISTS ${_file})

    set(_file ${CMAKE_BINARY_DIR}/DisabledFeatures.txt)
    if(EXISTS ${_file})
        file(REMOVE ${_file})
    endif(EXISTS ${_file})

    set(_macroLogFeatureAlreadyIncluded TRUE)

    include(FeatureSummary)

endif(NOT _macroLogFeatureAlreadyIncluded)

macro(MACRO_LOG_FEATURE _var _package _description _url) # _required _minvers _comments)

    string(TOUPPER "${ARGV4}" _required)
    set(_minvers "${ARGV5}")
    set(_comments "${ARGV6}")

    if(${_var})
        set(_LOGFILENAME ${CMAKE_BINARY_DIR}/EnabledFeatures.txt)
    else(${_var})
        if("${_required}" STREQUAL "TRUE")
            set(_LOGFILENAME ${CMAKE_BINARY_DIR}/MissingRequirements.txt)
        else("${_required}" STREQUAL "TRUE")
            set(_LOGFILENAME ${CMAKE_BINARY_DIR}/DisabledFeatures.txt)
        endif("${_required}" STREQUAL "TRUE")
    endif(${_var})

    set(_logtext "   * ${_package}")

    if(NOT ${_var})
        if(${_minvers} MATCHES ".*")
            set(_logtext "${_logtext} (${_minvers} or higher)")
        endif(${_minvers} MATCHES ".*")
        set(_logtext "${_logtext}  <${_url}>\n     ")
    else(NOT ${_var})
        set(_logtext "${_logtext} - ")
    endif(NOT ${_var})

    set(_logtext "${_logtext}${_description}")

    if(NOT ${_var})
        if(${_comments} MATCHES ".*")
            set(_logtext "${_logtext}\n     ${_comments}")
        endif(${_comments} MATCHES ".*")
        #      SET(_logtext "${_logtext}\n") #double-space missing features?
    endif(NOT ${_var})

    file(APPEND "${_LOGFILENAME}" "${_logtext}\n")

    if(COMMAND SET_PACKAGE_INFO) # in FeatureSummary.cmake since CMake 2.8.3
        set_package_info("${_package}" "\"${_description}\"" "${_url}" "\"${_comments}\"")
    endif(COMMAND SET_PACKAGE_INFO)

endmacro(MACRO_LOG_FEATURE)

macro(MACRO_DISPLAY_FEATURE_LOG)
    if(COMMAND FEATURE_SUMMARY) # in FeatureSummary.cmake since CMake 2.8.3
        feature_summary(FILENAME ${CMAKE_CURRENT_BINARY_DIR}/FindPackageLog.txt WHAT ALL)
    endif(COMMAND FEATURE_SUMMARY)

    set(_missingFile ${CMAKE_BINARY_DIR}/MissingRequirements.txt)
    set(_enabledFile ${CMAKE_BINARY_DIR}/EnabledFeatures.txt)
    set(_disabledFile ${CMAKE_BINARY_DIR}/DisabledFeatures.txt)

    if(EXISTS ${_missingFile}
       OR EXISTS ${_enabledFile}
       OR EXISTS ${_disabledFile}
    )
        set(_printSummary TRUE)
    endif(
        EXISTS ${_missingFile}
        OR EXISTS ${_enabledFile}
        OR EXISTS ${_disabledFile}
    )

    if(_printSummary)
        set(_missingDeps 0)
        if(EXISTS ${_enabledFile})
            file(READ ${_enabledFile} _enabled)
            file(REMOVE ${_enabledFile})
            set(_summary
                "${_summary}\n-----------------------------------------------------------------------------\n-- The following external packages were located on your system.\n-- This installation will have the extra features provided by these packages.\n-----------------------------------------------------------------------------\n${_enabled}"
            )
        endif(EXISTS ${_enabledFile})

        if(EXISTS ${_disabledFile})
            set(_missingDeps 1)
            file(READ ${_disabledFile} _disabled)
            file(REMOVE ${_disabledFile})
            set(_summary
                "${_summary}\n-----------------------------------------------------------------------------\n-- The following OPTIONAL packages could NOT be located on your system.\n-- Consider installing them to enable more features from this software.\n-----------------------------------------------------------------------------\n${_disabled}"
            )
        endif(EXISTS ${_disabledFile})

        if(EXISTS ${_missingFile})
            set(_missingDeps 1)
            file(READ ${_missingFile} _requirements)
            set(_summary
                "${_summary}\n-----------------------------------------------------------------------------\n-- The following REQUIRED packages could NOT be located on your system.\n-- You must install these packages before continuing.\n-----------------------------------------------------------------------------\n${_requirements}"
            )
            file(REMOVE ${_missingFile})
            set(_haveMissingReq 1)
        endif(EXISTS ${_missingFile})

        if(NOT ${_missingDeps})
            set(_summary
                "${_summary}\n-----------------------------------------------------------------------------\n-- Congratulations! All external packages have been found."
            )
        endif(NOT ${_missingDeps})

        message(${_summary})
        message("-----------------------------------------------------------------------------\n")

        if(_haveMissingReq)
            message(FATAL_ERROR "Exiting: Missing Requirements")
        endif(_haveMissingReq)

    endif(_printSummary)

endmacro(MACRO_DISPLAY_FEATURE_LOG)
