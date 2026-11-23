/*
 * storage.h  -  FILE HANDLING
 * ---------------------------
 * Saves and loads the bus, driver and trip records so the data survives
 * a restart. Files: buses.dat, drivers.dat, trips.dat (same format as the
 * console version of the project).
 */
#ifndef STORAGE_H
#define STORAGE_H

void saveData(void);
void loadData(void);

#endif
