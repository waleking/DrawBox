//#include <windows.h>
//#include "CBaseWnd.h"
//
//ATOM CBaseWnd ::MyRegisterClass(HINSTANCE hInstance)
//{
//	static TCHAR szWindowClass[]=TEXT("cn_fengzhuang_try");
//
//	WNDCLASSEX wcex;
//
//	wcex.cbSize = sizeof(WNDCLASSEX);
//
//	wcex.style			= CS_HREDRAW | CS_VREDRAW;
//	wcex.lpfnWndProc	= CBaseWnd::staticWndProc;
//	wcex.cbClsExtra		= 0;
//	wcex.cbWndExtra		= 0;
//	wcex.hInstance		= hInstance;
//	wcex.hIcon			= LoadIcon(hInstance, IDI_APPLICATION);
//	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
//	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
//	wcex.lpszMenuName	= MAKEINTRESOURCE(IDC_CN_FENGZHUANG_HENHAO);
//	wcex.lpszClassName	= szWindowClass;
//	wcex.hIconSm		= LoadIcon(wcex.hInstance,IDI_APPLICATION);
//
//	return RegisterClassEx(&wcex);
//}
//
//
//
//static  LRESULT CALLBACK CBaseWnd::staticWndProc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam)
//{
//	CBaseWnd* pWnd;
//	pWnd=FindInMyMap(hwnd);
//	if(pWnd==NULL)
//	{
//		MYMAP& m=getStaticMap();
//		//m.insert(std::make_pair(hwnd,this));不应当这样写，原因是static 函数没有 this 指针
//		pWnd=getStaticPWnd();
//		pWnd->m_hwnd=hwnd;
//		m.insert(std::make_pair(hwnd,pWnd));
//		return pWnd->WndProc(msg,wParam,lParam);
//	}
//	return pWnd->WndProc(msg,wParam,lParam);
//}
//
//static CBaseWnd* & CBaseWnd:: getStaticPWnd()
//{
//	static CBaseWnd* s_pWnd=NULL;
//	return s_pWnd;
//}
//
//static MYMAP& CBaseWnd:: getStaticMap()//被初始化应当是在create时候完成
//{
//	static MYMAP m;
//	return m;
//}
//static CBaseWnd* CBaseWnd:: FindInMyMap(HWND hwnd)
//{
//	MYMAP& m=getStaticMap();
//	MYMAP::iterator it=m.find(hwnd); 
//	if(it!=m.end())
//		return it->second;
//	else
//		return NULL;
//}
//virtual void CBaseWnd:: OnPaint()
//{
//	PAINTSTRUCT ps;
//	HDC hdc=BeginPaint(m_hwnd,&ps);
//	::EndPaint(m_hwnd,&ps);
//}
//
//static int& CBaseWnd:: getStaticCountWnd()
//{
//	static int i=0;
//	return i;
//}
//virtual void CBaseWnd:: OnDestroy()
//{
//	getStaticCountWnd()--;
//	if(getStaticCountWnd()==0)
//		::PostQuitMessage(0);
//
//}
//virtual LRESULT CALLBACK CBaseWnd:: WndProc(UINT message, WPARAM wParam, LPARAM lParam)
//{
//	switch(message)
//	{
//	case WM_COMMAND://  WM_COMMAND	- process the application menu
//		int wmId, wmEvent;
//		wmId    = LOWORD(wParam);
//		wmEvent = HIWORD(wParam);
//		// Parse the menu selections:
//		switch (wmId)
//		{
//		case IDM_ABOUT:
//			DialogBox(NULL, MAKEINTRESOURCE(IDD_ABOUTBOX), m_hwnd, CBaseWnd::About);
//			//为什么static的函数就好做函数指针？
//			break;
//		case IDM_EXIT:
//			DestroyWindow(m_hwnd);
//			break;
//		default:
//			return DefWindowProc(m_hwnd, message, wParam, lParam);
//		}
//		break;
//	case WM_PAINT:
//		OnPaint();
//		break;
//	case WM_DESTROY:
//		OnDestroy();
//		break;
//	default:
//		return OnDefault(message,wParam,lParam);
//	}
//	return 0;
//
//}
//
//virtual LRESULT CBaseWnd:: OnDefault(UINT message,WPARAM wParam,LPARAM lParam)
//{
//	return ::DefWindowProc(m_hwnd,message,wParam,lParam);
//}
//
//
//
//CBaseWnd::operator  HWND()
//{
//	return m_hwnd;
//}
//void  CBaseWnd::Create(HINSTANCE hInstance)
//{
//	getStaticCountWnd()++;
//	getStaticPWnd()=this;
//	m_hwnd=::CreateWindow(TEXT("cn_fengzhuang_try"),L"1" ,WS_OVERLAPPEDWINDOW,
//		CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL);
//	if(m_hwnd==NULL)
//		::OutputDebugString(L"create failed!\n");
//
//}
//void CBaseWnd::UpdateWindow()
//{
//	::UpdateWindow(m_hwnd);
//}
//void CBaseWnd::ShowWindow()
//{
//	::ShowWindow(m_hwnd,SW_SHOW);
//}
//// Message handler for about box.
//static INT_PTR CALLBACK CBaseWnd:: About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
//{
//	UNREFERENCED_PARAMETER(lParam);
//	switch (message)
//	{
//	case WM_INITDIALOG:
//		return (INT_PTR)TRUE;
//
//	case WM_COMMAND:
//		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
//		{
//			EndDialog(hDlg, LOWORD(wParam));
//			return (INT_PTR)TRUE;
//		}
//		break;
//	}
//	return (INT_PTR)FALSE;
//}