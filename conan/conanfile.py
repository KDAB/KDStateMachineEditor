###############################################################################
##  This file is part of the KDAB State Machine Editor Library.
##
##  Copyright (C) 2014-2020 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com.
##  All rights reserved.
##
##  Licensees holding valid commercial KDAB State Machine Editor Library
##  licenses may use this file in accordance with the KDAB State Machine Editor
##  Library License Agreement provided with the Software.
##
##  This file may be distributed and/or modified under the terms of the
##  GNU Lesser General Public License version 2.1 as published by the
##  Free Software Foundation and appearing in the file LICENSE.LGPL.txt included.
##
##  This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
##  WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
##
##  Contact info@kdab.com if any conditions of this licensing are not
##  clear to you.
###############################################################################

from conans import ConanFile, CMake, tools

class KDStateMachineEditorConan(ConanFile):
    name = "KDStateMachineEditor"
    version = "1.2.8"
    license = ("https://raw.githubusercontent.com/KDAB/KDStateMachineEditor/{0}/LICENSE.txt,"
               "https://raw.githubusercontent.com/KDAB/KDStateMachineEditor/{0}/LICENSE.LGPL.txt,"
               "https://raw.githubusercontent.com/KDAB/KDStateMachineEditor/{0}/LICENSE.BSD3.txt").format(version)
    author = "Klaralvdalens Datakonsult AB (KDAB) info@kdab.com"
    url = "https://github.com/KDAB/KDStateMachineEditor.git"
    description = "The KDAB State Machine Editor Library is a framework that can be used to help develop full-featured State Machine Editing graphical user interfaces and tools."
    generators = "cmake"
    options = dict({
        "build_examples": [True, False],
        "build_docs": [True, False],
        "build_tests": [True, False]
    })
    default_options = dict({
        "build_examples": False,
        "build_docs": False,
        "build_tests": False
    })

    def requirements(self):
        self.requires("qt/5.13.2@kdab/stable")

    def source(self):
        git = tools.Git(folder="")
        git.clone(self.url)
        git.checkout("%s"%self.version)

    def configure(self):
        # Use kdab flags to match qt package hash
        # ~$ conan create -ks -o qt:qttools=True -o qt:qtsvg=True -o qt:qtdeclarative=True -o qt:qtremoteobjects=True -o qt:qtscxml=True . 5.13.2@kdab/stable
        self.options["qt"].qtsvg = True
        self.options["qt"].qtdeclarative = True
        self.options["qt"].qtremoteobjects = True
        self.options["qt"].qtscxml = True
        self.options["qt"].qttools = True

    def build(self):
        self.cmake = CMake(self)
        self.cmake.definitions["WITH_INTERNAL_GRAPHVIZ"] = True
        self.cmake.definitions["BUILD_EXAMPLES"] = self.options.build_examples
        self.cmake.definitions["BUILD_DOCS"] = self.options.build_docs
        self.cmake.definitions["BUILD_TESTS"] = self.options.build_tests
        self.cmake.configure()
        self.cmake.build()

    def package(self):
        self.cmake.install()

    def package_info(self):
        self.env_info.CMAKE_PREFIX_PATH.append(self.package_folder)
