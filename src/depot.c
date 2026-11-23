/* depot.c  -  see depot.h */
#include <stdio.h>
#include <string.h>
#include "depot.h"
#include "state.h"
#include "queue.h"
#include "eventlog.h"

/* ---------------------------------------------------------------------
 *  Internal helpers
 * ------------------------------------------------------------------- */
static void putBusInBay(Bus *b, int bay)
{
    bays[bay] = b->id;
    b->bay = bay;
    b->status = PARKED;
}

/* If bays are vacant and buses are waiting, park the waiting buses. */
static void fillBaysFromQueue(void)
{
    for (int bay = 1; bay <= NUM_BAYS; bay++) {
        while (bays[bay] == 0 && queueCount() > 0) {
            int i = findBusById(dequeue());
            if (i >= 0 && buses[i].status == AVAILABLE) {
                putBusInBay(&buses[i], bay);
                flashBay(bay, MSG_OK);
                logMsg(MSG_INFO, "Waiting bus %s auto-parked in bay %d.", buses[i].number, bay);
            }
        }
    }
}

/* Empty a bay, then let the next waiting bus take it. */
static void freeBay(int bay)
{
    int i = findBusById(bays[bay]);
    if (i >= 0) {
        buses[i].bay = 0;
        if (buses[i].status == PARKED) buses[i].status = AVAILABLE;
    }
    bays[bay] = 0;
    fillBaysFromQueue();
}

/* ---------------------------------------------------------------------
 *  Bus management
 * ------------------------------------------------------------------- */
Bus *addBus(void)
{
    static const char *models[] = {"VOLVO 9400", "TATA STARBUS", "ASHOK LEYLAND", "EICHER SKYLINE"};
    static const int   caps[]   = {45, 52, 40, 35};

    if (busCount >= MAX_BUSES) { logMsg(MSG_FAIL, "Bus list is full."); return NULL; }

    Bus b;
    memset(&b, 0, sizeof b);
    b.id = nextBusId();
    int n = 1000 + (b.id * 137) % 9000;                       /* generate a unique number */
    do { snprintf(b.number, sizeof b.number, "PB65-%04d", n++); } while (findBusByNumber(b.number) >= 0);
    snprintf(b.model, sizeof b.model, "%s", models[b.id % 4]);
    b.capacity  = caps[b.id % 4];
    b.inService = 1;
    b.status    = AVAILABLE;

    buses[busCount++] = b;
    logMsg(MSG_OK, "Bus %s (%s, %d seats) registered.", b.number, b.model, b.capacity);
    return &buses[busCount - 1];
}

int deleteBus(Bus *b)
{
    if (b->status == ON_TRIP) { logMsg(MSG_FAIL, "Cannot delete %s while on a trip.", b->number); return 0; }

    int  id = b->id;
    char number[STR_LEN];
    snprintf(number, sizeof number, "%s", b->number);

    if (b->bay) bays[b->bay] = 0;
    removeFromQueue(id);

    for (int i = 0; i < tripCount; i++)                       /* cancel its scheduled trips */
        if (trips[i].busId == id && trips[i].status == T_SCHEDULED) {
            trips[i].status = T_CANCELLED;
            int d = findDriverById(trips[i].driverId);
            if (d >= 0) drivers[d].available = 1;
        }

    for (int i = (int)(b - buses); i < busCount - 1; i++) buses[i] = buses[i + 1];
    busCount--;

    logMsg(MSG_INFO, "Bus %s deleted.", number);
    fillBaysFromQueue();
    return 1;
}

void toggleMaintenance(Bus *b)
{
    if (b->status == ON_TRIP) { logMsg(MSG_FAIL, "%s is on a trip.", b->number); return; }
    b->inService = !b->inService;
    logMsg(MSG_INFO, "%s is now %s.", b->number, b->inService ? "in service" : "under maintenance");
}

/* ---------------------------------------------------------------------
 *  Driver management
 * ------------------------------------------------------------------- */
Driver *addDriver(void)
{
    static const char *names[] = {"RAM SINGH", "AMAN KAUR", "GURPREET SINGH", "HARJOT SANDHU",
                                  "MANPREET GILL", "SUKHDEV RAI", "JASPAL BAJWA", "NAVDEEP MANN"};
    if (driverCount >= MAX_DRIVERS) { logMsg(MSG_FAIL, "Driver list is full."); return NULL; }

    Driver d;
    memset(&d, 0, sizeof d);
    d.id = nextDriverId();
    snprintf(d.name, sizeof d.name, "%s", names[(d.id - 1) % 8]);
    snprintf(d.license, sizeof d.license, "PB-DL-%05d", 10000 + d.id * 7);
    d.available = 1;

    drivers[driverCount++] = d;
    logMsg(MSG_OK, "Driver %s registered.", d.name);
    return &drivers[driverCount - 1];
}

int deleteDriver(Driver *d)
{
    if (!d->available) {                       /* assigned to a scheduled or ongoing trip */
        logMsg(MSG_FAIL, "Cannot delete %s: driver is assigned to a trip.", d->name);
        return 0;
    }

    int  id = d->id;
    char name[STR_LEN];
    snprintf(name, sizeof name, "%s", d->name);

    for (int i = 0; i < tripCount; i++)        /* old trips no longer point at this driver */
        if (trips[i].driverId == id) trips[i].driverId = 0;

    for (int i = (int)(d - drivers); i < driverCount - 1; i++) drivers[i] = drivers[i + 1];
    driverCount--;

    logMsg(MSG_INFO, "Driver %s deleted.", name);
    return 1;
}

void assignDriver(Bus *b)
{
    int ti = findTripFor(b->id, T_SCHEDULED);
    if (ti < 0) { logMsg(MSG_FAIL, "%s has no scheduled trip.", b->number); return; }
    Trip *t = &trips[ti];

    for (int i = 0; i < driverCount; i++) {
        if (drivers[i].available) {
            int old = findDriverById(t->driverId);            /* release the previous driver */
            if (old >= 0) drivers[old].available = 1;
            drivers[i].available = 0;
            t->driverId = drivers[i].id;
            logMsg(MSG_OK, "Driver %s assigned to trip %d.", drivers[i].name, t->id);
            return;
        }
    }
    logMsg(MSG_FAIL, "No available driver. Add a driver first.");
}

/* ---------------------------------------------------------------------
 *  Parking management
 * ------------------------------------------------------------------- */
void parkBus(Bus *b)
{
    if (b->status == PARKED)  { logMsg(MSG_FAIL, "%s is already parked in bay %d.", b->number, b->bay); return; }
    if (b->status == ON_TRIP) { logMsg(MSG_FAIL, "%s is on a trip. Return it first.", b->number); return; }
    if (inQueue(b->id))       { logMsg(MSG_FAIL, "%s is already waiting for a bay.", b->number); return; }

    int bay = findFreeBay();
    if (bay) {
        putBusInBay(b, bay);
        flashBay(bay, MSG_OK);
        logMsg(MSG_OK, "%s parked in bay %d.", b->number, bay);
    } else {
        enqueue(b->id);
        logMsg(MSG_INFO, "All bays full. %s joined the waiting queue (position %d).", b->number, queueCount());
    }
}

void releaseBay(Bus *b)
{
    if (!b->bay) { logMsg(MSG_FAIL, "%s is not in a bay.", b->number); return; }
    int bay = b->bay;
    freeBay(bay);
    flashBay(bay, MSG_INFO);
    logMsg(MSG_INFO, "%s left bay %d. The bay is now vacant.", b->number, bay);
}

/* ---------------------------------------------------------------------
 *  Trip management
 * ------------------------------------------------------------------- */
void scheduleTrip(Bus *b)
{
    static const char *dest[] = {"CHANDIGARH", "LUDHIANA", "AMRITSAR", "SHIMLA", "DELHI", "JALANDHAR", "PATIALA"};

    if (!b->inService)        { logMsg(MSG_FAIL, "%s is under maintenance.", b->number); return; }
    if (b->status == ON_TRIP) { logMsg(MSG_FAIL, "%s is already on a trip.", b->number); return; }
    if (findTripFor(b->id, T_SCHEDULED) >= 0) { logMsg(MSG_FAIL, "%s already has a scheduled trip.", b->number); return; }
    if (tripCount >= MAX_TRIPS) { logMsg(MSG_FAIL, "Trip list is full."); return; }

    Trip t;
    memset(&t, 0, sizeof t);
    t.id = nextTripId();
    snprintf(t.destination, sizeof t.destination, "%s", dest[(t.id - 1) % 7]);
    snprintf(t.departure, sizeof t.departure, "%02d:%02d", 6 + ((t.id - 1) * 2) % 16, ((t.id - 1) * 15) % 60);
    t.busId  = b->id;
    t.status = T_SCHEDULED;

    for (int i = 0; i < driverCount; i++)                     /* auto-assign the first free driver */
        if (drivers[i].available) { drivers[i].available = 0; t.driverId = drivers[i].id; break; }

    trips[tripCount++] = t;

    if (t.driverId)
        logMsg(MSG_OK, "Trip %d: %s to %s at %s (driver %s).", t.id, b->number, t.destination,
               t.departure, drivers[findDriverById(t.driverId)].name);
    else
        logMsg(MSG_OK, "Trip %d: %s to %s at %s (no driver yet).", t.id, b->number, t.destination, t.departure);
}

void dispatchBus(Bus *b)
{
    int ti = findTripFor(b->id, T_SCHEDULED);
    int ok = 1;

    /* The four rules: trip scheduled, bus in service, bus parked, driver assigned */
    if (ti < 0)        { logMsg(MSG_FAIL, "Dispatch refused: no scheduled trip for %s.", b->number); ok = 0; }
    if (!b->inService) { logMsg(MSG_FAIL, "Dispatch refused: %s is under maintenance.", b->number); ok = 0; }
    if (!(b->status == PARKED && b->bay > 0 && bays[b->bay] == b->id)) {
        logMsg(MSG_FAIL, "Dispatch refused: %s is not parked in a bay.", b->number); ok = 0;
    }
    if (ti >= 0 && findDriverById(trips[ti].driverId) < 0) {
        logMsg(MSG_FAIL, "Dispatch refused: no driver assigned to trip %d.", trips[ti].id); ok = 0;
    }
    if (!ok) return;

    Trip *t = &trips[ti];
    int bay = b->bay;
    b->status = ON_TRIP;
    t->status = T_ONGOING;
    freeBay(bay);                                             /* the bus drives out of its bay */
    flashBay(bay, MSG_TRIP);
    logMsg(MSG_OK, "%s dispatched to %s with %s. Bay %d vacated.", b->number, t->destination,
           drivers[findDriverById(t->driverId)].name, bay);
}

void returnBus(Bus *b)
{
    int ti = findTripFor(b->id, T_ONGOING);
    if (ti < 0) { logMsg(MSG_FAIL, "%s is not on a trip.", b->number); return; }

    Trip *t = &trips[ti];
    b->status = AVAILABLE;
    int di = findDriverById(t->driverId);
    if (di >= 0) drivers[di].available = 1;
    t->status = T_COMPLETED;
    logMsg(MSG_OK, "%s returned from %s and is Available. Park it to reuse.", b->number, t->destination);
}

/* ---------------------------------------------------------------------
 *  Depot dashboard
 * ------------------------------------------------------------------- */
DepotStats getDepotStats(void)
{
    DepotStats s;
    memset(&s, 0, sizeof s);

    s.totalBuses = busCount;
    for (int i = 0; i < busCount; i++) {
        if (buses[i].status == AVAILABLE)   s.available++;
        else if (buses[i].status == PARKED) s.parked++;
        else                                s.onTrip++;
    }
    for (int i = 1; i <= NUM_BAYS; i++) if (bays[i]) s.occupiedBays++;
    s.vacantBays   = NUM_BAYS - s.occupiedBays;
    s.waiting      = queueCount();
    s.totalDrivers = driverCount;
    for (int i = 0; i < driverCount; i++) if (drivers[i].available) s.freeDrivers++;
    for (int i = 0; i < tripCount; i++) {
        if (trips[i].status == T_SCHEDULED)      s.tripsScheduled++;
        else if (trips[i].status == T_ONGOING)   s.tripsOngoing++;
        else if (trips[i].status == T_COMPLETED) s.tripsCompleted++;
    }
    return s;
}