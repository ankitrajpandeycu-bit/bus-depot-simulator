/*
 * models.h  -  DATA MODELS
 * ------------------------
 * The core structures of the project (Bus, Driver, Trip), the status
 * enumerations and the size limits. Every other file includes this one.
 */
#ifndef MODELS_H
#define MODELS_H

/* ---------- Limits ---------- */
#define MAX_BUSES    50
#define MAX_DRIVERS  50
#define MAX_TRIPS    200
#define NUM_BAYS     5      /* number of parking bays in the depot */
#define STR_LEN      32

/* ---------- Status values ---------- */
typedef enum { AVAILABLE = 0, PARKED = 1, ON_TRIP = 2 } BusStatus;
typedef enum { T_SCHEDULED = 0, T_ONGOING = 1, T_COMPLETED = 2, T_CANCELLED = 3 } TripStatus;

/* ---------- Records ---------- */
typedef struct {
    int  id;
    char number[STR_LEN];   /* registration number, e.g. PB65-1234 */
    char model[STR_LEN];
    int  capacity;
    int  inService;         /* 1 = fit for service, 0 = under maintenance */
    int  status;            /* BusStatus */
    int  bay;               /* bay number (1..NUM_BAYS), 0 = not parked */
} Bus;

typedef struct {
    int  id;
    char name[STR_LEN];
    char license[STR_LEN];
    int  available;         /* 1 = free, 0 = assigned / on a trip */
} Driver;

typedef struct {
    int  id;
    char destination[STR_LEN];
    char departure[8];      /* HH:MM */
    int  busId;
    int  driverId;          /* 0 = not yet assigned */
    int  status;            /* TripStatus */
} Trip;

#endif
