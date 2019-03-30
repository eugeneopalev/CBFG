#include "pch.h"
#include "font.h"
#include "defs.h"
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

#define STBIW_ZLIB_COMPRESS zlib_compress
#define STB_IMAGE_WRITE_STATIC
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb/stb_image_write.h"

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

bool Font::SetFontName(char *NewName)
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

char *Font::GetFontName()
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
	HFONT hFont;
	int RowDex, ColDex, Letter;
	HRGN ClipRgn;
	RECT CharArea;
	char Symbol[2];
	int eVal;
	unsigned char *img;

	// create device context
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
	hBitmap = CreateDIBSection(hdc, &bmi, DIB_RGB_COLORS, (void **)&img, NULL, 0);
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

	// draw selection
	SetDCPenColor(hdc, SelCol);
	SetDCBrushColor(hdc, SelCol);
	if (sel > -1)
	{
		RowDex = (sel / (MapWidth / CellWidth));
		ColDex = (sel - ((MapWidth / CellWidth) * RowDex));
		ColDex *= CellWidth;
		RowDex *= CellHeight;
		Rectangle(hdc, ColDex, RowDex, ColDex + CellWidth, RowDex + CellHeight);
	}

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

	unsigned char *a = img + 3;
	unsigned char *b = (unsigned char *)bmInfo.dsBm.bmBits;
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
	unsigned char *img;

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
	hBitmap = CreateDIBSection(hdc, &bmi, DIB_RGB_COLORS, (void **)&img, NULL, 0);
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

	SelectObject(hdc, hOldBitmap);

	return hBitmap;
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
	lf_.lfHeight = tVal;
	memcpy(&tVal, &dat[26], 4);
	lf_.lfWidth = tVal;
	memcpy(&Flags, &dat[30], 4);
	memcpy(&GridCol, &dat[34], sizeof(GridCol));
	memcpy(&WidthCol, &dat[46], sizeof(WidthCol));
	memcpy(&SelCol, &dat[58], sizeof(SelCol));
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
	tVal = (int)lf_.lfHeight;
	cfgfile.write((const char *)&tVal, sizeof(int));
	tVal = (int)lf_.lfWidth;
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
