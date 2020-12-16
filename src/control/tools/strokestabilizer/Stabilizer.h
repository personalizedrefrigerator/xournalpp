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

#include "control/tools/InputHandler.h"

class Settings;

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
     * @brief Initialize the stabilizer
     * @param pos The position of the button down event starting the stroke
     */
    virtual void initialize(const PositionInputData& pos) {}

    /**
     * @brief Push the event to the buffer and compute what point(s) to paint
     * @param pos The event to be pushed
     * @param zoom The zoom level to rescale the points' coordinates
     *
     * @return The number of points (segments) to be painted. The points are available in this.pointsToPaint
     *      if negative, no stabilization has been computed.
     *
     * Does nothing in the base class
     */
    virtual int feedMoveEvent(const PositionInputData& pos, double zoom) { return -1; }

    /**
     * @brief Push the event to the buffer
     * Does nothing in the base class
     */
    virtual void pushMoveEvent(const PositionInputData& pos) {}

    /**
     * @brief Contains the points to paint after a feedMoveEvent or a finishStroke
     * Always empty in the base class
     */
    std::deque<Point> pointsToPaint;
};

namespace StrokeStabilizerFactory {
/**
 * @brief Creates and returns the Stabilizer adapted to the parameters
 * @param settings Pointer to the Settings instance from which to read the parameters
 *
 * @return The Stabilizer
 */
std::unique_ptr<Stabilizer> getStabilizer(Settings* settings);
};  // namespace StrokeStabilizerFactory
