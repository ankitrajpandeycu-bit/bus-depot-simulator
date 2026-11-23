/* ui_draw.c  -  see ui_draw.h */
#include <stdio.h>
#include <stdarg.h>
#include "ui_draw.h"

const Color C_BG     = {20, 24, 32, 255};
const Color C_PANEL  = {34, 40, 52, 255};
const Color C_BORDER = {80, 92, 115, 255};
const Color C_TEXT   = {225, 230, 240, 255};
const Color C_DIM    = {140, 150, 170, 255};
const Color C_YELLOW = {250, 215, 70, 255};
const Color C_OK     = {90, 210, 120, 255};
const Color C_FAIL   = {240, 90, 90, 255};
const Color C_INFO   = {120, 180, 250, 255};
const Color C_AVAIL  = {70, 135, 240, 255};
const Color C_PARKED = {60, 180, 95, 255};
const Color C_TRIP   = {240, 150, 40, 255};
const Color C_MAINT  = {120, 125, 135, 255};

static Font gFont;

/* ---------- Colour / label mapping ---------- */
Color msgColor(MsgKind kind)
{
    switch (kind) {
        case MSG_OK:   return C_OK;
        case MSG_FAIL: return C_FAIL;
        case MSG_TRIP: return C_TRIP;
        default:       return C_INFO;
    }
}

Color busColor(const Bus *b)
{
    if (!b->inService) return C_MAINT;
    switch (b->status) {
        case PARKED:  return C_PARKED;
        case ON_TRIP: return C_TRIP;
        default:      return C_AVAIL;
    }
}

const char *statusShort(const Bus *b)
{
    if (!b->inService) return "MAINT";
    switch (b->status) {
        case PARKED:  return "PARKED";
        case ON_TRIP: return "TRIP";
        default:      return "AVAIL";
    }
}

/* ---------- Primitives ---------- */
void initUiFont(void) { gFont = GetFontDefault(); }

void fillRect(int x, int y, int w, int h, Color c)   { DrawRectangle(x, y, w, h, c); }
void strokeRect(int x, int y, int w, int h, Color c) { DrawRectangleLines(x, y, w, h, c); }

int hit(int mx, int my, int x, int y, int w, int h)
{
    return mx >= x && mx < x + w && my >= y && my < y + h;
}

void drawText(int x, int y, int scale, Color c, const char *text)
{
    DrawTextEx(gFont, text, (Vector2){(float)x, (float)y}, (float)(10 * scale), (float)scale, c);
}

int textW(int scale, const char *text)
{
    return (int)MeasureTextEx(gFont, text, (float)(10 * scale), (float)scale).x;
}

void textf(int x, int y, int scale, Color c, const char *fmt, ...)
{
    char buf[128];
    va_list ap;
    va_start(ap, fmt);
    vsnprintf(buf, sizeof buf, fmt, ap);
    va_end(ap);
    drawText(x, y, scale, c, buf);
}

/* ---------- Composite widgets ---------- */
void panel(int x, int y, int w, int h, const char *title)
{
    fillRect(x, y, w, h, C_PANEL);
    strokeRect(x, y, w, h, C_BORDER);
    drawText(x + 8, y + 8, 2, C_YELLOW, title);
}

/* "LABEL ........ value" row, value right-aligned (status panel) */
void statRow(int y, const char *label, Color c, const char *fmt, ...)
{
    char buf[64];
    va_list ap;
    va_start(ap, fmt);
    vsnprintf(buf, sizeof buf, fmt, ap);
    va_end(ap);
    drawText(900, y, 2, c, label);
    drawText(1180 - textW(2, buf), y, 2, c, buf);
}

/* A simple side-view bus: body, roof strip, windows, headlight, wheels. */
void drawBus(int x, int y, int w, int h, Color c, int selected)
{
    fillRect(x, y, w, h, c);
    fillRect(x, y, w, 4, (Color){(unsigned char)(c.r / 2 + 100), (unsigned char)(c.g / 2 + 100),
                                 (unsigned char)(c.b / 2 + 100), 255});
    int step = (w - 14) / 4;
    for (int i = 0; i < 4; i++)
        fillRect(x + 7 + i * step, y + 8, step - 5, h / 3, (Color){190, 225, 250, 255});
    fillRect(x + w - 4, y + h / 2, 4, 6, C_YELLOW);                                  /* headlight */
    fillRect(x + w / 6, y + h - 3, w / 7, 7, (Color){15, 15, 15, 255});              /* wheels */
    fillRect(x + w - w / 3, y + h - 3, w / 7, 7, (Color){15, 15, 15, 255});
    if (selected) {
        strokeRect(x - 3, y - 3, w + 6, h + 12, C_YELLOW);
        strokeRect(x - 4, y - 4, w + 8, h + 14, C_YELLOW);
    }
}
