/* eventlog.c  -  see eventlog.h */
#include <stdio.h>
#include <stdarg.h>
#include "eventlog.h"

static LogEntry entries[LOG_LINES];
static int      flashFrames[NUM_BAYS + 1];
static MsgKind  flashKind[NUM_BAYS + 1];

void logMsg(MsgKind kind, const char *fmt, ...)
{
    for (int i = 0; i < LOG_LINES - 1; i++) entries[i] = entries[i + 1];   /* scroll up */
    va_list ap;
    va_start(ap, fmt);
    vsnprintf(entries[LOG_LINES - 1].text, sizeof entries[0].text, fmt, ap);
    va_end(ap);
    entries[LOG_LINES - 1].kind = kind;
}

const LogEntry *getLogEntry(int index)
{
    if (index < 0 || index >= LOG_LINES) return NULL;
    return &entries[index];
}

void flashBay(int bay, MsgKind kind)
{
    if (bay < 1 || bay > NUM_BAYS) return;
    flashFrames[bay] = 40;
    flashKind[bay] = kind;
}

int bayFlashActive(int bay, MsgKind *kind)
{
    if (bay < 1 || bay > NUM_BAYS || flashFrames[bay] <= 0) return 0;
    *kind = flashKind[bay];
    return 1;
}

void updateFlashes(void)
{
    for (int i = 1; i <= NUM_BAYS; i++) if (flashFrames[i] > 0) flashFrames[i]--;
}