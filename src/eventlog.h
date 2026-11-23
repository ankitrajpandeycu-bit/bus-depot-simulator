/*
 * eventlog.h  -  EVENT LOG + BAY FLASH EFFECTS
 * --------------------------------------------
 * The depot logic reports what happened by calling logMsg(). The screen
 * simply displays the most recent messages. Message kinds (not colours)
 * are used here so this file does not depend on the graphics library.
 */
#ifndef EVENTLOG_H
#define EVENTLOG_H

#include "models.h"

#define LOG_LINES 5

typedef enum { MSG_OK, MSG_FAIL, MSG_INFO, MSG_TRIP } MsgKind;

typedef struct {
    char    text[110];
    MsgKind kind;
} LogEntry;

void logMsg(MsgKind kind, const char *fmt, ...);
const LogEntry *getLogEntry(int index);      /* 0 = oldest shown, LOG_LINES-1 = newest */

/* A bay "flashes" for a short time after something happens in it. */
void flashBay(int bay, MsgKind kind);
int  bayFlashActive(int bay, MsgKind *kind); /* 1 while flashing */
void updateFlashes(void);                    /* call once per frame */

#endif
