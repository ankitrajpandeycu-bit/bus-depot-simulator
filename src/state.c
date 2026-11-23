/* state.c  -  see state.h */
#include <string.h>
#include "state.h"

Bus    buses[MAX_BUSES];
Driver drivers[MAX_DRIVERS];
Trip   trips[MAX_TRIPS];
int    busCount = 0, driverCount = 0, tripCount = 0;
int    bays[NUM_BAYS + 1];

int findBusById(int id)
{
    for (int i = 0; i < busCount; i++) if (buses[i].id == id) return i;
    return -1;
}

int findBusByNumber(const char *number)
{
    for (int i = 0; i < busCount; i++) if (strcmp(buses[i].number, number) == 0) return i;
    return -1;
}

int findDriverById(int id)
{
    for (int i = 0; i < driverCount; i++) if (drivers[i].id == id) return i;
    return -1;
}

int findTripFor(int busId, int status)
{
    for (int i = 0; i < tripCount; i++)
        if (trips[i].busId == busId && trips[i].status == status) return i;
    return -1;
}

int findFreeBay(void)
{
    for (int i = 1; i <= NUM_BAYS; i++) if (bays[i] == 0) return i;
    return 0;
}

int nextBusId(void)    { int m = 0; for (int i = 0; i < busCount; i++)    if (buses[i].id > m)   m = buses[i].id;   return m + 1; }
int nextDriverId(void) { int m = 0; for (int i = 0; i < driverCount; i++) if (drivers[i].id > m) m = drivers[i].id; return m + 1; }
int nextTripId(void)   { int m = 0; for (int i = 0; i < tripCount; i++)   if (trips[i].id > m)   m = trips[i].id;   return m + 1; }
