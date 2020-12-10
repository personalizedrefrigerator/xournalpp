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

#include "StrokeStabilizer.h"


#define STAB_DEBUG

#ifdef STAB_DEBUG
#include <string>
#endif


struct GimplikeBufferedEvent: BufferedEvent {
    GimplikeBufferedEvent() = default;
    GimplikeBufferedEvent(double x, double y, double pressure, double velocity, guint32 timestamp):
            BufferedEvent(x, y, pressure), velocity(velocity), timestamp(timestamp) {}
    double velocity{};
    guint32 timestamp{};
};

class GimplikeStrokeStabilizer: public StrokeStabilizer {
public:
    GimplikeStrokeStabilizer(const PositionInputData& pos);
#ifndef STAB_DEBUG
    virtual ~GimplikeStrokeStabilizer() = default;
#else
    virtual ~GimplikeStrokeStabilizer() {
        string str = "Buffer length:\n";
        for (int i = 0; i < nbInBuffer.size(); i++) {
            str += " " + std::to_string(nbInBuffer[i]);
        }
        g_message(str.c_str());
    }
#endif

    /**
     * @brief Push the event to the buffer and stabilizes (in place) the coordinates of *point
     * @param pos The event to be pushed
     * @param zoom The zoom level to rescale the point's coordinates
     * @param point A pointer to the point being stabilized
     */
    virtual void stabilizePointUsingEvent(const PositionInputData& pos, double zoom, Point* point);

    /**
     * @brief Push the event to the buffer
     */
    virtual void pushEvent(const PositionInputData& pos);

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
    std::vector<int> nbInBuffer;
    int bufferSize = 0;
#endif
};
