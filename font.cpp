#include "pch.h"
#include "font.h"
#include "bfg.h"
#include "utils.h"
#include "resource.h"

#include "zlib/zlib.h"

static unsigned char *zlib_compress(unsigned char *data, int data_len, int *out_len, int quality)
{
	uLong bufSize = compressBound(data_len);

	unsigned char *buf = (unsigned char *)malloc(bufSize);
	if (buf == NULL)
	{
		return NULL;
	}
	if (compress2(buf, &bufSize, data, data_len, quality) != Z_OK)
	{
		free(buf);
		return NULL;
	}
	*out_len = bufSize;

	return buf;
}

//#define STBIW_ZLIB_COMPRESS zlib_compress
#define STB_IMAGE_WRITE_STATIC
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb/stb_image_write.h"

Font::Font()
{
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

	lf.lfHeight = 20;
	lf.lfWidth = 0;
	lf.lfEscapement = 0;
	lf.lfOrientation = 0;
	lf.lfWeight = FW_NORMAL;
	lf.lfItalic = FALSE;
	lf.lfUnderline = FALSE;
	lf.lfStrikeOut = FALSE;
	lf.lfCharSet = DEFAULT_CHARSET;
	lf.lfOutPrecision = OUT_DEFAULT_PRECIS;
	lf.lfClipPrecision = CLIP_DEFAULT_PRECIS;
	lf.lfQuality = NONANTIALIASED_QUALITY;
	lf.lfPitchAndFamily = DEFAULT_PITCH;
	lf.lfFaceName[0] = NULL;

	BkCol.Red = 0;
	BkCol.Green = 0;
	BkCol.Blue = 0;

	TextCol.Red = 255;
	TextCol.Green = 255;
	TextCol.Blue = 255;

	GridCol.Red = 170;
	GridCol.Green = 0;
	GridCol.Blue = 170;

	WidthCol.Red = 170;
	WidthCol.Green = 170;
	WidthCol.Blue = 0;

	SelCol.Red = 0;
	SelCol.Green = 154;
	SelCol.Blue = 0;
}

Font::~Font()
{
}

int Font::SetSize(int Which, int NewSize)
{
	switch (Which)
	{
	case MAPWIDTH:
		if (!IsPower(NewSize))
		{
			NewSize = 256;
		}

		MapWidth = NewSize;
		return MapWidth;

	case MAPHEIGHT:
		if (!IsPower(NewSize))
		{
			NewSize = 256;
		}

		MapHeight = NewSize;
		return MapHeight;

	case CELLWIDTH:
		if (NewSize < 8)
		{
			CellWidth = 8;
		}
		else if (NewSize > 256)
		{
			CellWidth = 256;
		}
		else
		{
			CellWidth = NewSize;
		}

		return CellWidth;

	case CELLHEIGHT:
		if (NewSize < 8)
		{
			CellHeight = 8;
		}
		else if (NewSize > 256)
		{
			CellHeight = 256;
		}
		else
		{
			CellHeight = NewSize;
		}

		return CellHeight;
	}

	return 0;
}

int Font::GetSize(int Which)
{
	switch (Which)
	{
	case MAPWIDTH:
		return MapWidth;

	case MAPHEIGHT:
		return MapHeight;

	case CELLWIDTH:
		return CellWidth;

	case CELLHEIGHT:
		return CellHeight;

	case MAXCHARS:
		return (MapWidth / CellWidth) * (MapHeight / CellHeight);
	}

	return 0;
}

unsigned char Font::SetBaseChar(int NewBase)
{
	if (NewBase < 0)
	{
		NewBase = 0;
	}

	if (NewBase > 255)
	{
		NewBase = 255;
	}

	return BaseChar = (unsigned char)NewBase;
}

unsigned char Font::GetBaseChar()
{
	return BaseChar;
}

int Font::SetGlobal(int Which, int Value)
{
	switch (Which)
	{
	case VOFFSET:
		gVMod = Value;
		break;

	case HOFFSET:
		gHMod = Value;
		break;

	case WIDTH:
		gWidthMod = Value;
		break;
	}

	return Value;
}

int Font::GetGlobal(int Which)
{
	switch (Which)
	{
	case VOFFSET:
		return gVMod;

	case HOFFSET:
		return gHMod;

	case WIDTH:
		return gWidthMod;
	}

	return 0;
}

int Font::SetCharVal(int Char, int Which, int NewVal)
{
	switch (Which)
	{
	case WOFFSET:
		WidthMod[Char] = NewVal;
		break;

	case HOFFSET:
		HMod[Char] = NewVal;
		break;

	case VOFFSET:
		VMod[Char] = NewVal;
		break;
	}

	return NewVal;
}

int Font::GetCharVal(int Char, int Which)
{
	switch (Which)
	{
	case WIDTH:
		return BaseWidth[Char];

	case HOFFSET:
		return HMod[Char];

	case VOFFSET:
		return VMod[Char];

	case WOFFSET:
		return WidthMod[Char];

	case EWIDTH:
		return WidthMod[Char] + BaseWidth[Char] + gWidthMod;
	}
	return 0;
}

long Font::SetFontHeight(long NewHeight)
{
	if (NewHeight < 1)
	{
		NewHeight = 1;
	}
	if (NewHeight > 256)
	{
		NewHeight = 256;
	}

	lf.lfHeight = NewHeight;
	return lf.lfHeight;
}

long Font::GetFontHeight()
{
	return lf.lfHeight;
}

long Font::SetFontWidth(long NewWidth)
{
	if (NewWidth < 0)
	{
		NewWidth = 0;
	}
	if (NewWidth > 256)
	{
		NewWidth = 256;
	}

	lf.lfWidth = NewWidth;
	return lf.lfWidth;
}

long Font::GetFontWidth()
{
	return lf.lfWidth;
}

bool Font::SetFontName(char *NewName)
{
	if (lstrcpy(lf.lfFaceName, NewName))
	{
		return true;
	}
	else
	{
		return false;
	}
}

char *Font::GetFontName()
{
	return lf.lfFaceName;
}

long Font::SetFontWeight(long NewWeight)
{
	lf.lfWeight = NewWeight;
	return lf.lfWeight;
}

long Font::GetFontWeight()
{
	return lf.lfWeight;
}

long Font::SetFontQuality(long NewQual)
{
	lf.lfQuality = (BYTE)NewQual;
	return lf.lfQuality;
}

long Font::GetFontQuality()
{
	return lf.lfQuality;
}

long Font::SetFontItalic(long NewItal)
{
	lf.lfItalic = (BYTE)NewItal;
	return lf.lfItalic;
}

long Font::GetFontItalic()
{
	return lf.lfItalic;
}

void Font::SetCol(int Which, BFG_RGB NewCol)
{
	BFG_RGB *Tgt;

	switch (Which)
	{
	case GRIDCOL:
		Tgt = &GridCol;
		break;

	case WIDTHCOL:
		Tgt = &WidthCol;
		break;

	case SELCOL:
		Tgt = &SelCol;
		break;

	case TEXTCOL:
		Tgt = &TextCol;
		break;

	case BACKCOL:
		Tgt = &BkCol;
		break;

	default:
		return;
	}

	Tgt->Red = NewCol.Red;
	Tgt->Green = NewCol.Green;
	Tgt->Blue = NewCol.Blue;
}

void Font::SetCol(int Which, unsigned char Red, unsigned char Green, unsigned char Blue)
{
	BFG_RGB *Tgt;

	switch (Which)
	{
	case GRIDCOL:
		Tgt = &GridCol;
		break;

	case WIDTHCOL:
		Tgt = &WidthCol;
		break;

	case SELCOL:
		Tgt = &SelCol;
		break;

	case TEXTCOL:
		Tgt = &TextCol;
		break;

	case BACKCOL:
		Tgt = &BkCol;
		break;

	default:
		return;
	}

	Tgt->Red = Red;
	Tgt->Green = Green;
	Tgt->Blue = Blue;
}

BFG_RGB Font::GetCol(int Which)
{
	switch (Which)
	{
	case GRIDCOL:
		return GridCol;
		break;

	case WIDTHCOL:
		return WidthCol;
		break;

	case SELCOL:
		return SelCol;
		break;

	case TEXTCOL:
		return TextCol;
		break;

	case BACKCOL:
		return BkCol;
		break;
	}

	return BkCol; // Default
}

bool Font::CalcWidths(HDC hdc)
{
	BOOL Test;
	int Letter;
	ABC CharWidth[256];
	int nttWidth[256];

	// Populate Width data
	Test = GetCharABCWidths(hdc, 0, 255, CharWidth);

	if (Test)
	{
		for (Letter = 0; Letter != 256; Letter++)
		{
			BaseWidth[Letter] = (unsigned char)(CharWidth[Letter].abcA + CharWidth[Letter].abcB + CharWidth[Letter].abcC);
		}
	}
	else
	{
		// GetCharWidth32 for non truetype fonts
		Test = GetCharWidth32(hdc, 0, 255, nttWidth);

		if (Test)
		{
			for (Letter = 0; Letter != 256; Letter++)
			{
				BaseWidth[Letter] = (unsigned char)nttWidth[Letter];
			}
		}
	}

	return true;
}

HBITMAP Font::DrawBitmap(HDC hdc, int flags, int sel)
{
	HBITMAP hBitmap;
	BITMAPINFO bmi;
	HPEN hPen;
	HBRUSH hBrush;
	HFONT hFont;
	int RowDex, ColDex, Letter;
	HRGN ClipRgn;
	RECT CharArea;
	char Symbol[2];
	int eVal;
	unsigned char *img;

	// create device context
	hdc = CreateCompatibleDC(hdc);
	if (!hdc)
	{
		return NULL;
	}

	ZeroMemory(&bmi, sizeof(bmi));
	bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bmi.bmiHeader.biWidth = MapWidth;
	bmi.bmiHeader.biHeight = MapHeight;
	bmi.bmiHeader.biPlanes = 1;
	bmi.bmiHeader.biBitCount = 32;
	bmi.bmiHeader.biCompression = BI_RGB;
	bmi.bmiHeader.biSizeImage = (MapWidth * MapHeight) * 4;
	hBitmap = CreateDIBSection(hdc, &bmi, DIB_RGB_COLORS, (void **)&img, NULL, 0);
	SelectObject(hdc, hBitmap);

	// draw background
	if (flags & DFM_ALPHA)
	{
		hPen = CreatePen(PS_SOLID, 0, RGB(0, 0, 0));
		hBrush = CreateSolidBrush(RGB(0, 0, 0));
	}
	else
	{
		hPen = CreatePen(PS_SOLID, 0, RGB(BkCol.Red, BkCol.Green, BkCol.Blue));
		hBrush = CreateSolidBrush(RGB(BkCol.Red, BkCol.Green, BkCol.Blue));
	}
	SelectObject(hdc, hPen);
	SelectObject(hdc, hBrush);

	Rectangle(hdc, 0, 0, MapWidth, MapHeight);

	DeleteObject(hBrush);
	DeleteObject(hPen);

	// draw selection
	hPen = CreatePen(PS_SOLID, 0, RGB(SelCol.Red, SelCol.Green, SelCol.Blue));
	hBrush = CreateSolidBrush(RGB(SelCol.Red, SelCol.Green, SelCol.Blue));

	if (sel > -1)
	{
		SelectObject(hdc, hPen);
		SelectObject(hdc, hBrush);
		RowDex = (sel / (MapWidth / CellWidth));
		ColDex = (sel - ((MapWidth / CellWidth) * RowDex));
		ColDex *= CellWidth;
		RowDex *= CellHeight;
		Rectangle(hdc, ColDex, RowDex, ColDex + CellWidth, RowDex + CellHeight);
	}

	DeleteObject(hBrush);
	DeleteObject(hPen);

	// draw characters
	hFont = CreateFontIndirect(&lf);
	SelectObject(hdc, hFont);

	CalcWidths(hdc);

	if (flags & DFM_ALPHA)
	{
		SetTextColor(hdc, RGB(255, 255, 255));
		SetBkColor(hdc, RGB(0, 0, 0));
	}
	else
	{
		SetTextColor(hdc, RGB(TextCol.Red, TextCol.Green, TextCol.Blue));
		SetBkColor(hdc, RGB(BkCol.Red, BkCol.Green, BkCol.Blue));
	}

	SetBkMode(hdc, TRANSPARENT);

	hPen = CreatePen(PS_SOLID, 0, RGB(WidthCol.Red, WidthCol.Green, WidthCol.Blue));
	SelectObject(hdc, hPen);

	Letter = BaseChar;

	for (RowDex = 0; RowDex < (MapHeight - CellHeight) + 1; RowDex += CellHeight)
	{
		for (ColDex = 0; ColDex < (MapWidth - CellWidth) + 1 && Letter < 256; ColDex += CellWidth)
		{
			// Set Clipping Region
			ClipRgn = CreateRectRgn(ColDex, RowDex, ColDex + CellWidth, RowDex + CellHeight);
			SelectClipRgn(hdc, ClipRgn);

			// Draw width marker
			if (flags & DFM_WIDTHLINE)
			{
				eVal = BaseWidth[Letter] + WidthMod[Letter] + gWidthMod;
				MoveToEx(hdc, ColDex + eVal, RowDex, NULL);
				LineTo(hdc, ColDex + eVal, RowDex + CellHeight);
			}

			// Render Char
			CharArea.left = ColDex + HMod[Letter] + gHMod;
			CharArea.right = ColDex + CellWidth;
			CharArea.top = RowDex + VMod[Letter] + gVMod;
			CharArea.bottom = RowDex + CellHeight;
			wsprintf(Symbol, "%c", Letter);
			Letter++;
			DrawText(hdc, Symbol, -1, &CharArea, DT_LEFT | DT_NOPREFIX | DT_NOCLIP);

			// Remove clip region
			SelectClipRgn(hdc, NULL);
			DeleteObject(ClipRgn);
		}
	}

	DeleteObject(hPen);

	// Draw grid lines
	hPen = CreatePen(PS_SOLID, 0, RGB(GridCol.Red, GridCol.Green, GridCol.Blue));

	if (flags & DFM_GRIDLINES)
	{
		SelectObject(hdc, hPen);

		for (RowDex = CellHeight - 1; RowDex < MapHeight; RowDex += CellHeight)
		{
			MoveToEx(hdc, 0, RowDex, NULL);
			LineTo(hdc, MapWidth, RowDex);
		}

		for (ColDex = CellWidth - 1; ColDex < MapWidth; ColDex += CellWidth)
		{
			MoveToEx(hdc, ColDex, 0, NULL);
			LineTo(hdc, ColDex, MapHeight);
		}
	}

	DeleteObject(hPen);

	// insert alpha channel
	HBITMAP hAlphaBitmap = DrawAlphaBitmap(hdc, hFont);

	DIBSECTION bmInfo;
	if (!GetObject(hAlphaBitmap, sizeof(DIBSECTION), &bmInfo))
	{
		return false;
	}

	unsigned char *a = img + 3;
	unsigned char *b = (unsigned char *)bmInfo.dsBm.bmBits;
	for (int i = 0; i < MapWidth * MapHeight; i++)
	{
		*a = *b;
		a += 4;
		b++;
	}

	DeleteObject(hFont);
	DeleteDC(hdc);

	return hBitmap;
}

HBITMAP Font::DrawAlphaBitmap(HDC hdc, HFONT hFnt)
{
	HBITMAP hBitmap;
	BITMAPINFO bmi;
	HPEN hPen;
	HBRUSH hBrush;
	int RowDex, ColDex, Letter;
	HRGN ClipRgn;
	RECT CharArea;
	char Symbol[2];
	unsigned char *img;

	ZeroMemory(&bmi, sizeof(bmi));
	bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bmi.bmiHeader.biWidth = MapWidth;
	bmi.bmiHeader.biHeight = MapHeight;
	bmi.bmiHeader.biPlanes = 1;
	bmi.bmiHeader.biBitCount = 8;
	bmi.bmiHeader.biCompression = BI_RGB;
	bmi.bmiHeader.biSizeImage = MapWidth * MapHeight;
	hBitmap = CreateDIBSection(hdc, &bmi, DIB_RGB_COLORS, (void **)&img, NULL, 0);
	SelectObject(hdc, hBitmap);

	// draw background
	hPen = CreatePen(PS_SOLID, 0, RGB(0, 0, 0));
	hBrush = CreateSolidBrush(RGB(0, 0, 0));
	SelectObject(hdc, hPen);
	SelectObject(hdc, hBrush);

	Rectangle(hdc, 0, 0, MapWidth, MapHeight);

	DeleteObject(hBrush);
	DeleteObject(hPen);

	// draw characters
	SelectObject(hdc, hFnt);

	CalcWidths(hdc);

	SetTextColor(hdc, RGB(255, 255, 255));
	SetBkMode(hdc, TRANSPARENT);

	Letter = BaseChar;
	for (RowDex = 0; RowDex < (MapHeight - CellHeight) + 1; RowDex += CellHeight)
	{
		for (ColDex = 0; ColDex < (MapWidth - CellWidth) + 1 && Letter < 256; ColDex += CellWidth)
		{
			// Set Clipping Region
			ClipRgn = CreateRectRgn(ColDex, RowDex, ColDex + CellWidth, RowDex + CellHeight);
			SelectClipRgn(hdc, ClipRgn);

			// Render Char
			CharArea.left = ColDex + HMod[Letter] + gHMod;
			CharArea.right = ColDex + CellWidth;
			CharArea.top = RowDex + VMod[Letter] + gVMod;
			CharArea.bottom = RowDex + CellHeight;
			wsprintf(Symbol, "%c", Letter);
			Letter++;
			DrawText(hdc, Symbol, -1, &CharArea, DT_LEFT | DT_NOPREFIX | DT_NOCLIP);

			// Remove clip region
			SelectClipRgn(hdc, NULL);
			DeleteObject(ClipRgn);
		}
	}

	return hBitmap;
}

LPLOGFONT Font::GetLogicalFont()
{
	return &lf;
}

int Font::LoadConfig(const char *fname)
{
	std::ifstream cfgfile;
	unsigned long fSize;
	char *dat;
	char Hdr[7];
	int tVal, Flags;

	cfgfile.open(fname, std::ios::binary);

	if (cfgfile.fail())
	{
		return -1;
	}

	cfgfile.seekg(0, std::ios_base::end);
	fSize = (unsigned long)cfgfile.tellg();
	cfgfile.seekg(0, std::ios_base::beg);

	dat = new char[fSize];

	if (!dat)
	{
		return -1;
	}

	cfgfile.read(dat, fSize);

	cfgfile.close();

	// Check ID
	lstrcpyn(Hdr, dat, 7);
	Hdr[6] = NULL;

	if (lstrcmp(Hdr, "BFGCFG"))
	{
		delete [] dat;
		return -1;
	}

	memcpy(&MapWidth, &dat[6], 4);
	memcpy(&MapHeight, &dat[10], 4);
	memcpy(&CellWidth, &dat[14], 4);
	memcpy(&CellHeight, &dat[18], 4);
	memcpy(&tVal, &dat[22], 4);
	lf.lfHeight = tVal;
	memcpy(&tVal, &dat[26], 4);
	lf.lfWidth = tVal;
	memcpy(&Flags, &dat[30], 4);
	memcpy(&GridCol, &dat[34], sizeof(GridCol));
	memcpy(&WidthCol, &dat[46], sizeof(WidthCol));
	memcpy(&SelCol, &dat[58], sizeof(SelCol));
	memcpy(&TextCol, &dat[70], sizeof(TextCol));
	memcpy(&BkCol, &dat[82], sizeof(BkCol));

	delete [] dat;

	return Flags;
}

bool Font::SaveConfig(const char *fname, bool Grid, bool Width_)
{
	std::ofstream cfgfile;
	int tVal, Flags = 0;

	cfgfile.open(fname, std::ios_base::binary | std::ios_base::trunc);

	if (cfgfile.fail())
	{
		return false;
	}

	cfgfile.write("BFGCFG", 6);
	cfgfile.write((const char *)&MapWidth, sizeof(int));
	cfgfile.write((const char *)&MapHeight, sizeof(int));
	cfgfile.write((const char *)&CellWidth, sizeof(int));
	cfgfile.write((const char *)&CellHeight, sizeof(int));
	tVal = (int)lf.lfHeight;
	cfgfile.write((const char *)&tVal, sizeof(int));
	tVal = (int)lf.lfWidth;
	cfgfile.write((const char *)&tVal, sizeof(int));
	if (Grid)
	{
		Flags |= SHOW_GRID;
	}
	if (Width_)
	{
		Flags |= SHOW_WIDTH;
	}
	cfgfile.write((const char *)&Flags, sizeof(Flags));
	cfgfile.write((const char *)&GridCol, sizeof(GridCol));
	cfgfile.write((const char *)&WidthCol, sizeof(WidthCol));
	cfgfile.write((const char *)&SelCol, sizeof(SelCol));
	cfgfile.write((const char *)&TextCol, sizeof(TextCol));
	cfgfile.write((const char *)&BkCol, sizeof(BkCol));

	cfgfile.close();

	return true;
}

void Font::ResetOffsets()
{
	int Loop;

	for (Loop = 0; Loop != 256; ++Loop)
	{
		WidthMod[Loop] = 0;
		VMod[Loop] = 0;
		HMod[Loop] = 0;
	}

	gWidthMod = gHMod = gVMod = 0;
}

bool Font::SaveFont(int Format, char *fname, int flags)
{
	bool Inv, Sat;
	Inv = Sat = false;

	if (flags & SAVE_INV_ALPHA)
	{
		Inv = true;
	}

	if (flags & SAVE_RGB_SAT)
	{
		Sat = true;
	}

	switch (Format)
	{
	case SAVE_CSV:
		return ExportCSVData(fname);
	}

	return false;
}

int Font::ExportMap(char *fname, int fmt)
{
	HWND hImgWin;
	HDC hdc;
	HBITMAP hBitmap;
	int Result;

	hImgWin = GetDlgItem(g_hMain, IMG_TEXT);
	hdc = GetDC(hImgWin);

	hBitmap = DrawBitmap(hdc, 0, -1);

	DIBSECTION bmInfo;
	if (!GetObject(hBitmap, sizeof(DIBSECTION), &bmInfo))
	{
		return false;
	}

	stbi_flip_vertically_on_write(1);

	switch (fmt)
	{
	case EXPORT_BMP:
		Result = stbi_write_bmp(fname, MapWidth, MapHeight, (32 / 8), bmInfo.dsBm.bmBits) != 0 ? SBM_OK : SBM_ERR_UNSUPPORTED;
		break;

	case EXPORT_TGA:
	case EXPORT_TGA32:
		Result = stbi_write_tga(fname, MapWidth, MapHeight, (32 / 8), bmInfo.dsBm.bmBits) != 0 ? SBM_OK : SBM_ERR_UNSUPPORTED;
		break;

	case EXPORT_PNG:
		Result = stbi_write_png(fname, MapWidth, MapHeight, (32 / 8), bmInfo.dsBm.bmBits, MapWidth * (32 / 8)) != 0 ? SBM_OK : SBM_ERR_UNSUPPORTED;
		break;

	default:
		Result = false;
		break;
	}

	ReleaseDC(hImgWin, hdc);

	return Result;
}

bool Font::ImportData(char *fname)
{
	UNREFERENCED_PARAMETER(fname);

#if 0
	FILE *in;
	long fsize, datptr;
	int Index, Val;
	char *data;

	in = fopen(fname, "r");

	if (in == NULL)
	{
		return FALSE;
	}

	// Get filesize
	fseek(in, 0, SEEK_END);
	fsize = ftell(in);
	rewind(in);

	// Allocate space for file contents
	data = new char[fsize];

	if (data == NULL)
	{
		fclose(in);
		return FALSE;
	}

	// Read in the file contents
	fread(data, fsize, 1, in);
	fclose(in);

	// Extract the font data
	datptr = 0;

	// Image Width
	while (data[datptr] != ',')
	{
		++datptr;
	}

	datptr++;
	sscanf(&data[datptr], "%d", &(cfg->ImgSize));

	// Image Height
	while (data[datptr] != ',')
	{
		++datptr;
	}

	datptr++;
	sscanf(&data[datptr], "%d", &(cfg->ImgSize));

	// Cell Width
	while (data[datptr] != ',')
	{
		++datptr;
	}

	datptr++;
	sscanf(&data[datptr], "%d", &(cfg->CellHeight));

	// Cell Height
	while (data[datptr] != ',')
	{
		++datptr;
	}

	datptr++;
	sscanf(&data[datptr], "%d", &(cfg->CellHeight));

	// Start char
	while (data[datptr] != ',')
	{
		++datptr;
	}

	datptr++;
	sscanf(&data[datptr], "%d", &(cfg->CharBase));

	// Font Name
	while (data[datptr] != ',')
	{
		++datptr;
	}

	datptr++;
	Index = 0;
	while (data[datptr] != '\n')
	{
		cfg->lf.lfFaceName[Index] = data[datptr];
		++Index;
		++datptr;
	}
	cfg->lf.lfFaceName[Index] = NULL;

	// Font Height
	while (data[datptr] != ',')
	{
		++datptr;
	}

	datptr++;
	sscanf(&data[datptr], "%d", &(cfg->lf.lfHeight));

	// Font Width
	while (data[datptr] != ',')
	{
		++datptr;
	}

	datptr++;
	sscanf(&data[datptr], "%d", &(cfg->lf.lfWidth));

	// Char Widths
	for (Index = 0; Index != 256; ++Index)
	{
		while (data[datptr] != ',')
		{
			++datptr;
		}

		datptr++;
		sscanf(&data[datptr], "%d", &Val);
		cfg->width[Index] = Val; // Prevents stack damage
	}

	// Char X Offsets
	for (Index = 0; Index != 256; ++Index)
	{
		while (data[datptr] != ',')
		{
			++datptr;
		}

		datptr++;
		sscanf(&data[datptr], "%d", &Val);
		cfg->hAdj[Index] = Val;
	}

	// Char Y Offsets
	for (Index = 0; Index != 256; ++Index)
	{
		while (data[datptr] != ',')
		{
			++datptr;
		}

		datptr++;
		sscanf(&data[datptr], "%d", &Val);
		cfg->vAdj[Index] = Val;
	}

	// Global Width Offset
	while (data[datptr] != ',')
	{
		++datptr;
	}

	datptr++;
	sscanf(&data[datptr], "%d", &Val);
	cfg->gwAdj = Val;

	// Global X Offset
	while (data[datptr] != ',')
	{
		++datptr;
	}

	datptr++;
	sscanf(&data[datptr], "%d", &Val);
	cfg->ghAdj = Val;

	// Global Y Offset
	while (data[datptr] != ',')
	{
		++datptr;
	}

	datptr++;
	sscanf(&data[datptr], "%d", &Val);
	cfg->gvAdj = Val;

	// Bold Value
	while (data[datptr] != ',')
	{
		++datptr;
	}

	datptr++;
	sscanf(&data[datptr], "%d", &Val);
	cfg->lf.lfWeight = Val;

	// Italic Value
	while (data[datptr] != ',')
	{
		++datptr;
	}

	datptr++;
	sscanf(&data[datptr], "%d", &Val);
	cfg->lf.lfItalic = Val;

	// AntiAlias Value
	while (data[datptr] != ',')
	{
		++datptr;
	}

	datptr++;
	sscanf(&data[datptr], "%d", &Val);
	cfg->lf.lfQuality = Val;

	delete[] data;
#endif

	return TRUE;
}

bool Font::ExportCSVData(char *fname)
{
	std::ofstream out;
	int Loop;

	out.open(fname, std::ios::out | std::ios::trunc);
	if (out.fail())
	{
		return false;
	}

	out << "Image Width," << MapWidth << "\n";
	out << "Image Height," << MapHeight << "\n";
	out << "Cell Width," << CellWidth << "\n";
	out << "Cell Height," << CellHeight << "\n";
	out << "Start Char," << (int)BaseChar << "\n";
	out << "Font Name," << lf.lfFaceName << "\n";
	out << "Font Height," << lf.lfHeight << "\n";
	out << "Font Width (0 is default)," << lf.lfWidth << "\n";

	for (Loop = 0; Loop != 256; ++Loop)
	{
		out << "Char " << Loop << " Base Width," << (int)BaseWidth[Loop] << "\n";
	}

	for (Loop = 0; Loop != 256; ++Loop)
	{
		out << "Char " << Loop << " Width Offset," << (int)WidthMod[Loop] << "\n";
	}

	for (Loop = 0; Loop != 256; ++Loop)
	{
		out << "Char " << Loop << " X Offset," << (int)HMod[Loop] << "\n";
	}

	for (Loop = 0; Loop != 256; ++Loop)
	{
		out << "Char " << Loop << " Y Offset," << (int)VMod[Loop] << "\n";
	}

	out << "Global Width Offset," << (int)gWidthMod << "\n";
	out << "Global X Offset," << (int)gHMod << "\n";
	out << "Global Y Offset," << (int)gVMod << "\n";
	out << "Bold," << lf.lfWeight << "\n";
	out << "Italic," << (int)lf.lfItalic << "\n";
	out << "AntiAlias," << (int)lf.lfQuality << "\n";

	out.close();

	return TRUE;
}

bool Font::IsPower(int TestValue)
{
	bool Ret = FALSE;
	float Val;

	Val = (float)TestValue;

	while (Val >= 2.0f)
	{
		if (Val == 2.0f)
		{
			Ret = TRUE;
		}

		Val = Val / 2.0f;
	}

	return Ret;
}
