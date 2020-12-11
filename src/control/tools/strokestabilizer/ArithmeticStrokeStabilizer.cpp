#include "ArithmeticStrokeStabilizer.h"

#include <cmath>
#include <numeric>

// #include <control/XournalMain.h>
// #include <gdk/gdk.h>

#include "control/tools/StrokeHandler.h"  // TODO Ugly fix, remove!

// Needed?
// #include "control/settings/Settings.h"
// #include "config-features.h"

ArithmeticStrokeStabilizer::ArithmeticStrokeStabilizer(const PositionInputData& pos):
        // TODO read config
        bufferLength(std::max(2, StrokeHandler::stabilizingBufferSize)) {
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
    BufferedEvent sum =
            std::reduce(begin(eventBuffer), end(eventBuffer), BufferedEvent(0.0, 0.0, 0.0), [](auto&& lhs, auto&& rhs) {
                return BufferedEvent(lhs.x + rhs.x, lhs.y + rhs.y, lhs.pressure + rhs.pressure);
            });

    /**
     * Pop the oldest event to keep the bufferLength constant
     */
    eventBuffer.pop_back();

    /**
     * Rescale the averaged coordinates before assigning them to the point
     */
    point->x = sum.x / ((double)bufferLength * zoom);
    point->y = sum.y / ((double)bufferLength * zoom);
    point->z = sum.pressure / (double)bufferLength;
}

void ArithmeticStrokeStabilizer::pushEvent(const PositionInputData& pos) {
    /**
     * Keep the length of the buffer constant
     */
    if (!eventBuffer.empty()) {
        eventBuffer.pop_back();
    } else {
        g_warning("ArithmeticStrokeStabilizer: The buffer is empty. It shouldn't be!");
    }

    eventBuffer.emplace_front(pos.x, pos.y, pos.pressure);
}
