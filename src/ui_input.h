/*
 * ui_input.h  -  MOUSE AND KEYBOARD HANDLING
 */
#ifndef UI_INPUT_H
#define UI_INPUT_H

void layoutButtons(void);                 /* position the 9 buttons (call once at startup) */
void handleClick(int mouseX, int mouseY); /* left mouse click: button press or bus selection */
void handleKeys(void);                    /* Delete = delete bus, M = maintenance */

#endif
