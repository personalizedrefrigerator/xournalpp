#include "GimplikeStabilizer.h"

#include <cmath>

GimplikeStabilizer::GimplikeStabilizer(double sigma, unsigned int lifespan):
        twoSigmaSquared(2 * sigma * sigma), eventLifespan(lifespan) {
    g_message("Created GimplikeStabilizer with 2σ² = %f and eventLifespan = %d", twoSigmaSquared, eventLifespan);
}

#ifndef STAB_DEBUG
void GimplikeStabilizer::initialize(const PositionInputData& pos) { eventBuffer.emplace_front(pos); }
#else
void GimplikeStabilizer::initialize(const PositionInputData& pos) {
    eventBuffer.emplace_front(pos);
    logBuffer.emplace_front(pos);
    logBuffer.front().status = 'F';
}
#endif

auto GimplikeStabilizer::feedMoveEvent(const PositionInputData& pos, double zoom) -> int {

    /**
     * First, clear the deque of points to paint
     */
    pointsToPaint.clear();

    /**
     * Compute the velocity and pushes the event to eventBuffer
     */
    pushMoveEvent(pos);
#ifdef STAB_DEBUG
    logBuffer.front().status = ' ';
#endif

    /**
     * Flush expired events
     */
#ifdef STAB_DEBUG
    maxBufferSize = std::max(maxBufferSize, bufferSize);
#endif
    if (pos.timestamp < eventLifespan) {  // Could this happen?
        g_warning("StrokeStabilizer: timestamp %d is smaller than event lifetime %d."
                  " Not flushing the buffer.",
                  pos.timestamp, eventLifespan);
    } else {
        const guint32 limitDate = pos.timestamp - eventLifespan;

        // No need to test !eventBuffer.empty() as we just pushed an element failing the test
        while (eventBuffer.back().timestamp < limitDate) {
            eventBuffer.pop_back();
#ifdef STAB_DEBUG
            bufferSize--;
#endif
        }
    }

    /**
     * Average the coordinates using the gimp-like weights
     */
    double weightedSumOfX = 0;
    double weightedSumOfY = 0;
    double weightedSumOfPressures = 0;
    double weight;
    double sumOfWeights = 0;
    double sumOfVelocities = 0;

    for (auto&& event: eventBuffer) {
        /**
         * The first weight is always 1
         */
        weight = exp(-sumOfVelocities * sumOfVelocities / twoSigmaSquared);
        sumOfVelocities += event.velocity;
        weightedSumOfX += weight * event.x;
        weightedSumOfY += weight * event.y;
        weightedSumOfPressures += weight * event.pressure;
        sumOfWeights += weight;
    }

    //     if (sumOfWeights == 0) {  // Sanity check. Should never happen
    //         g_warning("sumOfWeights = %f, bufferSize = %zu, sumOfVelocities = %f, weight = %f, (x,y) = (%f,%f)",
    //                   sumOfWeights, eventBuffer.size(), sumOfVelocities, weight, weightedSumOfX, weightedSumOfY);
    // #ifdef STAB_DEBUG
    //         dumpBuffer();
    // #endif
    //     }

    double averagedPressure = weightedSumOfPressures / sumOfWeights;

    /**
     * Rescale the averaged coordinates before assigning them to the point
     */
    sumOfWeights *= zoom;
    pointsToPaint.emplace_back(weightedSumOfX / sumOfWeights, weightedSumOfY / sumOfWeights, averagedPressure);
    return 1;
}

void GimplikeStabilizer::pushMoveEvent(const PositionInputData& pos) {
#ifdef STAB_DEBUG
    bufferSize++;
    logBuffer.emplace_front(pos);
#endif
    if (eventBuffer.empty()) {
        g_warning("GimplikeStabilizer::pushEvent: Empty buffer");
        eventBuffer.emplace_front(pos);
        return;
    }

    /**
     * Compute the velocity using timestamps and the previous measure point
     * Issue: timestamps are in ms. They don't seem to be precise enough. Different events often have the same
     * timestamp...
     */
    GimplikeBufferedEvent& lastEvent = eventBuffer.front();

    guint32 timelaps = pos.timestamp - lastEvent.timestamp;
    if (timelaps == 0) {
        g_warning("Oh oh: Events with same timestamps: %d\n  lastEvent.(x,y) = (%f,%f)\n   "
                  "newEvent.(x,y) = (%f,%f)",
                  pos.timestamp, lastEvent.x, lastEvent.y, pos.x, pos.y);
        timelaps = 1;
    }

    /**
     * Create and push the event
     * See GimplikeBufferedEvent::GimplikeBufferedEvent(...)
     */
    eventBuffer.emplace_front(pos, hypot(pos.x - lastEvent.x, pos.y - lastEvent.y) / ((double)timelaps));
}

// #ifdef STAB_DEBUG
// void GimplikeStabilizer::dumpBuffer() {
//     int i = 0;
//     double sumOfVelocities = 0, weight = 0;
//     printf("Buffer:\n id | timestamp |     x     |     y     |     P     |     v     |    sumV    |     w     |\n");
//     for (auto&& event: eventBuffer) {
//         sumOfVelocities += event.velocity;  // Mimicking Gimp's formula
//         weight = exp(-sumOfVelocities * sumOfVelocities / twoSigmaSquared);
//         printf(" %2d | %9d | %9.5f | %9.5f | %9.6f | %9.6f | %10.6f | %9.6f |\n", ++i, event.timestamp, event.x,
//         event.y,
//                event.pressure, event.velocity, sumOfVelocities, weight);
//     }
// }
// #endif

auto GimplikeStabilizer::getLastEvent() -> BufferedEvent { return eventBuffer.front(); }
