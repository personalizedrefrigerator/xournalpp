/*
 * Xournal++
 *
 *  Dialog for applying transformations/effects to
 * the selection.
 *
 * @author Xournal++ Team
 * https://github.com/xournalpp/xournalpp
 *
 * @license GNU GPLv2 or later
 */

#pragma once

#include "gui/GladeGui.h"

class TransformSelectionDialog : public GladeGui {
public:
    TransformSelectionDialog(GladeSearchpath* gladeSearchPath);
    virtual ~TransformSelectionDialog();

public:
    virtual void show(GtkWindow* parent) override;
};
