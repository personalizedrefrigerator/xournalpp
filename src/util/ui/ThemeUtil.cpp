// GPL-2.0-or-later

#include "filesystem.h"

//  Contains work based on GPLv2+ code in Inkscape
// See https://gitlab.com/inkscape/inkscape/-/blob/ec367c61a1b3664f4dff379fe4f5e1dd5d495817/src/ui/themes.cpp
using namespace Util;

std::vector<GtkThemeDescription> getAvailableThemes() {
    GError** err = NULL;
    char** themes = g_resources_enumerate_children("/org/gtk/libgtk/theme", G_RESOURCE_LOOKUP_FLAGS_NONE, err);

    if (err) {
        return {};
    }

    std::vector<GtkThemeDescription> result;

    for (size_t i = 0; themes[i] != NULL; i++) {
        std::string theme { themes[i] };

        // See https://gitlab.com/inkscape/inkscape/-/blob/ec367c61a1b3664f4dff379fe4f5e1dd5d495817/src/ui/themes.cpp#L70
        if (g_str_has_suffix(themes[i], "/")) {
            // Remove the trailing backslash
            theme.resize(theme.size() - 1);

            result.push_back(theme);
        }
    }

    // TODO Support for non-built-in themes (e.g. installed in a user's home
    //  directory). See the Inkscape source code for reference.

    g_strfreev(themes);
    return result;
}

void applyTheme(const GtkThemeDescription& description) {
    GtkSettings* appSettings = gtk_settings_get_default();

    if (description.name == "") {
        gtk_settings_reset_property(appSettings, "gtk-theme-name");
    } else {
        g_object_set(appSettings, "gtk-theme-name", description.name);
    }
}

