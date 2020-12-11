/*
 * Xournal++
 *
 * Handles input of strokes
 *
 * @author Xournal++ Team
 * https://github.com/xournalpp/xournalpp
 *
 * @license GNU GPLv2 or later
 */

#pragma once

#include "control/settings/Settings.h"
#include "control/tools/InputHandler.h"


enum StabilizingAlgorithm { STABILIZING_NONE, STABILIZING_ARITHMETIC_MEAN, STABILIZING_GIMP_EURISTICS };

struct BufferedEvent {
    BufferedEvent() = default;
    BufferedEvent(double x, double y, double pressure): x(x), y(y), pressure(pressure) {}
    double x{};
    double y{};
    double pressure{};
};

/**
 * @brief A parent class, used as default (no stabilization)
 */
class StrokeStabilizer {
public:
    StrokeStabilizer() = default;
    virtual ~StrokeStabilizer() = default;

    /**
     * @brief Push the event to the buffer and stabilizes (in place) the coordinates of *point
     * @param pos The event to be pushed
     * @param zoom The zoom level to rescale the point's coordinates
     * @param point A pointer to the point being stabilized
     */
    virtual void stabilizePointUsingEvent(const PositionInputData& pos, double zoom, Point* point) {}

    /**
     * @brief Push the event to the buffer
     */
    virtual void pushEvent(const PositionInputData& pos) {}

    /**
     * TODO
     * Finishing strokes
     */
    //     Point getClosingPoint(const PositionInputData& pos);
};

namespace StrokeStabilizerFactory {
/**
 * @brief Creates and returns the StrokeStabilizer adapted to the parameters
 * @param pos PositionInputData corresponding to the ButtonDown event triggering the stroke
 *
 * @return The StrokeStabilizer
 */
std::unique_ptr<StrokeStabilizer> getStabilizer(PositionInputData const& pos);
};  // namespace StrokeStabilizerFactory
