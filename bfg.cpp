#include "pch.h"
#include "font.h"
#include "utils.h"
#include "defs.h"
#include "resource.h"

//#pragma comment(linker,"\"/manifestdependency:type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")

HINSTANCE g_hInstance;
HWND g_hMain;
Font Fnt;
AppInfo info;
long OldProc;

HBRUSH g_hBackground;

BOOL CALLBACK ConfigWinProc(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam);
BOOL CALLBACK PreviewWinProc(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam);
BOOL CALLBACK SaveOptProc(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam);

BOOL GetTargetName(char *fname, const char *Title, const char *filter, const char *DefExt)
{
	OPENFILENAME fileopeninfo;

	memset(&fileopeninfo, 0, sizeof(fileopeninfo));
	fileopeninfo.lStructSize = sizeof(OPENFILENAME);
	fileopeninfo.hwndOwner = g_hMain;
	fileopeninfo.hInstance = g_hInstance;
	fileopeninfo.lpstrFilter = filter;
	fileopeninfo.nFilterIndex = 1;
	fileopeninfo.lpstrFile = fname;
	fileopeninfo.nMaxFile = 255;
	fileopeninfo.lpstrTitle = Title;
	fileopeninfo.Flags = OFN_OVERWRITEPROMPT | OFN_HIDEREADONLY | OFN_NONETWORKBUTTON;
	fileopeninfo.lpstrDefExt = DefExt;

	return GetSaveFileName(&fileopeninfo);
}

BOOL CALLBACK TextWinProc(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam)
{
	int Row, Col, Sel;

	switch (msg)
	{
	case WM_ERASEBKGND:
		return TRUE;
		break;

	case WM_LBUTTONDOWN:
		SetFocus(hDlg);
		if ((LOWORD(lParam) / info.Zoom) <= Fnt.GetSize(MAPWIDTH) && (HIWORD(lParam) / info.Zoom) <= Fnt.GetSize(MAPHEIGHT))
		{
			Row = (int)((HIWORD(lParam) + info.vScr) / info.Zoom) / Fnt.GetSize(CELLHEIGHT);
			Col = (int)((LOWORD(lParam) + info.hScr) / info.Zoom) / Fnt.GetSize(CELLWIDTH);

			// Limit selection
			Sel = (Row * (Fnt.GetSize(MAPWIDTH) / Fnt.GetSize(CELLWIDTH))) + Col;
			if (Sel + Fnt.GetBaseChar() > 255)
			{
				info.Select = 255 - Fnt.GetBaseChar();
			}
			else
			{
				info.Select = Sel;
			}

			if (info.ModAll == TRUE)
			{
				SendDlgItemMessage(g_hMain, RAD_SEL, BM_SETCHECK, BST_CHECKED, 0);
				SendDlgItemMessage(g_hMain, RAD_ALL, BM_SETCHECK, BST_UNCHECKED, 0);
				EnableWindow(GetDlgItem(g_hMain, TXT_WIDTH), TRUE);
				EnableWindow(GetDlgItem(g_hMain, STA_WIDTH), TRUE);
				info.ModAll = FALSE;
			}

			SendMessage(g_hMain, WM_APP + 1, 0, 0);
			CreateFontMap();
		}
		return TRUE;
	}

	return CallWindowProc((WNDPROC)OldProc, hDlg, msg, wParam, lParam);
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
	std::string VerData, VerNum;
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

		tVal = Fnt.GetSize(MAPWIDTH);
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

		tVal = Fnt.GetSize(MAPHEIGHT);
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

		SendDlgItemMessage(hDlg, CBO_ALIAS, CB_ADDSTRING, 0, (LPARAM)"None");
		SendDlgItemMessage(hDlg, CBO_ALIAS, CB_ADDSTRING, 0, (LPARAM)"Normal Anti-Alias");
		SendDlgItemMessage(hDlg, CBO_ALIAS, CB_ADDSTRING, 0, (LPARAM)"ClearType (WinXP Only)");
		SendDlgItemMessage(hDlg, CBO_ALIAS, CB_SETCURSEL, 0, 0);

		wsprintf(Text, "%d", Fnt.GetSize(CELLWIDTH));
		SendDlgItemMessage(hDlg, TXT_CELLWIDTH, WM_SETTEXT, 0, (LPARAM)Text);
		wsprintf(Text, "%d", Fnt.GetSize(CELLHEIGHT));
		SendDlgItemMessage(hDlg, TXT_CELLHEIGHT, WM_SETTEXT, 0, (LPARAM)Text);
		wsprintf(Text, "%d", Fnt.GetFontWidth());
		SendDlgItemMessage(hDlg, TXT_FONTWIDTH, WM_SETTEXT, 0, (LPARAM)Text);
		wsprintf(Text, "%d", Fnt.GetFontHeight());
		SendDlgItemMessage(hDlg, TXT_FONTHEIGHT, WM_SETTEXT, 0, (LPARAM)Text);

		SendDlgItemMessage(hDlg, SPN_CELLWIDTH, UDM_SETRANGE, 0, MAKELONG(256, 8));
		SendDlgItemMessage(hDlg, SPN_CELLHEIGHT, UDM_SETRANGE, 0, MAKELONG(256, 8));
		SendDlgItemMessage(hDlg, SPN_FONTHEIGHT, UDM_SETRANGE, 0, MAKELONG(256, 1));
		SendDlgItemMessage(hDlg, SPN_FONTWIDTH, UDM_SETRANGE, 0, MAKELONG(256, 0));
		SendDlgItemMessage(hDlg, SPN_WIDTH, UDM_SETRANGE, 0, MAKELONG(100, -100));
		SendDlgItemMessage(hDlg, SPN_START, UDM_SETRANGE, 0, MAKELONG(254, 0));

		SendDlgItemMessage(hDlg, RAD_ALL, BM_SETCHECK, BST_CHECKED, 0);

		info.MaxChars = Fnt.GetSize(MAXCHARS);

		PostMessage(hDlg, WM_APP, 0, 0);
		return TRUE;

	case WM_DRAWITEM:
		if (wParam == ODR_FORECOL)
		{
			dc = ((LPDRAWITEMSTRUCT)lParam)->hDC;
			GetClientRect(hDlg, &rcArea);
			hBr = CreateSolidBrush(Fnt.GetColor());
			FillRect(dc, &rcArea, hBr);
			DeleteObject(hBr);
		}

		if (wParam == ODR_BACKCOL)
		{
			dc = ((LPDRAWITEMSTRUCT)lParam)->hDC;
			GetClientRect(hDlg, &rcArea);
			hBr = CreateSolidBrush(Fnt.GetCol(BACKCOL));
			FillRect(dc, &rcArea, hBr);
			DeleteObject(hBr);
		}

		CreateFontMap();
		return TRUE;

	case WM_APP:
		if (info.Grid)
		{
			CheckMenuItem(GetMenu(g_hMain), ID_VIEW_SHOWGRID, MF_CHECKED);
		}
		else
		{
			CheckMenuItem(GetMenu(g_hMain), ID_VIEW_SHOWGRID, MF_UNCHECKED);
		}

		if (info.wMarker)
		{
			CheckMenuItem(GetMenu(g_hMain), ID_VIEW_WIDTHMARKERS, MF_CHECKED);
		}
		else
		{
			CheckMenuItem(GetMenu(g_hMain), ID_VIEW_WIDTHMARKERS, MF_UNCHECKED);
		}

		EnableWindow(GetDlgItem(g_hMain, SCR_HOR), FALSE);
		EnableWindow(GetDlgItem(g_hMain, SCR_VERT), FALSE);

		Fnt.SetBaseChar(32);
		wsprintf(Text, "%d", Fnt.GetBaseChar());
		SendDlgItemMessage(hDlg, TXT_START, WM_SETTEXT, 0, (LPARAM)Text);

		SendMessage(g_hMain, WM_APP + 1, 0, 0);
		EnableWindow(GetDlgItem(g_hMain, TXT_WIDTH), FALSE);
		EnableWindow(GetDlgItem(g_hMain, STA_WIDTH), FALSE);

		CalcScroll();
		CreateFontMap();
		return FALSE;

	case WM_APP + 1: // Control Update
		if (info.ModAll == TRUE)
		{
			wsprintf(Text, "%d", Fnt.GetGlobal(HOFFSET));
			SendDlgItemMessage(g_hMain, TXT_XADJ, WM_SETTEXT, 0, (LPARAM)Text);
			wsprintf(Text, "%d", Fnt.GetGlobal(VOFFSET));
			SendDlgItemMessage(g_hMain, TXT_YADJ, WM_SETTEXT, 0, (LPARAM)Text);
			wsprintf(Text, "%d", Fnt.GetGlobal(WIDTH));
			SendDlgItemMessage(g_hMain, TXT_WADJ, WM_SETTEXT, 0, (LPARAM)Text);
			SendDlgItemMessage(g_hMain, TXT_WIDTH, WM_SETTEXT, 0, (LPARAM)"");
		}
		else
		{
			wsprintf(Text, "%d", Fnt.GetCharVal(info.Select + Fnt.GetBaseChar(), HOFFSET));
			SendDlgItemMessage(g_hMain, TXT_XADJ, WM_SETTEXT, 0, (LPARAM)Text);
			wsprintf(Text, "%d", Fnt.GetCharVal(info.Select + Fnt.GetBaseChar(), VOFFSET));
			SendDlgItemMessage(g_hMain, TXT_YADJ, WM_SETTEXT, 0, (LPARAM)Text);
			wsprintf(Text, "%d", Fnt.GetCharVal(info.Select + Fnt.GetBaseChar(), WOFFSET));
			SendDlgItemMessage(g_hMain, TXT_WADJ, WM_SETTEXT, 0, (LPARAM)Text);
			wsprintf(Text, "%d", Fnt.GetCharVal(info.Select + Fnt.GetBaseChar(), EWIDTH));
			SendDlgItemMessage(g_hMain, TXT_WIDTH, WM_SETTEXT, 0, (LPARAM)Text);
			wsprintf(Text, "Adjust Selection (%d) Only", info.Select + Fnt.GetBaseChar());
			SendDlgItemMessage(g_hMain, RAD_SEL, WM_SETTEXT, 0, (LPARAM)Text);
		}
		return TRUE;

	case WM_CLOSE:
	case WM_DESTROY:
		EndDialog(hDlg, 0);
		PostQuitMessage(0);
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
			info.hScr = SendDlgItemMessage(hDlg, SCR_HOR, SBM_GETPOS, 0, 0);
			SetScrollPos(GetDlgItem(hDlg, SCR_HOR), SB_CTL, info.hScr, TRUE);
			CreateFontMap();
			return 0;
		}

		case SB_LINELEFT:
			if (info.hScroll == FALSE)
			{
				return 0;
			}
			info.hScr -= 8;
			if (info.hScr < 0)
			{
				info.hScr = 0;
			}
			SetScrollPos(GetDlgItem(g_hMain, SCR_HOR), SB_CTL, info.hScr, TRUE);
			CreateFontMap();
			return 0;

		case SB_LINERIGHT:
			if (info.hScroll == FALSE)
			{
				return 0;
			}
			info.hScr += 8;
			scrInf.cbSize = sizeof(SCROLLINFO);
			scrInf.fMask = SIF_RANGE;
			GetScrollInfo(GetDlgItem(g_hMain, SCR_HOR), SB_CTL, &scrInf);
			if (info.hScr > scrInf.nMax)
			{
				info.hScr = scrInf.nMax;
			}
			SetScrollPos(GetDlgItem(g_hMain, SCR_HOR), SB_CTL, info.hScr, TRUE);
			CreateFontMap();
			return 0;

		case SB_PAGELEFT:
			info.hScr -= 24;
			SetScrollPos(GetDlgItem(g_hMain, SCR_HOR), SB_CTL, info.hScr, TRUE);
			CreateFontMap();
			return 0;

		case SB_PAGERIGHT:
			info.hScr += 24;
			SetScrollPos(GetDlgItem(g_hMain, SCR_HOR), SB_CTL, info.hScr, TRUE);
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
			info.vScr = HIWORD(wParam);
			CreateFontMap();
			return 0;

		case SB_LINEUP:
			if (info.vScroll == FALSE)
			{
				return 0;
			}
			info.vScr -= 8;
			if (info.vScr < 0)
			{
				info.vScr = 0;
			}
			SetScrollPos(GetDlgItem(g_hMain, SCR_VERT), SB_CTL, info.vScr, TRUE);
			CreateFontMap();
			return 0;

		case SB_LINEDOWN:
			if (info.vScroll == FALSE)
			{
				return 0;
			}
			info.vScr += 8;
			scrInf.cbSize = sizeof(SCROLLINFO);
			scrInf.fMask = SIF_RANGE;
			GetScrollInfo(GetDlgItem(g_hMain, SCR_VERT), SB_CTL, &scrInf);
			if (info.vScr > scrInf.nMax)
			{
				info.vScr = scrInf.nMax;
			}
			SetScrollPos(GetDlgItem(g_hMain, SCR_VERT), SB_CTL, info.vScr, TRUE);
			CreateFontMap();
			return 0;

		case SB_PAGEDOWN:
			info.vScr += 24;
			SetScrollPos(GetDlgItem(g_hMain, SCR_VERT), SB_CTL, info.vScr, TRUE);
			CreateFontMap();
			return 0;

		case SB_PAGEUP:
			info.vScr -= 24;
			SetScrollPos(GetDlgItem(g_hMain, SCR_VERT), SB_CTL, info.vScr, TRUE);
			CreateFontMap();
			return 0;
		}

		return FALSE;
	}

	case WM_NOTIFY:
	{
		NMUPDOWN *Hdr;
		Hdr = (LPNMUPDOWN) lParam;

		if (Hdr->hdr.code == UDN_DELTAPOS)
		{
			switch (Hdr->hdr.idFrom)
			{
			case SPN_CELLHEIGHT:
				Fnt.SetSize(CELLHEIGHT, Hdr->iPos + Hdr->iDelta);
				info.MaxChars = Fnt.GetSize(MAXCHARS);
				info.Select = LimitSelection(info.Select, info.MaxChars);
				CreateFontMap();
				return 0;

			case SPN_CELLWIDTH:
				Fnt.SetSize(CELLWIDTH, Hdr->iPos + Hdr->iDelta);
				info.MaxChars = Fnt.GetSize(MAXCHARS);
				info.Select = LimitSelection(info.Select, info.MaxChars);
				CreateFontMap();
				return 0;

			case SPN_FONTHEIGHT:
				Fnt.SetFontHeight(Hdr->iPos + Hdr->iDelta);
				CreateFontMap();
				return 0;

			case SPN_FONTWIDTH:
				Fnt.SetFontWidth(Hdr->iPos + Hdr->iDelta);
				CreateFontMap();
				return 0;

			case SPN_WIDTH:
				if (info.ModAll)
				{
					Fnt.SetGlobal(WIDTH, Hdr->iPos + Hdr->iDelta);
					CreateFontMap();
				}
				else
				{
					Fnt.SetCharVal(info.Select + Fnt.GetBaseChar(), WOFFSET, Hdr->iPos + Hdr->iDelta);
					wsprintf(Text, "%d", Fnt.GetCharVal(info.Select + Fnt.GetBaseChar(), EWIDTH));
					SendDlgItemMessage(g_hMain, TXT_WIDTH, WM_SETTEXT, 0, (LPARAM)Text);
					CreateFontMap();
				}
				return 0;

			case SPN_START:
				if (Hdr->iPos > 0)
				{
					Fnt.SetBaseChar(Hdr->iPos + Hdr->iDelta);
				}

				if (Fnt.GetBaseChar() + info.Select > 255)
				{
					info.Select = 255 - Fnt.GetBaseChar();
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
			cf.lpLogFont = Fnt.GetLogicalFont();
			//cf.iPointSize = 0;
			cf.Flags = CF_BOTH | CF_INITTOLOGFONTSTRUCT | CF_EFFECTS | CF_ENABLEHOOK;
			cf.rgbColors = Fnt.GetColor();
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
				//Fnt.SetFontName(Text);
				CreateFontMap();
			}

			return TRUE;

		case ID_COLOUR_SETTEXTCOLOUR:
		case ODR_FORECOL:
			SelCol.lStructSize = sizeof(CHOOSECOLOR);
			SelCol.hwndOwner = hDlg;
			SelCol.rgbResult = Fnt.GetColor();
			SelCol.lpCustColors = (LPDWORD)CustCol;
			SelCol.Flags = CC_FULLOPEN | CC_RGBINIT | CC_ANYCOLOR;
			if (ChooseColor(&SelCol))
			{
				Fnt.SetColor(SelCol.rgbResult);
			}

			InvalidateRgn(hDlg, NULL, NULL);
			return TRUE;

		case ID_COLOUR_SETBACKGROUNDCOLOUR:
		case ODR_BACKCOL:
			SelCol.lStructSize = sizeof(CHOOSECOLOR);
			SelCol.hwndOwner = hDlg;
			SelCol.rgbResult = Fnt.GetCol(BACKCOL);
			SelCol.lpCustColors = (LPDWORD)CustCol;
			SelCol.Flags = CC_FULLOPEN | CC_RGBINIT | CC_ANYCOLOR;
			if (ChooseColor(&SelCol))
			{
				Fnt.SetCol(BACKCOL, SelCol.rgbResult);
			}

			InvalidateRgn(hDlg, NULL, NULL);
			return TRUE;

		case ID_VIEW_SHOWGRID:
			info.Grid ^= 1;
			if (info.Grid)
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
			info.wMarker ^= 1;
			if (info.wMarker)
			{
				CheckMenuItem(GetMenu(g_hMain), ID_VIEW_WIDTHMARKERS, MF_CHECKED);
			}
			else
			{
				CheckMenuItem(GetMenu(g_hMain), ID_VIEW_WIDTHMARKERS, MF_UNCHECKED);
			}
			CreateFontMap();
			return TRUE;

		case CMD_LEFT:
			if (info.ModAll)
			{
				tVal = Fnt.GetGlobal(HOFFSET);
				Fnt.SetGlobal(HOFFSET, tVal - 1);
				SendMessage(g_hMain, WM_APP + 1, 0, 0);
			}
			else
			{
				tVal = Fnt.GetCharVal(Fnt.GetBaseChar() + info.Select, HOFFSET);
				Fnt.SetCharVal(Fnt.GetBaseChar() + info.Select, HOFFSET, tVal - 1);
				SendMessage(g_hMain, WM_APP + 1, 0, 0);
			}
			CreateFontMap();
			return TRUE;

		case CMD_RIGHT:
			if (info.ModAll)
			{
				tVal = Fnt.GetGlobal(HOFFSET);
				Fnt.SetGlobal(HOFFSET, tVal + 1);
				SendMessage(g_hMain, WM_APP + 1, 0, 0);
			}
			else
			{
				tVal = Fnt.GetCharVal(Fnt.GetBaseChar() + info.Select, HOFFSET);
				Fnt.SetCharVal(Fnt.GetBaseChar() + info.Select, HOFFSET, tVal + 1);
				SendMessage(g_hMain, WM_APP + 1, 0, 0);
			}
			CreateFontMap();
			return TRUE;

		case CMD_UP:
			if (info.ModAll)
			{
				tVal = Fnt.GetGlobal(VOFFSET);
				Fnt.SetGlobal(VOFFSET, tVal - 1);
				SendMessage(g_hMain, WM_APP + 1, 0, 0);
			}
			else
			{
				tVal = Fnt.GetCharVal(Fnt.GetBaseChar() + info.Select, VOFFSET);
				Fnt.SetCharVal(Fnt.GetBaseChar() + info.Select, VOFFSET, tVal - 1);
				SendMessage(g_hMain, WM_APP + 1, 0, 0);
			}
			CreateFontMap();
			return TRUE;

		case CMD_DOWN:
			if (info.ModAll)
			{
				tVal = Fnt.GetGlobal(VOFFSET);
				Fnt.SetGlobal(VOFFSET, tVal + 1);
				SendMessage(g_hMain, WM_APP + 1, 0, 0);
			}
			else
			{
				tVal = Fnt.GetCharVal(Fnt.GetBaseChar() + info.Select, VOFFSET);
				Fnt.SetCharVal(Fnt.GetBaseChar() + info.Select, VOFFSET, tVal + 1);
				SendMessage(g_hMain, WM_APP + 1, 0, 0);
			}
			CreateFontMap();
			return TRUE;

		case RAD_ALL:
			info.ModAll = TRUE;
			EnableWindow(GetDlgItem(g_hMain, TXT_WIDTH), FALSE);
			EnableWindow(GetDlgItem(g_hMain, STA_WIDTH), FALSE);
			SendDlgItemMessage(g_hMain, RAD_SEL, WM_SETTEXT, 0, (LPARAM)"Adjust Selection Only");
			SendMessage(g_hMain, WM_APP + 1, 0, 0);
			CreateFontMap();
			return TRUE;

		case RAD_SEL:
			info.ModAll = FALSE;
			SendMessage(g_hMain, WM_APP + 1, 0, 0);
			EnableWindow(GetDlgItem(g_hMain, TXT_WIDTH), TRUE);
			EnableWindow(GetDlgItem(g_hMain, STA_WIDTH), TRUE);
			wsprintf(Text, "Adjust Selection (%d) Only", info.Select + Fnt.GetBaseChar());
			SendDlgItemMessage(g_hMain, RAD_SEL, WM_SETTEXT, 0, (LPARAM)Text);
			CreateFontMap();
			return TRUE;

		case ID_FILE_RESET:
			Flags = Fnt.LoadConfig("bfg.cfg");
			tVal = Fnt.GetSize(MAPWIDTH);
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

			tVal = Fnt.GetSize(MAPHEIGHT);
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

			wsprintf(Text, "%d", Fnt.GetSize(CELLHEIGHT));
			SendDlgItemMessage(g_hMain, TXT_CELLHEIGHT, WM_SETTEXT, 0, (LPARAM)Text);

			wsprintf(Text, "%d", Fnt.GetSize(CELLWIDTH));
			SendDlgItemMessage(g_hMain, TXT_CELLWIDTH, WM_SETTEXT, 0, (LPARAM)Text);

			info.MaxChars = Fnt.GetSize(MAXCHARS);

			info.hScr = 0;
			info.vScr = 0;
			info.Zoom = 1.0f;

			if (Flags & SHOW_GRID)
			{
				info.Grid = true;
				CheckMenuItem(GetMenu(g_hMain), ID_VIEW_SHOWGRID, MF_CHECKED);
			}
			else
			{
				info.Grid = false;
				CheckMenuItem(GetMenu(g_hMain), ID_VIEW_SHOWGRID, MF_UNCHECKED);
			}

			if (Flags & SHOW_WIDTH)
			{
				info.wMarker = true;
				CheckMenuItem(GetMenu(g_hMain), ID_VIEW_WIDTHMARKERS, MF_CHECKED);
			}
			else
			{
				info.wMarker = false;
				CheckMenuItem(GetMenu(g_hMain), ID_VIEW_WIDTHMARKERS, MF_UNCHECKED);
			}

			Fnt.SetBaseChar(32);
			wsprintf(Text, "%d", 32);
			SendDlgItemMessage(g_hMain, TXT_START, WM_SETTEXT, 0, (LPARAM)Text);

			wsprintf(Text, "%d", Fnt.GetFontHeight());
			SendDlgItemMessage(g_hMain, TXT_FONTHEIGHT, WM_SETTEXT, 0, (LPARAM)Text);

			wsprintf(Text, "%d", Fnt.GetFontWidth());
			SendDlgItemMessage(g_hMain, TXT_FONTWIDTH, WM_SETTEXT, 0, (LPARAM)Text);

			Fnt.SetFontQuality(NONANTIALIASED_QUALITY);
			SendDlgItemMessage(hDlg, CBO_ALIAS, CB_SETCURSEL, 0, 0);
			CheckMenuItem(GetMenu(g_hMain), ID_ANTIALIAS_NONE, MF_CHECKED);
			CheckMenuItem(GetMenu(g_hMain), ID_ANTIALIAS_NORMAL, MF_UNCHECKED);
			CheckMenuItem(GetMenu(g_hMain), ID_ANTIALIAS_CLEARTYPE, MF_UNCHECKED);

			Fnt.ResetOffsets();
			info.ModAll = TRUE;
			info.Select = 0;
			SendDlgItemMessage(g_hMain, RAD_ALL, BM_SETCHECK, BST_CHECKED, 0);
			SendDlgItemMessage(g_hMain, RAD_SEL, BM_SETCHECK, BST_UNCHECKED, 0);
			SendDlgItemMessage(g_hMain, RAD_SEL, WM_SETTEXT, 0, (LPARAM)"Adjust Selection Only");
			EnableWindow(GetDlgItem(g_hMain, TXT_WIDTH), FALSE);
			EnableWindow(GetDlgItem(g_hMain, STA_WIDTH), FALSE);
			SendMessage(g_hMain, WM_APP + 1, 0, 0);

			CreateFontMap();
			return TRUE;

		case ID_EXPORT_BITMAP:
			lstrcpy(Text, "ExportedFont.bmp");
			if (GetTargetName(Text, "Export BMP", "Bitmap Images (BMP)\0*.bmp\0All Files\0*.*\0\0", "bmp"))
			{
				if (Fnt.ExportMap(Text, EXPORT_BMP) != SBM_OK)
				{
					MessageBox(hDlg, "Export Failed", "Error", MB_OK | MB_ICONEXCLAMATION);
				}
			}
			return TRUE;

		case ID_EXPORT_TARGA:
			lstrcpy(Text, "ExportedFont.tga");
			if (GetTargetName(Text, "Export TGA", "Targa Images (TGA)\0*.tga\0All Files\0*.*\0\0", "tga"))
			{
				if (Fnt.ExportMap(Text, EXPORT_TGA) != SBM_OK)
				{
					MessageBox(hDlg, "Export Failed", "Error", MB_OK | MB_ICONEXCLAMATION);
				}
			}
			return TRUE;

		case ID_EXPORT_TARGA32:
			lstrcpy(Text, "ExportedFont.tga");
			if (GetTargetName(Text, "Export TGA", "Targa Images (TGA)\0*.tga\0All Files\0*.*\0\0", "tga"))
			{
				if (Fnt.ExportMap(Text, EXPORT_TGA32) != SBM_OK)
				{
					MessageBox(hDlg, "Export Failed", "Error", MB_OK | MB_ICONEXCLAMATION);
				}
			}
			return TRUE;

		case ID_EXPORT_PORTABLENETWORKGRAPHICS:
			lstrcpy(Text, "ExportedFont.png");
			if (GetTargetName(Text, "Export PNG", "Portable Network Graphics Images (PNG)\0*.png\0All Files\0*.*\0\0", "png"))
			{
				if (Fnt.ExportMap(Text, EXPORT_PNG) != SBM_OK)
				{
					MessageBox(hDlg, "Export Failed", "Error", MB_OK | MB_ICONEXCLAMATION);
				}
			}
			return TRUE;

		case ID_EXPORT_FONTDATA:
			lstrcpy(Text, "FontData.csv");
			if (GetTargetName(Text, "Export Font Data", "Comma Separated Values (CSV)\0*.csv\0All Files\0*.*\0\0", "csv"))
			{
				if (!Fnt.SaveFont(SAVE_CSV, Text))
				{
					MessageBox(hDlg, "Export Failed", "Error", MB_OK | MB_ICONEXCLAMATION);
				}
			}
			return TRUE;

		case ID_FILE_EXIT:
			EndDialog(hDlg, 0);
			PostQuitMessage(0);
			return TRUE;

		case ID_VIEW_ZOOMIN:
			if (info.Zoom < 4.0f)
			{
				info.Zoom *= 2;
				CalcScroll();
				CreateFontMap();
			}
			return TRUE;

		case ID_VIEW_ZOOMOUT:
			if (info.Zoom > 0.5f)
			{
				info.Zoom /= 2;
				CalcScroll();
				CreateFontMap();
			}
			return TRUE;

		case ID_ANTIALIAS_NONE:
			SendDlgItemMessage(hDlg, CBO_ALIAS, CB_SETCURSEL, 0, 0);
			CheckMenuItem(GetMenu(g_hMain), ID_ANTIALIAS_NONE, MF_CHECKED);
			CheckMenuItem(GetMenu(g_hMain), ID_ANTIALIAS_NORMAL, MF_UNCHECKED);
			CheckMenuItem(GetMenu(g_hMain), ID_ANTIALIAS_CLEARTYPE, MF_UNCHECKED);
			Fnt.SetFontQuality(NONANTIALIASED_QUALITY);
			CreateFontMap();
			return TRUE;

		case ID_ANTIALIAS_NORMAL:
			SendDlgItemMessage(hDlg, CBO_ALIAS, CB_SETCURSEL, 1, 0);
			CheckMenuItem(GetMenu(g_hMain), ID_ANTIALIAS_NONE, MF_UNCHECKED);
			CheckMenuItem(GetMenu(g_hMain), ID_ANTIALIAS_NORMAL, MF_CHECKED);
			CheckMenuItem(GetMenu(g_hMain), ID_ANTIALIAS_CLEARTYPE, MF_UNCHECKED);
			Fnt.SetFontQuality(ANTIALIASED_QUALITY);
			CreateFontMap();
			return TRUE;

		case ID_ANTIALIAS_CLEARTYPE:
			SendDlgItemMessage(hDlg, CBO_ALIAS, CB_SETCURSEL, 2, 0);
			CheckMenuItem(GetMenu(g_hMain), ID_ANTIALIAS_NONE, MF_UNCHECKED);
			CheckMenuItem(GetMenu(g_hMain), ID_ANTIALIAS_NORMAL, MF_UNCHECKED);
			CheckMenuItem(GetMenu(g_hMain), ID_ANTIALIAS_CLEARTYPE, MF_CHECKED);
			Fnt.SetFontQuality(5); // CLEARTYPE_QUALITY;
			CreateFontMap();
			return TRUE;

		case ID_TOOLS_PREVIEW:
			DialogBox(g_hInstance, MAKEINTRESOURCE(DLG_PREVIEW), hDlg, PreviewWinProc);
			return TRUE;

		case ID_TOOLS_CONFIGURATION:
			DialogBox(g_hInstance, MAKEINTRESOURCE(DLG_CONFIG), hDlg, ConfigWinProc);
			info.MaxChars = Fnt.GetSize(MAXCHARS);
			info.Select = LimitSelection(info.Select, info.MaxChars);
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
				tVal = Fnt.SetSize(CELLWIDTH, atoi(Text));
				wsprintf(Text, "%d", tVal);
				SendDlgItemMessage(hDlg, TXT_CELLWIDTH, WM_SETTEXT, 0, (LPARAM)Text);
				info.MaxChars = Fnt.GetSize(MAXCHARS);
				CreateFontMap();
				return TRUE;

			case TXT_CELLHEIGHT:
				SendDlgItemMessage(hDlg, TXT_CELLHEIGHT, WM_GETTEXT, 256, (LPARAM)Text);
				tVal = Fnt.SetSize(CELLHEIGHT, atoi(Text));
				wsprintf(Text, "%d", tVal);
				SendDlgItemMessage(hDlg, TXT_CELLHEIGHT, WM_SETTEXT, 0, (LPARAM)Text);
				info.MaxChars = Fnt.GetSize(MAXCHARS);
				CreateFontMap();
				return TRUE;

			case TXT_FONTWIDTH:
				SendDlgItemMessage(hDlg, TXT_FONTWIDTH, WM_GETTEXT, 256, (LPARAM)Text);
				tVal = Fnt.SetFontWidth(atoi(Text));
				wsprintf(Text, "%d", tVal);
				SendDlgItemMessage(hDlg, TXT_FONTWIDTH, WM_SETTEXT, 0, (LPARAM)Text);
				CreateFontMap();
				return TRUE;

			case TXT_FONTHEIGHT:
				SendDlgItemMessage(hDlg, TXT_FONTHEIGHT, WM_GETTEXT, 256, (LPARAM)Text);
				tVal = Fnt.SetFontHeight(atoi(Text));
				wsprintf(Text, "%d", tVal);
				SendDlgItemMessage(hDlg, TXT_FONTHEIGHT, WM_SETTEXT, 0, (LPARAM)Text);
				CreateFontMap();
				return TRUE;

			case TXT_START:
				SendDlgItemMessage(hDlg, TXT_START, WM_GETTEXT, 256, (LPARAM)Text);
				tVal = Fnt.SetBaseChar(atoi(Text));
				wsprintf(Text, "%d", tVal);
				SendDlgItemMessage(hDlg, TXT_START, WM_SETTEXT, 0, (LPARAM)Text);
				CreateFontMap();
				return TRUE;

			case TXT_XADJ:
				if (info.ModAll)
				{
					SendDlgItemMessage(hDlg, TXT_XADJ, WM_GETTEXT, 256, (LPARAM)Text);
					tVal = Limit(atoi(Text));
					tVal = Fnt.SetGlobal(HOFFSET, tVal);
					wsprintf(Text, "%d", tVal);
					SendDlgItemMessage(hDlg, TXT_XADJ, WM_SETTEXT, 0, (LPARAM)Text);
					CreateFontMap();
				}
				else
				{
					SendDlgItemMessage(hDlg, TXT_XADJ, WM_GETTEXT, 256, (LPARAM)Text);
					tVal = Limit(atoi(Text));
					tVal = Fnt.SetCharVal(info.Select + Fnt.GetBaseChar(), HOFFSET, tVal);
					wsprintf(Text, "%d", tVal);
					SendDlgItemMessage(hDlg, TXT_XADJ, WM_SETTEXT, 0, (LPARAM)Text);
					CreateFontMap();
				}

			case TXT_YADJ:
				if (info.ModAll)
				{
					SendDlgItemMessage(hDlg, TXT_YADJ, WM_GETTEXT, 256, (LPARAM)Text);
					tVal = Limit(atoi(Text));
					tVal = Fnt.SetGlobal(VOFFSET, tVal);
					wsprintf(Text, "%d", tVal);
					SendDlgItemMessage(hDlg, TXT_YADJ, WM_SETTEXT, 0, (LPARAM)Text);
					CreateFontMap();
				}
				else
				{
					SendDlgItemMessage(hDlg, TXT_YADJ, WM_GETTEXT, 256, (LPARAM)Text);
					tVal = Limit(atoi(Text));
					tVal = Fnt.SetCharVal(info.Select + Fnt.GetBaseChar(), VOFFSET, tVal);
					wsprintf(Text, "%d", tVal);
					SendDlgItemMessage(hDlg, TXT_YADJ, WM_SETTEXT, 0, (LPARAM)Text);
					CreateFontMap();
				}

			case TXT_WADJ:
				if (info.ModAll)
				{
					SendDlgItemMessage(hDlg, TXT_WADJ, WM_GETTEXT, 256, (LPARAM)Text);
					tVal = Limit(atoi(Text));
					tVal = Fnt.SetGlobal(WIDTH, tVal);
					wsprintf(Text, "%d", tVal);
					SendDlgItemMessage(hDlg, TXT_WADJ, WM_SETTEXT, 0, (LPARAM)Text);
					CreateFontMap();
				}
				else
				{
					SendDlgItemMessage(hDlg, TXT_WADJ, WM_GETTEXT, 256, (LPARAM)Text);
					tVal = Limit(atoi(Text));
					tVal = Fnt.SetCharVal(info.Select + Fnt.GetBaseChar(), WOFFSET, tVal);
					wsprintf(Text, "%d", tVal);
					SendDlgItemMessage(hDlg, TXT_WADJ, WM_SETTEXT, 0, (LPARAM)Text);

					CreateFontMap();
					wsprintf(Text, "%d", Fnt.GetCharVal(info.Select + Fnt.GetBaseChar(), EWIDTH));
					SendDlgItemMessage(g_hMain, TXT_WIDTH, WM_SETTEXT, 0, (LPARAM)Text);
				}
				return TRUE;
			}
			return FALSE;

		case CBN_SELCHANGE:
			switch (LOWORD(wParam))
			{
			case CBO_ALIAS:
				RowDex = SendDlgItemMessage(hDlg, CBO_ALIAS, CB_GETCURSEL, 0, 0);
				if (RowDex == 0)
				{
					Fnt.SetFontQuality(NONANTIALIASED_QUALITY);
					CheckMenuItem(GetMenu(g_hMain), ID_ANTIALIAS_NONE, MF_CHECKED);
					CheckMenuItem(GetMenu(g_hMain), ID_ANTIALIAS_NORMAL, MF_UNCHECKED);
					CheckMenuItem(GetMenu(g_hMain), ID_ANTIALIAS_CLEARTYPE, MF_UNCHECKED);
				}
				else if (RowDex == 1)
				{
					Fnt.SetFontQuality(ANTIALIASED_QUALITY);
					CheckMenuItem(GetMenu(g_hMain), ID_ANTIALIAS_NONE, MF_UNCHECKED);
					CheckMenuItem(GetMenu(g_hMain), ID_ANTIALIAS_NORMAL, MF_CHECKED);
					CheckMenuItem(GetMenu(g_hMain), ID_ANTIALIAS_CLEARTYPE, MF_UNCHECKED);
				}
				else if (RowDex == 2)
				{
					Fnt.SetFontQuality(5); //CLEARTYPE_QUALITY;
					CheckMenuItem(GetMenu(g_hMain), ID_ANTIALIAS_NONE, MF_UNCHECKED);
					CheckMenuItem(GetMenu(g_hMain), ID_ANTIALIAS_NORMAL, MF_UNCHECKED);
					CheckMenuItem(GetMenu(g_hMain), ID_ANTIALIAS_CLEARTYPE, MF_CHECKED);
				}
				CreateFontMap();
				return TRUE;

			case CBO_IMGXRES:
				RowDex = SendDlgItemMessage(hDlg, CBO_IMGXRES, CB_GETCURSEL, 0, 0);
				if (RowDex == 0)
				{
					Fnt.SetSize(MAPWIDTH, 16);
				}
				else if (RowDex == 1)
				{
					Fnt.SetSize(MAPWIDTH, 32);
				}
				else if (RowDex == 2)
				{
					Fnt.SetSize(MAPWIDTH, 64);
				}
				else if (RowDex == 3)
				{
					Fnt.SetSize(MAPWIDTH, 128);
				}
				else if (RowDex == 4)
				{
					Fnt.SetSize(MAPWIDTH, 256);
				}
				else if (RowDex == 5)
				{
					Fnt.SetSize(MAPWIDTH, 512);
				}
				else if (RowDex == 6)
				{
					Fnt.SetSize(MAPWIDTH, 1024);
				}
				else if (RowDex == 7)
				{
					Fnt.SetSize(MAPWIDTH, 2048);
				}
				else if (RowDex == 8)
				{
					Fnt.SetSize(MAPWIDTH, 4096);
				}

				info.MaxChars = Fnt.GetSize(MAXCHARS);
				CalcScroll();
				CreateFontMap();
				return TRUE;

			case CBO_IMGYRES:
				RowDex = SendDlgItemMessage(hDlg, CBO_IMGYRES, CB_GETCURSEL, 0, 0);
				if (RowDex == 0)
				{
					Fnt.SetSize(MAPHEIGHT, 16);
				}
				else if (RowDex == 1)
				{
					Fnt.SetSize(MAPHEIGHT, 32);
				}
				else if (RowDex == 2)
				{
					Fnt.SetSize(MAPHEIGHT, 64);
				}
				else if (RowDex == 3)
				{
					Fnt.SetSize(MAPHEIGHT, 128);
				}
				else if (RowDex == 4)
				{
					Fnt.SetSize(MAPHEIGHT, 256);
				}
				else if (RowDex == 5)
				{
					Fnt.SetSize(MAPHEIGHT, 512);
				}
				else if (RowDex == 6)
				{
					Fnt.SetSize(MAPHEIGHT, 1024);
				}
				else if (RowDex == 7)
				{
					Fnt.SetSize(MAPHEIGHT, 2048);
				}
				else if (RowDex == 8)
				{
					Fnt.SetSize(MAPHEIGHT, 4096);
				}

				info.MaxChars = Fnt.GetSize(MAXCHARS);
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

	MSG msg;
	msg.wParam = 1;

	g_hInstance = hInstance;

	info.hScr = 0;
	info.vScr = 0;
	info.Zoom = 1.0f;
	info.hScroll = FALSE;
	info.vScroll = FALSE;
	info.ModAll = TRUE;
	info.Select = 0;

	int Ret = Fnt.LoadConfig("bfg.cfg");
	if (Ret == -1)
	{
		info.Grid = true;
		info.wMarker = true;
		Fnt.SaveConfig("bfg.cfg", true, true);
	}
	else
	{
		info.Grid = Ret & SHOW_GRID;
		info.wMarker = Ret & SHOW_WIDTH;
	}

	g_hMain = CreateDialog(g_hInstance, MAKEINTRESOURCE(RES_DLG_MAIN), NULL, MainProc);
	if (!g_hMain)
	{
		goto exit;
	}

	CreateWindow("STATIC", "TextWin", WS_POPUP, 0, 0, 100, 100, g_hMain, NULL, g_hInstance, NULL);
	OldProc = GetWindowLong(GetDlgItem(g_hMain, IMG_TEXT), GWL_WNDPROC);
	SetWindowLong(GetDlgItem(g_hMain, IMG_TEXT), GWL_WNDPROC, (LONG)TextWinProc);
	SetClassLong(GetDlgItem(g_hMain, IMG_TEXT), GCL_HBRBACKGROUND, NULL);

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
	DeleteObject(g_hBackground);

	return msg.wParam;
}
