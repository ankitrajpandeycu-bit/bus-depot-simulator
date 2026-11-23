/*
 * depot.h  -  DEPOT RULES (business logic)
 * ----------------------------------------
 * All the operations of the simulator: registering buses and drivers,
 * parking, scheduling, dispatching and returning. Each function checks the
 * depot rules, changes the data, and reports the result with logMsg().
 *
 * This module knows nothing about windows or drawing.
 */
#ifndef DEPOT_H
#define DEPOT_H

#include "models.h"

/* Bus management */
Bus    *addBus(void);                 /* returns the new bus, or NULL if full */
int     deleteBus(Bus *b);            /* returns 1 if deleted */
void    toggleMaintenance(Bus *b);

/* Driver management */
Driver *addDriver(void);
int     deleteDriver(Driver *d);      /* returns 1 if deleted (refused while on a trip) */
void    assignDriver(Bus *b);         /* give the bus's scheduled trip a free driver */

/* Parking management */
void    parkBus(Bus *b);              /* free bay -> park, otherwise join the waiting queue */
void    releaseBay(Bus *b);           /* bus leaves its bay */

/* Trip management */
void    scheduleTrip(Bus *b);
void    dispatchBus(Bus *b);          /* checks all rules, then Parked -> On Trip */
void    returnBus(Bus *b);            /* On Trip -> Available, driver freed */

/* Depot dashboard */
typedef struct {
    int totalBuses, available, parked, onTrip;
    int occupiedBays, vacantBays, waiting;
    int freeDrivers, totalDrivers;
    int tripsScheduled, tripsOngoing, tripsCompleted;
} DepotStats;

DepotStats getDepotStats(void);

#endif