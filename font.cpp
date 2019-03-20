#include "pch.h"
#include "font.h"
#include "utils.h"

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
	int loop;

	BaseChar = 32;

	MapWidth = 256;
	MapHeight = 256;
	CellHeight = 32;
	CellWidth = 32;
	gHMod = 0;
	gVMod = 0;
	gWidthMod = 0;

	for (loop = 0; loop != 256; loop++)
	{
		HMod[loop] = 0;
		VMod[loop] = 0;
		WidthMod[loop] = 0;
	}

	fnt = NULL;

	FntDef.lfHeight = 20;
	FntDef.lfWidth = 0;
	FntDef.lfEscapement = 0;
	FntDef.lfOrientation = 0;
	FntDef.lfWeight = FW_NORMAL;
	FntDef.lfItalic = FALSE;
	FntDef.lfUnderline = FALSE;
	FntDef.lfStrikeOut = FALSE;
	FntDef.lfCharSet = DEFAULT_CHARSET;
	FntDef.lfOutPrecision = OUT_DEFAULT_PRECIS;
	FntDef.lfClipPrecision = CLIP_DEFAULT_PRECIS;
	FntDef.lfQuality = NONANTIALIASED_QUALITY;
	FntDef.lfPitchAndFamily = DEFAULT_PITCH;
	FntDef.lfFaceName[0] = NULL;

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
	DeleteObject(fnt);
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

	FntDef.lfHeight = NewHeight;
	return FntDef.lfHeight;
}

long Font::GetFontHeight()
{
	return FntDef.lfHeight;
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

	FntDef.lfWidth = NewWidth;
	return FntDef.lfWidth;
}

long Font::GetFontWidth()
{
	return FntDef.lfWidth;
}

bool Font::SetFontName(char *NewName)
{
	if (lstrcpy(FntDef.lfFaceName, NewName))
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
	return FntDef.lfFaceName;
}

long Font::SetFontWeight(long NewWeight)
{
	FntDef.lfWeight = NewWeight;
	return FntDef.lfWeight;
}

long Font::GetFontWeight()
{
	return FntDef.lfWeight;
}

long Font::SetFontQuality(long NewQual)
{
	FntDef.lfQuality = (BYTE)NewQual;
	return FntDef.lfQuality;
}

long Font::GetFontQuality()
{
	return FntDef.lfQuality;
}

long Font::SetFontItalic(long NewItal)
{
	FntDef.lfItalic = (BYTE)NewItal;
	return FntDef.lfItalic;
}

long Font::GetFontItalic()
{
	return FntDef.lfItalic;
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
			BaseWidth[Letter] = (unsigned char)(CharWidth[Letter].abcA +
			                                    CharWidth[Letter].abcB +
			                                    CharWidth[Letter].abcC);
	}
	else
	{
		// GetCharWidth32 for non truetype fonts
		Test = GetCharWidth32(hdc, 0, 255, nttWidth);

		if (Test)
			for (Letter = 0; Letter != 256; Letter++)
			{
				BaseWidth[Letter] = (unsigned char)nttWidth[Letter];
			}
	}

	return true;
}

HBITMAP *Font::DrawFontMap(int Flags, int Sel)
{
	HDC wDC, mDC;
	HBITMAP *fDIB;
	BITMAPINFO BMDat;
	HBRUSH Brush;
	HPEN Pen;
	int RowDex, ColDex, Letter;
	HRGN ClipRgn;
	RECT CharArea;
	char Symbol[2];
	int eVal;

	// Create Device context
	wDC = CreateDC("DISPLAY", NULL, NULL, NULL);
	mDC = CreateCompatibleDC(wDC);

	if (!wDC || !mDC)
	{
		return NULL;
	}

	// Create bitmap for font rendering
	fDIB = new HBITMAP;
	if (!fDIB)
	{
		return NULL;
	}

	BMDat.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	BMDat.bmiHeader.biWidth = MapWidth;
	BMDat.bmiHeader.biHeight = MapHeight;
	BMDat.bmiHeader.biPlanes = 1;
	BMDat.bmiHeader.biBitCount = 24;
	BMDat.bmiHeader.biCompression = BI_RGB;
	BMDat.bmiHeader.biSizeImage = (MapWidth * MapHeight) * 3;

	*fDIB = CreateDIBSection(mDC, &BMDat, DIB_RGB_COLORS, NULL, NULL, 0);

	if (!fDIB)
	{
		return NULL;
	}

	if (!SelectObject(mDC, *fDIB))
	{
		return NULL;
	}

	// Fill background
	if (Flags & DFM_ALPHA)
	{
		Brush = CreateSolidBrush(RGB(0, 0, 0));
		Pen = CreatePen(PS_SOLID, 0, RGB(0, 0, 0));
	}
	else
	{
		Brush = CreateSolidBrush(RGB(BkCol.Red, BkCol.Green, BkCol.Blue));
		Pen = CreatePen(PS_SOLID, 0, RGB(BkCol.Red, BkCol.Green, BkCol.Blue));
	}

	SelectObject(mDC, Brush);
	SelectObject(mDC, Pen);

	Rectangle(mDC, 0, 0, MapWidth, MapHeight);

	DeleteObject(Pen);
	DeleteObject(Brush);

	// Draw Selection
	Pen = CreatePen(PS_SOLID, 0, RGB(SelCol.Red, SelCol.Green, SelCol.Blue));
	Brush = CreateSolidBrush(RGB(SelCol.Red, SelCol.Green, SelCol.Blue));

	if (Sel > -1)
	{
		SelectObject(mDC, Pen);
		SelectObject(mDC, Brush);
		RowDex = (Sel / (MapWidth / CellWidth));
		ColDex = (Sel - ((MapWidth / CellWidth) * RowDex));
		ColDex *= CellWidth;
		RowDex *= CellHeight;
		Rectangle(mDC, ColDex, RowDex, ColDex + CellWidth, RowDex + CellHeight);
	}

	DeleteObject(Brush);
	DeleteObject(Pen);

	// Draw letters
	// Create the font
	if (fnt)
	{
		DeleteObject(fnt);
	}

	fnt = CreateFontIndirect(&FntDef);

	SelectObject(mDC, fnt);

	CalcWidths(mDC);

	if (Flags & DFM_ALPHA)
	{
		SetTextColor(mDC, RGB(255, 255, 255));
		SetBkColor(mDC, RGB(0, 0, 0));
	}
	else
	{
		SetTextColor(mDC, RGB(TextCol.Red, TextCol.Green, TextCol.Blue));
		SetBkColor(mDC, RGB(BkCol.Red, BkCol.Green, BkCol.Blue));
	}

	SetBkMode(mDC, TRANSPARENT);

	Pen = CreatePen(PS_SOLID, 0, RGB(WidthCol.Red, WidthCol.Green, WidthCol.Blue));
	SelectObject(mDC, Pen);

	Letter = BaseChar;

	for (RowDex = 0; RowDex < (MapHeight - CellHeight) + 1; RowDex += CellHeight)
	{
		for (ColDex = 0; ColDex < (MapWidth - CellWidth) + 1 && Letter < 256; ColDex += CellWidth)
		{
			// Set Clipping Region
			ClipRgn = CreateRectRgn(ColDex, RowDex, ColDex + CellWidth, RowDex + CellHeight);
			SelectClipRgn(mDC, ClipRgn);

			// Draw width marker
			if (Flags & DFM_WIDTHLINE)
			{
				eVal = BaseWidth[Letter] + WidthMod[Letter] + gWidthMod;
				MoveToEx(mDC, ColDex + eVal, RowDex, NULL);
				LineTo(mDC, ColDex + eVal, RowDex + CellHeight);
			}

			// Render Char
			CharArea.left = ColDex + HMod[Letter] + gHMod;
			CharArea.right = ColDex + CellWidth;
			CharArea.top = RowDex + VMod[Letter] + gVMod;
			CharArea.bottom = RowDex + CellHeight;
			wsprintf(Symbol, "%c", Letter);
			Letter++;
			DrawText(mDC, Symbol, -1, &CharArea, DT_LEFT | DT_NOPREFIX | DT_NOCLIP);

			// Remove clip region
			SelectClipRgn(mDC, NULL);
			DeleteObject(ClipRgn);
		}
	}

	DeleteObject(Pen);

	// Draw grid lines
	Pen = CreatePen(PS_SOLID, 0, RGB(GridCol.Red, GridCol.Green, GridCol.Blue));

	if (Flags & DFM_GRIDLINES)
	{
		SelectObject(mDC, Pen);

		for (RowDex = CellHeight - 1; RowDex < MapHeight; RowDex += CellHeight)
		{
			MoveToEx(mDC, 0, RowDex, NULL);
			LineTo(mDC, MapWidth, RowDex);
		}

		for (ColDex = CellWidth - 1; ColDex < MapWidth; ColDex += CellWidth)
		{
			MoveToEx(mDC, ColDex, 0, NULL);
			LineTo(mDC, ColDex, MapHeight);
		}
	}

	DeleteObject(Pen);
	DeleteDC(wDC);
	DeleteDC(mDC);

	return fDIB;
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
	FntDef.lfHeight = tVal;
	memcpy(&tVal, &dat[26], 4);
	FntDef.lfWidth = tVal;
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
	tVal = (int)FntDef.lfHeight;
	cfgfile.write((const char *)&tVal, sizeof(int));
	tVal = (int)FntDef.lfWidth;
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
	HBITMAP *hBMP;
	FontFileHeader Hdr;
	DIBSECTION bmInfo;
	int Result;

	Init_SBM_Image();

	// Populate header
	Hdr.ID1 = 0xBF;
	Hdr.ID2 = 0xF2;
	Hdr.BPP = 24;
	Hdr.ImageWidth = MapWidth;
	Hdr.ImageHeight = MapHeight;
	Hdr.CellHeight = CellHeight;
	Hdr.CellWidth = CellHeight;
	Hdr.StartPoint = BaseChar;

	// Create the SBM image
	Create(Hdr.ImageWidth, Hdr.ImageHeight, Hdr.BPP);

	// Render the font image
	hBMP = DrawFontMap(0, -1);

	// Grab the bitmap information
	if (!GetObject(*hBMP, sizeof(DIBSECTION), &bmInfo))
	{
		return false;
	}

	// Copy bitmap to SBM
	memcpy(GetImg(), bmInfo.dsBm.bmBits, (Hdr.ImageWidth * Hdr.ImageHeight) * (Hdr.BPP / 8));

	// Free the bitmap
	delete hBMP;

#if 0
	// Add in alpha channel if required
	if (fmt == EXPORT_TGA32)
	{
		// Render new alpha fontmap
		hBMP = DrawFontMap(DFM_ALPHA, -1);

		// Create the SBM alpha image
		AlphaImg.Create(Hdr.ImageWidth, Hdr.ImageHeight, Hdr.BPP);

		// Get RGB data ptr from Img
		if (!GetObject(*hBMP, sizeof(DIBSECTION), &bmInfo))
		{
			return false;
		}

		// Copy bitmap to alpha SBM
		memcpy(AlphaImg.GetImg(), bmInfo.dsBm.bmBits, (Hdr.ImageWidth * Hdr.ImageHeight) * (Hdr.BPP / 8));

		// Free the bitmap
		delete hBMP;

		// Grayscale the alphamap
		AlphaImg.Grayscale();

		// Insert alpha channel into font map
		FntImg.InsertAlpha(AlphaImg.GetImg());
	}
#endif

	FlipImg();

	switch (fmt)
	{
	case EXPORT_BMP:
		Result = stbi_write_bmp(fname, Width, Height, (BPP / 8), ImgData) != 0 ? SBM_OK : SBM_ERR_UNSUPPORTED;
		break;

	case EXPORT_TGA:
	case EXPORT_TGA32:
		Result = stbi_write_tga(fname, Width, Height, (BPP / 8), ImgData) != 0 ? SBM_OK : SBM_ERR_UNSUPPORTED;
		break;

	case EXPORT_PNG:
		Result = stbi_write_png(fname, Width, Height, (BPP / 8), ImgData, Width * (BPP / 8)) != 0 ? SBM_OK : SBM_ERR_UNSUPPORTED;
		break;

	default:
		Result = false;
		break;
	}

	Deinit_SBM_Image();

	return Result;
}

bool Font::ImportData(char *fname)
{
	UNREFERENCED_PARAMETER(fname);

#if 0
	extern Font *Fnt;

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
		cfg->FntDef.lfFaceName[Index] = data[datptr];
		++Index;
		++datptr;
	}
	cfg->FntDef.lfFaceName[Index] = NULL;

	// Font Height
	while (data[datptr] != ',')
	{
		++datptr;
	}

	datptr++;
	sscanf(&data[datptr], "%d", &(cfg->FntDef.lfHeight));

	// Font Width
	while (data[datptr] != ',')
	{
		++datptr;
	}

	datptr++;
	sscanf(&data[datptr], "%d", &(cfg->FntDef.lfWidth));

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
	cfg->FntDef.lfWeight = Val;

	// Italic Value
	while (data[datptr] != ',')
	{
		++datptr;
	}

	datptr++;
	sscanf(&data[datptr], "%d", &Val);
	cfg->FntDef.lfItalic = Val;

	// AntiAlias Value
	while (data[datptr] != ',')
	{
		++datptr;
	}

	datptr++;
	sscanf(&data[datptr], "%d", &Val);
	cfg->FntDef.lfQuality = Val;

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
	out << "Font Name," << FntDef.lfFaceName << "\n";
	out << "Font Height," << FntDef.lfHeight << "\n";
	out << "Font Width (0 is default)," << FntDef.lfWidth << "\n";

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
	out << "Bold," << FntDef.lfWeight << "\n";
	out << "Italic," << (int)FntDef.lfItalic << "\n";
	out << "AntiAlias," << (int)FntDef.lfQuality << "\n";

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

void Font::Init_SBM_Image()
{
	Width = Height = 0;
	FileSize = ImageSize = 0;
	BPP = Encode = 0;
	ImgData = PalData = FileData = NULL;
}

void Font::Deinit_SBM_Image()
{
	FreeMem((void **)&ImgData);
	FreeMem((void **)&PalData);
	FreeMem((void **)&FileData);
}

// Create an empty image with specified size and colour depth
int Font::Create(int width, int height, int bpp)
{
	switch (bpp)
	{
	case 24:
	case 32:
		FreeMem((void **)&ImgData);
		FreeMem((void **)&PalData);
		FreeMem((void **)&FileData);

		ImgData = new unsigned char[(width * height) * (bpp / 8)];

		if (ImgData == NULL)
		{
			return SBM_ERR_MEM_FAIL;
		}

		Width = width;
		Height = height;
		BPP = bpp;

		break;

	default:
		return SBM_ERR_UNSUPPORTED;
	}

	return SBM_OK;
}

void Font::Reset()
{
	Width = Height = 0;
	FileSize = ImageSize = 0;
	BPP = Encode = 0;

	FreeMem((void **)&ImgData);
	FreeMem((void **)&PalData);
	FreeMem((void **)&FileData);
}

bool MyStrCmp(const char *Str1, const char *Str2) // Case insenstive string comparison
{
	while ((*Str1 > 96 ? *Str1 - 32 : *Str1) == (*Str2 > 96 ? *Str2 - 32 : *Str2))
	{
		if (*Str1 == NULL)
		{
			return 1;
		}
		Str1++;
		Str2++;
	}
	return 0;
}

void Font::FreeMem(void **Ptr)
{
	if (*Ptr != NULL)
	{
		delete [] *Ptr;
		*Ptr = NULL;
	}
}

void Font::BGRtoRGB()
{
	unsigned long Index, nPixels;
	unsigned char *bCur;
	unsigned char bTemp;
	int iPixelSize;

	// Set ptr to start of image
	bCur = ImgData;

	// Calc number of pixels
	nPixels = Width * Height;

	// Get pixel size in bytes
	iPixelSize = BPP / 8;

	for (Index = 0; Index != nPixels; Index++) // For each pixel
	{
		bTemp = *bCur;    // Get Blue value
		*bCur = *(bCur + 2); // Swap red value into first position
		*(bCur + 2) = bTemp; // Write back blue to last position

		bCur += iPixelSize; // Jump to next pixel
	}
}

int Font::InsertAlpha(unsigned char *Alpha)
{
	unsigned char *NewImg;
	int PixLoop;
	int RGBPtr, ImgPtr;

	if (BPP != 24)
	{
		return SBM_ERR_UNSUPPORTED;
	}

	NewImg = new unsigned char[(Width * Height) * 4];

	if (NewImg == NULL)
	{
		return SBM_ERR_MEM_FAIL;
	}

	for (PixLoop = 0, RGBPtr = 0, ImgPtr = 0; PixLoop != (Width * Height); ++PixLoop)
	{
		NewImg[ImgPtr] = ImgData[RGBPtr];
		NewImg[ImgPtr + 1] = ImgData[RGBPtr + 1];
		NewImg[ImgPtr + 2] = ImgData[RGBPtr + 2];
		NewImg[ImgPtr + 3] = Alpha[PixLoop];
		RGBPtr += 3;
		ImgPtr += 4;
	}

	FreeMem((void **)&ImgData);
	ImgData = NewImg;
	BPP = 32;
	ImageSize = ((Width * Height) * (BPP / 8));

	return SBM_OK;
}

void Font::FlipImg()
{
	unsigned char bTemp;
	unsigned char *pLine1, *pLine2;
	int iLineLen, iIndex;

	iLineLen = Width * (BPP / 8);
	pLine1 = ImgData;
	pLine2 = &ImgData[iLineLen * (Height - 1)];

	for (; pLine1 < pLine2; pLine2 -= (iLineLen * 2))
	{
		for (iIndex = 0; iIndex != iLineLen; pLine1++, pLine2++, iIndex++)
		{
			bTemp = *pLine1;
			*pLine1 = *pLine2;
			*pLine2 = bTemp;
		}
	}
}

int Font::Grayscale()
{
	unsigned char *NewData;
	unsigned long ImgPtr, AlphaPtr;
	float AlphaVal;

	NewData = new unsigned char[Width * Height];

	if (NewData == NULL)
	{
		return SBM_ERR_MEM_FAIL;
	}

	switch (BPP)
	{
	case 24:
		ImageSize = Width * Height;
		for (ImgPtr = 0, AlphaPtr = 0; AlphaPtr < ImageSize; AlphaPtr++, ImgPtr += 3)
		{
			AlphaVal = ImgData[ImgPtr] * 0.3f;
			AlphaVal += ImgData[ImgPtr + 1] * 0.59f;
			AlphaVal += ImgData[ImgPtr + 2] * 0.11f;
			NewData[AlphaPtr] = (unsigned char)AlphaVal;
		}

		delete [] ImgData;
		ImgData = NewData;
		BPP = 8;
		break;

	default:
		delete [] NewData;
		return SBM_ERR_UNSUPPORTED;
		break;
	}

	return SBM_OK;
}

int Font::InvertCol()
{
	unsigned long Loop;

	switch (BPP)
	{
	case 8:
		if (PalData == NULL) // Grayscale image
		{
			for (Loop = 0; Loop < ImageSize; ++Loop)
			{
				ImgData[Loop] = 255 - ImgData[Loop];
			}
		}
		else
		{
			return SBM_ERR_UNSUPPORTED;
		}

		break;

	default:
		return SBM_ERR_UNSUPPORTED;
		break;
	}

	return SBM_OK;
}

int Font::Saturate(unsigned char KeyR, unsigned char KeyG, unsigned char KeyB, unsigned char SatR, unsigned char SatG, unsigned char SatB)
{
	long Loop, PixCount;

	PixCount = (Width * Height);

	switch (BPP)
	{
	case 24:
		PixCount *= 3;
		for (Loop = 0; Loop < PixCount; Loop += 3)
		{
			if (ImgData[Loop] != KeyR || ImgData[Loop + 1] != KeyG || ImgData[Loop + 2] != KeyB)
			{
				ImgData[Loop] = SatR;
				ImgData[Loop + 1] = SatG;
				ImgData[Loop + 2] = SatB;
			}
		}
		break;

	default:
		return SBM_ERR_UNSUPPORTED;
		break;
	}

	return SBM_OK;
}

int Font::GetBPP()
{
	return BPP;
}

int Font::GetWidth()
{
	return Width;
}

int Font::GetHeight()
{
	return Height;
}

unsigned char *Font::GetImg()
{
	return ImgData;
}

unsigned char *Font::GetPalette()
{
	return PalData;
}
