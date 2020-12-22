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

#include <cmath>

#include "control/tools/InputHandler.h"

class Settings;

// Debug
#define STAB_DEBUG

#ifdef STAB_DEBUG
#include <deque>
#endif

enum StabilizingAlgorithm {
    STABILIZING_NONE,
    STABILIZING_ARITHMETIC_MEAN,
    STABILIZING_GIMP_EURISTICS,
    STABILIZING_DEADZONE
};

struct MathVect {
    double dx{};
    double dy{};
    static double scalarProduct(MathVect u, MathVect v) { return u.dx * v.dx + u.dy * v.dy; }
    double norm() { return hypot(dx, dy); }
    bool nonZero() { return (dx != 0) || (dy != 0); }
};

struct BufferedEvent {
    BufferedEvent() = default;
    BufferedEvent(double x, double y, double pressure): x(x), y(y), pressure(pressure) {}
    double x{};
    double y{};
    double pressure{};
};

#ifdef STAB_DEBUG
struct LogEvent: BufferedEvent {
    char status = 'I';
    guint32 ts;
    LogEvent(const PositionInputData& pos): BufferedEvent(pos.x, pos.y, pos.pressure), ts(pos.timestamp) {}
    static void bufferDumpHeader(string name) {
        printf("Buffer: %s\n  id | timestamp  | status |     x      |     y      |     P     |\n", name.c_str());
    }
    virtual void bufferDump(int i) {
        printf(" %3d | %10d |    %c   | %10.5f | %10.5f | %9.6f |\n", i, ts, status, x, y, pressure);
    }
};
#endif

/**
 * @brief A parent class, used as default (no stabilization)
 */
class Stabilizer {
public:
    Stabilizer() = default;
    virtual ~Stabilizer() = default;

    /**
     * @brief Initialize the stabilizer
     * @param pos The position of the button down event starting the stroke
     */
#ifndef STAB_DEBUG
    virtual void initialize(const PositionInputData& pos) {}
#else
    virtual void initialize(const PositionInputData& pos) {
        pushMoveEvent(pos);
        logBuffer.front().status = 'F';
        return;
    }
#endif

    /**
     * @brief Push the event to the buffer and compute what point(s) to paint
     * @param pos The event to be pushed
     * @param zoom The zoom level to rescale the points' coordinates
     *
     * @return The number of points (segments) to be painted. The points are available in this.pointsToPaint
     *      if negative, no stabilization has been computed.
     *
     * Does nothing in the base class
     */
#ifndef STAB_DEBUG
    virtual int feedMoveEvent(const PositionInputData& pos, double zoom) { return -1; }
#else
    virtual int feedMoveEvent(const PositionInputData& pos, double zoom) {
        pushMoveEvent(pos);
        logBuffer.front().status = ' ';
        return -1;
    }
#endif

    /**
     * @brief Push the event to the buffer
     * Does nothing in the base class
     */
#ifndef STAB_DEBUG
    virtual void pushMoveEvent(const PositionInputData& pos) {}
#else
    virtual void pushMoveEvent(const PositionInputData& pos) { logBuffer.emplace_front(pos); }

    void dumpBuffer() {
        int i = 0;
        LogEvent::bufferDumpHeader(getInfo());
        for (auto&& event: logBuffer) {
            event.bufferDump(++i);
        }
    }
    std::deque<LogEvent> logBuffer;

    virtual auto getInfo() -> string { return "No stabilizer"; }
#endif

    /**
     * @brief Pushes points to pointsToPaint to finish the stroke neatly
     * @param zoom The zoom ratio to apply to the points
     * @param stroke A pointer to the current stroke
     *
     * Does nothing in the base class
     */
    virtual void finishStroke(double zoom, Stroke* stroke) {}

    /**
     * @brief Contains the points to paint after a feedMoveEvent or a finishStroke
     * The front should be painted first
     *
     * Always empty in the base class
     */
    std::list<Point> pointsToPaint;
};

/**
 * @brief An intermediate for Stabilizers in need of a stroke finisher
 */
class StabilizerWithFinisher: public Stabilizer {
public:
    /**
     * @brief Pushes points to pointsToPaint to finish the stroke neatly
     * @param zoom The zoom ratio to apply to the points
     * @param stroke A pointer to the current stroke
     */
    virtual void finishStroke(double zoom, Stroke* stroke);

private:
    /**
     * @brief Get the last event received by the stabilizer
     * @return The last event received
     */
    virtual BufferedEvent getLastEvent() = 0;
};

namespace StrokeStabilizerFactory {
/**
 * @brief Creates and returns the Stabilizer adapted to the parameters
 * @param settings Pointer to the Settings instance from which to read the parameters
 *
 * @return The Stabilizer
 */
std::unique_ptr<Stabilizer> getStabilizer(Settings* settings);
};  // namespace StrokeStabilizerFactory
