#include "pch.h"
#include "font.h"
#include "config.h"
#include "bfg.h"
#include "resource.h"

#include "zlib/zlib.h"

static unsigned char* zlib_compress(unsigned char* data, int data_len, int* out_len, int quality)
{
	uLong bufSize = compressBound(data_len);

	unsigned char* buf = (unsigned char*)malloc(bufSize);
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

#define STBIW_ZLIB_COMPRESS zlib_compress
#define STB_IMAGE_WRITE_STATIC
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb/stb_image_write.h"

Font::Font() :
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

	stbi_flip_vertically_on_write(1);
}

Font::~Font()
{
}

void Font::GetCharRect(HDC hdc, char chr, PRECT rect)
{
	char buf[2];

	rect->left = 0;
	rect->right = 0;
	rect->top = 0;
	rect->bottom = 0;
	wsprintf(buf, "%c", chr);
	int ret = DrawText(hdc, buf, -1, rect, DT_NOCLIP | DT_CALCRECT | DT_NOPREFIX);
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

int Font::GetCharVal(int Char, int Which)
{
	switch (Which)
	{
	case WIDTH:
	case EWIDTH:
		return BaseWidth[Char];
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

	lf_.lfHeight = NewHeight;
	return lf_.lfHeight;
}

long Font::GetFontHeight()
{
	return lf_.lfHeight;
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

	lf_.lfWidth = NewWidth;
	return lf_.lfWidth;
}

long Font::GetFontWidth()
{
	return lf_.lfWidth;
}

bool Font::SetFontName(char* NewName)
{
	if (lstrcpy(lf_.lfFaceName, NewName))
	{
		return true;
	}
	else
	{
		return false;
	}
}

char* Font::GetFontName()
{
	return lf_.lfFaceName;
}

long Font::SetFontQuality(long NewQual)
{
	lf_.lfQuality = (BYTE)NewQual;
	return lf_.lfQuality;
}

long Font::GetFontQuality()
{
	return lf_.lfQuality;
}

void Font::SetCol(int Which, COLORREF NewCol)
{
	switch (Which)
	{
	case GRIDCOL:
		GridCol = NewCol;
		break;

	case WIDTHCOL:
		WidthCol = NewCol;
		break;

	case SELCOL:
		SelCol = NewCol;
		break;

	case BACKCOL:
		BkCol = NewCol;
		break;

	default:
		return;
	}
}

COLORREF Font::GetCol(int Which)
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

	case BACKCOL:
		return BkCol;
		break;
	}

	return BkCol;
}

bool Font::CalcWidths(HDC hdc)
{	
	ABC CharWidth[256];
	int nttWidth[256];
	int i;

	if (GetCharABCWidths(hdc, 0, 255, CharWidth))
	{
		for (i = 0; i != 256; i++)
		{
			BaseWidth[i] = CharWidth[i].abcA + CharWidth[i].abcB + CharWidth[i].abcC;
		}
	}
	// GetCharWidth32 for non truetype fonts
	else if (GetCharWidth32(hdc, 0, 255, nttWidth))
	{
		for (i = 0; i != 256; i++)
		{
			BaseWidth[i] = nttWidth[i];
		}
	}

	return true;
}

HBITMAP Font::DrawBitmap(HDC hdc, int flags)
{
	HBITMAP hBitmap;
	BITMAPINFO bmi;
	HFONT hFont;
	int RowDex, ColDex, Letter;
	HRGN ClipRgn;
	RECT CharArea;
	char Symbol[2];
	int eVal;
	unsigned char* img;

	// create memory device context
	hdc = CreateCompatibleDC(hdc);
	if (hdc == NULL)
	{
		return NULL;
	}
	SelectObject(hdc, GetStockObject(DC_PEN));
	SelectObject(hdc, GetStockObject(DC_BRUSH));

	// create bitmap
	ZeroMemory(&bmi, sizeof(bmi));
	bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bmi.bmiHeader.biWidth = MapWidth;
	bmi.bmiHeader.biHeight = MapHeight;
	bmi.bmiHeader.biPlanes = 1;
	bmi.bmiHeader.biBitCount = 32;
	bmi.bmiHeader.biCompression = BI_RGB;
	bmi.bmiHeader.biSizeImage = (MapWidth * MapHeight) * 4;
	hBitmap = CreateDIBSection(hdc, &bmi, DIB_RGB_COLORS, (void**)&img, NULL, 0);
	if (hBitmap == NULL)
	{
		return NULL;
	}

	ERROR_INVALID_PARAMETER;

	SelectObject(hdc, hBitmap);

	// draw background
	if (flags & DFM_ALPHA)
	{
		SetDCPenColor(hdc, RGB(0, 0, 0));
		SetDCBrushColor(hdc, RGB(0, 0, 0));
	}
	else
	{
		SetDCPenColor(hdc, BkCol);
		SetDCBrushColor(hdc, BkCol);
	}
	Rectangle(hdc, 0, 0, MapWidth, MapHeight);

	// draw characters
	hFont = CreateFontIndirect(&lf_);
	SelectObject(hdc, hFont);

	CalcWidths(hdc);

	if (flags & DFM_ALPHA)
	{
		SetTextColor(hdc, RGB(255, 255, 255));
		SetBkColor(hdc, RGB(0, 0, 0));
	}
	else
	{
		SetTextColor(hdc, color_);
		SetBkColor(hdc, BkCol);
	}

	SetBkMode(hdc, TRANSPARENT);

	SetDCPenColor(hdc, WidthCol);
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
				eVal = BaseWidth[Letter];
				MoveToEx(hdc, ColDex + eVal, RowDex, NULL);
				LineTo(hdc, ColDex + eVal, RowDex + CellHeight);
			}

			// Render Char
			CharArea.left = ColDex;
			CharArea.right = ColDex + CellWidth;
			CharArea.top = RowDex;
			CharArea.bottom = RowDex + CellHeight;
			wsprintf(Symbol, "%c", Letter);
			Letter++;
			DrawText(hdc, Symbol, -1, &CharArea, DT_NOPREFIX | DT_NOCLIP);

			GetCharRect(hdc, Letter, &CharArea);

			// Remove clip region
			SelectClipRgn(hdc, NULL);
			DeleteObject(ClipRgn);
		}
	}

	// draw grid lines
	SetDCPenColor(hdc, GridCol);
	if (flags & DFM_GRIDLINES)
	{
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

	// insert alpha channel
	HBITMAP hAlphaBitmap = DrawAlphaBitmap(hdc, hFont);

	DIBSECTION bmInfo;
	if (!GetObject(hAlphaBitmap, sizeof(DIBSECTION), &bmInfo))
	{
		return false;
	}

	unsigned char* a = img + 3;
	unsigned char* b = (unsigned char*)bmInfo.dsBm.bmBits;
	for (int i = 0; i < MapWidth * MapHeight; i++)
	{
		*a = *b;
		a += 4;
		b++;
	}

	DeleteObject(hAlphaBitmap);
	DeleteObject(hFont);

	DeleteDC(hdc);

	return hBitmap;
}

HBITMAP Font::DrawAlphaBitmap(HDC hdc, HFONT hFnt)
{
	HBITMAP hBitmap, hOldBitmap;
	BITMAPINFO bmi;
	int RowDex, ColDex, Letter;
	HRGN ClipRgn;
	RECT CharArea;
	char Symbol[2];
	unsigned char* img;

	//SelectObject(hdc, GetStockObject(DC_PEN));
	//SelectObject(hdc, GetStockObject(DC_BRUSH));

	ZeroMemory(&bmi, sizeof(bmi));
	bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bmi.bmiHeader.biWidth = MapWidth;
	bmi.bmiHeader.biHeight = MapHeight;
	bmi.bmiHeader.biPlanes = 1;
	bmi.bmiHeader.biBitCount = 8;
	bmi.bmiHeader.biCompression = BI_RGB;
	bmi.bmiHeader.biSizeImage = MapWidth * MapHeight;
	hBitmap = CreateDIBSection(hdc, &bmi, DIB_RGB_COLORS, (void**)&img, NULL, 0);
	hOldBitmap = (HBITMAP)SelectObject(hdc, hBitmap);

	// draw background
	SetDCPenColor(hdc, RGB(0, 0, 0));
	SetDCBrushColor(hdc, RGB(0, 0, 0));
	Rectangle(hdc, 0, 0, MapWidth, MapHeight);

	// draw characters
	//SelectObject(hdc, hFnt);

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
			CharArea.left = ColDex;
			CharArea.right = ColDex + CellWidth;
			CharArea.top = RowDex;
			CharArea.bottom = RowDex + CellHeight;
			wsprintf(Symbol, "%c", Letter);
			Letter++;
			DrawText(hdc, Symbol, -1, &CharArea, DT_NOPREFIX | DT_NOCLIP);

			// Remove clip region
			SelectClipRgn(hdc, NULL);
			DeleteObject(ClipRgn);
		}
	}

	SelectObject(hdc, hOldBitmap);

	return hBitmap;
}

bool Font::SaveFont(int Format, char* fname, int flags)
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

bool Font::Export(char* fname, int fmt)
{
	HWND hImgWin;
	HDC hdc;
	HBITMAP hBitmap;
	bool ret;

	hImgWin = GetDlgItem(g_hMain, IMG_TEXT);
	hdc = GetDC(hImgWin);

	hBitmap = DrawBitmap(hdc, 0);

	DIBSECTION bmInfo;
	if (!GetObject(hBitmap, sizeof(DIBSECTION), &bmInfo))
	{
		return false;
	}

	switch (fmt)
	{
	case EXPORT_BMP:
		ret = stbi_write_bmp(fname, MapWidth, MapHeight, (32 / 8), bmInfo.dsBm.bmBits) != 0;
		break;

	case EXPORT_TGA:
		ret = stbi_write_tga(fname, MapWidth, MapHeight, (32 / 8), bmInfo.dsBm.bmBits) != 0;
		break;

	case EXPORT_PNG:
		ret = stbi_write_png(fname, MapWidth, MapHeight, (32 / 8), bmInfo.dsBm.bmBits, MapWidth * (32 / 8)) != 0;
		break;

	default:
		ret = false;
	}

	ReleaseDC(hImgWin, hdc);

	return ret;
}

bool Font::ExportCSVData(char* fname)
{
#if 0
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
	out << "Font Name," << lf_.lfFaceName << "\n";
	out << "Font Height," << lf_.lfHeight << "\n";
	out << "Font Width (0 is default)," << lf_.lfWidth << "\n";

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
	out << "Bold," << lf_.lfWeight << "\n";
	out << "Italic," << (int)lf_.lfItalic << "\n";
	out << "AntiAlias," << (int)lf_.lfQuality << "\n";

	out.close();
#endif

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
