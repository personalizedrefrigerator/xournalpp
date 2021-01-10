/*
 * Xournal++
 *
 * Enum for all actions
 *
 * @author Xournal++ Team
 * https://github.com/xournalpp/xournalpp
 *
 * @license GNU GPLv2 or later
 */

#pragma once

#include <string>
using std::string;

/******************************************************************************
*******************************************************************************

RUN THE GENERATOR IF YOU CHANGE THIS FILE!

php generateConvert.php

*******************************************************************************
******************************************************************************/

enum ActionType {
    ACTION_NONE = 0,

    // Menu file
    ACTION_NEW = 100,
    ACTION_OPEN,
    ACTION_ANNOTATE_PDF,
    ACTION_SAVE,
    ACTION_SAVE_AS,
    ACTION_EXPORT_AS_PDF,
    ACTION_EXPORT_AS,
    ACTION_PRINT,
    ACTION_QUIT,

    // Menu edit
    ACTION_UNDO = 200,
    ACTION_REDO,
    ACTION_CUT,
    ACTION_COPY,
    ACTION_PASTE,
    ACTION_SEARCH,
    ACTION_DELETE,
    ACTION_SETTINGS,

    // Menu navigation
    ACTION_GOTO_FIRST = 300,
    ACTION_GOTO_BACK,
    ACTION_GOTO_PAGE,
    ACTION_GOTO_NEXT,
    ACTION_GOTO_LAST,
    ACTION_GOTO_NEXT_LAYER,
    ACTION_GOTO_PREVIOUS_LAYER,
    ACTION_GOTO_TOP_LAYER,
    ACTION_GOTO_NEXT_ANNOTATED_PAGE,
    ACTION_GOTO_PREVIOUS_ANNOTATED_PAGE,

    // Menu Journal
    ACTION_NEW_PAGE_BEFORE = 400,
    ACTION_NEW_PAGE_AFTER,
    ACTION_NEW_PAGE_AT_END,
    ACTION_APPEND_NEW_PDF_PAGES,

    ACTION_CONFIGURE_PAGE_TEMPLATE,

    ACTION_DELETE_PAGE,
    ACTION_NEW_LAYER,
    ACTION_DELETE_LAYER,
    ACTION_PAPER_FORMAT,
    ACTION_PAPER_BACKGROUND_COLOR,

    // Menu Tools
    // Has to be in the same order as in Tool.h: ToolType!
    ACTION_TOOL_PEN = 500,
    ACTION_TOOL_ERASER,
    ACTION_TOOL_HIGHLIGHTER,
    ACTION_TOOL_TEXT,
    ACTION_TOOL_IMAGE,
    ACTION_TOOL_SELECT_RECT,
    ACTION_TOOL_SELECT_REGION,
    ACTION_TOOL_SELECT_OBJECT,
    ACTION_TOOL_PLAY_OBJECT,
    ACTION_TOOL_VERTICAL_SPACE,
    ACTION_TOOL_HAND,

    ACTION_TOOL_DEFAULT,

    ACTION_SHAPE_RECOGNIZER,
    ACTION_TOOL_DRAW_RECT,
    ACTION_TOOL_DRAW_CIRCLE,
    ACTION_TOOL_DRAW_ARROW,
    ACTION_TOOL_DRAW_COORDINATE_SYSTEM,
    ACTION_RULER,
    ACTION_TOOL_DRAW_SPLINE,

    ACTION_TOOL_FLOATING_TOOLBOX,

    ACTION_TOOL_LINE_STYLE_PLAIN,
    ACTION_TOOL_LINE_STYLE_DASH,
    ACTION_TOOL_LINE_STYLE_DASH_DOT,
    ACTION_TOOL_LINE_STYLE_DOT,

    ACTION_SIZE_VERY_FINE,
    ACTION_SIZE_FINE,
    ACTION_SIZE_MEDIUM,
    ACTION_SIZE_THICK,
    ACTION_SIZE_VERY_THICK,

    ACTION_TOOL_ERASER_STANDARD,
    ACTION_TOOL_ERASER_WHITEOUT,
    ACTION_TOOL_ERASER_DELETE_STROKE,

    ACTION_TOOL_ERASER_SIZE_VERY_FINE,
    ACTION_TOOL_ERASER_SIZE_FINE,
    ACTION_TOOL_ERASER_SIZE_MEDIUM,
    ACTION_TOOL_ERASER_SIZE_THICK,
    ACTION_TOOL_ERASER_SIZE_VERY_THICK,

    ACTION_TOOL_PEN_SIZE_VERY_FINE,
    ACTION_TOOL_PEN_SIZE_FINE,
    ACTION_TOOL_PEN_SIZE_MEDIUM,
    ACTION_TOOL_PEN_SIZE_THICK,
    ACTION_TOOL_PEN_SIZE_VERY_THICK,

    ACTION_TOOL_PEN_FILL,
    ACTION_TOOL_PEN_FILL_OPACITY,

    ACTION_TOOL_HIGHLIGHTER_SIZE_VERY_FINE,
    ACTION_TOOL_HIGHLIGHTER_SIZE_FINE,
    ACTION_TOOL_HIGHLIGHTER_SIZE_MEDIUM,
    ACTION_TOOL_HIGHLIGHTER_SIZE_THICK,
    ACTION_TOOL_HIGHLIGHTER_SIZE_VERY_THICK,

    ACTION_TOOL_HIGHLIGHTER_FILL,
    ACTION_TOOL_HIGHLIGHTER_FILL_OPACITY,

    ACTION_TOOL_FILL,

    ACTION_ROTATION_SNAPPING,
    ACTION_GRID_SNAPPING,

    // Used for all colors
    ACTION_SELECT_COLOR,
    ACTION_SELECT_COLOR_CUSTOM,

    ACTION_SELECT_FONT,
    ACTION_FONT_BUTTON_CHANGED,
    ACTION_TEX,

    // Menu View
    ACTION_ZOOM_IN = 600,
    ACTION_ZOOM_OUT,
    ACTION_ZOOM_FIT,
    ACTION_ZOOM_100,
    ACTION_FULLSCREEN,
    ACTION_REDUCE_CONTENTS,
    ACTION_VIEW_PAIRED_PAGES,
    ACTION_VIEW_PRESENTATION_MODE,
    ACTION_MANAGE_TOOLBAR,
    ACTION_CUSTOMIZE_TOOLBAR,
    ACTION_AUDIO_RECORD,
    ACTION_AUDIO_PAUSE_PLAYBACK,
    ACTION_AUDIO_STOP_PLAYBACK,
    ACTION_AUDIO_SEEK_FORWARDS,
    ACTION_AUDIO_SEEK_BACKWARDS,
    ACTION_SET_PAIRS_OFFSET,
    ACTION_SET_COLUMNS,
    ACTION_SET_COLUMNS_1,
    ACTION_SET_COLUMNS_2,
    ACTION_SET_COLUMNS_3,
    ACTION_SET_COLUMNS_4,
    ACTION_SET_COLUMNS_5,
    ACTION_SET_COLUMNS_6,
    ACTION_SET_COLUMNS_7,
    ACTION_SET_COLUMNS_8,
    ACTION_SET_ROWS,
    ACTION_SET_ROWS_1,
    ACTION_SET_ROWS_2,
    ACTION_SET_ROWS_3,
    ACTION_SET_ROWS_4,
    ACTION_SET_ROWS_5,
    ACTION_SET_ROWS_6,
    ACTION_SET_ROWS_7,
    ACTION_SET_ROWS_8,
    ACTION_SET_LAYOUT_HORIZONTAL,
    ACTION_SET_LAYOUT_VERTICAL,
    ACTION_SET_LAYOUT_L2R,
    ACTION_SET_LAYOUT_R2L,
    ACTION_SET_LAYOUT_T2B,
    ACTION_SET_LAYOUT_B2T,

    // Plugin Menu
    ACTION_PLUGIN_MANAGER = 700,

    // Menu Help
    ACTION_ABOUT = 800,
    ACTION_HELP,

    // Footer, not really an action, but need an identifier too
    ACTION_FOOTER_PAGESPIN = 900,
    ACTION_FOOTER_ZOOM_SLIDER,
    ACTION_FOOTER_LAYER,

    // Used to select no item in a group...
    ACTION_NOT_SELECTED = 1
};

ActionType ActionType_fromString(const string& value);
string ActionType_toString(ActionType value);
