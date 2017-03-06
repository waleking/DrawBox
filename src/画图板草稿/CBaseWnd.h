#include <windows.h>
#include <map>
#include "resource.h"

class CBaseWnd
{
public:

	ATOM MyRegisterClass(HINSTANCE hInstance)
	{
		static TCHAR szWindowClass[]=TEXT("cn_fengzhuang_try");

		WNDCLASSEX wcex;

		wcex.cbSize = sizeof(WNDCLASSEX);

		wcex.style			= CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
		wcex.lpfnWndProc	= CBaseWnd::staticWndProc;
		wcex.cbClsExtra		= 0;
		wcex.cbWndExtra		= 0;
		wcex.hInstance		= hInstance;
		wcex.hIcon			= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON1));
		wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
		wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
		wcex.lpszMenuName	= MAKEINTRESOURCE(IDC_CN_FENGZHUANG_HENHAO);
		wcex.lpszClassName	= szWindowClass;
		wcex.hIconSm		= LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_ICONSMALL));

		m_hinstance=hInstance;
		return RegisterClassEx(&wcex);
	}

	typedef std::map<HWND,CBaseWnd*> MYMAP;//std作用域很重要

	static LRESULT CALLBACK staticWndProc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam)
	{
		CBaseWnd* pWnd;
		pWnd=FindInMyMap(hwnd);
		if(pWnd==NULL)
		{
			MYMAP& m=getStaticMap();
			//m.insert(std::make_pair(hwnd,this));不应当这样写，原因是static 函数没有 this 指针
			pWnd=getStaticPWnd();
			pWnd->m_hwnd=hwnd;
			m.insert(std::make_pair(hwnd,pWnd));
			return pWnd->WndProc(msg,wParam,lParam);
		}
		return pWnd->WndProc(msg,wParam,lParam);
	}

	HWND getMHwnd()
	{
		return m_hwnd;
	}

	static CBaseWnd* & getStaticPWnd()
	{
		static CBaseWnd* s_pWnd=NULL;
		return s_pWnd;
	}

	static MYMAP& getStaticMap()//被初始化应当是在create时候完成
	{
		static MYMAP m;
		return m;
	}
	static CBaseWnd* FindInMyMap(HWND hwnd)
	{
		MYMAP& m=getStaticMap();
		MYMAP::iterator it=m.find(hwnd); 
		if(it!=m.end())
			return it->second;
		else
			return NULL;
	}
	virtual void OnPaint(WPARAM wParam,LPARAM lParam)
	{
		PAINTSTRUCT ps;
		HDC hdc=BeginPaint(m_hwnd,&ps);
		::EndPaint(m_hwnd,&ps);
	}

	static int& getStaticCountWnd()
	{
		static int i=0;
		return i;
	}
	virtual void OnDestroy()
	{
		getStaticCountWnd()--;
		if(getStaticCountWnd()==0)
			::PostQuitMessage(0);
	}
	virtual void OnSize(WPARAM wParam,LPARAM lParam)
	{
		m_Width=LOWORD(lParam);
		m_Height=HIWORD(lParam);
		if(wParam==SIZE_MAXIMIZED)
		{
			OutputDebugStringA("MAX\n");
		}
	}
	virtual LRESULT CALLBACK WndProc(UINT message, WPARAM wParam, LPARAM lParam)
	{
		switch(message)
		{
		case WM_SIZE:
			{
				OnSize(wParam,lParam);
				break;
			}
		case WM_COMMAND://  WM_COMMAND	- process the application menu
			int wmId, wmEvent;
			wmId    = LOWORD(wParam);
			wmEvent = HIWORD(wParam);
			// Parse the menu selections:
			switch (wmId)
			{
			case IDM_ABOUT:
				DialogBox(NULL, MAKEINTRESOURCE(IDD_ABOUTBOX), m_hwnd, CBaseWnd::About);
				//为什么static的函数就好做函数指针？
				break;
			case IDM_EXIT:
				DestroyWindow(m_hwnd);
				break;
				break;
			default:
				return DefWindowProc(m_hwnd, message, wParam, lParam);
			}
			break;
		case WM_PAINT:
			OnPaint(wParam,lParam);
			break;
		case WM_DESTROY:
			OnDestroy();
			break;
		default:
			return OnDefault(message,wParam,lParam);
		}
		return 0;

	}

	virtual LRESULT OnDefault(UINT message,WPARAM wParam,LPARAM lParam)
	{
		return ::DefWindowProc(m_hwnd,message,wParam,lParam);
	}

	operator HWND()
	{
		return m_hwnd;
	}
	void Create(HINSTANCE hInstance,bool fOnNewWnd=false)
	{
		////
		getStaticCountWnd()++;
		getStaticPWnd()=this;
		TCHAR* windowNum=new TCHAR[3];
		if(getStaticCountWnd()<=9)
		{
			*(windowNum)=getStaticCountWnd()+48;//加48是要转化为char型或者TCHAR型
			*(windowNum+1)=0;
			*(windowNum+2)=0;
		}
		else if(getStaticCountWnd()<=99)
		{
			*(windowNum)=(getStaticCountWnd()-getStaticCountWnd()/10)/10+48;//得到高位
			*(windowNum+1)=(getStaticCountWnd()/10)+48;//得到低位
			*(windowNum+2)=0;
		}
		TCHAR windowName[64];
		for(int j=0;j<64;j++)
		{
			windowName[j]=0;
		}
		if(fOnNewWnd==false)
		{
			windowName[0]=TEXT('画');
			windowName[1]=TEXT('图');
			windowName[2]=TEXT('板');
			windowName[3]=TEXT('文');
			windowName[4]=TEXT('档');
		}
		else
		{
			windowName[0]=TEXT('新');
			windowName[1]=TEXT('建');
			windowName[2]=TEXT('画');
			windowName[3]=TEXT('图');
			windowName[4]=TEXT('板');
			windowName[5]=TEXT('文');
			windowName[6]=TEXT('档');
		}
		lstrcat(windowName,windowNum);


		for(int index=1;index<=98;index++)
		{
			if(::FindWindow(TEXT("cn_fengzhuang_try"),windowName)!=NULL)
			{
				if(getStaticCountWnd()+index<=9)
				{
					*(windowNum)=getStaticCountWnd()+48+index;//加48是要转化为char型或者TCHAR型
					*(windowNum+1)=0;
					*(windowNum+2)=0;
				}
				else if(getStaticCountWnd()+index<=99)
				{
					*(windowNum)=(getStaticCountWnd()+index)/10+48;//得到高位
					*(windowNum+1)=((getStaticCountWnd()+index)%10)+48;//得到低位
					*(windowNum+2)=0;
				}
				for(int j=0;j<64;j++)
				{
					windowName[j]=0;
				}
				if(fOnNewWnd==false)
				{
					windowName[0]=TEXT('画');
					windowName[1]=TEXT('图');
					windowName[2]=TEXT('板');
					windowName[3]=TEXT('文');
					windowName[4]=TEXT('档');
				}
				else
				{
					windowName[0]=TEXT('新');
					windowName[1]=TEXT('建');
					windowName[2]=TEXT('画');
					windowName[3]=TEXT('图');
					windowName[4]=TEXT('板');
					windowName[5]=TEXT('文');
					windowName[6]=TEXT('档');
				}
				lstrcat(windowName,windowNum);
			}
			else
				break;
		}

		m_hwnd=::CreateWindow(TEXT("cn_fengzhuang_try"),windowName,WS_OVERLAPPEDWINDOW,
			CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL);
		if(m_hwnd==NULL)
			::OutputDebugString(L"create failed!\n");
		
	}
	void UpdateWindow()
	{
		::UpdateWindow(m_hwnd);
	}
	void ShowWindow()
	{
		::ShowWindow(m_hwnd,SW_SHOW);
	}
	// Message handler for about box.
	static INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
	{
		UNREFERENCED_PARAMETER(lParam);
		switch (message)
		{
		case WM_INITDIALOG:
			return (INT_PTR)TRUE;

		case WM_COMMAND:
			if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
			{
				EndDialog(hDlg, LOWORD(wParam));
				return (INT_PTR)TRUE;
			}
			break;
		}
		return (INT_PTR)FALSE;
	}


protected:
	HINSTANCE m_hinstance;
	HWND m_hwnd;
	int m_Width;
	int m_Height;
	TCHAR    szTitleName[260];//标题栏
	TCHAR    szFileName[260];//文件名
};