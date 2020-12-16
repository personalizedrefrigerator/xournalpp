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
#include <map>

#include "Stabilizer.h"


struct GimplikeBufferedEvent: BufferedEvent {
    GimplikeBufferedEvent() = default;
    GimplikeBufferedEvent(const PositionInputData& pos, double velocity = 0):
            BufferedEvent(pos.x, pos.y, pos.pressure), velocity(velocity), timestamp(pos.timestamp) {}
    GimplikeBufferedEvent(double x, double y, double pressure, double velocity, guint32 timestamp):
            BufferedEvent(x, y, pressure), velocity(velocity), timestamp(timestamp) {}
    double velocity{};
    guint32 timestamp{};
};

class GimplikeStabilizer: public Stabilizer {
public:
    GimplikeStabilizer(double sigma, unsigned int lifespan);
#ifndef STAB_DEBUG
    virtual ~GimplikeStabilizer() = default;
#else
    virtual ~GimplikeStabilizer() { g_message("maxBufferSize = %d", std::max(maxBufferSize, bufferSize)); }
#endif

    /**
     * @brief Initialize the stabilizer
     * @param pos The position of the button down event starting the stroke
     */
    virtual void initialize(const PositionInputData& pos);

    /**
     * @brief Push the event to the buffer and compute what point(s) to paint
     * @param pos The event to be pushed
     * @param zoom The zoom level to rescale the points' coordinates
     *
     * @return The number of points (segments) to be painted. The points are available in this.pointsToPaint
     *      if negative, no stabilization has been computed.
     */
    virtual int feedMoveEvent(const PositionInputData& pos, double zoom);

    /**
     * @brief Push the event to the buffer
     */
    virtual void pushMoveEvent(const PositionInputData& pos);

private:
    /**
     * @brief The Gaussian parameter
     */
    const double twoSigmaSquared;

    /**
     * @brief The time (in ms) an event will be kept in the buffer
     */
    const guint32 eventLifespan;

    /**
     * @brief A queue containing the relevant information on the last events
     * The beginning of the queue contains the most recent event
     * The end of the queue contains the most ancient event stored
     *
     * TODO Find faster data structure? Is std::vector faster?
     */
    std::deque<GimplikeBufferedEvent> eventBuffer;

#ifdef STAB_DEBUG
    int maxBufferSize = 0;
    int bufferSize = 0;
#endif
};
