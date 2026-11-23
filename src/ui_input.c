/*
 * ui_input.c  -  MOUSE AND KEYBOARD HANDLING
 * ------------------------------------------
 * Turns clicks and key presses into calls to the depot functions.
 * Clicking a bus selects it; clicking a button acts on the selected bus.
 */
#include <stddef.h>
#include "raylib.h"
#include "ui_input.h"
#include "ui_layout.h"
#include "ui_draw.h"
#include "state.h"
#include "queue.h"
#include "depot.h"
#include "storage.h"
#include "eventlog.h"

int selId = 0;  
int selDriverId = 0;

Button buttons[BUTTON_COUNT] = {
    {"ADD BUS", 0,0,0,0}, {"ADD DRV", 0,0,0,0}, {"PARK", 0,0,0,0},
    {"RELEASE", 0,0,0,0}, {"SCHEDULE", 0,0,0,0}, {"ASSIGN", 0,0,0,0},
    {"DISPATCH", 0,0,0,0}, {"RETURN", 0,0,0,0}, {"SAVE", 0,0,0,0}
};

/* button numbers (same order as the array above) */
enum { BTN_ADD_BUS, BTN_ADD_DRIVER, BTN_PARK, BTN_RELEASE, BTN_SCHEDULE,
       BTN_ASSIGN, BTN_DISPATCH, BTN_RETURN, BTN_SAVE };

Bus *selectedBus(void)
{
    int i = findBusById(selId);
    return i >= 0 ? &buses[i] : NULL;
}

Driver *selectedDriver(void)
{
    int i = findDriverById(selDriverId);
    return i >= 0 ? &drivers[i] : NULL;
}

void layoutButtons(void)
{
    for (int i = 0; i < BUTTON_COUNT; i++) {
        buttons[i].x = 10 + i * 132;
        buttons[i].y = 662;
        buttons[i].w = 124;
        buttons[i].h = 34;
    }
}

/* Run the action for one button. */
static void doButton(int which)
{
    /* these need no selected bus */
    if (which == BTN_ADD_BUS)    { Bus *nb = addBus(); if (nb) selId = nb->id; return; }
    if (which == BTN_ADD_DRIVER) { addDriver(); return; }
    if (which == BTN_SAVE)       { saveData(); return; }

    /* these act on the selected bus */
    Bus *b = selectedBus();
    if (!b) { logMsg(MSG_FAIL, "Select a bus first."); return; }

    switch (which) {
        case BTN_PARK:     parkBus(b);      break;
        case BTN_RELEASE:  releaseBay(b);   break;
        case BTN_SCHEDULE: scheduleTrip(b); break;
        case BTN_ASSIGN:   assignDriver(b); break;
        case BTN_DISPATCH: dispatchBus(b);  break;
        case BTN_RETURN:   returnBus(b);    break;
    }
}

void handleClick(int mx, int my)
{
    /* 1. buttons */
    for (int i = 0; i < BUTTON_COUNT; i++)
        if (hit(mx, my, buttons[i].x, buttons[i].y, buttons[i].w, buttons[i].h)) { doButton(i); return; }

    /* 2. fleet list rows */
    for (int i = 0; i < busCount && i < FLEET_ROWS; i++)
        if (hit(mx, my, FLEET_X + 4, FLEET_Y + 30 + i * 21, 242, 20)) { selId = buses[i].id; return; }

    /* 3. parking bays */
    for (int i = 1; i <= NUM_BAYS; i++) {
        int bx = BAY_X0 + (i - 1) * (BAY_W + BAY_GAP);
        if (bays[i] && hit(mx, my, bx, BAY_Y0, BAY_W, BAY_H)) { selId = bays[i]; return; }
    }

    /* 4. waiting queue */
    for (int i = 0; i < queueCount() && i < QUEUE_SLOTS; i++)
        if (hit(mx, my, QUEUE_X + 10 + i * 74, QUEUE_Y + 34, 62, 30)) { selId = queueAt(i); return; }

    /* 5. driver rows in the status panel */
    for (int i = 0; i < driverCount && i < DRIVER_ROWS; i++)
        if (hit(mx, my, 890, DRIVER_ROW_Y + i * DRIVER_ROW_H - 2, 300, DRIVER_ROW_H)) { selDriverId = drivers[i].id; return; }

    /* 6. buses driving on the road */
    for (int i = 0; i < roadHitCount; i++)
        if (hit(mx, my, roadHits[i].x, roadHits[i].y, 76, 28)) { selId = roadHits[i].busId; return; }
}

void handleKeys(void)
{
    Bus *b = selectedBus();
    if (b) {
        if (IsKeyPressed(KEY_DELETE) && deleteBus(b)) selId = 0;
        if (IsKeyPressed(KEY_M)) toggleMaintenance(b);
    }

    if (IsKeyPressed(KEY_D)) {
        Driver *d = selectedDriver();
        if (!d) logMsg(MSG_FAIL, "Click a driver in the list first, then press D.");
        else if (deleteDriver(d)) selDriverId = 0;
    }
}