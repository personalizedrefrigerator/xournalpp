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

#include "control/XournalMain.h"
#include "view/DocumentView.h"

#include "InputHandler.h"
#include "SnapToGridInputHandler.h"

class ShapeRecognizer;

enum SmoothingAlgorithm { SMOOTHING_NONE, SMOOTHING_ARITHMETIC_MEAN, SMOOTHING_GIMP_EURISTICS };

class BufferedEvent {
public:
    BufferedEvent(double x, double y, double pressure);
    double x;
    double y;
    double pressure;
    double velocity;
};

/**
 * @brief The stroke handler draws a stroke on a XojPageView
 *
 * The stroke is drawn using a cairo_surface_t* as a mask:
 * As the pointer moves on the canvas single segments are
 * drawn opaquely on the initially transparent masking
 * surface. The surface is used to mask the stroke
 * when drawing it to the XojPageView
 */
class StrokeHandler: public InputHandler {
public:
    StrokeHandler(XournalView* xournal, XojPageView* redrawable, const PageRef& page);
    virtual ~StrokeHandler();

    void draw(cairo_t* cr);

    bool onMotionNotifyEvent(const PositionInputData& pos);
    void onButtonReleaseEvent(const PositionInputData& pos);
    void onButtonPressEvent(const PositionInputData& pos);
    void onButtonDoublePressEvent(const PositionInputData& pos);
    bool onKeyEvent(GdkEventKey* event);
    /**
     * Reset the shape recognizer, only implemented by drawing instances,
     * but needs to be in the base interface.
     */
    virtual void resetShapeRecognizer();

    static SmoothingAlgorithm smoothingAlgorithm;
    static double smoothingTwoSigmaSquare;
    static int smoothingBufferSize;

protected:
    void strokeRecognizerDetected(ShapeRecognizerResult* result, Layer* layer);
    void destroySurface();

protected:
    Point buttonDownPoint;  // used for tapSelect and filtering - never snapped to grid.
    SnapToGridInputHandler snappingHandler;

private:
    /**
     * The masking surface
     */
    cairo_surface_t* surfMask;

    /**
     * And the corresponding cairo_t*
     */
    cairo_t* crMask;

    DocumentView view;

    ShapeRecognizer* reco;


    // to filter out short strokes (usually the user tapping on the page to select it)
    guint32 startStrokeTime{};
    static guint32 lastStrokeTime;  // persist across strokes - allow us to not ignore persistent dotting.

    /**
     * @brief A queue containing the relevant information on the last Move events
     * The beginning of the queue contains the most recent event
     * The end of the queue contains the most ancient event stored
     */
    std::deque<BufferedEvent> eventQueue;

    /**
     * @brief The timestamp of the most recent event in the queue
     */
    guint32 lastEventTimestamp;

    /**
     * @brief Append a segment to the stroke and paint it
     * @param point The endpoint of the segment
     */
    void addStrokeSegment(Point& point);


    /**
     * @brief Computes the average of the events in eventQueue according
     * to the algorithm defined in StrokeHandler::smoothingAlgorithm.
     * Converts this averaged event into a point using the parameter zoom
     *
     * In theory, this function is only called if smoothingAlgorithm != SMOOTHING_NONE
     *
     * @param zoom The current zoom to apply.
     *
     * @return The averaged point to be painted.
     */
    Point getAveragedPoint(const double zoom);
};
