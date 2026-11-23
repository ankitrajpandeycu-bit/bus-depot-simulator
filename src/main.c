/*
 * main.c  -  PROGRAM ENTRY POINT
 * ------------------------------
 * Opens the window, loads saved data, runs the main loop
 * (input -> draw), and saves the data when the program ends.
 *
 * Bus Depot Operation Simulator - raylib GUI edition
 */
#include <string.h>
#include "raylib.h"
#include "ui_layout.h"
#include "ui_scene.h"
#include "ui_input.h"
#include "ui_draw.h"
#include "state.h"
#include "depot.h"
#include "storage.h"

/* Used only by "--snapshot file.png" (saves one screenshot and exits).
 * Handy for testing without clicking. */
static void loadDemoScenario(void)
{
    for (int i = 0; i < 3; i++) addDriver();
    for (int i = 0; i < 7; i++) addBus();
    for (int i = 0; i < 7; i++) parkBus(&buses[i]);

    scheduleTrip(&buses[0]); dispatchBus(&buses[0]);
    scheduleTrip(&buses[1]); dispatchBus(&buses[1]);
    scheduleTrip(&buses[2]);
    dispatchBus(&buses[3]);               /* refused: no trip scheduled */
    selId = buses[2].id;
    tick = 150;
}

int main(int argc, char *argv[])
{
    const char *snapshot = (argc >= 3 && strcmp(argv[1], "--snapshot") == 0) ? argv[2] : NULL;

    SetTraceLogLevel(LOG_WARNING);
    InitWindow(WIN_W, WIN_H, "Bus Depot Operation Simulator");
    SetExitKey(KEY_NULL);                  /* we handle Esc ourselves so data gets saved */
    SetTargetFPS(60);
    initUiFont();
    layoutButtons();

    if (snapshot) loadDemoScenario();
    else          loadData();

    int quit = 0, frames = 0;
    while (!WindowShouldClose() && !quit) {
        /* ---- input ---- */
        Vector2 m = GetMousePosition();
        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) handleClick((int)m.x, (int)m.y);
        handleKeys();
        if (IsKeyPressed(KEY_ESCAPE)) quit = 1;

        /* ---- draw ---- */
        BeginDrawing();
        renderScene((int)m.x, (int)m.y);
        EndDrawing();

        if (snapshot && ++frames == 5) {
            Image img = LoadImageFromScreen();
            ExportImage(img, snapshot);
            UnloadImage(img);
            break;
        }
    }

    if (!snapshot) saveData();
    CloseWindow();
    return 0;
}
