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

#include "StrokeStabilizer.h"

class ArithmeticStrokeStabilizer: public StrokeStabilizer {
public:
    ArithmeticStrokeStabilizer(const PositionInputData& pos);
    virtual ~ArithmeticStrokeStabilizer() = default;

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
     * @brief The length of the buffer
     */
    const int bufferLength;

    /**
     * @brief A queue containing the relevant information on the last events
     * The beginning of the queue contains the most recent event
     * The end of the queue contains the most ancient event stored
     *
     * TODO The length is fixed here, so pick a better suited data structure
     * e.g. Array or boost::circular_buffer
     */
    std::deque<BufferedEvent> eventBuffer;
};
