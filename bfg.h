#ifndef BFG_H
#define BFG_H

#include "font.h"

#define SHOW_GRID   0x1
#define SHOW_WIDTH  0x2
#define FONT_BOLD   0x4
#define FONT_ITALIC 0x8

#define WIDTH   1
#define EWIDTH  6

// SetSize Constants
#define MAPWIDTH    1
#define MAPHEIGHT   2
#define CELLWIDTH   3
#define CELLHEIGHT  4
#define MAXCHARS    10

// DrawFontMap Constants
#define DFM_WIDTHLINE 2
#define DFM_GRIDLINES 4
#define DFM_ALPHA     8

// Config Window Constants
#define GRIDCOL  0
#define WIDTHCOL 1
#define SELCOL   2
#define BACKCOL  4

// FontSave Constants
#define SAVE_CSV   110

#define SAVE_RGB_SAT   0x100
#define SAVE_INV_ALPHA 0x200

extern HINSTANCE g_hInstance;
extern HWND g_hMain;
extern Font g_font;
extern HBRUSH g_hBackground;

#endif
