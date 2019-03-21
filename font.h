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
	HBITMAP *DrawFontMap(int Flags, int Sel);

	int  LoadConfig(const char *fname);
	bool SaveConfig(const char *fname, bool Grid, bool Width_);
	void ResetOffsets();

	bool SaveFont(int Format, char *fname, int Flags = 0);
	int  ExportMap(char *fname, int fmt);
	bool ImportData(char *fname);

	void SetCol(int Which, BYTE Red, BYTE Green, BYTE Blue);
	void SetCol(int Which, BFG_RGB Col);
	BFG_RGB GetCol(int Which);

	// SBM_Image stuff
	void Init_SBM_Image();
	void Deinit_SBM_Image();
	int Create(int width, int height, int bpp);
	void Reset();                  // Clear Image
	unsigned char *GetImg();       // Return a pointer to image data
	
	// Utility Functions
	void FlipImg();   // Invert image vertically
	int InsertAlpha(unsigned char *Alpha); // Adds an alpha channel to image
	int Grayscale();  // Converts image to 8 bit gray
	int InvertCol();  // Inverts colour values
	void BGRtoRGB();  // Convert between RGB and BGR formats

	// Sets all non-KeyCol pixels to SatCol
	int Saturate(unsigned char KeyR, unsigned char KeyG, unsigned char KeyB, unsigned char SatR, unsigned char SatG, unsigned char SatB);

private:
	LOGFONT FntDef;
	HFONT fnt;
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

	// SBM_Image stuff
	int Width, Height;
	int BPP, Encode, Planes;
	unsigned long FileSize, ImageSize, Offset;
	unsigned char *ImgData, *PalData, *FileData;
	short BPL;

	void FreeMem(void **Ptr); // Safe delete []
};

#endif
