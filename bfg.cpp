#include "pch.h"
#include "bfg.h"
#include "config.h"
#include "resource.h"

#pragma comment(linker,"\"/manifestdependency:type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")

HINSTANCE g_hInstance;
HWND g_hMain;
Font g_font;

HBRUSH g_hBackground;

BOOL CALLBACK ConfigWinProc(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam);
BOOL CALLBACK PreviewWinProc(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam);
BOOL CALLBACK SaveOptProc(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam);

void CreateFontMap()
{
	HDC Wdc, Mdc, Fdc;
	HWND hImgWin;
	HBITMAP FntMap;
	HBITMAP mMap;
	BITMAPINFO BMDat;
	RECT ClipArea;
	HRGN ClipRgn;
	int Opt = 0;

	const int MapWidth = g_font.GetSize(MAPWIDTH);
	const int MapHeight = g_font.GetSize(MAPHEIGHT);

	// Get the target window
	hImgWin = GetDlgItem(g_hMain, IMG_TEXT);
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
	if (g_config.Grid)
	{
		Opt |= DFM_GRIDLINES;
	}

	if (g_config.wMarker)
	{
		Opt |= DFM_WIDTHLINE;
	}

	FntMap = g_font.DrawBitmap(Fdc, Opt);

	// Select Font Map into Font DC
	SelectObject(Fdc, FntMap);

	// Gray out bitmap
	FillRect(Mdc, &ClipArea, (HBRUSH)GetStockObject(GRAY_BRUSH));

	// Set Clipping Region
	ClipRgn = CreateRectRgn(ClipArea.left, ClipArea.top, ClipArea.right, ClipArea.bottom);
	SelectClipRgn(Mdc, ClipRgn);

	// Copy Font into buffer
	SetStretchBltMode(Mdc, WHITEONBLACK);
	StretchBlt(Mdc, 0, 0, (int)(MapWidth * g_config.Zoom), (int)(MapHeight * g_config.Zoom), Fdc, (int)(g_config.hScr / g_config.Zoom), (int)(g_config.vScr / g_config.Zoom), MapWidth, MapHeight, SRCCOPY);

	// Copy Font into window
	SetStretchBltMode(Wdc, WHITEONBLACK);
	BitBlt(Wdc, 0, 0, (int)ClipArea.right, (int)ClipArea.bottom, Mdc, 0, 0, SRCCOPY);

	// Clean up
	DeleteObject(FntMap);
	DeleteObject(mMap);
	DeleteObject(ClipRgn);
	DeleteObject(mMap);
	ReleaseDC(hImgWin, Wdc);
	DeleteDC(Mdc);
	DeleteDC(Fdc);
}

void CalcScroll()
{
	RECT WinSize;
	int XDelta, YDelta;
	SCROLLINFO sInf;
	int TexWidth, TexHt;
	int RowPitch;

	TexWidth = g_font.GetSize(MAPWIDTH);
	TexHt = g_font.GetSize(MAPHEIGHT);
	RowPitch = TexWidth / g_font.GetSize(CELLWIDTH);

	GetClientRect(GetDlgItem(g_hMain, IMG_TEXT), &WinSize);

	// Calculate something?
	XDelta = (int)(TexWidth * g_config.Zoom) - WinSize.right;
	YDelta = (int)(TexHt * g_config.Zoom) - WinSize.bottom;

	if (XDelta > 0)
	{
		SetScrollRange(GetDlgItem(g_hMain, SCR_HOR), SB_CTL, 0, XDelta, FALSE);

		sInf.cbSize = sizeof(SCROLLINFO);
		sInf.fMask = SIF_RANGE;
		GetScrollInfo(GetDlgItem(g_hMain, SCR_HOR), SB_CTL, &sInf);
		if (g_config.hScr > sInf.nMax)
		{
			g_config.hScr = sInf.nMax;
		}

		SetScrollPos(GetDlgItem(g_hMain, SCR_HOR), SB_CTL, g_config.hScr, TRUE);
		g_config.hScroll = TRUE;
	}
	else // Prevent offset pushing texture off left edge of window
	{
		SetScrollPos(GetDlgItem(g_hMain, SCR_HOR), SB_CTL, 0, TRUE);
		g_config.hScr = 0;
		EnableWindow(GetDlgItem(g_hMain, SCR_HOR), FALSE);
		g_config.hScroll = FALSE;
	}

	if (YDelta > 0)
	{
		SetScrollRange(GetDlgItem(g_hMain, SCR_VERT), SB_CTL, 0, YDelta, FALSE);

		sInf.cbSize = sizeof(SCROLLINFO);
		sInf.fMask = SIF_RANGE;
		GetScrollInfo(GetDlgItem(g_hMain, SCR_VERT), SB_CTL, &sInf);
		if (g_config.vScr > sInf.nMax)
		{
			g_config.vScr = sInf.nMax;
		}

		SetScrollPos(GetDlgItem(g_hMain, SCR_VERT), SB_CTL, g_config.vScr, TRUE);
		g_config.vScroll = TRUE;
	}
	else // Prevent offset pushing texture off top edge of window
	{
		SetScrollPos(GetDlgItem(g_hMain, SCR_VERT), SB_CTL, 0, TRUE);
		g_config.vScr = 0;
		EnableWindow(GetDlgItem(g_hMain, SCR_VERT), FALSE);
		g_config.vScroll = FALSE;
	}
}

UINT_PTR CALLBACK Lpcfhookproc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	static LPCHOOSEFONT cf;
	LOGFONT lf;
	HWND hCtrl;
	LONG hMy, cboHght;
	RECT rcCtrl;
	int pic;
	LPDRAWITEMSTRUCT lpdis;
	CHOOSECOLOR SelCol;
	static COLORREF CustCol[16]; // array of custom colors for color picker

	switch (uMsg)
	{
	case WM_INITDIALOG:
		//SendDlgItemMessage(hDlg, WM_CHOOSEFONT_GETLOGFONT, CB_ADDSTRING, 0, (LPARAM)"None");
		//SendMessage(hDlg, WM_CHOOSEFONT_GETLOGFONT, 0, (LPARAM)&lf);
		cf = (LPCHOOSEFONT)lParam;

		// replace default color picker with our own one
		hCtrl = GetDlgItem(hDlg, cmb4);
		hDlg = GetParent(hCtrl);

		GetWindowRect(hCtrl, &rcCtrl);
		MapWindowPoints(NULL, hDlg, (LPPOINT)&rcCtrl, 2);
		rcCtrl.right -= rcCtrl.left;
		rcCtrl.bottom -= rcCtrl.top;

		ShowWindow(hCtrl, SW_HIDE);

		// create static image
		hCtrl = CreateWindow("STATIC", NULL, WS_VISIBLE | WS_CHILD | SS_OWNERDRAW, rcCtrl.left, rcCtrl.top, rcCtrl.bottom, rcCtrl.bottom, hDlg, (HMENU)10100, g_hInstance, NULL);

		// create button
		hCtrl = CreateWindow("BUTTON", "Change...", WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON, rcCtrl.left + rcCtrl.bottom + 8, rcCtrl.top, rcCtrl.right - (rcCtrl.bottom + 8), rcCtrl.bottom, hDlg, (HMENU)10101, g_hInstance, NULL);

		/*SetParent(hMy, hDlg);*/
		/*
		rc.right -= 20;*/
		//MapWindowPoints(hCtrl, hDlg, rc, 2);
		//cboHght = rc.bottom - rc.top;
		//MoveWindow(hCtrl, rc.left, rc.top, rc.right, rc.bottom, TRUE);
		//SetWindowPos(hCtrl, NULL, 0, 0, rc.right, rc.bottom, SWP_NOMOVE | SWP_NOZORDER);
		//ShowWindow(hCbo, SW_HIDE);
		//hGrp = GetDlgItem(hDlg, grp1);
		//ShowWindow(hGrp, SW_HIDE);
		/*GetClientRect(hGrp, rc: MapWindowPoints hGrp, hDlg, rc, 2);
		SetWindowPos(hGrp, 0, rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top + pic.ScaleHeight - cboHght, SWP_NOZORDER);*/
		//ShowWindow(hCbo, SW_HIDE);
		//hCbo = GetDlgItem(hDlg, stc6);

		//ShowWindow(hCbo, SW_HIDE);
		//GetWindowRect(hDlg, &rc);
		//SetWindowPos(hDlg, NULL, rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top + pic.ScaleHeight - cboHght, SWP_NOZORDER);
		return TRUE;

	case WM_DESTROY:
		hCtrl = GetDlgItem(hDlg, cmb4);
		//hDlg = GetParent(hCtrl);
		GetClientRect(hCtrl, &rcCtrl);
		return FALSE;

	case WM_CTLCOLORDLG:
		return TRUE;

	case WM_DRAWITEM:
		if (wParam == 10100)
		{
			lpdis = (LPDRAWITEMSTRUCT)lParam;
			SelectObject(lpdis->hDC, GetStockObject(DC_PEN));
			SelectObject(lpdis->hDC, GetStockObject(DC_BRUSH));

			SetDCPenColor(lpdis->hDC, RGB(0, 0, 0));
			SetDCBrushColor(lpdis->hDC, cf->rgbColors);
			Rectangle(lpdis->hDC, lpdis->rcItem.left, lpdis->rcItem.top, lpdis->rcItem.right, lpdis->rcItem.bottom);

			return TRUE;
		}
		break;

	case WM_COMMAND:
		int i = LOWORD(wParam);
		switch (i)
		{
		case 10101:
			ZeroMemory(&SelCol, sizeof(SelCol));
			SelCol.lStructSize = sizeof(CHOOSECOLOR);
			SelCol.hwndOwner = hDlg;
			SelCol.rgbResult = cf->rgbColors;
			SelCol.lpCustColors = (LPDWORD)CustCol;
			SelCol.Flags = CC_RGBINIT | CC_FULLOPEN | CC_ANYCOLOR;
			if (ChooseColor(&SelCol))
			{
				cf->rgbColors = SelCol.rgbResult;
			}

			InvalidateRgn(hDlg, NULL, NULL);
			return TRUE;

		default:
			break;
		}
		break;
	}

	return FALSE;
}

BOOL CALLBACK MainProc(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam)
{
	HDC dc;
	char Text[256];
	int RowDex;
	int tVal, Flags;
	SCROLLINFO scrInf;
	RECT rcArea;
	HBRUSH hBr;
	COLORREF ColVal;
	CHOOSECOLOR SelCol;
	static COLORREF CustCol[16]; // array of custom colors for color picker
	CHOOSEFONT cf;

	switch (msg)
	{
	case WM_INITDIALOG:
		SendMessage(hDlg, WM_SETICON, ICON_BIG, (LPARAM)LoadIcon(g_hInstance, MAKEINTRESOURCE(APP_ICON)));
		SendDlgItemMessage(hDlg, CBO_IMGXRES, CB_ADDSTRING, 0, (LPARAM)"16");
		SendDlgItemMessage(hDlg, CBO_IMGXRES, CB_ADDSTRING, 0, (LPARAM)"32");
		SendDlgItemMessage(hDlg, CBO_IMGXRES, CB_ADDSTRING, 0, (LPARAM)"64");
		SendDlgItemMessage(hDlg, CBO_IMGXRES, CB_ADDSTRING, 0, (LPARAM)"128");
		SendDlgItemMessage(hDlg, CBO_IMGXRES, CB_ADDSTRING, 0, (LPARAM)"256");
		SendDlgItemMessage(hDlg, CBO_IMGXRES, CB_ADDSTRING, 0, (LPARAM)"512");
		SendDlgItemMessage(hDlg, CBO_IMGXRES, CB_ADDSTRING, 0, (LPARAM)"1024");
		SendDlgItemMessage(hDlg, CBO_IMGXRES, CB_ADDSTRING, 0, (LPARAM)"2048");
		SendDlgItemMessage(hDlg, CBO_IMGXRES, CB_ADDSTRING, 0, (LPARAM)"4096");

		SendDlgItemMessage(hDlg, CBO_IMGYRES, CB_ADDSTRING, 0, (LPARAM)"16");
		SendDlgItemMessage(hDlg, CBO_IMGYRES, CB_ADDSTRING, 0, (LPARAM)"32");
		SendDlgItemMessage(hDlg, CBO_IMGYRES, CB_ADDSTRING, 0, (LPARAM)"64");
		SendDlgItemMessage(hDlg, CBO_IMGYRES, CB_ADDSTRING, 0, (LPARAM)"128");
		SendDlgItemMessage(hDlg, CBO_IMGYRES, CB_ADDSTRING, 0, (LPARAM)"256");
		SendDlgItemMessage(hDlg, CBO_IMGYRES, CB_ADDSTRING, 0, (LPARAM)"512");
		SendDlgItemMessage(hDlg, CBO_IMGYRES, CB_ADDSTRING, 0, (LPARAM)"1024");
		SendDlgItemMessage(hDlg, CBO_IMGYRES, CB_ADDSTRING, 0, (LPARAM)"2048");
		SendDlgItemMessage(hDlg, CBO_IMGYRES, CB_ADDSTRING, 0, (LPARAM)"4096");

		SendDlgItemMessage(hDlg, IDC_SLIDER1, TBM_SETRANGE, (WPARAM)TRUE, (LPARAM)MAKELONG(0, 255));
		SendDlgItemMessage(hDlg, IDC_SLIDER1, TBM_SETPOS, (WPARAM)FALSE, 0);
		//SendDlgItemMessage(hDlg, IDC_SLIDER1, TBM_SETTICFREQ, (WPARAM)8, 0);

		tVal = g_font.GetSize(MAPWIDTH);
		if (tVal == 32)
		{
			SendDlgItemMessage(hDlg, CBO_IMGXRES, CB_SETCURSEL, 1, 0);
		}
		else if (tVal == 64)
		{
			SendDlgItemMessage(hDlg, CBO_IMGXRES, CB_SETCURSEL, 2, 0);
		}
		else if (tVal == 128)
		{
			SendDlgItemMessage(hDlg, CBO_IMGXRES, CB_SETCURSEL, 3, 0);
		}
		else if (tVal == 256)
		{
			SendDlgItemMessage(hDlg, CBO_IMGXRES, CB_SETCURSEL, 4, 0);
		}
		else if (tVal == 512)
		{
			SendDlgItemMessage(hDlg, CBO_IMGXRES, CB_SETCURSEL, 5, 0);
		}
		else if (tVal == 1024)
		{
			SendDlgItemMessage(hDlg, CBO_IMGXRES, CB_SETCURSEL, 6, 0);
		}
		else if (tVal == 2048)
		{
			SendDlgItemMessage(hDlg, CBO_IMGXRES, CB_SETCURSEL, 7, 0);
		}
		else if (tVal == 4096)
		{
			SendDlgItemMessage(hDlg, CBO_IMGXRES, CB_SETCURSEL, 8, 0);
		}
		else
		{
			SendDlgItemMessage(hDlg, CBO_IMGXRES, CB_SETCURSEL, 0, 0);
		}

		tVal = g_font.GetSize(MAPHEIGHT);
		if (tVal == 32)
		{
			SendDlgItemMessage(hDlg, CBO_IMGYRES, CB_SETCURSEL, 1, 0);
		}
		if (tVal == 64)
		{
			SendDlgItemMessage(hDlg, CBO_IMGYRES, CB_SETCURSEL, 2, 0);
		}
		if (tVal == 128)
		{
			SendDlgItemMessage(hDlg, CBO_IMGYRES, CB_SETCURSEL, 3, 0);
		}
		else if (tVal == 256)
		{
			SendDlgItemMessage(hDlg, CBO_IMGYRES, CB_SETCURSEL, 4, 0);
		}
		else if (tVal == 512)
		{
			SendDlgItemMessage(hDlg, CBO_IMGYRES, CB_SETCURSEL, 5, 0);
		}
		else if (tVal == 1024)
		{
			SendDlgItemMessage(hDlg, CBO_IMGYRES, CB_SETCURSEL, 6, 0);
		}
		else if (tVal == 2048)
		{
			SendDlgItemMessage(hDlg, CBO_IMGYRES, CB_SETCURSEL, 7, 0);
		}
		else if (tVal == 4096)
		{
			SendDlgItemMessage(hDlg, CBO_IMGYRES, CB_SETCURSEL, 8, 0);
		}
		else
		{
			SendDlgItemMessage(hDlg, CBO_IMGYRES, CB_SETCURSEL, 0, 0);
		}

		wsprintf(Text, "%d", g_font.GetSize(CELLWIDTH));
		SendDlgItemMessage(hDlg, TXT_CELLWIDTH, WM_SETTEXT, 0, (LPARAM)Text);
		wsprintf(Text, "%d", g_font.GetSize(CELLHEIGHT));
		SendDlgItemMessage(hDlg, TXT_CELLHEIGHT, WM_SETTEXT, 0, (LPARAM)Text);
		wsprintf(Text, "%d", g_font.GetFontWidth());
		SendDlgItemMessage(hDlg, TXT_FONTWIDTH, WM_SETTEXT, 0, (LPARAM)Text);
		wsprintf(Text, "%d", g_font.GetFontHeight());
		SendDlgItemMessage(hDlg, TXT_FONTHEIGHT, WM_SETTEXT, 0, (LPARAM)Text);

		SendDlgItemMessage(hDlg, SPN_CELLWIDTH, UDM_SETRANGE, 0, MAKELONG(256, 8));
		SendDlgItemMessage(hDlg, SPN_CELLHEIGHT, UDM_SETRANGE, 0, MAKELONG(256, 8));
		SendDlgItemMessage(hDlg, SPN_FONTHEIGHT, UDM_SETRANGE, 0, MAKELONG(256, 1));
		SendDlgItemMessage(hDlg, SPN_FONTWIDTH, UDM_SETRANGE, 0, MAKELONG(256, 0));
		SendDlgItemMessage(hDlg, SPN_START, UDM_SETRANGE, 0, MAKELONG(254, 0));

		g_config.MaxChars = g_font.GetSize(MAXCHARS);

		PostMessage(hDlg, WM_APP, 0, 0);
		return TRUE;

	case WM_APP:
		if (g_config.Grid)
		{
			CheckMenuItem(GetMenu(g_hMain), ID_VIEW_SHOWGRID, MF_CHECKED);
		}
		else
		{
			CheckMenuItem(GetMenu(g_hMain), ID_VIEW_SHOWGRID, MF_UNCHECKED);
		}

		if (g_config.wMarker)
		{
			CheckMenuItem(GetMenu(g_hMain), ID_VIEW_WIDTHMARKERS, MF_CHECKED);
		}
		else
		{
			CheckMenuItem(GetMenu(g_hMain), ID_VIEW_WIDTHMARKERS, MF_UNCHECKED);
		}

		EnableWindow(GetDlgItem(g_hMain, SCR_HOR), FALSE);
		EnableWindow(GetDlgItem(g_hMain, SCR_VERT), FALSE);

		g_font.SetBaseChar(32);
		wsprintf(Text, "%d", g_font.GetBaseChar());
		SendDlgItemMessage(hDlg, TXT_START, WM_SETTEXT, 0, (LPARAM)Text);

		SendMessage(g_hMain, WM_APP + 1, 0, 0);
		EnableWindow(GetDlgItem(g_hMain, STA_WIDTH), FALSE);

		CalcScroll();
		CreateFontMap();
		return FALSE;

	case WM_CLOSE:
	case WM_DESTROY:
		EndDialog(hDlg, 0);
		PostQuitMessage(0);
		return TRUE;

	case WM_GETMINMAXINFO:
		((LPMINMAXINFO)lParam)->ptMinTrackSize.x = 600;
		((LPMINMAXINFO)lParam)->ptMinTrackSize.y = 400;
		return TRUE;

	case WM_HSCROLL:
	{
		LRESULT pos = SendDlgItemMessage(hDlg, IDC_SLIDER1, TBM_GETPOS, 0, 0);
		WCHAR buf[4];
		wsprintfW(buf, L"%ld", pos);
		SetDlgItemTextW(hDlg, IDC_EDIT7, buf);

		switch (LOWORD(wParam))
		{
		case SB_THUMBTRACK:
		{
			g_config.hScr = SendDlgItemMessage(hDlg, SCR_HOR, SBM_GETPOS, 0, 0);
			SetScrollPos(GetDlgItem(hDlg, SCR_HOR), SB_CTL, g_config.hScr, TRUE);
			CreateFontMap();
			return 0;
		}

		case SB_LINELEFT:
			if (g_config.hScroll == FALSE)
			{
				return 0;
			}
			g_config.hScr -= 8;
			if (g_config.hScr < 0)
			{
				g_config.hScr = 0;
			}
			SetScrollPos(GetDlgItem(g_hMain, SCR_HOR), SB_CTL, g_config.hScr, TRUE);
			CreateFontMap();
			return 0;

		case SB_LINERIGHT:
			if (g_config.hScroll == FALSE)
			{
				return 0;
			}
			g_config.hScr += 8;
			scrInf.cbSize = sizeof(SCROLLINFO);
			scrInf.fMask = SIF_RANGE;
			GetScrollInfo(GetDlgItem(g_hMain, SCR_HOR), SB_CTL, &scrInf);
			if (g_config.hScr > scrInf.nMax)
			{
				g_config.hScr = scrInf.nMax;
			}
			SetScrollPos(GetDlgItem(g_hMain, SCR_HOR), SB_CTL, g_config.hScr, TRUE);
			CreateFontMap();
			return 0;

		case SB_PAGELEFT:
			g_config.hScr -= 24;
			SetScrollPos(GetDlgItem(g_hMain, SCR_HOR), SB_CTL, g_config.hScr, TRUE);
			CreateFontMap();
			return 0;

		case SB_PAGERIGHT:
			g_config.hScr += 24;
			SetScrollPos(GetDlgItem(g_hMain, SCR_HOR), SB_CTL, g_config.hScr, TRUE);
			CreateFontMap();
			return 0;
		}

		return FALSE;
	}

	case WM_VSCROLL:
	{
		switch (LOWORD(wParam))
		{
		case SB_THUMBTRACK:
			SetScrollPos((HWND)lParam, SB_CTL, HIWORD(wParam), TRUE);
			g_config.vScr = HIWORD(wParam);
			CreateFontMap();
			return 0;

		case SB_LINEUP:
			if (g_config.vScroll == FALSE)
			{
				return 0;
			}
			g_config.vScr -= 8;
			if (g_config.vScr < 0)
			{
				g_config.vScr = 0;
			}
			SetScrollPos(GetDlgItem(g_hMain, SCR_VERT), SB_CTL, g_config.vScr, TRUE);
			CreateFontMap();
			return 0;

		case SB_LINEDOWN:
			if (g_config.vScroll == FALSE)
			{
				return 0;
			}
			g_config.vScr += 8;
			scrInf.cbSize = sizeof(SCROLLINFO);
			scrInf.fMask = SIF_RANGE;
			GetScrollInfo(GetDlgItem(g_hMain, SCR_VERT), SB_CTL, &scrInf);
			if (g_config.vScr > scrInf.nMax)
			{
				g_config.vScr = scrInf.nMax;
			}
			SetScrollPos(GetDlgItem(g_hMain, SCR_VERT), SB_CTL, g_config.vScr, TRUE);
			CreateFontMap();
			return 0;

		case SB_PAGEDOWN:
			g_config.vScr += 24;
			SetScrollPos(GetDlgItem(g_hMain, SCR_VERT), SB_CTL, g_config.vScr, TRUE);
			CreateFontMap();
			return 0;

		case SB_PAGEUP:
			g_config.vScr -= 24;
			SetScrollPos(GetDlgItem(g_hMain, SCR_VERT), SB_CTL, g_config.vScr, TRUE);
			CreateFontMap();
			return 0;
		}

		return FALSE;
	}

	case WM_NOTIFY:
	{
		NMUPDOWN* Hdr;
		Hdr = (LPNMUPDOWN)lParam;

		if (Hdr->hdr.code == UDN_DELTAPOS)
		{
			switch (Hdr->hdr.idFrom)
			{
			case SPN_CELLHEIGHT:
				g_font.SetSize(CELLHEIGHT, Hdr->iPos + Hdr->iDelta);
				g_config.MaxChars = g_font.GetSize(MAXCHARS);
				CreateFontMap();
				return 0;

			case SPN_CELLWIDTH:
				g_font.SetSize(CELLWIDTH, Hdr->iPos + Hdr->iDelta);
				g_config.MaxChars = g_font.GetSize(MAXCHARS);
				CreateFontMap();
				return 0;

			case SPN_FONTHEIGHT:
				g_font.SetFontHeight(Hdr->iPos + Hdr->iDelta);
				CreateFontMap();
				return 0;

			case SPN_FONTWIDTH:
				g_font.SetFontWidth(Hdr->iPos + Hdr->iDelta);
				CreateFontMap();
				return 0;

			case SPN_START:
				if (Hdr->iPos > 0)
				{
					g_font.SetBaseChar(Hdr->iPos + Hdr->iDelta);
				}

				SendMessage(g_hMain, WM_APP + 1, 0, 0);
				CreateFontMap();
				return 0;

			}
		}
		return 0;
	}

	case WM_COMMAND:
	{
		switch (LOWORD(wParam))
		{
		case IDC_BUTTON1:
			// https://docs.microsoft.com/en-us/windows/desktop/api/commdlg/ns-commdlg-choosefontw
			ZeroMemory(&cf, sizeof(cf));
			cf.lStructSize = sizeof(cf);
			cf.hwndOwner = g_hMain;
			//cf.hDC = (HDC)NULL;
			cf.lpLogFont = g_font.GetLogicalFont();
			//cf.iPointSize = 0;
			cf.Flags = CF_BOTH | CF_INITTOLOGFONTSTRUCT | CF_EFFECTS | CF_ENABLEHOOK;
			cf.rgbColors = g_font.GetColor();
			//cf.lCustData = 0L;
			cf.lpfnHook = (LPCFHOOKPROC)Lpcfhookproc;
			//cf.lpTemplateName = (LPSTR)NULL;
			cf.hInstance = g_hInstance;
			//cf.lpszStyle = (LPSTR)NULL;
			//cf.nFontType = SCREEN_FONTTYPE;
			//cf.nSizeMin = 0;
			//cf.nSizeMax = 0;

			if (ChooseFont(&cf))
			{
				//g_font.SetFontName(Text);
				CreateFontMap();
			}

			return TRUE;

		case ID_COLOUR_SETTEXTCOLOUR:
			SelCol.lStructSize = sizeof(CHOOSECOLOR);
			SelCol.hwndOwner = hDlg;
			SelCol.rgbResult = g_font.GetColor();
			SelCol.lpCustColors = (LPDWORD)CustCol;
			SelCol.Flags = CC_FULLOPEN | CC_RGBINIT | CC_ANYCOLOR;
			if (ChooseColor(&SelCol))
			{
				g_font.SetColor(SelCol.rgbResult);
			}

			InvalidateRgn(hDlg, NULL, NULL);
			return TRUE;

		case ID_COLOUR_SETBACKGROUNDCOLOUR:
			SelCol.lStructSize = sizeof(CHOOSECOLOR);
			SelCol.hwndOwner = hDlg;
			SelCol.rgbResult = g_font.GetCol(BACKCOL);
			SelCol.lpCustColors = (LPDWORD)CustCol;
			SelCol.Flags = CC_FULLOPEN | CC_RGBINIT | CC_ANYCOLOR;
			if (ChooseColor(&SelCol))
			{
				g_font.SetCol(BACKCOL, SelCol.rgbResult);
			}

			InvalidateRgn(hDlg, NULL, NULL);
			return TRUE;

		case ID_VIEW_SHOWGRID:
			g_config.Grid ^= 1;
			if (g_config.Grid)
			{
				CheckMenuItem(GetMenu(g_hMain), ID_VIEW_SHOWGRID, MF_CHECKED);
			}
			else
			{
				CheckMenuItem(GetMenu(g_hMain), ID_VIEW_SHOWGRID, MF_UNCHECKED);
			}
			CreateFontMap();
			return TRUE;

		case ID_VIEW_WIDTHMARKERS:
			g_config.wMarker ^= 1;
			if (g_config.wMarker)
			{
				CheckMenuItem(GetMenu(g_hMain), ID_VIEW_WIDTHMARKERS, MF_CHECKED);
			}
			else
			{
				CheckMenuItem(GetMenu(g_hMain), ID_VIEW_WIDTHMARKERS, MF_UNCHECKED);
			}
			CreateFontMap();
			return TRUE;

		case ID_FILE_RESET:
			Flags = 0;
			tVal = g_font.GetSize(MAPWIDTH);
			if (tVal == 32)
			{
				SendDlgItemMessage(hDlg, CBO_IMGXRES, CB_SETCURSEL, 1, 0);
			}
			else if (tVal == 64)
			{
				SendDlgItemMessage(hDlg, CBO_IMGXRES, CB_SETCURSEL, 2, 0);
			}
			else if (tVal == 128)
			{
				SendDlgItemMessage(hDlg, CBO_IMGXRES, CB_SETCURSEL, 3, 0);
			}
			else if (tVal == 256)
			{
				SendDlgItemMessage(hDlg, CBO_IMGXRES, CB_SETCURSEL, 4, 0);
			}
			else if (tVal == 512)
			{
				SendDlgItemMessage(hDlg, CBO_IMGXRES, CB_SETCURSEL, 5, 0);
			}
			else if (tVal == 1024)
			{
				SendDlgItemMessage(hDlg, CBO_IMGXRES, CB_SETCURSEL, 6, 0);
			}
			else if (tVal == 2048)
			{
				SendDlgItemMessage(hDlg, CBO_IMGXRES, CB_SETCURSEL, 7, 0);
			}
			else if (tVal == 4096)
			{
				SendDlgItemMessage(hDlg, CBO_IMGXRES, CB_SETCURSEL, 8, 0);
			}
			else
			{
				SendDlgItemMessage(hDlg, CBO_IMGXRES, CB_SETCURSEL, 0, 0);
			}

			tVal = g_font.GetSize(MAPHEIGHT);
			if (tVal == 32)
			{
				SendDlgItemMessage(hDlg, CBO_IMGYRES, CB_SETCURSEL, 1, 0);
			}
			else if (tVal == 64)
			{
				SendDlgItemMessage(hDlg, CBO_IMGYRES, CB_SETCURSEL, 2, 0);
			}
			else if (tVal == 128)
			{
				SendDlgItemMessage(hDlg, CBO_IMGYRES, CB_SETCURSEL, 3, 0);
			}
			else if (tVal == 256)
			{
				SendDlgItemMessage(hDlg, CBO_IMGYRES, CB_SETCURSEL, 4, 0);
			}
			else if (tVal == 512)
			{
				SendDlgItemMessage(hDlg, CBO_IMGYRES, CB_SETCURSEL, 5, 0);
			}
			else if (tVal == 1024)
			{
				SendDlgItemMessage(hDlg, CBO_IMGYRES, CB_SETCURSEL, 6, 0);
			}
			else if (tVal == 2048)
			{
				SendDlgItemMessage(hDlg, CBO_IMGYRES, CB_SETCURSEL, 7, 0);
			}
			else if (tVal == 4096)
			{
				SendDlgItemMessage(hDlg, CBO_IMGYRES, CB_SETCURSEL, 8, 0);
			}
			else
			{
				SendDlgItemMessage(hDlg, CBO_IMGYRES, CB_SETCURSEL, 0, 0);
			}

			wsprintf(Text, "%d", g_font.GetSize(CELLHEIGHT));
			SendDlgItemMessage(g_hMain, TXT_CELLHEIGHT, WM_SETTEXT, 0, (LPARAM)Text);

			wsprintf(Text, "%d", g_font.GetSize(CELLWIDTH));
			SendDlgItemMessage(g_hMain, TXT_CELLWIDTH, WM_SETTEXT, 0, (LPARAM)Text);

			g_config.MaxChars = g_font.GetSize(MAXCHARS);

			g_config.hScr = 0;
			g_config.vScr = 0;
			g_config.Zoom = 1.0f;

			if (Flags & SHOW_GRID)
			{
				g_config.Grid = true;
				CheckMenuItem(GetMenu(g_hMain), ID_VIEW_SHOWGRID, MF_CHECKED);
			}
			else
			{
				g_config.Grid = false;
				CheckMenuItem(GetMenu(g_hMain), ID_VIEW_SHOWGRID, MF_UNCHECKED);
			}

			if (Flags & SHOW_WIDTH)
			{
				g_config.wMarker = true;
				CheckMenuItem(GetMenu(g_hMain), ID_VIEW_WIDTHMARKERS, MF_CHECKED);
			}
			else
			{
				g_config.wMarker = false;
				CheckMenuItem(GetMenu(g_hMain), ID_VIEW_WIDTHMARKERS, MF_UNCHECKED);
			}

			g_font.SetBaseChar(32);
			wsprintf(Text, "%d", 32);
			SendDlgItemMessage(g_hMain, TXT_START, WM_SETTEXT, 0, (LPARAM)Text);

			wsprintf(Text, "%d", g_font.GetFontHeight());
			SendDlgItemMessage(g_hMain, TXT_FONTHEIGHT, WM_SETTEXT, 0, (LPARAM)Text);

			wsprintf(Text, "%d", g_font.GetFontWidth());
			SendDlgItemMessage(g_hMain, TXT_FONTWIDTH, WM_SETTEXT, 0, (LPARAM)Text);

			g_font.SetFontQuality(NONANTIALIASED_QUALITY);
			CheckMenuItem(GetMenu(g_hMain), ID_ANTIALIAS_NONE, MF_CHECKED);
			CheckMenuItem(GetMenu(g_hMain), ID_ANTIALIAS_NORMAL, MF_UNCHECKED);
			CheckMenuItem(GetMenu(g_hMain), ID_ANTIALIAS_CLEARTYPE, MF_UNCHECKED);

			EnableWindow(GetDlgItem(g_hMain, STA_WIDTH), FALSE);
			SendMessage(g_hMain, WM_APP + 1, 0, 0);

			CreateFontMap();
			return TRUE;

		case ID_FILE_EXPORT40051:
			lstrcpy(Text, "ExportedFont");

			OPENFILENAME ofn;
			memset(&ofn, 0, sizeof(ofn));
			ofn.lStructSize = sizeof(OPENFILENAME);
			ofn.hwndOwner = g_hMain;
			ofn.hInstance = g_hInstance;
			ofn.lpstrFilter = "Bitmap Images (BMP)\0*.bmp\0Targa Images (TGA)\0*.tga\0Portable Network Graphics Images (PNG)\0*.png\0Comma Separated Values (CSV)\0*.csv\0\0";
			ofn.nFilterIndex = 1;
			ofn.lpstrFile = Text;
			ofn.nMaxFile = 255;
			ofn.lpstrTitle = "Export Font";
			ofn.Flags = OFN_OVERWRITEPROMPT | OFN_HIDEREADONLY | OFN_NONETWORKBUTTON;
			ofn.lpstrDefExt = "bmp";
			if (GetSaveFileName(&ofn))
			{
				if (!g_font.Export(ofn.lpstrFile, ofn.nFilterIndex))
				{
					MessageBox(hDlg, "Export Failed", "Error", MB_OK | MB_ICONERROR);
				}
			}

			return TRUE;

		case ID_FILE_EXIT:
			EndDialog(hDlg, 0);
			PostQuitMessage(0);
			return TRUE;

		case ID_VIEW_ZOOMIN:
			if (g_config.Zoom < 4.0f)
			{
				g_config.Zoom *= 2;
				CalcScroll();
				CreateFontMap();
			}
			return TRUE;

		case ID_VIEW_ZOOMOUT:
			if (g_config.Zoom > 0.5f)
			{
				g_config.Zoom /= 2;
				CalcScroll();
				CreateFontMap();
			}
			return TRUE;

		case ID_ANTIALIAS_NONE:
			CheckMenuItem(GetMenu(g_hMain), ID_ANTIALIAS_NONE, MF_CHECKED);
			CheckMenuItem(GetMenu(g_hMain), ID_ANTIALIAS_NORMAL, MF_UNCHECKED);
			CheckMenuItem(GetMenu(g_hMain), ID_ANTIALIAS_CLEARTYPE, MF_UNCHECKED);
			g_font.SetFontQuality(NONANTIALIASED_QUALITY);
			CreateFontMap();
			return TRUE;

		case ID_ANTIALIAS_NORMAL:
			CheckMenuItem(GetMenu(g_hMain), ID_ANTIALIAS_NONE, MF_UNCHECKED);
			CheckMenuItem(GetMenu(g_hMain), ID_ANTIALIAS_NORMAL, MF_CHECKED);
			CheckMenuItem(GetMenu(g_hMain), ID_ANTIALIAS_CLEARTYPE, MF_UNCHECKED);
			g_font.SetFontQuality(ANTIALIASED_QUALITY);
			CreateFontMap();
			return TRUE;

		case ID_ANTIALIAS_CLEARTYPE:
			CheckMenuItem(GetMenu(g_hMain), ID_ANTIALIAS_NONE, MF_UNCHECKED);
			CheckMenuItem(GetMenu(g_hMain), ID_ANTIALIAS_NORMAL, MF_UNCHECKED);
			CheckMenuItem(GetMenu(g_hMain), ID_ANTIALIAS_CLEARTYPE, MF_CHECKED);
			g_font.SetFontQuality(5); // CLEARTYPE_QUALITY;
			CreateFontMap();
			return TRUE;

		case ID_TOOLS_PREVIEW:
			DialogBox(g_hInstance, MAKEINTRESOURCE(DLG_PREVIEW), hDlg, PreviewWinProc);
			return TRUE;

		case ID_TOOLS_CONFIGURATION:
			DialogBox(g_hInstance, MAKEINTRESOURCE(DLG_CONFIG), hDlg, ConfigWinProc);
			g_config.MaxChars = g_font.GetSize(MAXCHARS);
			SendMessage(g_hMain, WM_APP + 1, 0, 0);
			InvalidateRgn(hDlg, NULL, NULL);
			CreateFontMap();
			return TRUE;

		case ID_HELP_ABOUT:
			MessageBox(g_hMain, "bfg 0.0", "About bfg", MB_OK);
			return TRUE;

		}

		switch (HIWORD(wParam)) // Notifications
		{
		case EN_KILLFOCUS:
			switch (LOWORD(wParam))
			{
			case TXT_CELLWIDTH:
				SendDlgItemMessage(hDlg, TXT_CELLWIDTH, WM_GETTEXT, 256, (LPARAM)Text);
				tVal = g_font.SetSize(CELLWIDTH, atoi(Text));
				wsprintf(Text, "%d", tVal);
				SendDlgItemMessage(hDlg, TXT_CELLWIDTH, WM_SETTEXT, 0, (LPARAM)Text);
				g_config.MaxChars = g_font.GetSize(MAXCHARS);
				CreateFontMap();
				return TRUE;

			case TXT_CELLHEIGHT:
				SendDlgItemMessage(hDlg, TXT_CELLHEIGHT, WM_GETTEXT, 256, (LPARAM)Text);
				tVal = g_font.SetSize(CELLHEIGHT, atoi(Text));
				wsprintf(Text, "%d", tVal);
				SendDlgItemMessage(hDlg, TXT_CELLHEIGHT, WM_SETTEXT, 0, (LPARAM)Text);
				g_config.MaxChars = g_font.GetSize(MAXCHARS);
				CreateFontMap();
				return TRUE;

			case TXT_FONTWIDTH:
				SendDlgItemMessage(hDlg, TXT_FONTWIDTH, WM_GETTEXT, 256, (LPARAM)Text);
				tVal = g_font.SetFontWidth(atoi(Text));
				wsprintf(Text, "%d", tVal);
				SendDlgItemMessage(hDlg, TXT_FONTWIDTH, WM_SETTEXT, 0, (LPARAM)Text);
				CreateFontMap();
				return TRUE;

			case TXT_FONTHEIGHT:
				SendDlgItemMessage(hDlg, TXT_FONTHEIGHT, WM_GETTEXT, 256, (LPARAM)Text);
				tVal = g_font.SetFontHeight(atoi(Text));
				wsprintf(Text, "%d", tVal);
				SendDlgItemMessage(hDlg, TXT_FONTHEIGHT, WM_SETTEXT, 0, (LPARAM)Text);
				CreateFontMap();
				return TRUE;

			case TXT_START:
				SendDlgItemMessage(hDlg, TXT_START, WM_GETTEXT, 256, (LPARAM)Text);
				tVal = g_font.SetBaseChar(atoi(Text));
				wsprintf(Text, "%d", tVal);
				SendDlgItemMessage(hDlg, TXT_START, WM_SETTEXT, 0, (LPARAM)Text);
				CreateFontMap();
				return TRUE;
			}
			return FALSE;

		case CBN_SELCHANGE:
			switch (LOWORD(wParam))
			{
			case CBO_IMGXRES:
				RowDex = SendDlgItemMessage(hDlg, CBO_IMGXRES, CB_GETCURSEL, 0, 0);
				if (RowDex == 0)
				{
					g_font.SetSize(MAPWIDTH, 16);
				}
				else if (RowDex == 1)
				{
					g_font.SetSize(MAPWIDTH, 32);
				}
				else if (RowDex == 2)
				{
					g_font.SetSize(MAPWIDTH, 64);
				}
				else if (RowDex == 3)
				{
					g_font.SetSize(MAPWIDTH, 128);
				}
				else if (RowDex == 4)
				{
					g_font.SetSize(MAPWIDTH, 256);
				}
				else if (RowDex == 5)
				{
					g_font.SetSize(MAPWIDTH, 512);
				}
				else if (RowDex == 6)
				{
					g_font.SetSize(MAPWIDTH, 1024);
				}
				else if (RowDex == 7)
				{
					g_font.SetSize(MAPWIDTH, 2048);
				}
				else if (RowDex == 8)
				{
					g_font.SetSize(MAPWIDTH, 4096);
				}

				g_config.MaxChars = g_font.GetSize(MAXCHARS);
				CalcScroll();
				CreateFontMap();
				return TRUE;

			case CBO_IMGYRES:
				RowDex = SendDlgItemMessage(hDlg, CBO_IMGYRES, CB_GETCURSEL, 0, 0);
				if (RowDex == 0)
				{
					g_font.SetSize(MAPHEIGHT, 16);
				}
				else if (RowDex == 1)
				{
					g_font.SetSize(MAPHEIGHT, 32);
				}
				else if (RowDex == 2)
				{
					g_font.SetSize(MAPHEIGHT, 64);
				}
				else if (RowDex == 3)
				{
					g_font.SetSize(MAPHEIGHT, 128);
				}
				else if (RowDex == 4)
				{
					g_font.SetSize(MAPHEIGHT, 256);
				}
				else if (RowDex == 5)
				{
					g_font.SetSize(MAPHEIGHT, 512);
				}
				else if (RowDex == 6)
				{
					g_font.SetSize(MAPHEIGHT, 1024);
				}
				else if (RowDex == 7)
				{
					g_font.SetSize(MAPHEIGHT, 2048);
				}
				else if (RowDex == 8)
				{
					g_font.SetSize(MAPHEIGHT, 4096);
				}

				g_config.MaxChars = g_font.GetSize(MAXCHARS);
				CalcScroll();
				CreateFontMap();
				return TRUE;
			}
			return FALSE;

		}

	default:
		return 0;
	}
	}
}

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);
	UNREFERENCED_PARAMETER(nCmdShow);

	g_hInstance = hInstance;

	LoadConfig();

	MSG msg;
	msg.wParam = 1;

	g_hMain = CreateDialog(g_hInstance, MAKEINTRESOURCE(RES_DLG_MAIN), NULL, MainProc);
	if (!g_hMain)
	{
		goto exit;
	}

	g_hBackground = CreatePatternBrush(LoadBitmap(g_hInstance, MAKEINTRESOURCE(RES_BMP_BACKGROUND)));
	if (g_hBackground == NULL)
	{
		goto exit;
	}

	//HACCEL hAccels = LoadAccelerators(g_hInstance, MAKEINTRESOURCEW(RES_ACCELERATORS));
	for (;;)
	{
		switch (GetMessage(&msg, NULL, 0, 0))
		{
		case -1:
		case FALSE:
			goto exit;

		default:
			if (!IsDialogMessage(g_hMain, &msg))
			{
				if (/*TranslateAccelerator(g_hMain, hAccels, &msg) == 0*/1)
				{
					TranslateMessage(&msg);
					DispatchMessage(&msg);
				}
			}
			break;
		}
	}

exit:
	if (g_hBackground != NULL)
	{
		DeleteObject(g_hBackground);
	}

	return msg.wParam;
}
