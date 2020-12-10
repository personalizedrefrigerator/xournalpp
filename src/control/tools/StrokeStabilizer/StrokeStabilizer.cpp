#include "StrokeStabilizer.h"

#include <cmath>

// #include <control/XournalMain.h>
// #include <gdk/gdk.h>

// Needed?
// #include "control/settings/Settings.h"
// #include "config-features.h"

#include "control/tools/StrokeHandler.h"  // TODO Ugly fix, remove

#include "ArithmeticStrokeStabilizer.h"
#include "GimplikeStrokeStabilizer.h"


auto StrokeStabilizerFactory::getStabilizer(const PositionInputData& pos) -> StrokeStabilizer* {
    /**
     * Figure out how config is done
     */
    if (StrokeHandler::stabilizingAlgorithm == STABILIZING_GIMP_EURISTICS) {
        g_message("Creating GimplikeStrokeStabilizer");
        return new GimplikeStrokeStabilizer(pos);
    }

    if (StrokeHandler::stabilizingAlgorithm == STABILIZING_ARITHMETIC_MEAN) {
        g_message("Creating ArithmeticStrokeStabilizer");
        return new ArithmeticStrokeStabilizer(pos);
    }

    /**
     * Defaults to no stabilization
     */
    g_message("Creating default StrokeStabilizer");
    return new StrokeStabilizer();
}
