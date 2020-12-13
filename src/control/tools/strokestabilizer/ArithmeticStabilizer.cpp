#include "ArithmeticStabilizer.h"

#include <cmath>
#include <numeric>

// #include <control/XournalMain.h>
// #include <gdk/gdk.h>

// Needed?
// #include "control/settings/Settings.h"
// #include "config-features.h"

ArithmeticStabilizer::ArithmeticStabilizer(const PositionInputData& pos):
        // TODO read config
        bufferLength(std::max(2, StrokeStabilizerFactory::bufferSize)) {
    /**
     * Fill the queue with copies of our starting point
     * Is this the best way to do this? We could also aggregate the first events as they come
     * but this would add a test (on the length of the queue) at every event triggered.
     */
    for (size_t i = 1; i < bufferLength; i++) {
        // The next event will be pushed before averaging, so there will be exactly bufferLength elements
        // in the queue then.
        eventBuffer.emplace_front(pos.x, pos.y, pos.pressure);
    }
    g_message("Created ArithmeticStabilizer with bufferLength = %zu", bufferLength);
}

int ArithmeticStabilizer::feedMoveEvent(const PositionInputData& pos, double zoom) {

    /**
     * Create and push the event
     */
    eventBuffer.emplace_front(pos.x, pos.y, pos.pressure);

    /**
     * Average the coordinates using an arithmetic mean
     */
    BufferedEvent sum = std::accumulate(
            begin(eventBuffer), end(eventBuffer), BufferedEvent(0.0, 0.0, 0.0), [](auto&& lhs, auto&& rhs) {
                return BufferedEvent(lhs.x + rhs.x, lhs.y + rhs.y, lhs.pressure + rhs.pressure);
            });

    /**
     * Pop the oldest event to keep the bufferLength constant
     */
    eventBuffer.pop_back();

    pointsToPaint.clear();
    /**
     * Rescale the averaged coordinates before assigning them to the point
     */
    pointsToPaint.emplace_back(sum.x / ((double)bufferLength * zoom), sum.y / ((double)bufferLength * zoom),
                               sum.pressure / (double)bufferLength);
    return 1;
}

void ArithmeticStabilizer::pushMoveEvent(const PositionInputData& pos) {
    /**
     * Keep the length of the buffer constant
     */
    if (!eventBuffer.empty()) {
        eventBuffer.pop_back();
    } else {
        g_warning("ArithmeticStabilizer: The buffer is empty. It should never be!");
    }

    eventBuffer.emplace_front(pos.x, pos.y, pos.pressure);
}
