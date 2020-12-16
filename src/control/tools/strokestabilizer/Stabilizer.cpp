#include "Stabilizer.h"

#include <cmath>

#include "control/settings/Settings.h"

#include "ArithmeticStabilizer.h"
#include "DeadzoneStabilizer.h"
#include "GimplikeStabilizer.h"


auto StrokeStabilizerFactory::getStabilizer(Settings* settings) -> std::unique_ptr<Stabilizer> {

    StabilizingAlgorithm algorithm = settings->getStabilizerAlgorithm();
    if (algorithm == STABILIZING_ARITHMETIC_MEAN) {
        return std::make_unique<ArithmeticStabilizer>(settings->getStabilizerBuffersize());
    }

    if (algorithm == STABILIZING_GIMP_EURISTICS) {
        return std::make_unique<GimplikeStabilizer>(settings->getStabilizerSigma(),
                                                    settings->getStabilizerEventLifespan());
    }

    if (algorithm == STABILIZING_DEADZONE) {
        return std::make_unique<DeadzoneStabilizer>(
                settings->getStabilizerDeadzoneRadius(), settings->getStabilizerCuspDetetion(),
                settings->getStabilizerDeadzoneAveraging(), settings->getStabilizerSigma(),
                settings->getStabilizerEventLifespan());
    }

    /**
     * Defaults to no stabilization
     */
    g_message("Creating default Stabilizer");
    return std::make_unique<Stabilizer>();
}
