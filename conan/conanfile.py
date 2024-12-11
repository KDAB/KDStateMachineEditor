# This file is part of the KDAB State Machine Editor Library.
#
# SPDX-FileCopyrightText: 2019 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
# Author: Renato Araujo Oliveira Filho <renato.araujo@kdab.com>
#
# SPDX-License-Identifier: LGPL-2.1-only OR LicenseRef-KDAB-KDStateMachineEditor
#
# Licensees holding valid commercial KDAB State Machine Editor Library
# licenses may use this file in accordance with the KDAB State Machine Editor
# Library License Agreement provided with the Software.
#
# Contact info@kdab.com if any conditions of this licensing are not clear to you.
#

from conans import ConanFile, CMake, tools

class KDStateMachineEditorConan(ConanFile):
    name = "KDStateMachineEditor"
    version = "2.0.0"
    license = ("https://raw.githubusercontent.com/KDAB/KDStateMachineEditor/{0}/LICENSES/LicenseRef-KDAB-KDStateMachineEditor.txt,"
               "https://raw.githubusercontent.com/KDAB/KDStateMachineEditor/{0}/LICENSES/LGPL-2.1-only.txt,"
               "https://raw.githubusercontent.com/KDAB/KDStateMachineEditor/{0}/LICENSES/BSD-3-Clause.txt").format(version)
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
        git.checkout("v{0}".format(self.version))

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
        self.cmake.definitions["KDSME_INTERNAL_GRAPHVIZ"] = True
        self.cmake.definitions["KDSME_EXAMPLES"] = self.options.build_examples
        self.cmake.definitions["KDSME_DOCS"] = self.options.build_docs
        self.cmake.definitions["BUILD_TESTING"] = self.options.build_tests
        self.cmake.configure()
        self.cmake.build()

    def package(self):
        self.cmake.install()

    def package_info(self):
        self.env_info.CMAKE_PREFIX_PATH.append(self.package_folder)
