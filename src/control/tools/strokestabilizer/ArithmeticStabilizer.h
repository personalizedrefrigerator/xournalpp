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

#include "Stabilizer.h"

class ArithmeticStabilizer: public Stabilizer {
public:
    ArithmeticStabilizer(size_t buffersize);
    virtual ~ArithmeticStabilizer() = default;

    /**
     * @brief Initialize the stabilizer
     * @param pos The position of the button down event starting the stroke
     */
    virtual void initialize(const PositionInputData& pos);

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
     * @brief The length of the buffer
     */
    size_t const bufferLength;

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
