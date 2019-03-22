#include "pch.h"
#include "font.h"
#include "resource.h"

HDC glDC;
HWND hGL;

extern HINSTANCE g_hInstance;
extern HWND g_hMain;
extern Font Fnt;

BOOL CALLBACK PreviewWinProc(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam)
{
	int nLines, Loop, chLoop, offset, i, x, y;
	int CurX, CurY;
	float RowFactor, ColFactor, U, V;
	int SrcCol, SrcRow, RowPitch;
	unsigned char Text[255];
	HBITMAP *hBMP;
	Font FntImg;
	RECT glRect;
	LRESULT lTxt;
	BITMAP          bitmap;
	HDC             hdcMem;
	HGDIOBJ         oldBitmap;
	PAINTSTRUCT ps;
	HDC hdc;
	HBRUSH hbrWhite, hbrGray;
	RECT rc, drc;
	BOOL igr, gr;
	LPDRAWITEMSTRUCT lpdis;
	HGDIOBJ original;

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

	UNREFERENCED_PARAMETER(lParam);

	switch (msg)
	{
	case WM_INITDIALOG:
		if (!lstrlen(PText))
		{
			srand((unsigned int)time(NULL));
			offset = rand() % 13;
			lstrcpy(PText, &Sample[offset][0]);
		}
		SendDlgItemMessage(hDlg, TXT_PREVIEW, EM_LIMITTEXT, 254, 0);
		SendDlgItemMessage(hDlg, TXT_PREVIEW, WM_SETTEXT, 0, (LPARAM)PText);
		return TRUE;

	case WM_APP:
		CurX = CurY = 0;

		GetClientRect(hGL, &glRect);
		//glViewport(0, 0, glRect.right, glRect.bottom);
		//BkCol = Fnt.GetCol(BACKCOL);
		//glClearColor(((float)BkCol.Red / 255.0f), ((float)BkCol.Green / 255.0f), ((float)BkCol.Blue / 255.0f), 0.0f);

		/*glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		glOrtho(0.0f, glRect.right, glRect.bottom, 0.0f, -10.0f, 10.0f);
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);*/

		nLines = SendDlgItemMessage(hDlg, TXT_PREVIEW, EM_GETLINECOUNT, 0, 0);

		RowPitch = Fnt.GetSize(MAPWIDTH) / Fnt.GetSize(CELLWIDTH);
		RowFactor = (float)Fnt.GetSize(CELLHEIGHT) / (float)Fnt.GetSize(MAPHEIGHT);
		ColFactor = (float)Fnt.GetSize(CELLWIDTH) / (float)Fnt.GetSize(MAPWIDTH);

		//glBegin(GL_QUADS);
		for (Loop = 0; Loop != nLines; ++Loop)
		{
			Text[0] = 0xFF;
			Text[1] = 0;
			lTxt = SendDlgItemMessage(hDlg, TXT_PREVIEW, EM_GETLINE, Loop, (LPARAM)Text);
			Text[lTxt] = NULL;

			for (chLoop = 0; chLoop != lTxt; ++chLoop)
			{
				SrcRow = (Text[chLoop] - Fnt.GetBaseChar()) / RowPitch;
				SrcCol = (Text[chLoop] - Fnt.GetBaseChar()) - (SrcRow * RowPitch);
				U = ColFactor * SrcCol;
				V = RowFactor * SrcRow;

				/*glTexCoord2f(U, V);
				glVertex2i(CurX, CurY);
				glTexCoord2f(U + ColFactor, V);
				glVertex2i(CurX + Fnt.GetSize(CELLWIDTH), CurY);
				glTexCoord2f(U + ColFactor, V + RowFactor);
				glVertex2i(CurX + Fnt.GetSize(CELLWIDTH), CurY + Fnt.GetSize(CELLHEIGHT));
				glTexCoord2f(U, V + RowFactor);
				glVertex2i(CurX, CurY + Fnt.GetSize(CELLHEIGHT));*/
				CurX += Fnt.GetCharVal(Text[chLoop], EWIDTH);
			}
			CurX = 0;
			CurY += Fnt.GetSize(CELLHEIGHT);
		}

		SwapBuffers(glDC);
		return TRUE;

#if 0
	case WM_PAINT:
		hdc = BeginPaint(GetDlgItem(hDlg, IDC_GL), &ps);

		//
		hbrWhite = (HBRUSH)GetStockObject(WHITE_BRUSH);
		hbrGray = (HBRUSH)GetStockObject(LTGRAY_BRUSH);
		FillRect(hdc, &ps.rcPaint, hbrWhite);
		for (y = ps.rcPaint.top; y < ps.rcPaint.bottom; y += 8)
		{
			gr = (y / 8) % 2;

			for (x = ps.rcPaint.left; x < ps.rcPaint.right; x += 8)
			{
				drc.left = x;
				drc.top = y;
				drc.right = min(x + 8, ps.rcPaint.right);
				drc.bottom = min(y + 8, ps.rcPaint.bottom);

				if (gr)
				{
					FillRect(hdc, &drc, hbrGray);
				}
				gr = !gr;
			}
		}

		//
		hdcMem = CreateCompatibleDC(hdc);

		hBMP = Fnt.DrawFontMap(FALSE, -1);
		original = SelectObject(hdcMem, *hBMP);

		GetObject(*hBMP, sizeof(bitmap), &bitmap);
		BitBlt(hdc, ps.rcPaint.left, ps.rcPaint.top, ps.rcPaint.right, ps.rcPaint.bottom, hdcMem, 0, 0, SRCPAINT);

		SelectObject(hdcMem, original);
		DeleteDC(hdcMem);

		DeleteObject(*hBMP);

		EndPaint(hDlg, &ps);
		return TRUE;
#endif

	case WM_CLOSE:
		SendDlgItemMessage(hDlg, TXT_PREVIEW, WM_GETTEXT, 1024, (LPARAM)PText);
		EndDialog(hDlg, 0);
		return TRUE;

	case WM_DRAWITEM:
		lpdis = (LPDRAWITEMSTRUCT)lParam;

		FillRect(lpdis->hDC, &lpdis->rcItem, (HBRUSH)GetStockObject(WHITE_BRUSH));

		hbrGray = (HBRUSH)GetStockObject(LTGRAY_BRUSH);
		for (y = lpdis->rcItem.top; y < lpdis->rcItem.bottom; y += 8)
		{
			gr = y / 8 % 2;
			for (x = lpdis->rcItem.left; x < lpdis->rcItem.right; x += 8)
			{
				if (gr)
				{
					drc.left = x;
					drc.top = y;
					drc.right = min(x + 8, lpdis->rcItem.right);
					drc.bottom = min(y + 8, lpdis->rcItem.bottom);
					FillRect(lpdis->hDC, &drc, hbrGray);
				}
				gr = !gr;
			}
		}
		return TRUE;

	case WM_COMMAND:
	{
		switch (HIWORD(wParam))
		{
		case EN_CHANGE:
			SendMessage(hDlg, WM_APP, 0, 0);
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
			SendMessage(hDlg, WM_APP, 0, 0);
			return TRUE;

		case CMD_TEST_PANGRAM:
			offset = rand() % 13;
			SendDlgItemMessage(hDlg, TXT_PREVIEW, WM_SETTEXT, 0, (LPARAM)&Sample[offset][0]);
			SendMessage(hDlg, WM_APP, 0, 0);
			return TRUE;
		}

	}

	default:
		return FALSE;
	}
}
