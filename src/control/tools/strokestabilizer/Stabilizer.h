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

#include <deque>

#include "control/settings/Settings.h"
#include "control/tools/InputHandler.h"


// Debug
#define STAB_DEBUG


enum StabilizingAlgorithm {
    STABILIZING_NONE,
    STABILIZING_ARITHMETIC_MEAN,
    STABILIZING_GIMP_EURISTICS,
    STABILIZING_DEADZONE
};

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
class Stabilizer {
public:
    Stabilizer() = default;
    virtual ~Stabilizer() = default;

    /**
     * @brief Push the event to the buffer and compute what point(s) to paint
     * @param pos The event to be pushed
     * @param zoom The zoom level to rescale the points' coordinates
     *
     * @return The number of points (segments) to be painted. The points are available in this.pointsToPaint
     *      if negative, no stabilization has been computed.
     */
    virtual int feedMoveEvent(const PositionInputData& pos, double zoom) { return -1; }

    /**
     * @brief Push the event to the buffer
     */
    virtual void pushMoveEvent(const PositionInputData& pos) {}

    /**
     * TODO
     * Finishing strokes
     */
    //     Point getClosingPoint(const PositionInputData& pos);

    /**
     * @brief Contains the points to paint after a feedEvent
     */
    std::deque<Point> pointsToPaint;
};

// Change back to namespace once ugly fix if removed
class StrokeStabilizerFactory {
public:
    // TODO Ugly fix. Remove!
    static StabilizingAlgorithm algorithm;
    static double deadzoneRadius;
    static double twoSigmaSquare;
    static int bufferSize;
    static int eventLifespan;
    static bool cuspDetection;
    static bool averagingOn;

    /**
     * @brief Creates and returns the StrokeStabilizer adapted to the parameters
     * @param pos PositionInputData corresponding to the ButtonDown event triggering the stroke
     *
     * @return The StrokeStabilizer
     */
    static std::unique_ptr<Stabilizer> getStabilizer(PositionInputData const& pos);
};  // namespace StrokeStabilizerFactory
