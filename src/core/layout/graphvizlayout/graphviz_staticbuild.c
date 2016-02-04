#include <graphviz/gvplugin.h>
#include <graphviz/gvc.h>

extern gvplugin_library_t gvplugin_dot_layout_LTX_library;

lt_symlist_t lt_preloaded_symbols[] = {
    { "gvplugin_dot_layout_LTX_library", (void*)(&gvplugin_dot_layout_LTX_library) },
    { 0, 0 }
};

GVC_t* gvContextWithStaticPlugins()
{
    return gvContextPlugins(lt_preloaded_symbols, 0);
}
