#include "Stabilizer.h"

#include <cmath>

#include "control/settings/Settings.h"
#include "model/SplineSegment.h"

#include "ArithmeticStabilizer.h"
#include "DeadzoneStabilizer.h"
#include "GimplikeStabilizer.h"


auto StrokeStabilizerFactory::getStabilizer(Settings* settings) -> std::unique_ptr<Stabilizer> {

    StabilizingAlgorithm algorithm = settings->getStabilizerAlgorithm();
    if (algorithm == STABILIZING_ARITHMETIC_MEAN) {
        return std::make_unique<ArithmeticStabilizer>(settings->getStabilizerBuffersize());
    }

    if (algorithm == STABILIZING_GIMP_EURISTICS) {
        return std::make_unique<GimplikeStabilizer>(settings->getStabilizerSigma(),
                                                    settings->getStabilizerEventLifespan());
    }

    if (algorithm == STABILIZING_DEADZONE) {
        return std::make_unique<DeadzoneStabilizer>(
                settings->getStabilizerDeadzoneRadius(), settings->getStabilizerCuspDetetion(),
                settings->getStabilizerDeadzoneAveraging(), settings->getStabilizerSigma(),
                settings->getStabilizerEventLifespan());
    }

    /**
     * Defaults to no stabilization
     */
    g_message("Creating default Stabilizer");
    return std::make_unique<Stabilizer>();
}

void StabilizerWithFinisher::finishStroke(double zoom, Stroke* stroke) {
    /**
     * Using the last two points of the stroke and the targetted getLastPoint(), draw a spline quadratic segment
     * to conclude the stroke smoothly.
     */
    pointsToPaint.clear();

    int pointCount = stroke->getPointCount();
    if (pointCount == 0) {
        return;
    }

    BufferedEvent lastEvent = getLastEvent();
    if (pointCount == 1) {
        /**
         * Draw a line segment
         */
        pointsToPaint.emplace_back(lastEvent.x / zoom, lastEvent.y / zoom, lastEvent.pressure);
        return;
    }

    /**
     * Draw a quadratic spline segment, with first tangent vector parallel to AB
     */
    Point B = stroke->getPoint(pointCount - 1);
    Point A = stroke->getPoint(pointCount - 2);
    Point C(lastEvent.x / zoom, lastEvent.y / zoom, lastEvent.pressure);

    MathVect vAB = {B.x - A.x, B.y - A.y};
    MathVect vBC = {C.x - B.x, C.y - B.y};
    const double squaredNormBC = vBC.dx * vBC.dx + vBC.dy * vBC.dy;

    /**
     * The first argument of std::min would give a symmetric quadratic spline segment.
     * The std::min ensures the spline segment stays reasonably close to its nodes
     */
    double distance = std::min(std::abs(squaredNormBC * vAB.norm() / (2 * MathVect::scalarProduct(vAB, vBC))),
                               sqrt(squaredNormBC));

    // Quadratic control point
    Point Q = B.lineTo(A, -distance);

    /**
     * The quadratic control point is converted into two cubic control points
     */
    // Equivalent to fp = B.lineTo(Q, 2/3*distance), but avoids recomputing the norms
    Point fp((Q.x - B.x) * 2 / 3 + B.x, (Q.y - B.y) * 2 / 3 + B.y);
    // Equivalent to sp = C.lineTo(Q, 2/3*distance), but avoids recomputing the norms
    Point sp((Q.x - C.x) * 2 / 3 + C.x, (Q.y - C.y) * 2 / 3 + C.y);


    SplineSegment spline(B, fp, sp, C);
    /**
     * TODO Add support for spline segments in Stroke and replace the point sequence by a single spline segment
     */
    pointsToPaint = spline.toPointSequence();
    pointsToPaint.push_back(C);
    pointsToPaint.pop_front();
}
