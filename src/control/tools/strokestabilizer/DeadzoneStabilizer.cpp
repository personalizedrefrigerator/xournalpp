#include "DeadzoneStabilizer.h"


DeadzoneStabilizer::DeadzoneStabilizer(double dzRadius, bool cuspDetection, bool averaging, double sigma,
                                       unsigned int lifespan):
        deadzoneRadius(dzRadius),
        twoSigmaSquared(2 * sigma * sigma),
        eventLifespan(lifespan),
        cuspDetectionOn(cuspDetection),
        averagingOn(averaging),
        lastLiveDirection({0, 0}) {
    g_message("Created DeadzoneStabilizer with 2σ² = %f, eventLifespan = %u, deadzoneRadius = %f, averaging = %d, cusp "
              "detection = %d",
              twoSigmaSquared, eventLifespan, deadzoneRadius, averagingOn, cuspDetectionOn);
}

void DeadzoneStabilizer::initialize(const PositionInputData& pos) {
    lastPaintedEvent = {pos.x, pos.y, pos.pressure};
    eventBuffer.emplace_front(pos);
}


auto DeadzoneStabilizer::feedMoveEvent(const PositionInputData& pos, double zoom) -> int {

    //     if (eventBuffer.empty()) {
    //         g_warning("DeadzoneStabilizer: eventBuffer is empty. This should never happen!");
    //         eventBuffer.push_front(deadzoneBuffer.back());
    //     }

    MathVect movement = {pos.x - lastPaintedEvent.x, pos.y - lastPaintedEvent.y};
    double ratio = 1 - (deadzoneRadius / movement.norm());

    //     g_message("Ratio = %f", ratio);
    if (ratio <= 0) {
        /**
         * The event occurred inside the deadzone. Ignore it: return 0 (no points to draw)
         */
        //         g_message("Deadzone event");
        return 0;
    }

    pointsToPaint.clear();

    if (cuspDetectionOn && (MathVect::scalarProduct(movement, lastLiveDirection) < 0)) {
        /**
         * lastLiveDirection != 0 and the angle between movement and lastLiveDirection is greater than 90°
         *
         * We have a clear change of direction. This is a cusp. Draw the entire cusp
         */
        g_message(">>>  Cusp detected!");

        /**
         * Clear the averaging buffer so that the cusp is considered as if it was the beginning of the stroke
         */
#ifdef STAB_DEBUG
        maxBufferSize = std::max(maxBufferSize, bufferSize);
        bufferSize = 0;
#endif
        eventBuffer.clear();
        eventBuffer.push_front(lastLiveEvent);

        /**
         * Paint the tip of the cusp. Rescale the averaged coordinates before assigning them to the point
         */
        pointsToPaint.emplace_back(lastLiveEvent.x / zoom, lastLiveEvent.y / zoom, lastLiveEvent.pressure);


        /**
         * Paint the way back from the tip of the cusp
         * To do so, we create a fake point between pos and lastLiveEvent
         */
        MathVect diff = {pos.x - lastLiveEvent.x, pos.y - lastLiveEvent.y};
        double diffNorm = diff.norm();
        double coeff = deadzoneRadius / diffNorm;

        if (coeff > 0.71) {
            /**
             * With the scalarProduct being negative, we should always have coeff < sqrt(2)/2 < 0.71
             */
            g_warning("Coefficient > sqrt(2)/2. This should never happen!");
        }

        lastLiveDirection.dx = coeff * diff.dx;
        lastLiveDirection.dy = coeff * diff.dy;

        lastPaintedEvent.x = pos.x - lastLiveDirection.dx;
        lastPaintedEvent.y = pos.y - lastLiveDirection.dy;
        lastPaintedEvent.pressure = coeff * lastLiveEvent.pressure + (1 - coeff) * pos.pressure;

        pointsToPaint.emplace_back(lastPaintedEvent.x / zoom, lastPaintedEvent.y / zoom, lastPaintedEvent.pressure);

        /**
         * Push this fake point (as an event) in eventBuffer. Fake timestamp and set consistent velocity.
         */
#ifdef STAB_DEBUG
        bufferSize++;
#endif
        eventBuffer.emplace_front(lastPaintedEvent.x, lastPaintedEvent.y, lastPaintedEvent.pressure, coeff * diffNorm,
                                  lastLiveEvent.timestamp + 1);

        /**
         * Update lastLiveEvent to pos
         */
        lastLiveEvent = DeadzoneBufferedEvent(pos);

        return 2;  // 2 points to paint
    } else {
        /**
         * Not a cusp. Normal behaviour
         */

        /**
         * Update lastLiveEvent and lastLiveDirection
         */
        lastLiveEvent = DeadzoneBufferedEvent(pos);
        lastLiveDirection = movement;

        if (averagingOn) {
            emplaceInAveragingBuffer(lastPaintedEvent.x + ratio * movement.dx, lastPaintedEvent.y + ratio * movement.dy,
                                     pos.pressure, pos.timestamp);

            // if (ratio * deadzoneRadius / zoom < 0.3) {  // 0.3 = InputHandler.cpp's PIXEL_MOTION_THRESHOLD
            //     /**
            //      * The resulting point is to close to the last painted point
            //      */
            //     return 0;
            // }
            paintAverage(zoom);
        } else {
            lastPaintedEvent.x += ratio * movement.dx;
            lastPaintedEvent.y += ratio * movement.dy;
            pointsToPaint.emplace_back(lastPaintedEvent.x / zoom, lastPaintedEvent.y / zoom, pos.pressure);
        }
        return 1;
    }
}

void DeadzoneStabilizer::paintAverage(double zoom) {
    if (eventBuffer.empty()) {
        g_warning("DeadzoneStabilizer::paintAverage: empty eventBuffer."
                  "This should never happen!");
        return;
    }
    guint32 timestamp = eventBuffer.front().timestamp;
    /**
     * Flush expired events
     */
#ifdef STAB_DEBUG
    maxBufferSize = std::max(maxBufferSize, bufferSize);
#endif
    if (timestamp < eventLifespan) {  // Could this happen?
        g_warning("StrokeStabilizer: timestamp %d is smaller than event lifetime %d."
                  " Not flushing the buffer.",
                  timestamp, eventLifespan);
    } else {
        const guint32 limitDate = timestamp - eventLifespan;

        while ((!eventBuffer.empty()) && (eventBuffer.back().timestamp < limitDate)) {
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

    /**
     * Update the last painted coordinates
     */
    lastPaintedEvent.x = weightedSumOfX / sumOfWeights;
    lastPaintedEvent.y = weightedSumOfY / sumOfWeights;

    /**
     * Rescale the averaged coordinates before assigning them to the point
     */
    pointsToPaint.emplace_back(lastPaintedEvent.x / zoom, lastPaintedEvent.y / zoom,
                               weightedSumOfPressures / sumOfWeights);
}

void DeadzoneStabilizer::pushMoveEvent(const PositionInputData& pos) {
    // Just ignore it.
}

void DeadzoneStabilizer::emplaceInAveragingBuffer(double x, double y, double pressure, guint32 timestamp) {
    DeadzoneBufferedEvent event(x, y, pressure, 0, timestamp);
    pushToAveragingBuffer(event);
}


void DeadzoneStabilizer::pushToAveragingBuffer(DeadzoneBufferedEvent& event) {

#ifdef STAB_DEBUG
    bufferSize++;
#endif
    if (eventBuffer.empty()) {
        g_message("DeadzoneStabilizer::pushToAveragingBuffer: Empty buffer. This should never happen!");
        eventBuffer.push_front(event);
        return;
    }

    /**
     * Compute the velocity using timestamps and the previous measure point
     * Issue: timestamps are in ms. They don't seem to be precise enough. Different events often have the same
     * timestamp...
     */
    DeadzoneBufferedEvent& lastEvent = eventBuffer.front();

    guint32 timelaps = event.timestamp - lastEvent.timestamp;
    if (timelaps == 0) {
        g_warning("Oh oh: Events with same timestamps: %d\n  lastEvent.(x,y) = (%f,%f)\n"
                  "  newEvent.(x,y) = (%f,%f)",
                  event.timestamp, lastEvent.x, lastEvent.y, event.x, event.y);
        timelaps = 1;
    }

    /**
     * Update velocity
     */
    event.velocity = hypot(event.x - lastEvent.x, event.y - lastEvent.y) / ((double)timelaps);

    /**
     * Create and push the event
     */
    eventBuffer.push_front(event);
}
