/* storage.c  -  see storage.h */
#include <stdio.h>
#include <string.h>
#include "storage.h"
#include "state.h"
#include "eventlog.h"

#define BUS_FILE     "buses.dat"
#define DRIVER_FILE  "drivers.dat"
#define TRIP_FILE    "trips.dat"

/* File layout: an int record count, followed by that many records. */
static int writeFile(const char *name, const void *data, size_t size, int count)
{
    FILE *fp = fopen(name, "wb");
    if (!fp) { logMsg(MSG_FAIL, "Could not write %s.", name); return 0; }
    fwrite(&count, sizeof count, 1, fp);
    if (count > 0) fwrite(data, size, (size_t)count, fp);
    fclose(fp);
    return 1;
}

static int readFile(const char *name, void *data, size_t size, int max, int *count)
{
    FILE *fp = fopen(name, "rb");
    if (!fp) return 0;                                  /* first run: no file yet */
    int n = 0;
    if (fread(&n, sizeof n, 1, fp) != 1 || n < 0 || n > max) { fclose(fp); return 0; }
    if (n > 0 && fread(data, size, (size_t)n, fp) != (size_t)n) { fclose(fp); return 0; }
    fclose(fp);
    *count = n;
    return 1;
}

void saveData(void)
{
    int ok = writeFile(BUS_FILE,    buses,   sizeof(Bus),    busCount);
    ok &=    writeFile(DRIVER_FILE, drivers, sizeof(Driver), driverCount);
    ok &=    writeFile(TRIP_FILE,   trips,   sizeof(Trip),   tripCount);
    if (ok) logMsg(MSG_OK, "Saved %d buses, %d drivers, %d trips.", busCount, driverCount, tripCount);
}

void loadData(void)
{
    readFile(BUS_FILE,    buses,   sizeof(Bus),    MAX_BUSES,   &busCount);
    readFile(DRIVER_FILE, drivers, sizeof(Driver), MAX_DRIVERS, &driverCount);
    readFile(TRIP_FILE,   trips,   sizeof(Trip),   MAX_TRIPS,   &tripCount);

    /* rebuild the bay table from the saved bus records */
    memset(bays, 0, sizeof bays);
    for (int i = 0; i < busCount; i++) {
        int bay = buses[i].bay;
        if (buses[i].status == PARKED && bay >= 1 && bay <= NUM_BAYS && bays[bay] == 0)
            bays[bay] = buses[i].id;
        else if (buses[i].status == PARKED) { buses[i].status = AVAILABLE; buses[i].bay = 0; }
    }

    if (busCount || driverCount || tripCount)
        logMsg(MSG_INFO, "Loaded %d buses, %d drivers, %d trips from file.", busCount, driverCount, tripCount);
    else
        logMsg(MSG_INFO, "Empty depot. Click ADD BUS and ADD DRV to begin.");
}
