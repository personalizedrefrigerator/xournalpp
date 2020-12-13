#include "Stabilizer.h"

#include <cmath>

// #include <control/XournalMain.h>
// #include <gdk/gdk.h>

// Needed?
// #include "control/settings/Settings.h"
// #include "config-features.h"

#include "ArithmeticStabilizer.h"
#include "DeadzoneStabilizer.h"
#include "GimplikeStabilizer.h"


// Ugly fix. Remove!
StabilizingAlgorithm StrokeStabilizerFactory::algorithm;
double StrokeStabilizerFactory::deadzoneRadius;
double StrokeStabilizerFactory::twoSigmaSquare;
int StrokeStabilizerFactory::bufferSize;
int StrokeStabilizerFactory::eventLifespan;
bool StrokeStabilizerFactory::cuspDetection;
bool StrokeStabilizerFactory::averagingOn;


auto StrokeStabilizerFactory::getStabilizer(PositionInputData const& pos) -> std::unique_ptr<Stabilizer> {
    /**
     * TODO Figure out how Settings work
     */
    if (algorithm == STABILIZING_GIMP_EURISTICS) {
        g_message("Creating GimplikeStabilizer");
        return std::make_unique<GimplikeStabilizer>(pos);
    }

    if (algorithm == STABILIZING_ARITHMETIC_MEAN) {
        g_message("Creating ArithmeticStabilizer");
        return std::make_unique<ArithmeticStabilizer>(pos);
    }

    if (algorithm == STABILIZING_DEADZONE) {
        g_message("Creating DeadzoneStabilizer");
        return std::make_unique<DeadzoneStabilizer>(pos);
    }

    /**
     * Defaults to no stabilization
     */
    g_message("Creating default Stabilizer");
    return std::make_unique<Stabilizer>();
}
