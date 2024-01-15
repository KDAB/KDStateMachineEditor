/*
  This file is part of the KDAB State Machine Editor Library.

  SPDX-FileCopyrightText: 2016 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Kevin Funk <kevin.funk@kdab.com>

  SPDX-License-Identifier: LGPL-2.1-only OR LicenseRef-KDAB-KDStateMachineEditor

  Licensees holding valid commercial KDAB State Machine Editor Library
  licenses may use this file in accordance with the KDAB State Machine Editor
  Library License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
*/

#include <graphviz/gvplugin.h>
#include <graphviz/gvc.h>

extern gvplugin_library_t gvplugin_dot_layout_LTX_library;

lt_symlist_t lt_preloaded_symbols[] = {
    { "gvplugin_dot_layout_LTX_library", ( void * )(&gvplugin_dot_layout_LTX_library) },
    { 0, 0 }
};

GVC_t *gvContextWithStaticPlugins()
{
    return gvContextPlugins(lt_preloaded_symbols, 0);
}
