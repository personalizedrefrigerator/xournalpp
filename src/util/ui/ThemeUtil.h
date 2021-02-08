/*
 * Xournal++
 *
 * Color utility, does color conversions
 *
 * @author Xournal++ Team
 * https://github.com/xournalpp/xournalpp
 *
 * @license GNU GPLv2 or later
 */

#pragma once

#include <vector>
#include <string>

namespace Util
{

struct GtkThemeDescription {
    // According to https://developer.gnome.org/gtk3/stable/GtkSettings.html#GtkSettings--gtk-theme-name,
    // gtk-tmeme-name is a char*, so we're using a std::string here.
    std::string name;
    // bool supportsDarkTheme;
}

[[maybe_unused]] std::vector<GtkThemeDescription> getAvailableThemes();

/**
 * Applies the given theme or resets the theme to the default if 
 * the name of the given theme is empty.
 *
 * @param theme The theme to apply.
 */
[[maybe_unused]] void applyTheme(const GtkThemeDescription&);

/**
 * Set whether we should try to use a dark variant of
 * the current GTK theme.
 */
void setPreferDarkTheme(bool preferDarkTheme);

}