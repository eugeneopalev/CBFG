#ifndef CONFIG_H
#define CONFIG_H

typedef struct config_s
{
	int ImgWidth, ImgHeight, CellWidth, CellHeight, FontHeight, FontWidth;
	COLORREF BackCol, ForeCol, GridCol, WidthCol, SelCol;
	int Flags;
	int hScr, vScr;
	bool Grid, wMarker, vScroll, hScroll;
	float Zoom;
	int MaxChars;
} config_t;

extern config_t g_config;

void LoadConfig(void);
void SaveConfig(void);

#endif
