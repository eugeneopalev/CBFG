#ifndef FONT_H
#define FONT_H

#define EXPORT_BMP 1
#define EXPORT_TGA 2
#define EXPORT_PNG 3

class Font
{
public:
	Font();
	~Font();

	void GetCharRect(HDC hdc, char chr, PRECT rect);

	int GetSize(int Which);
	int SetSize(int Which, int NewSize);
	unsigned char GetBaseChar();
	unsigned char SetBaseChar(int);
	int GetCharVal(int Char, int Which);
	long GetFontHeight();
	long SetFontHeight(long);
	long GetFontWidth();
	long SetFontWidth(long);
	long GetFontQuality();
	long SetFontQuality(long);
	char* GetFontName();
	bool SetFontName(char*);
	bool CalcWidths(HDC);

	// Render bitmap to specific device
	HBITMAP DrawBitmap(HDC hdc, int flags);

	bool SaveFont(int Format, char* fname, int Flags = 0);
	bool Export(char* fname, int fmt);

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

	int MapWidth, MapHeight;
	int CellHeight, CellWidth;
	unsigned char BaseChar;
	int BaseWidth[256];
	COLORREF BkCol, GridCol, WidthCol, SelCol;

	bool IsPower(int TestVal);
	bool ExportCSVData(char* fname);
};

#endif
