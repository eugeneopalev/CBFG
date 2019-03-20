#include "pch.h"
#include "font.h"
#include "utils.h"
#include "resource.h"

extern HWND hMain;
extern BFontMap *Fnt;
extern AppInfo *info;

BFG_RGB MakeRGB(unsigned char Red, unsigned char Green, unsigned char Blue)
{
	BFG_RGB Pack;

	Pack.Red = Red;
	Pack.Green = Green;
	Pack.Blue = Blue;

	return Pack;
}

char Limit(int Val)
{
	if (Val < -128)
	{
		Val = -128;
	}
	if (Val > 127)
	{
		Val = 127;
	}

	return (char) Val;
}

int LimitSelection(int Sel, int MaxChar)
{
	if (Sel > MaxChar)
	{
		Sel = MaxChar - 1;
	}

	return Sel;
}

void CreateFontMap()
{
	HDC Wdc, Mdc, Fdc;
	HWND hImgWin;
	HBITMAP *FntMap;
	HBITMAP mMap;
	BITMAPINFO BMDat;
	RECT ClipArea;
	HRGN ClipRgn;
	int Opt = 0, Selection;

	const int MapWidth = Fnt->GetSize(MAPWIDTH);
	const int MapHeight = Fnt->GetSize(MAPHEIGHT);

	// Get the target window
	hImgWin = GetDlgItem(hMain, IMG_TEXT);
	if (hImgWin == NULL)
	{
		return;
	}

	// Get target's DC
	Wdc = GetDC(hImgWin);
	if (Wdc == NULL)
	{
		return;
	}

	// Create memory DC
	Mdc = CreateCompatibleDC(Wdc);
	if (Mdc == NULL)
	{
		return;
	}

	// Create DC for font
	Fdc = CreateCompatibleDC(Wdc);
	if (Fdc == NULL)
	{
		return;
	}

	// Get size of target window
	GetClientRect(hImgWin, &ClipArea);

	// Specify bitmap type and size
	BMDat.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	BMDat.bmiHeader.biWidth = ClipArea.right;
	BMDat.bmiHeader.biHeight = ClipArea.right;
	BMDat.bmiHeader.biPlanes = 1;
	BMDat.bmiHeader.biBitCount = 24;
	BMDat.bmiHeader.biCompression = BI_RGB;
	BMDat.bmiHeader.biSizeImage = (ClipArea.right * ClipArea.right) * 3;

	// Create the bitmap
	mMap = CreateDIBSection(Mdc, &BMDat, DIB_RGB_COLORS, NULL, NULL, 0);
	if (!mMap)
	{
		return;
	}

	// Select into memory DC
	if (!SelectObject(Mdc, mMap))
	{
		return;
	}

	// Render the font
	if (info->Grid)
	{
		Opt |= DFM_GRIDLINES;
	}

	if (info->wMarker)
	{
		Opt |= DFM_WIDTHLINE;
	}

	if (info->ModAll)
	{
		Selection = -1;
	}
	else
	{
		Selection = info->Select;
	}

	FntMap = Fnt->DrawFontMap(Opt, Selection);

	// Select Font Map into Font DC
	SelectObject(Fdc, *FntMap);

	// Gray out bitmap
	FillRect(Mdc, &ClipArea, (HBRUSH)GetStockObject(GRAY_BRUSH));

	// Set Clipping Region
	ClipRgn = CreateRectRgn(ClipArea.left, ClipArea.top, ClipArea.right, ClipArea.bottom);
	SelectClipRgn(Mdc, ClipRgn);

	// Copy Font into buffer
	SetStretchBltMode(Mdc, WHITEONBLACK);
	StretchBlt(Mdc, 0, 0, (int)(MapWidth * info->Zoom), (int)(MapHeight * info->Zoom),
	           Fdc, (int)(info->hScr / info->Zoom), (int)(info->vScr / info->Zoom),
	           MapWidth, MapHeight, SRCCOPY);

	// Copy Font into window
	SetStretchBltMode(Wdc, WHITEONBLACK);
	BitBlt(Wdc, 0, 0, (int)ClipArea.right, (int)ClipArea.bottom,
	       Mdc, 0, 0, SRCCOPY);

	// Clean up
	DeleteObject(*FntMap);
	DeleteObject(mMap);
	DeleteObject(ClipRgn);
	DeleteObject(mMap);
	ReleaseDC(hImgWin, Wdc);
	DeleteDC(Mdc);
	DeleteDC(Fdc);
}

void CalcScroll()
{
	extern BFontMap *Fnt;
	extern AppConfig *AppCfg;
	extern AppInfo *info;

	RECT WinSize;
	int XDelta, YDelta;
	SCROLLINFO sInf;
	int TexWidth, TexHt;
	int CharRow, CharCol, RowPitch;

	TexWidth = Fnt->GetSize(MAPWIDTH);
	TexHt = Fnt->GetSize(MAPHEIGHT);
	RowPitch = TexWidth / Fnt->GetSize(CELLWIDTH);

	GetClientRect(GetDlgItem(hMain, IMG_TEXT), &WinSize);

	if (!info->ModAll) // Check for active selection
	{
		// Calculate position of char
		CharCol = info->Select;
		CharRow = CharCol / RowPitch;
		CharCol -= CharRow * RowPitch;

		// Convert to pixels (Zoom?)
		CharCol *= Fnt->GetSize(CELLWIDTH);
		CharRow *= Fnt->GetSize(CELLHEIGHT);
	}

	// Calculate something?
	XDelta = (int)(TexWidth * info->Zoom) - WinSize.right;
	YDelta = (int)(TexHt * info->Zoom) - WinSize.bottom;

	if (XDelta > 0)
	{
		SetScrollRange(GetDlgItem(hMain, SCR_HOR), SB_CTL, 0, XDelta, FALSE);

		sInf.cbSize = sizeof(SCROLLINFO);
		sInf.fMask = SIF_RANGE;
		GetScrollInfo(GetDlgItem(hMain, SCR_HOR), SB_CTL, &sInf);
		if (info->hScr > sInf.nMax)
		{
			info->hScr = sInf.nMax;
		}

		SetScrollPos(GetDlgItem(hMain, SCR_HOR), SB_CTL, info->hScr, TRUE);
		info->hScroll = TRUE;
	}
	else // Prevent offset pushing texture off left edge of window
	{
		SetScrollPos(GetDlgItem(hMain, SCR_HOR), SB_CTL, 0, TRUE);
		info->hScr = 0;
		EnableWindow(GetDlgItem(hMain, SCR_HOR), FALSE);
		info->hScroll = FALSE;
	}

	if (YDelta > 0)
	{
		SetScrollRange(GetDlgItem(hMain, SCR_VERT), SB_CTL, 0, YDelta, FALSE);

		sInf.cbSize = sizeof(SCROLLINFO);
		sInf.fMask = SIF_RANGE;
		GetScrollInfo(GetDlgItem(hMain, SCR_VERT), SB_CTL, &sInf);
		if (info->vScr > sInf.nMax)
		{
			info->vScr = sInf.nMax;
		}

		SetScrollPos(GetDlgItem(hMain, SCR_VERT), SB_CTL, info->vScr, TRUE);
		info->vScroll = TRUE;
	}
	else // Prevent offset pushing texture off top edge of window
	{
		SetScrollPos(GetDlgItem(hMain, SCR_VERT), SB_CTL, 0, TRUE);
		info->vScr = 0;
		EnableWindow(GetDlgItem(hMain, SCR_VERT), FALSE);
		info->vScroll = FALSE;
	}
}

BOOL CheckOverwrite(char *fname)
{
	std::ifstream testfile;

	testfile.open(fname);

	if (!testfile.fail())
	{
		testfile.close();
		if (MessageBox(hMain, "Overwrite existing file?", "Filename already exists", MB_YESNO | MB_ICONQUESTION) == IDNO)
		{
			return FALSE;
		}
	}

	return TRUE;
}

SBM_Image::SBM_Image()
{
	Width = Height = 0;
	FileSize = ImageSize = 0;
	BPP = Encode = 0;
	ImgData = PalData = FileData = NULL;
}

SBM_Image::~SBM_Image()
{
	FreeMem((void **)&ImgData);
	FreeMem((void **)&PalData);
	FreeMem((void **)&FileData);
}

// Create an empty image with specified size and colour depth
int SBM_Image::Create(int width, int height, int bpp)
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

void SBM_Image::Reset()
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

int SBM_Image::SaveBMP(char *fname)
{
	std::ofstream out;
	int Res = SBM_ERR_UNSUPPORTED;
	DWORD Data;
	WORD wData;

	switch (BPP)
	{
	case 24:
		out.open(fname, std::ios::binary | std::ios::trunc);
		if (out.fail())
		{
			Res = SBM_ERR_NO_FILE;
			break;
		}

		// Write ID
		out.write("BM", 2);

		// Write filesize
		Data =/*0x436*/54 + ((Width * Height) * (BPP / 8));
		out.write((char *)&Data, 4);

		// Write reserved area
		Data = 0;
		out.write((char *)&Data, 4);

		// Write offset
		Data = 0x36;
		out.write((char *)&Data, 4);

		// Write header size
		Data = 0x28;
		out.write((char *)&Data, 4);

		// Write width
		Data = Width;
		out.write((char *)&Data, 4);

		// Write height
		Data = Height;
		out.write((char *)&Data, 4);

		// Write planes
		wData = 1;
		out.write((char *)&wData, 2);

		// Write BPP
		wData = (WORD)BPP;
		out.write((char *)&wData, 2);

		// Write Compression
		Data = 0;
		out.write((char *)&Data, 4);

		// Write data size
		Data = (Width * Height) * (BPP / 8);
		out.write((char *)&Data, 4);

		// Write Resolutions and Colors
		Data = 0;
		out.write((char *)&Data, 4);
		out.write((char *)&Data, 4);
		out.write((char *)&Data, 4);
		out.write((char *)&Data, 4);

		// Write image data
		out.write((char *)ImgData, (Width * Height) * (BPP / 8));

		out.close();

		Res = SBM_OK;
		break;

	default:
		Res = SBM_ERR_UNSUPPORTED;
		break;
	}

	return Res;
}

int SBM_Image::SavePCX(char *fname)
{
	UNREFERENCED_PARAMETER(fname);

	return SBM_ERR_UNSUPPORTED;
}

int SBM_Image::SaveTGA(char *filename)
{
	std::ofstream out;

	switch (BPP)
	{
	case 24:
	case 32:
		// Open output file
		out.open(filename, std::ios::binary | std::ios::trunc);

		if (out.fail())
		{
			return SBM_ERR_NO_FILE;
		}

		// Write ID,PalType and ImgType
		out.put(0);
		out.put(0);
		out.put(2);

		// Write zeros into PalInfo area
		out.put(0);
		out.put(0);
		out.put(0);
		out.put(0);

		// Write BPP into PalInfo
		out.write((const char *)&BPP, 1);

		// Write zeros into Img Start co-ords
		out.put(0);
		out.put(0);
		out.put(0);
		out.put(0);

		// Write Width, Height and BPP
		out.write((char *)&Width, 2);
		out.write((char *)&Height, 2);
		out.write((char *)&BPP, 1);

		// Write descriptor
		out.put(0);

		// Write Image data
		out.write((char *)ImgData, ((Width * Height) * (BPP / 8)));

		// Write Footer
		out.write("\0\0\0\0\0\0\0\0TRUEVISION-XFILE.\0", 26);

		// Close file
		out.close();
		break;

	default:
		return SBM_ERR_UNSUPPORTED;
	}

	return SBM_OK;
}

int SBM_Image::SaveRaw(char *filename)
{
	std::ofstream out;

	// Open output file
	out.open(filename, std::ios::binary | std::ios::trunc);

	if (out.fail())
	{
		return SBM_ERR_NO_FILE;
	}

	// Write Image data
	out.write((char *)ImgData, ((Width * Height) * (BPP / 8)));

	// Close file
	out.close();

	return SBM_OK;
}

void SBM_Image::FreeMem(void **Ptr)
{
	if (*Ptr != NULL)
	{
		delete [] *Ptr;
		*Ptr = NULL;
	}
}

void SBM_Image::BGRtoRGB()
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

int SBM_Image::InsertAlpha(unsigned char *Alpha)
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

void SBM_Image::FlipImg()
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

int SBM_Image::Grayscale()
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

int SBM_Image::InvertCol()
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

int SBM_Image::Saturate(unsigned char KeyR, unsigned char KeyG, unsigned char KeyB,
                        unsigned char SatR, unsigned char SatG, unsigned char SatB)
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

int SBM_Image::GetBPP()
{
	return BPP;
}

int SBM_Image::GetWidth()
{
	return Width;
}

int SBM_Image::GetHeight()
{
	return Height;
}

unsigned char *SBM_Image::GetImg()
{
	return ImgData;
}

unsigned char *SBM_Image::GetPalette()
{
	return PalData;
}
