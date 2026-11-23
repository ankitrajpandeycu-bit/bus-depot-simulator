/*
 * ui_draw.h  -  DRAWING TOOLKIT (raylib)
 * --------------------------------------
 * Colours and small drawing helpers: rectangles, text, panels, and the
 * little bus picture. Nothing here knows about depot rules.
 */
#ifndef UI_DRAW_H
#define UI_DRAW_H

#include "raylib.h"
#include "models.h"
#include "eventlog.h"

/* ---------- Colours ---------- */
extern const Color C_BG, C_PANEL, C_BORDER, C_TEXT, C_DIM, C_YELLOW;
extern const Color C_OK, C_FAIL, C_INFO;
extern const Color C_AVAIL, C_PARKED, C_TRIP, C_MAINT;

Color       msgColor(MsgKind kind);          /* log message kind -> colour */
Color       busColor(const Bus *b);          /* bus status -> colour */
const char *statusShort(const Bus *b);       /* "PARKED", "TRIP", ... */

/* ---------- Primitives ---------- */
void initUiFont(void);
void fillRect(int x, int y, int w, int h, Color c);
void strokeRect(int x, int y, int w, int h, Color c);
int  hit(int mx, int my, int x, int y, int w, int h);   /* is the point inside the box? */

/* text size = 10 * scale pixels (scale 1, 2, 3) */
void drawText(int x, int y, int scale, Color c, const char *text);
int  textW(int scale, const char *text);
void textf(int x, int y, int scale, Color c, const char *fmt, ...);

/* ---------- Composite widgets ---------- */
void panel(int x, int y, int w, int h, const char *title);
void statRow(int y, const char *label, Color c, const char *fmt, ...);
void drawBus(int x, int y, int w, int h, Color c, int selected);

#endif
