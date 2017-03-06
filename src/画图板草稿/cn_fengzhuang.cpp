//#include "CBaseWnd.h"
#include "CNewWnd.h"

int WINAPI WinMain(HINSTANCE hInstance,HINSTANCE hPrevInstance,LPSTR lpCmdLine,int nShowCmd )
{
	MSG msg;
	HACCEL hAccelTable;

	CNewWnd mywnd;
	mywnd.MyRegisterClass(hInstance);
	mywnd.Create(hInstance);
	mywnd.Initialize();
	mywnd.m_set_pen(PS_SOLID,2,RGB(0,0,255));
	mywnd.m_set_brush(BS_NULL,HS_DIAGCROSS,RGB(0,0,255));
	mywnd.UpdateWindow();
	mywnd.ShowWindow();

	/*CNewWnd mywnd2;
	mywnd2.MyRegisterClass(hInstance);
	mywnd2.Create(hInstance);
	mywnd2.m_set_pen(PS_SOLID,1,RGB(0,0,255));
	mywnd2.m_set_brush(BS_NULL,HS_DIAGCROSS,RGB(0,0,255));
	mywnd2.UpdateWindow();
	mywnd2.ShowWindow();*/

	hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_CN_FENGZHUANG_HENHAO));

	// Main message loop:
	while (GetMessage(&msg, NULL, 0, 0))
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return (int) msg.wParam;
}
