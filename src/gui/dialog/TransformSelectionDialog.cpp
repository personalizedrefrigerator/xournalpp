#include "TransformSelectionDialog.h"
#include "gui/GladeGui.h"

TransformSelectionDialog::TransformSelectionDialog(GladeSearchpath* gladeSearchPath)
        : GladeGui(gladeSearchPath, "applyEffectDialog.glade", "applyTransformationDialog")
{

}

TransformSelectionDialog::~TransformSelectionDialog() { }

void TransformSelectionDialog::show(GtkWindow* parent)
{
    gtk_window_set_transient_for(GTK_WINDOW(this->window), parent);
    int result = gtk_dialog_run(GTK_DIALOG(this->window));
    gtk_widget_hide(this->window);

    // OK Button
    if (result == 1) {
        // Do stuff.
    } else {
        // Don't do stuff
    }
}
