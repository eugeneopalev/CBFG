#ifndef FONT_H
#define FONT_H

#include "defs.h"

#define SBM_OK              0x1
#define SBM_ERR_NO_FILE     0x2
#define SBM_ERR_MEM_FAIL    0x4
#define SBM_ERR_BAD_FORMAT  0x8
#define SBM_ERR_UNSUPPORTED 0x40

class Font
{
public:
	Font();
	~Font();
	int  GetSize(int Which);
	int  SetSize(int Which, int NewSize);
	unsigned char GetBaseChar();
	unsigned char SetBaseChar(int);
	int GetGlobal(int Which);
	int SetGlobal(int Which, int NewVal);
	int GetCharVal(int Char, int Which);
	int SetCharVal(int Char, int Which, int NewVal);
	long GetFontHeight();
	long SetFontHeight(long);
	long GetFontWidth();
	long SetFontWidth(long);
	long GetFontWeight();
	long SetFontWeight(long);
	long GetFontQuality();
	long SetFontQuality(long);
	long GetFontItalic();
	long SetFontItalic(long);
	char *GetFontName();
	bool SetFontName(char *);
	bool CalcWidths(HDC);
	HBITMAP DrawBitmap(HDC hdc, int flags, int sel);

	LPLOGFONT GetLogicalFont();

	int  LoadConfig(const char *fname);
	bool SaveConfig(const char *fname, bool Grid, bool Width_);
	void ResetOffsets();

	bool SaveFont(int Format, char *fname, int Flags = 0);
	int  ExportMap(char *fname, int fmt);
	bool ImportData(char *fname);

	void SetCol(int Which, BYTE Red, BYTE Green, BYTE Blue);
	void SetCol(int Which, BFG_RGB Col);
	BFG_RGB GetCol(int Which);

private:
	HBITMAP DrawAlphaBitmap(HDC hdc, HFONT hFnt);

	LOGFONT lf;
	int  MapWidth, MapHeight;
	int  CellHeight, CellWidth;
	unsigned char BaseChar;
	int BaseWidth[256];
	int WidthMod[256];
	int VMod[256];
	int HMod[256];
	int gWidthMod, gHMod, gVMod;
	BFG_RGB BkCol, TextCol, GridCol, WidthCol, SelCol;

	bool IsPower(int TestVal);
	bool ExportCSVData(char *fname);
};

#endif
