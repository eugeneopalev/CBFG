#ifndef UTILS_H
#define UTILS_H

#define EXPORT_TGA   1
#define EXPORT_BMP   2
#define EXPORT_TXT   3
#define EXPORT_TGA32 4
#define EXPORT_PNG   5

typedef struct
{
	int CellWidth, CellHeight, ImgSize;
	LOGFONT FntDef;
	HFONT fnt;
	int CharBase;
	char width[256];
	char wAdj[256];
	char hAdj[256];
	char vAdj[256];
	char ghAdj, gvAdj, gwAdj;
} Config;

char Limit(int Val);
int LimitSelection(int Sel, int MaxChar);
void CreateFontMap();
HBITMAP* RenderFont(bool Markers);
unsigned char* MakeAlpha();
void CalcScroll();
int ExportMap(char* fname, int fmt);

#endif
