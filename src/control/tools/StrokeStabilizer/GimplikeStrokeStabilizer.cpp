#include "GimplikeStrokeStabilizer.h"

#include <cmath>

#include <control/tools/StrokeHandler.h>  // TODO Ugly fix, remove!

// #include <control/XournalMain.h>
// #include <gdk/gdk.h>

// Needed?
// #include "control/settings/Settings.h"
// #include "config-features.h"

GimplikeStrokeStabilizer::GimplikeStrokeStabilizer(const PositionInputData& pos):
        // TODO read config
        twoSigmaSquared(StrokeHandler::stabilizingTwoSigmaSquare),
        eventLifespan(StrokeHandler::stabilizingEventLifespan) {
    eventBuffer.emplace_front(pos.x, pos.y, pos.pressure, 0, pos.timestamp);
    g_message("Created GimplikeStrokeStabilizer with 2σ² = %f and eventLifespan = %d", twoSigmaSquared, eventLifespan);
}

void GimplikeStrokeStabilizer::stabilizePointUsingEvent(const PositionInputData& pos, double zoom, Point* point) {

    /**
     * Compute the velocity and pushes the event
     */
    pushEvent(pos);

    /**
     * Flush expired events
     */
#ifdef STAB_DEBUG
    nbInBuffer.push_back(bufferSize);
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
#ifdef STAB_DEBUG
    nbInBuffer.push_back(bufferSize);
#endif

    /**
     * Average the coordinates using the gimp-like weights
     */
    double weightedSumOfX = 0;
    double weightedSumOfY = 0;
    double weightedSumOfPressures = 0;
    double weight;
    double sumOfWeights = 0;
    double sumOfVelocities = 0;

    // Type of it: std::deque<GimplikeBufferedEvent>::const_iterator
    for (auto it = eventBuffer.cbegin(); it != eventBuffer.cend(); ++it) {
        sumOfVelocities += (*it).velocity;  // Mimicking Gimp's formula
        weight = exp(-sumOfVelocities * sumOfVelocities / twoSigmaSquared);
        weightedSumOfX += weight * (*it).x;
        weightedSumOfY += weight * (*it).y;
        weightedSumOfPressures += weight * (*it).pressure;
        sumOfWeights += weight;
    }

    if (sumOfWeights == 0) {  // Sanity check. Should never happen
        g_warning("sumOfWeights = %f, bufferSize = %zu, sumOfVelocities = %f, weight = %f, (x,y) = (%f,%f)",
                  sumOfWeights, eventBuffer.size(), sumOfVelocities, weight, weightedSumOfX, weightedSumOfY);
        int i = 0;
        sumOfVelocities = 0;
        printf("Buffer:\n id | timestamp |     x     |     y     |     v     |    sumV    |     w     |\n");
        for (auto it = eventBuffer.cbegin(); it != eventBuffer.cend(); ++it) {
            i++;
            sumOfVelocities += (*it).velocity;  // Mimicking Gimp's formula
            weight = exp(-sumOfVelocities * sumOfVelocities / twoSigmaSquared);
            printf(" %2d | %9d | %9f | %9f | %9f | %10f | %9f |\n", i, (*it).timestamp, (*it).x, (*it).y,
                   (*it).velocity, sumOfVelocities, weight);
        }
    }

    //     double x=point->x, y=point->y, z=point->z; //debug


    point->z = weightedSumOfPressures / sumOfWeights;

    /**
     * Rescale the averaged coordinates before assigning them to the point
     */
    sumOfWeights *= zoom;
    point->x = weightedSumOfX / sumOfWeights;
    point->y = weightedSumOfY / sumOfWeights;

    //     g_message("old.(x,y,z) = (%f,%f,%f)   new.(x,y,z) = (%f,%f,%f)", x,y,z,point->x,point->y,point->z);
}

void GimplikeStrokeStabilizer::pushEvent(const PositionInputData& pos) {

#ifdef STAB_DEBUG
    bufferSize++;
#endif
    if (eventBuffer.empty()) {
        g_warning("GimplikeStrokeStabilizer::pushEvent: Empty buffer");
        eventBuffer.emplace_front(pos.x, pos.y, pos.pressure, 0, pos.timestamp);
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
    eventBuffer.emplace_front(pos.x, pos.y, pos.pressure,
                              hypot(pos.x - lastEvent.x, pos.y - lastEvent.y) / ((double)timelaps), pos.timestamp);
}
