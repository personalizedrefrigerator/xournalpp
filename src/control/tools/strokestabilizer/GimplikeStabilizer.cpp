#include "GimplikeStabilizer.h"

#include <cmath>

GimplikeStabilizer::GimplikeStabilizer(double sigma, unsigned int lifespan):
        twoSigmaSquared(2 * sigma * sigma), eventLifespan(lifespan) {
    g_message("Created GimplikeStabilizer with 2σ² = %f and eventLifespan = %d", twoSigmaSquared, eventLifespan);
}

void GimplikeStabilizer::initialize(const PositionInputData& pos) { eventBuffer.emplace_front(pos); }

auto GimplikeStabilizer::feedMoveEvent(const PositionInputData& pos, double zoom) -> int {

    /**
     * First, clear the deque of points to paint
     */
    pointsToPaint.clear();

    /**
     * Compute the velocity and pushes the event to eventBuffer
     */
    pushMoveEvent(pos);

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
        sumOfVelocities += event.velocity;  // Mimicking Gimp's formula
        weight = exp(-sumOfVelocities * sumOfVelocities / twoSigmaSquared);
        weightedSumOfX += weight * event.x;
        weightedSumOfY += weight * event.y;
        weightedSumOfPressures += weight * event.pressure;
        sumOfWeights += weight;
    }

    if (sumOfWeights == 0) {  // Sanity check. Should never happen
        g_warning("sumOfWeights = %f, bufferSize = %zu, sumOfVelocities = %f, weight = %f, (x,y) = (%f,%f)",
                  sumOfWeights, eventBuffer.size(), sumOfVelocities, weight, weightedSumOfX, weightedSumOfY);
        int i = 0;
        sumOfVelocities = 0;
        printf("Buffer:\n id | timestamp |     x     |     y     |     v     |    sumV    |     w     |\n");
        for (auto&& event: eventBuffer) {
            i++;
            sumOfVelocities += event.velocity;  // Mimicking Gimp's formula
            weight = exp(-sumOfVelocities * sumOfVelocities / twoSigmaSquared);
            printf(" %2d | %9d | %9f | %9f | %9f | %10f | %9f |\n", i, event.timestamp, event.x, event.y,
                   event.velocity, sumOfVelocities, weight);
        }
    }

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
