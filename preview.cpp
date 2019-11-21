#include "pch.h"
#include "font.h"
#include "config.h"
#include "bfg.h"
#include "resource.h"

HDC glDC;
HWND hGL;

BOOL CALLBACK ResizeWndProc(HWND hCtrl, LPARAM lParam)
{
	LPRECT rcWnd = (LPRECT)lParam;

	int i = GetWindowLongW(hCtrl, GWL_ID);
	switch (i)
	{
	case IDC_GL:
		MoveWindow(hCtrl, rcWnd->left, rcWnd->top, rcWnd->right, rcWnd->bottom - 96, TRUE);
		break;

	case IDC_STATIC:
		SetWindowPos(hCtrl, NULL, rcWnd->top + 8, rcWnd->bottom - 88, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
		break;

	case TXT_PREVIEW:
		MoveWindow(hCtrl, rcWnd->top + 8, rcWnd->bottom - 72, rcWnd->right - 96, 64, TRUE);
		break;

	default:
		break;
	}

	return TRUE;
}

// this function brought from here:
// https://docs.microsoft.com/en-us/windows/desktop/gdi/alpha-blending-a-bitmap
void DrawAlphaBlend(HWND hWnd, HDC hdcwnd)
{
	HDC hdc;               // handle of the DC we will create
	BLENDFUNCTION bf;      // structure for alpha blending
	HBITMAP hbitmap;       // bitmap handle
	BITMAPINFO bmi;        // bitmap header
	VOID* pvBits;          // pointer to DIB section
	ULONG   ulWindowWidth, ulWindowHeight;      // window width/height
	ULONG   ulBitmapWidth, ulBitmapHeight;      // bitmap width/height
	RECT    rt;            // used for getting window dimensions
	UINT32   x, y;         // stepping variables
	UCHAR ubAlpha;         // used for doing transparent gradient
	UCHAR ubRed;
	UCHAR ubGreen;
	UCHAR ubBlue;
	float fAlphaFactor;    // used to do premultiply

	// get window dimensions
	GetClientRect(hWnd, &rt);

	// calculate window width/height
	ulWindowWidth = rt.right - rt.left;
	ulWindowHeight = rt.bottom - rt.top;

	// make sure we have at least some window size
	if ((!ulWindowWidth) || (!ulWindowHeight))
	{
		return;
	}

	// divide the window into 3 horizontal areas
	ulWindowHeight /= 3;

	// create a DC for our bitmap -- the source DC for AlphaBlend
	hdc = CreateCompatibleDC(hdcwnd);

	// zero the memory for the bitmap info
	ZeroMemory(&bmi, sizeof(BITMAPINFO));

	// setup bitmap info
	// set the bitmap width and height to 60% of the width and height of each of the three horizontal areas. Later on, the blending will occur in the center of each of the three areas.
	bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bmi.bmiHeader.biWidth = ulBitmapWidth = ulWindowWidth - (ulWindowWidth / 5) * 2;
	bmi.bmiHeader.biHeight = ulBitmapHeight = ulWindowHeight - (ulWindowHeight / 5) * 2;
	bmi.bmiHeader.biPlanes = 1;
	bmi.bmiHeader.biBitCount = 32;
	bmi.bmiHeader.biCompression = BI_RGB;
	bmi.bmiHeader.biSizeImage = ulBitmapWidth * ulBitmapHeight * 4;

	// create our DIB section and select the bitmap into the dc
	hbitmap = CreateDIBSection(hdc, &bmi, DIB_RGB_COLORS, &pvBits, NULL, 0x0);
	SelectObject(hdc, hbitmap);

	// in top window area, constant alpha = 50%, but no source alpha
	// the color format for each pixel is 0xaarrggbb
	// set all pixels to blue and set source alpha to zero
	for (y = 0; y < ulBitmapHeight; y++)
	{
		for (x = 0; x < ulBitmapWidth; x++)
		{
			((UINT32*)pvBits)[x + y * ulBitmapWidth] = 0x000000ff;
		}
	}

	bf.BlendOp = AC_SRC_OVER;
	bf.BlendFlags = 0;
	bf.SourceConstantAlpha = 0x7f;  // half of 0xff = 50% transparency
	bf.AlphaFormat = 0;             // ignore source alpha channel
	if (!AlphaBlend(hdcwnd, ulWindowWidth / 5, ulWindowHeight / 5, ulBitmapWidth, ulBitmapHeight, hdc, 0, 0, ulBitmapWidth, ulBitmapHeight, bf))
	{
		return;    // alpha blend failed
	}

	// in middle window area, constant alpha = 100% (disabled), source
	// alpha is 0 in middle of bitmap and opaque in rest of bitmap
	for (y = 0; y < ulBitmapHeight; y++)
	{
		for (x = 0; x < ulBitmapWidth; x++)
		{
			if ((x > (int)(ulBitmapWidth / 5)) && (x < (ulBitmapWidth - ulBitmapWidth / 5)) && (y > (int)(ulBitmapHeight / 5)) && (y < (ulBitmapHeight - ulBitmapHeight / 5)))
				//in middle of bitmap: source alpha = 0 (transparent).
				// This means multiply each color component by 0x00.
				// Thus, after AlphaBlend, we have a, 0x00 * r,
				// 0x00 * g,and 0x00 * b (which is 0x00000000)
				// for now, set all pixels to red
			{
				((UINT32*)pvBits)[x + y * ulBitmapWidth] = 0x00ff0000;
			}
			else
				// in the rest of bitmap, source alpha = 0xff (opaque)
				// and set all pixels to blue
			{
				((UINT32*)pvBits)[x + y * ulBitmapWidth] = 0xff0000ff;
			}
		}
	}

	bf.BlendOp = AC_SRC_OVER;
	bf.BlendFlags = 0;
	bf.AlphaFormat = AC_SRC_ALPHA;  // use source alpha
	bf.SourceConstantAlpha = 0xff;  // opaque (disable constant alpha)
	if (!AlphaBlend(hdcwnd, ulWindowWidth / 5, ulWindowHeight / 5 + ulWindowHeight, ulBitmapWidth, ulBitmapHeight, hdc, 0, 0, ulBitmapWidth, ulBitmapHeight, bf))
	{
		return;
	}

	// bottom window area, use constant alpha = 75% and a changing
	// source alpha. Create a gradient effect using source alpha, and
	// then fade it even more with constant alpha
	ubRed = 0x00;
	ubGreen = 0x00;
	ubBlue = 0xff;

	for (y = 0; y < ulBitmapHeight; y++)
	{
		for (x = 0; x < ulBitmapWidth; x++)
		{
			// for a simple gradient, base the alpha value on the x
			// value of the pixel
			ubAlpha = (UCHAR)((float)x / (float)ulBitmapWidth * 255);
			//calculate the factor by which we multiply each component
			fAlphaFactor = (float)ubAlpha / (float)0xff;
			// multiply each pixel by fAlphaFactor, so each component
			// is less than or equal to the alpha value.
			((UINT32*)pvBits)[x + y * ulBitmapWidth]
				= (ubAlpha << 24) |                       //0xaa000000
				((UCHAR)(ubRed * fAlphaFactor) << 16) |  //0x00rr0000
				((UCHAR)(ubGreen * fAlphaFactor) << 8) | //0x0000gg00
				((UCHAR)(ubBlue * fAlphaFactor));      //0x000000bb
		}
	}

	bf.BlendOp = AC_SRC_OVER;
	bf.BlendFlags = 0;
	bf.AlphaFormat = AC_SRC_ALPHA;   // use source alpha
	bf.SourceConstantAlpha = 0xbf;   // use constant alpha, with 75% opaqueness
	AlphaBlend(hdcwnd, ulWindowWidth / 5, ulWindowHeight / 5 + 2 * ulWindowHeight, ulBitmapWidth, ulBitmapHeight, hdc, 0, 0, ulBitmapWidth, ulBitmapHeight, bf);

	// do cleanup
	DeleteObject(hbitmap);
	DeleteDC(hdc);
}

BOOL CALLBACK PreviewWinProc(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam)
{
	RECT rcDlg;

	int nLines, Loop, chLoop, offset, i, x, y;
	int CurX, CurY;
	float RowFactor, ColFactor, U, V;
	int SrcCol, SrcRow, RowPitch;
	unsigned char Text[255];
	HBITMAP hBMP;
	Font FntImg;
	RECT glRect;
	LRESULT lTxt;
	BITMAP          bitmap;
	HDC             hdcMem;
	HGDIOBJ         oldBitmap;
	//PAINTSTRUCT ps;
	//HDC hdc;
	HBRUSH hbrWhite, hbrGray;
	RECT rc, drc;
	BOOL igr, gr;
	LPDRAWITEMSTRUCT lpdis;
	HGDIOBJ original;
	BLENDFUNCTION bf;      // structure for alpha blending

	static char PText[1024];
	char Sample[13][128] =
	{
		{"THE QUICK BROWN FOX JUMPS OVER A LAZY DOG"},
		{"JACKDAWS LOVE MY BIG SPHINX OF QUARTZ"},
		{"QUICK WAFTING ZEPHYRS VEX BOLD JIM"},
		{"PACK MY BOX WITH FIVE DOZEN LIQUOR JUGS"},
		{"SIX BIG DEVILS FROM JAPAN QUICKLY FORGOT HOW TO WALTZ"},
		{"CRAZY FREDERICKA BOUGHT MANY VERY EXQUISITE OPAL JEWELS"},
		{"FEW QUIPS GALVANIZED THE MOCK JURY BOX"},
		{"THE FIVE BOXING WIZARDS JUMP QUICKLY"},
		{"A SHORT BRIMLESS FELT HAT BARELY BLOCKS OUT THE SOUND OF A CELTIC VIOLIN"},
		{"BEING BOUNCED AROUND QUICKLY ANNOYED THE DISHEVELED TAXI DRIVERS"},
		{"MY GIRL WOVE SIX DOZEN PLAID JACKETS BEFORE SHE QUIT"},
		{"SIXTY ZIPPERS WERE QUICKLY PICKED FROM THE WOVEN JUTE BAG"},
		{"MY FAXED JOKE WON A PAGER IN THE CABLE TV QUIZ SHOW"}
	};

	switch (msg)
	{
	case WM_INITDIALOG:
		if (!lstrlen(PText))
		{
			offset = rand() % 13;
			lstrcpy(PText, &Sample[offset][0]);
		}
		SendDlgItemMessage(hDlg, TXT_PREVIEW, EM_LIMITTEXT, 254, 0);
		SendDlgItemMessage(hDlg, TXT_PREVIEW, WM_SETTEXT, 0, (LPARAM)PText);

		return TRUE;

	case WM_SIZE:
		GetClientRect(hDlg, &rcDlg);
		EnumChildWindows(hDlg, ResizeWndProc, (LPARAM)&rcDlg);
		return TRUE;

	case WM_GETMINMAXINFO:
		((LPMINMAXINFO)lParam)->ptMinTrackSize.x = 320;
		((LPMINMAXINFO)lParam)->ptMinTrackSize.y = 240;
		return TRUE;

	case WM_APP:
		CurX = CurY = 0;

		GetClientRect(hGL, &glRect);
		//glViewport(0, 0, glRect.right, glRect.bottom);
		//BkCol = g_font.GetCol(BACKCOL);
		//glClearColor(((float)BkCol.Red / 255.0f), ((float)BkCol.Green / 255.0f), ((float)BkCol.Blue / 255.0f), 0.0f);

		/*glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		glOrtho(0.0f, glRect.right, glRect.bottom, 0.0f, -10.0f, 10.0f);
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);*/

		nLines = SendDlgItemMessage(hDlg, TXT_PREVIEW, EM_GETLINECOUNT, 0, 0);

		RowPitch = g_font.GetSize(MAPWIDTH) / g_font.GetSize(CELLWIDTH);
		RowFactor = (float)g_font.GetSize(CELLHEIGHT) / (float)g_font.GetSize(MAPHEIGHT);
		ColFactor = (float)g_font.GetSize(CELLWIDTH) / (float)g_font.GetSize(MAPWIDTH);

		//glBegin(GL_QUADS);
		for (Loop = 0; Loop != nLines; ++Loop)
		{
			Text[0] = 0xFF;
			Text[1] = 0;
			lTxt = SendDlgItemMessage(hDlg, TXT_PREVIEW, EM_GETLINE, Loop, (LPARAM)Text);
			Text[lTxt] = NULL;

			for (chLoop = 0; chLoop != lTxt; ++chLoop)
			{
				SrcRow = (Text[chLoop] - g_font.GetBaseChar()) / RowPitch;
				SrcCol = (Text[chLoop] - g_font.GetBaseChar()) - (SrcRow * RowPitch);
				U = ColFactor * SrcCol;
				V = RowFactor * SrcRow;

				/*glTexCoord2f(U, V);
				glVertex2i(CurX, CurY);
				glTexCoord2f(U + ColFactor, V);
				glVertex2i(CurX + g_font.GetSize(CELLWIDTH), CurY);
				glTexCoord2f(U + ColFactor, V + RowFactor);
				glVertex2i(CurX + g_font.GetSize(CELLWIDTH), CurY + g_font.GetSize(CELLHEIGHT));
				glTexCoord2f(U, V + RowFactor);
				glVertex2i(CurX, CurY + g_font.GetSize(CELLHEIGHT));*/
				CurX += g_font.GetCharVal(Text[chLoop], EWIDTH);
			}
			CurX = 0;
			CurY += g_font.GetSize(CELLHEIGHT);
		}

		SwapBuffers(glDC);
		return TRUE;

	case WM_CLOSE:
		SendDlgItemMessage(hDlg, TXT_PREVIEW, WM_GETTEXT, 1024, (LPARAM)PText);
		EndDialog(hDlg, 0);
		return TRUE;

	case WM_DRAWITEM:
		lpdis = (LPDRAWITEMSTRUCT)lParam;

		FillRect(lpdis->hDC, &lpdis->rcItem, g_hBackground);

		hdcMem = CreateCompatibleDC(lpdis->hDC);

		hBMP = g_font.DrawBitmap(hdcMem, 0);
		original = SelectObject(hdcMem, hBMP);

#if 0
		bf.BlendOp = AC_SRC_OVER;
		bf.BlendFlags = 0;
		bf.AlphaFormat = AC_SRC_ALPHA;  // use source alpha
		bf.SourceConstantAlpha = 0xff;  // opaque (disable constant alpha)
		AlphaBlend(lpdis->hDC, lpdis->rcItem.left, lpdis->rcItem.top, lpdis->rcItem.right, lpdis->rcItem.bottom, hdcMem, 0, 0, g_font.GetSize(MAPWIDTH), g_font.GetSize(MAPHEIGHT), bf);
#endif

		BitBlt(lpdis->hDC, lpdis->rcItem.left, lpdis->rcItem.top, lpdis->rcItem.right, lpdis->rcItem.bottom, hdcMem, 0, 0, SRCCOPY);

		SelectObject(hdcMem, original);

		DeleteObject(hBMP);
		DeleteDC(hdcMem);

		//DrawAlphaBlend(hDlg, lpdis->hDC);

		return TRUE;

	case WM_COMMAND:
	{
		switch (HIWORD(wParam))
		{
		case EN_CHANGE:
			RedrawWindow(hDlg, NULL, NULL, RDW_INVALIDATE);
			return TRUE;
		}

		switch (LOWORD(wParam))
		{
		case CMD_OK:
			SendDlgItemMessage(hDlg, TXT_PREVIEW, WM_GETTEXT, 1024, (LPARAM)PText);
			EndDialog(hDlg, 0);
			return TRUE;

		case CMD_TEST_CLEAR:
			SendDlgItemMessage(hDlg, TXT_PREVIEW, WM_SETTEXT, 0, (LPARAM)"");
			RedrawWindow(hDlg, NULL, NULL, RDW_INVALIDATE);
			//SendMessage(hDlg, WM_APP, 0, 0);
			return TRUE;

		case CMD_TEST_PANGRAM:
			offset = rand() % 13;
			SendDlgItemMessage(hDlg, TXT_PREVIEW, WM_SETTEXT, 0, (LPARAM)&Sample[offset][0]);
			//SendMessage(hDlg, WM_APP, 0, 0);
			return TRUE;
		}

	}

	default:
		return FALSE;
	}
}
