#include <commdlg.h>

class CPopFile
{
public:
	void PopFileInitialize (HWND hwnd)
	{
		static TCHAR szFilter[]=TEXT ("Text Files (*.wind)\0*.wind\0")  \
			TEXT ("ASCII Files (*.ASC)\0*.asc\0") \
			TEXT ("All Files (*.*)\0*.*\0\0") ;
		ofn.lStructSize       = sizeof (OPENFILENAME) ;
		ofn.hwndOwner         = hwnd ;
		ofn.hInstance         = NULL ;
		ofn.lpstrFilter       = szFilter ;
		ofn.lpstrCustomFilter = szFilter ;
		ofn.nMaxCustFilter    = sizeof(szFilter) ;
		ofn.nFilterIndex      = 0 ;
		ofn.lpstrFile         = NULL ;          // Set in Open and Close functions
		ofn.nMaxFile          = 260 ;          //MAX_PATH
		ofn.lpstrFileTitle    = NULL ;          // Set in Open and Close functions
		ofn.nMaxFileTitle     = 260;			//MAX_PATH
		ofn.lpstrInitialDir   = NULL;
		ofn.lpstrTitle        = NULL;
		ofn.Flags             = 0;             // Set in Open and Close functions
		ofn.nFileOffset       = 0;
		ofn.nFileExtension    = 0;
		ofn.lpstrDefExt       = TEXT ("wind");
		ofn.lCustData         = 0L;
		ofn.lpfnHook          = NULL;
		ofn.lpTemplateName    = NULL;
	}

	BOOL PopFileOpenDlg (HWND hwnd, PTSTR pstrFileName, PTSTR pstrTitleName)
	{
		ofn.hwndOwner         = hwnd;
		ofn.lpstrFile         = pstrFileName;
		ofn.lpstrFileTitle    = pstrTitleName;
		ofn.Flags             = OFN_HIDEREADONLY | OFN_CREATEPROMPT;

		return GetOpenFileName (&ofn);
	}

	BOOL PopFileSaveDlg (HWND hwnd, PTSTR pstrFileName, PTSTR pstrTitleName)
	{
		ofn.hwndOwner         = hwnd;
		ofn.lpstrFile         = pstrFileName;
		ofn.lpstrFileTitle    = pstrTitleName;
		ofn.Flags             = OFN_OVERWRITEPROMPT;

		return GetSaveFileName (&ofn);
	}
private:
	OPENFILENAME ofn;
};
