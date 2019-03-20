#include "pch.h"

extern HINSTANCE G_Inst;
extern HWND hMain;

BOOL GetSourceName(char *fname, const char *title, const char *filter, const char *DefExt)
{
	OPENFILENAME fileopeninfo;
	fname[0] = NULL;

	fileopeninfo.lStructSize = sizeof(OPENFILENAME);
	fileopeninfo.hwndOwner = hMain;
	fileopeninfo.hInstance = G_Inst;
	fileopeninfo.lpstrFilter = filter;
	fileopeninfo.lpstrCustomFilter = NULL;
	fileopeninfo.nMaxCustFilter = 0;
	fileopeninfo.nFilterIndex = 1;
	fileopeninfo.lpstrFile = fname;
	fileopeninfo.nMaxFile = 255;
	fileopeninfo.lpstrFileTitle = NULL;
	fileopeninfo.nMaxFileTitle = 0;
	fileopeninfo.lpstrInitialDir = NULL;
	fileopeninfo.lpstrTitle = title;
	fileopeninfo.Flags = OFN_OVERWRITEPROMPT | OFN_HIDEREADONLY | OFN_NONETWORKBUTTON;
	fileopeninfo.nFileOffset = 0;
	fileopeninfo.nFileExtension = 0;
	fileopeninfo.lpstrDefExt = DefExt;

	return GetOpenFileName(&fileopeninfo);
}

BOOL GetTargetName(char *fname, const char *Title, const char *filter, const char *DefExt)
{
	OPENFILENAME fileopeninfo;

	fileopeninfo.lStructSize = sizeof(OPENFILENAME);
	fileopeninfo.hwndOwner = hMain;
	fileopeninfo.hInstance = G_Inst;
	fileopeninfo.lpstrFilter = filter;
	fileopeninfo.lpstrCustomFilter = NULL;
	fileopeninfo.nMaxCustFilter = 0;
	fileopeninfo.nFilterIndex = 1;
	fileopeninfo.lpstrFile = fname;
	fileopeninfo.nMaxFile = 255;
	fileopeninfo.lpstrFileTitle = NULL;
	fileopeninfo.nMaxFileTitle = 0;
	fileopeninfo.lpstrInitialDir = NULL;
	fileopeninfo.lpstrTitle = Title;
	fileopeninfo.Flags = OFN_OVERWRITEPROMPT | OFN_HIDEREADONLY | OFN_NONETWORKBUTTON;
	fileopeninfo.nFileOffset = 0;
	fileopeninfo.nFileExtension = 0;
	fileopeninfo.lpstrDefExt = DefExt;

	return GetSaveFileName(&fileopeninfo);
}
