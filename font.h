#ifndef FONT_H
#define FONT_H

#define SBM_OK              0x1
#define SBM_ERR_NO_FILE     0x2
#define SBM_ERR_MEM_FAIL    0x4
#define SBM_ERR_BAD_FORMAT  0x8
#define SBM_ERR_UNSUPPORTED 0x40

class Font
{
public:
	Font():
		color_(RGB(255, 255, 255)),
		BkCol(RGB(0, 0, 0)),
		GridCol(RGB(170, 0, 170)),
		WidthCol(RGB(170, 170, 0)),
		SelCol(RGB(0, 154, 0))
	{
		ZeroMemory(&lf_, sizeof(lf_));

		BaseChar = 32;

		MapWidth = 256;
		MapHeight = 256;
		CellHeight = 32;
		CellWidth = 32;
		gHMod = 0;
		gVMod = 0;
		gWidthMod = 0;

		for (int loop = 0; loop != 256; loop++)
		{
			HMod[loop] = 0;
			VMod[loop] = 0;
			WidthMod[loop] = 0;
		}

		/*lf_.lfHeight = 20;
		lf_.lfWidth = 0;
		lf_.lfEscapement = 0;
		lf_.lfOrientation = 0;
		lf_.lfWeight = FW_NORMAL;
		lf_.lfItalic = FALSE;
		lf_.lfUnderline = FALSE;
		lf_.lfStrikeOut = FALSE;
		lf_.lfCharSet = DEFAULT_CHARSET;
		lf_.lfOutPrecision = OUT_DEFAULT_PRECIS;
		lf_.lfClipPrecision = CLIP_DEFAULT_PRECIS;
		lf_.lfQuality = NONANTIALIASED_QUALITY;
		lf_.lfPitchAndFamily = DEFAULT_PITCH;
		lf_.lfFaceName[0] = NULL;*/
	}
	~Font()
	{
	}

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
	long GetFontQuality();
	long SetFontQuality(long);
	char *GetFontName();
	bool SetFontName(char *);
	bool CalcWidths(HDC);
	HBITMAP DrawBitmap(HDC hdc, int flags, int sel);

	int  LoadConfig(const char *fname);
	bool SaveConfig(const char *fname, bool Grid, bool Width_);
	void ResetOffsets();

	bool SaveFont(int Format, char *fname, int Flags = 0);
	int  ExportMap(char *fname, int fmt);

	void SetCol(int Which, COLORREF Col);
	COLORREF GetCol(int Which);

	LPLOGFONT GetLogicalFont()
	{
		return &lf_;
	}

	COLORREF GetColor() const
	{
		return color_;
	}
	void SetColor(COLORREF color)
	{
		color_ = color;
	}

private:
	HBITMAP DrawAlphaBitmap(HDC hdc, HFONT hFnt);

	LOGFONT lf_;
	COLORREF color_;

	int  MapWidth, MapHeight;
	int  CellHeight, CellWidth;
	unsigned char BaseChar;
	int BaseWidth[256];
	int WidthMod[256];
	int VMod[256];
	int HMod[256];
	int gWidthMod, gHMod, gVMod;
	COLORREF BkCol, GridCol, WidthCol, SelCol;

	bool IsPower(int TestVal);
	bool ExportCSVData(char *fname);
};

#endif
