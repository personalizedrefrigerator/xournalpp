#include "ArithmeticStrokeStabilizer.h"

#include <cmath>

// #include <control/XournalMain.h>
// #include <gdk/gdk.h>

#include "control/tools/StrokeHandler.h"  // TODO Ugly fix, remove!

// Needed?
// #include "control/settings/Settings.h"
// #include "config-features.h"

ArithmeticStrokeStabilizer::ArithmeticStrokeStabilizer(const PositionInputData& pos):
        // TODO read config
        bufferLength(StrokeHandler::stabilizingBufferSize) {
    /**
     * Fill the queue with copies of our starting point
     * Is this the best way to do this? We could also aggregate the first events as they come
     * but this would add a test (on the length of the queue) at every event triggered.
     */
    for (int i = 1; i < bufferLength; i++) {
        // The next event will be pushed before averaging, so there will be exactly bufferLength elements
        // in the queue then.
        eventBuffer.emplace_front(pos.x, pos.y, pos.pressure);
    }
    g_message("Created ArithmeticStrokeStabilizer with bufferLength = %d", bufferLength);
}

void ArithmeticStrokeStabilizer::stabilizePointUsingEvent(const PositionInputData& pos, double zoom, Point* point) {

    /**
     * Create and push the event
     */
    eventBuffer.emplace_front(pos.x, pos.y, pos.pressure);

    /**
     * Average the coordinates using an arithmetic mean
     */
    double sumOfX = 0;
    double sumOfY = 0;
    double sumOfPressures = 0;

    // Type of it: std::deque<BufferedEvent>::const_iterator
    for (auto it = eventBuffer.cbegin(); it != eventBuffer.cend(); ++it) {
        sumOfX += (*it).x;
        sumOfY += (*it).y;
        sumOfPressures += (*it).pressure;
    }

    /**
     * Pop the oldest event to keep the bufferLength constant
     */
    eventBuffer.pop_back();

    /**
     * Rescale the averaged coordinates before assigning them to the point
     */
    point->x = sumOfX / ((double)bufferLength * zoom);
    point->y = sumOfY / ((double)bufferLength * zoom);
    point->z = sumOfPressures / (double)bufferLength;
}

void ArithmeticStrokeStabilizer::pushEvent(const PositionInputData& pos) {
    eventBuffer.emplace_front(pos.x, pos.y, pos.pressure);
    /**
     * Keep the length of the buffer constant
     */
    eventBuffer.pop_back();
}
