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

#include <cmath>
#include <deque>
#include <map>

#include "GimplikeStabilizer.h"


#ifdef STAB_DEBUG
#include <string>
#endif

/**
 * Extend the class if need be
 */
using DeadzoneBufferedEvent = GimplikeBufferedEvent;

struct MathVect {
    double dx{};
    double dy{};
    static double scalarProduct(MathVect u, MathVect v) { return u.dx * v.dx + u.dy * v.dy; }
    double norm() { return hypot(dx, dy); }
    bool nonZero() { return (dx != 0) || (dy != 0); }
};

class DeadzoneStabilizer: public Stabilizer {
public:
    DeadzoneStabilizer(const PositionInputData& pos);
#ifndef STAB_DEBUG
    virtual ~DeadzoneStabilizer() = default;
#else
    virtual ~DeadzoneStabilizer() { showBufferLength(); }

    void showBufferLength() {
        string str = "Buffer length:\n";
        for (size_t i = 0; i < nbInBuffer.size(); i++) {
            str += " " + std::to_string(nbInBuffer[i]);
        }
        g_message("%s", str.c_str());
    }
#endif

    /**
     * @brief Push the event to the buffer and stabilizes (in place) the coordinates of *point
     * @param pos The event to be pushed
     * @param zoom The zoom level to rescale the point's coordinates
     */
    virtual int feedMoveEvent(const PositionInputData& pos, double zoom);

    /**
     * @brief Push the event to the buffer
     */
    virtual void pushMoveEvent(const PositionInputData& pos);

private:
    /**
     * @brief Push an event to the secondary averaging buffer
     * @param event The event to push
     */
    void pushToAveragingBuffer(DeadzoneBufferedEvent& event);

    /**
     * @brief Create and push an event to the secondary averaging buffer
     * @param x The x coordinate of the event
     * @param y The y coordinate of the event
     * @param pressure The pressure value at the event
     * @param timestamp The timestamp of the event
     */
    void emplaceInAveragingBuffer(double x, double y, double pressure, guint32 timestamp);

    /**
     * @brief Average the events in eventBuffer and push the obtained point to pointsToPaint. Updates lastPaintedEvent.
     * @param zoom The zoom to apply to the point
     */
    void paintAverage(double zoom);

    /**
     * @brief The deadzone radius
     */
    const double deadzoneRadius;

    /**
     * @brief The Gaussian parameter
     */
    const double twoSigmaSquared;

    /**
     * @brief The time (in ms) an event will be kept in the buffer
     */
    const guint32 eventLifespan;

    /**
     * @brief Flag to turn on/off the cusp detection
     */
    const bool cuspDetectionOn;

    /**
     * @brief Flag to turn on/off the cusp detection
     */
    const bool averagingOn;

    /**
     * @brief A queue containing the relevant information on the last events
     * The beginning of the queue contains the most recent event
     * The end of the queue contains the most ancient event stored
     *
     * TODO Find faster data structure?
     */
    std::deque<DeadzoneBufferedEvent> eventBuffer;

    /**
     * @brief The last (stabilized) painted point, center of the deadzone
     */
    BufferedEvent lastPaintedEvent;

    /**
     * @brief The direction of the last event outside the deadzone
     */
    MathVect lastLiveDirection;

    /**
     * @brief The last event outside the deadzone
     */
    DeadzoneBufferedEvent lastLiveEvent;

#ifdef STAB_DEBUG
    std::vector<int> nbInBuffer;
    int bufferSize = 0;
#endif
};
