/*
 * ui_scene.c  -  DRAWS THE WHOLE SCREEN
 * -------------------------------------
 * Reads the depot data and draws: fleet list, parking yard, waiting queue,
 * road, status panel, driver list, event log and buttons.
 */
#include "ui_scene.h"
#include "ui_draw.h"
#include "ui_layout.h"
#include "state.h"
#include "queue.h"
#include "depot.h"
#include "eventlog.h"

int tick = 0;
RoadHit roadHits[MAX_ROAD];
int     roadHitCount = 0;

/* ------------------------------------------------------------------ */
static void drawFleetPanel(void)
{
    panel(FLEET_X, FLEET_Y, 250, 470, "FLEET");

    for (int i = 0; i < busCount && i < FLEET_ROWS; i++) {
        Bus *b = &buses[i];
        int ry = FLEET_Y + 32 + i * 21;
        if (b->id == selId) fillRect(FLEET_X + 4, ry - 2, 242, 20, (Color){60, 70, 95, 255});
        fillRect(FLEET_X + 8, ry, 12, 14, busColor(b));
        drawText(FLEET_X + 28, ry, 2, C_TEXT, b->number);
        drawText(FLEET_X + 148, ry, 2, C_DIM, statusShort(b));
    }
    if (busCount > FLEET_ROWS)
        textf(FLEET_X + 8, FLEET_Y + 452, 1, C_DIM, "+%d MORE BUSES NOT SHOWN", busCount - FLEET_ROWS);
    if (busCount == 0)
        drawText(FLEET_X + 10, FLEET_Y + 40, 2, C_DIM, "NO BUSES YET");
}

/* ------------------------------------------------------------------ */
static void drawYard(void)
{
    panel(YARD_X, YARD_Y, 610, 250, "DEPOT YARD - PARKING BAYS");

    for (int i = 1; i <= NUM_BAYS; i++) {
        int bx = BAY_X0 + (i - 1) * (BAY_W + BAY_GAP);
        int by = BAY_Y0;

        fillRect(bx, by, BAY_W, BAY_H, (Color){52, 58, 72, 255});
        strokeRect(bx, by, BAY_W, BAY_H, bays[i] ? C_PARKED : C_BORDER);
        for (int k = 0; k < BAY_W; k += 14)                               /* painted bay line */
            fillRect(bx + k, by + BAY_H - 6, 8, 3, (Color){230, 230, 120, 255});
        textf(bx + 6, by + 6, 2, C_TEXT, "BAY %d", i);

        int bi = findBusById(bays[i]);
        if (bi >= 0) {
            Bus *b = &buses[bi];
            drawBus(bx + 8, by + 60, 94, 52, busColor(b), b->id == selId);
            drawText(bx + 6, by + 30, 1, C_TEXT, b->number);
            drawText(bx + 6, by + 128, 1, C_DIM, b->model);
        } else {
            drawText(bx + 22, by + 78, 2, C_DIM, "VACANT");
        }

        MsgKind kind;                                                      /* flash after an event */
        if (bayFlashActive(i, &kind)) {
            Color fc = msgColor(kind);
            strokeRect(bx - 2, by - 2, BAY_W + 4, BAY_H + 4, fc);
            strokeRect(bx - 3, by - 3, BAY_W + 6, BAY_H + 6, fc);
        }
    }
}

/* ------------------------------------------------------------------ */
static void drawQueue(void)
{
    panel(QUEUE_X, QUEUE_Y, 610, 80, "WAITING QUEUE (NEXT FIRST)");

    if (queueCount() == 0) drawText(QUEUE_X + 10, QUEUE_Y + 44, 2, C_DIM, "EMPTY");

    for (int i = 0; i < queueCount() && i < QUEUE_SLOTS; i++) {
        int bi = findBusById(queueAt(i));
        if (bi < 0) continue;
        int qx = QUEUE_X + 10 + i * 74;
        drawBus(qx, QUEUE_Y + 34, 62, 30, busColor(&buses[bi]), buses[bi].id == selId);
        drawText(qx, QUEUE_Y + 22, 1, C_TEXT, buses[bi].number + 5);       /* last 4 digits */
    }
}

/* ------------------------------------------------------------------ */
static void drawRoad(void)
{
    panel(ROAD_X, ROAD_Y, 610, 120, "ROAD - BUSES ON TRIP");
    fillRect(ROAD_X + 4, ROAD_Y + 28, ROAD_W - 8, 88, (Color){45, 45, 50, 255});

    for (int x = -((tick * 2) % 40); x < ROAD_W - 8; x += 40)             /* moving lane dashes */
        if (x >= 0) fillRect(ROAD_X + 4 + x, ROAD_Y + 70, 22, 3, (Color){230, 230, 230, 255});

    roadHitCount = 0;
    int k = 0;
    for (int i = 0; i < busCount && roadHitCount < MAX_ROAD; i++) {
        if (buses[i].status != ON_TRIP) continue;

        int bx = ROAD_X + 4 + (int)((tick * 3 + k * 230) % (ROAD_W + 100)) - 90;
        int by = (k % 2) ? ROAD_Y + 80 : ROAD_Y + 34;                      /* two lanes */
        k++;
        if (bx > ROAD_X + ROAD_W - 12 || bx < ROAD_X + 4) continue;        /* off the visible road */

        drawBus(bx, by, 76, 28, busColor(&buses[i]), buses[i].id == selId);
        int ti = findTripFor(buses[i].id, T_ONGOING);
        if (ti >= 0) drawText(bx, by - 11, 1, C_TRIP, trips[ti].destination);

        roadHits[roadHitCount].busId = buses[i].id;
        roadHits[roadHitCount].x = bx;
        roadHits[roadHitCount].y = by;
        roadHitCount++;
    }
}

/* ------------------------------------------------------------------ */
static void drawStatusPanel(void)
{
    panel(890, 50, 300, 470, "DEPOT STATUS");

    DepotStats s = getDepotStats();
    int y = 82;
    statRow(y, "TOTAL BUSES", C_TEXT,   "%d", s.totalBuses);                          y += 19;
    statRow(y, "AVAILABLE",   C_AVAIL,  "%d", s.available);                           y += 19;
    statRow(y, "PARKED",      C_PARKED, "%d", s.parked);                              y += 19;
    statRow(y, "ON TRIP",     C_TRIP,   "%d", s.onTrip);                              y += 19;
    statRow(y, "BAYS USED",   C_TEXT,   "%d/%d", s.occupiedBays, NUM_BAYS);           y += 19;
    statRow(y, "BAYS VACANT", C_TEXT,   "%d/%d", s.vacantBays, NUM_BAYS);             y += 19;
    statRow(y, "WAITING",     C_TEXT,   "%d", s.waiting);                             y += 19;
    statRow(y, "TRIPS S/O/C", C_TEXT,   "%d/%d/%d", s.tripsScheduled, s.tripsOngoing, s.tripsCompleted);

    /* ---- selected bus details ---- */
    drawText(900, 245, 2, C_YELLOW, "SELECTED BUS");
    Bus *sb = selectedBus();
    if (sb) {
        drawText(900, 266, 2, C_TEXT, sb->number);
        drawText(900, 285, 2, busColor(sb), statusShort(sb));
        if (sb->bay) textf(900 + textW(2, statusShort(sb)) + 12, 285, 2, C_DIM, "BAY %d", sb->bay);

        int ti = findTripFor(sb->id, T_SCHEDULED);
        if (ti < 0) ti = findTripFor(sb->id, T_ONGOING);
        if (ti >= 0) {
            textf(900, 304, 2, C_TEXT, "%s %s", trips[ti].destination, trips[ti].departure);
            int di = findDriverById(trips[ti].driverId);
            drawText(900, 323, 2, di >= 0 ? C_OK : C_FAIL, di >= 0 ? drivers[di].name : "NO DRIVER");
        } else {
            drawText(900, 304, 2, C_DIM, "NO ACTIVE TRIP");
        }
    } else {
        drawText(900, 266, 2, C_DIM, "NONE");
    }

    /* ---- drivers ---- */
    drawText(900, 352, 2, C_YELLOW, "DRIVERS");
    textf(1180 - textW(2, "FREE 0") - 6, 352, 2, C_DIM, "FREE %d", s.freeDrivers);
    for (int i = 0; i < driverCount && i < DRIVER_ROWS; i++) {
        int ry = DRIVER_ROW_Y + i * DRIVER_ROW_H;
        if (drivers[i].id == selDriverId) fillRect(894, ry - 2, 292, DRIVER_ROW_H, (Color){60, 70, 95, 255});
        drawText(900, ry, 2, C_TEXT, drivers[i].name);
        fillRect(1100, ry + 2, 10, 12, drivers[i].available ? C_OK : C_TRIP);
        drawText(1116, ry, 2, C_DIM, drivers[i].available ? "FREE" : "BUSY");
    }
    if (driverCount == 0) drawText(900, 376, 2, C_DIM, "NO DRIVERS YET");
}

/* ------------------------------------------------------------------ */
static void drawEventLog(void)
{
    panel(10, 530, 1180, 120, "EVENT LOG");
    for (int i = 0; i < LOG_LINES; i++) {
        const LogEntry *e = getLogEntry(i);
        if (e && e->text[0]) drawText(20, 560 + i * 17, 2, msgColor(e->kind), e->text);
    }
}

static void drawButtons(int mx, int my)
{
    for (int i = 0; i < BUTTON_COUNT; i++) {
        Button *b = &buttons[i];
        int over = hit(mx, my, b->x, b->y, b->w, b->h);
        fillRect(b->x, b->y, b->w, b->h, over ? (Color){70, 90, 130, 255} : (Color){48, 58, 80, 255});
        strokeRect(b->x, b->y, b->w, b->h, over ? C_YELLOW : C_BORDER);
        drawText(b->x + (b->w - textW(2, b->label)) / 2, b->y + 9, 2, C_TEXT, b->label);
    }
    drawText(10, 708, 2, C_DIM, "DEL = DELETE BUS   D = DELETE DRIVER   M = MAINTENANCE   ESC = SAVE+QUIT");
}

/* ------------------------------------------------------------------ */
void renderScene(int mx, int my)
{
    tick++;
    ClearBackground(C_BG);

    drawText(10, 12, 3, C_TEXT, "BUS DEPOT SIMULATOR");
    drawText(430, 20, 2, C_DIM, "SELECT A BUS, THEN CLICK AN ACTION");

    drawFleetPanel();
    drawYard();
    drawQueue();
    drawRoad();
    drawStatusPanel();
    drawEventLog();
    drawButtons(mx, my);

    updateFlashes();
}