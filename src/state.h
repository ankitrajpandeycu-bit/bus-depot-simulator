/*
 * state.h  -  GLOBAL DEPOT DATA + LOOKUP HELPERS
 * ----------------------------------------------
 * Holds the arrays of buses, drivers and trips, the parking-bay table,
 * and small helper functions to find records.
 */
#ifndef STATE_H
#define STATE_H

#include "models.h"

extern Bus    buses[MAX_BUSES];
extern Driver drivers[MAX_DRIVERS];
extern Trip   trips[MAX_TRIPS];
extern int    busCount, driverCount, tripCount;

/* bays[i] = id of the bus parked in bay i, or 0 if vacant (index 0 unused) */
extern int    bays[NUM_BAYS + 1];

/* Each find function returns an array index, or -1 if not found. */
int findBusById(int id);
int findBusByNumber(const char *number);
int findDriverById(int id);
int findTripFor(int busId, int status);   /* first trip of this bus with this status */

int findFreeBay(void);                    /* first vacant bay number, 0 if none */

int nextBusId(void);
int nextDriverId(void);
int nextTripId(void);

#endif
