/*
 * ui_layout.h  -  SCREEN LAYOUT + SHARED UI STATE
 * -----------------------------------------------
 * Where every panel sits on the 1200x730 window, plus the few variables
 * shared between drawing (ui_scene.c) and mouse/keyboard handling
 * (ui_input.c).
 */
#ifndef UI_LAYOUT_H
#define UI_LAYOUT_H

#include "models.h"

#define WIN_W 1200
#define WIN_H 730

/* Fleet list (left) */
#define FLEET_X    10
#define FLEET_Y    50
#define FLEET_ROWS 20

/* Depot yard with parking bays (top middle) */
#define YARD_X   270
#define YARD_Y   50
#define BAY_W    110
#define BAY_H    170
#define BAY_GAP  12
#define BAY_X0   (YARD_X + 6)
#define BAY_Y0   (YARD_Y + 34)

/* Waiting queue (middle) */
#define QUEUE_X  270
#define QUEUE_Y  310
#define QUEUE_SLOTS 8

/* Road (bottom middle) */
#define ROAD_X   270
#define ROAD_Y   400
#define ROAD_W   610
#define MAX_ROAD 16

/* Action buttons (bottom row) */
#define BUTTON_COUNT 9
typedef struct { const char *label; int x, y, w, h; } Button;
extern Button buttons[BUTTON_COUNT];

/* ---- shared UI state ---- */
extern int selId;                 /* id of the selected bus, 0 = none */
extern int tick;                  /* animation clock, +1 per frame */

typedef struct { int busId, x, y; } RoadHit;   /* where a moving bus was drawn (for clicking it) */
extern RoadHit roadHits[MAX_ROAD];
extern int     roadHitCount;

extern int selDriverId;           /* id of the selected driver, 0 = none */

Bus    *selectedBus(void);        /* the selected bus, or NULL */
Driver *selectedDriver(void);     /* the selected driver, or NULL */

/* Driver rows in the status panel (for drawing and clicking) */
#define DRIVER_ROWS   7
#define DRIVER_ROW_Y  376
#define DRIVER_ROW_H  20

#endif