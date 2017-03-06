#include "CBaseWnd.h"

#include "CLine.h"
#include "CRect.h"
#include "CEllipse.h"
#include "CPolygon.h"
#include "CPopFile.h"
//#include <GdiPlus.h>
//#include <GdiPlusMetaFile.h> 希望添加图片
#include <vector>
#include <commctrl.h>
#pragma comment(lib,"comctl32.lib") 

class CNewWnd:public CBaseWnd
{

public:
	void ClipMouse(bool trap)//“困住”鼠标
	{
		RECT rcClip;//鼠标被约束的区域
		if(trap==true)
		{
			GetWindowRect(getMHwnd(),&rcClip);
			rcClip.left=rcClip.left+4;
			rcClip.top=rcClip.top+50;
			rcClip.right=rcClip.right-4;
			rcClip.bottom=rcClip.bottom-25;
		}
		else
		{
			rcClip.left=0;
			rcClip.top=0;
			rcClip.right=1024;
			rcClip.bottom=768;
		}
		ClipCursor(&rcClip);
	}

	static CBaseShape::eSHAPE& eGgetGState()//取用静态变量，来表示当前的图形状态
	{
		static CBaseShape::eSHAPE s_eGState=CBaseShape::eNull;
		return s_eGState;
	}

	static POINT& EndPLastTime()//取用静态变量，表示上次鼠标位置
	{
		static POINT sEndLast;
		return sEndLast;
	}

	static bool& fFinalSelect()//取用静态变量，表示图形是否被最终选择
	{
		static bool f_select=false;
		return f_select;
	}

	static bool& fMoveLDown()//取用静态变量，记录此前一个消息是否是up，告知POLYGON的LBUTTONUP
	{
		static bool s_fMoveLDown=false;
		return s_fMoveLDown;
	}

	static bool& fSelect()//取用静态变量，判断是否按下SELECT键
	{
		static bool f_Select=false;
		return f_Select;
	}

	static bool& fSelectAll()//取用静态变量，判断是否按下全选键
	{
		static bool f_select;
		return f_select;
	}

	void Initialize()//初始化窗口
	{
		////设置菜单的checkmenu
		HMENU hmenu=GetMenu(getMHwnd());
		CheckMenuItem(hmenu,IDM_LINE,MF_CHECKED);
		sMenuCheckState1()=IDM_LINE;
		CheckMenuItem(hmenu,ID_LINE2,MF_CHECKED);
		sMenuCheckState2()=ID_LINE2;
		CheckMenuItem(hmenu,ID_FILL_NULL,MF_CHECKED);
		sMenuCheckState3()=ID_FILL_NULL;
		CheckMenuItem(hmenu,ID_PS_SOLID,MF_CHECKED);
		sMenuCheckState4()=ID_PS_SOLID;
		
		eGgetGState()=CBaseShape::eLine;//默认是画线
		////状态栏
		hStatusWnd=::CreateStatusWindow(WS_CHILD|WS_VISIBLE,L"状态栏",getMHwnd(),NULL);
		int pint[5]={100,360,520,640,-1};//分成5个部分
		::SendMessage(hStatusWnd,SB_SETPARTS,5,(LPARAM)pint);//第3个参数是分割的区域数量
		TCHAR sztemp_status[64];
		wsprintf(sztemp_status, L"状态栏:  直线");
		SendMessage(hStatusWnd, SB_SETTEXT, 0, (LPARAM)(LPSTR)sztemp_status);
		wsprintf(sztemp_status, L"视图:  100％");
		SendMessage(hStatusWnd, SB_SETTEXT, 4, (LPARAM)(LPSTR)sztemp_status);/**/
		/*wsprintf(sztemp_status, L"画笔:    实线   宽度 2   蓝色");
		SendMessage(hStatusWnd, SB_SETTEXT, 1, (LPARAM)(LPSTR)sztemp_status);
		wsprintf(sztemp_status, L"画刷:    无填充    蓝色");
		SendMessage(hStatusWnd, SB_SETTEXT, 2, (LPARAM)(LPSTR)sztemp_status);*/

		//popfile的初始化
		popfile.PopFileInitialize(getMHwnd());
	}

	void m_set_brush(UINT s,long h,COLORREF cr)//设置画刷
	{
		m_brush_color=cr;
		m_brush_style=s;
		m_brush_hatch=h;
	}

	void m_set_pen(UINT ps,int w,COLORREF cr)//设置画笔
	{
		m_pen_style=ps;
		m_pen_width=w;
		m_pen_color=cr;
	}

	void OnCopyData(WPARAM wParam, LPARAM lParam)//消息处理，WM_COPYDATA
	{
		HDC hdc=GetDC(getMHwnd());
		PCOPYDATASTRUCT pcpData;
		pcpData=(PCOPYDATASTRUCT)lParam; 
		switch(pcpData->dwData)
		{
		case CBaseShape::eEllipse:
			{
				CEllipse* ptemp=(CEllipse*)(pcpData->lpData);
				CEllipse* pellipse=new CEllipse(*ptemp);
				pellipse->mSetPen();
				pellipse->mSetBrush();
				vpShape.push_back(pellipse);
				sNeedToSave()=true;
				InvalidateRect(getMHwnd(),NULL,true);
				OutputDebugStringA("copydata ellipse\n");
				break;
			}
		case CBaseShape::eLine:
			{
				CLine* ptempline=(CLine*)(pcpData->lpData);
				CLine* pline=new CLine(*ptempline);
				pline->mSetPen();
				vpShape.push_back(pline);
				sNeedToSave()=true;
				InvalidateRect(getMHwnd(),NULL,true);
				OutputDebugStringA("copydata eline\n");
				break;
			}
		case CBaseShape::eRect:
			{
				CRect* ptemprect=(CRect*)(pcpData->lpData);
				CRect* prect=new CRect(*ptemprect);
				prect->mSetPen();
				prect->mSetBrush();
				vpShape.push_back(prect);
				sNeedToSave()=true;
				InvalidateRect(getMHwnd(),NULL,true);
				OutputDebugStringA("copydata eRect\n");
				break;
			}
		case CBaseShape::ePolygon:
			{
				CPolygon* ptemppolygon=(CPolygon*)(pcpData->lpData);//这里有问题，不能正确获得多边形各点的坐标
				CPolygon* ppolygon=new CPolygon(*ptemppolygon);
				ppolygon->mSetPen();
				ppolygon->mSetBrush();
				vpShape.push_back(ppolygon);
				sNeedToSave()=true;
				InvalidateRect(getMHwnd(),NULL,true);
				OutputDebugStringA("copydata ePolygon\n");
				break;
			}
		default:
			break;
		}
	}

	int OnDelete()//消息处理函数，处理IDM_DELETE和VK_DELETE,递归调用型
	{
		if(fSelect()==1)
			if(sSelectedShapeCount()>=1)
				if(!vpShape.empty())
				{
					std::vector<CBaseShape*>::iterator ittemp=vpShape.end();
					std::vector<CBaseShape*>::iterator it=vpShape.begin();
					for(;it!=vpShape.end();it++)
					{
						if(it!=vpShape.end() && ((*it)->mGetFSelected()==1))
						{
							ittemp=it;
							//vpShape.erase(ittemp);//不能贸然删除原来的指针，因为它还要做递增
						}
					}
					if(ittemp!=vpShape.end())
					{
						vpShape.erase(ittemp);
						OnDelete();//递归调用
					}
				}
		InvalidateRect(getMHwnd(),NULL,true);
		fFinalSelect()=false;
		OnSelect();
		return 1;
	}

	void OnDeleteAll()//消息处理函数，处理IDM_DELETE和VK_DELETE,非递归调用型，全部删除
	{
		if(fSelectAll()==true)
			if(!vpShape.empty())
			{
				vpShape.erase(vpShape.begin(),vpShape.end());
			}
	}

	virtual void OnExit()//消息处理函数,处理退出的消息
	{
		TCHAR sztemp[512]=L"是否保存对 ";
		GetWindowText(getMHwnd(),szTitleName,260);
		lstrcat(sztemp,szTitleName);
		TCHAR sztemp2[]=L" 的修改";
		lstrcat(sztemp,sztemp2);
		
		if(sNeedToSave()==true)
		{
			int answer=MessageBox(getMHwnd(),sztemp,L"画图板－提示",MB_YESNOCANCEL);
			if(answer==IDNO)
			{
				DestroyWindow(getMHwnd());
			}
			else if(answer==IDYES)
			{
				OnSaveAs();
			}
		}
		else
			DestroyWindow(getMHwnd());
	}


	void OnLButtonDown(WPARAM wParam,LPARAM lParam)//消息处理函数，WM_LBUTTONDOWN
	{
		POINT& StartPoint=sPoint();
		StartPoint.x=LOWORD(lParam); 
		StartPoint.y=HIWORD(lParam);
		POINT& EndPointLastTime=EndPLastTime();
		EndPointLastTime.x=StartPoint.x;
		EndPointLastTime.y=StartPoint.y;
		if(eGgetGState()==CBaseShape::eNull)
			SetCursor(LoadCursor(NULL,IDC_ARROW));
		else
		{
			if(eGgetGState()==CBaseShape::ePolygon)
			{
				tempPolygon.mPolygonBuildItSelf(StartPoint);
				sPoint()=StartPoint;
				sFDBL2()=false;
			}
			if(fSelect()==false)
				SetCursor(LoadCursor(NULL,IDC_CROSS));
		}
	}

	bool OnLButtonUp(WPARAM wParam,LPARAM lParam)//消息处理函数，处理WM_LBUTTONUP
	{
		//对DBLCLK的处理太可怕了。。。
		//先要查看消息队列，然后判断时间间隔，这样才能做到万无一失！！！
		OutputDebugStringA("up\n");
		ClipMouse(false);//松开对鼠标的捕获
		char sztemp[128];
		long TimeNow=GetMessageTime();
		long TimeDelta=TimeNow-sTimeLast();
		wsprintfA(sztemp,"the timedelta is %d milliseconds\n",TimeDelta);
		OutputDebugStringA(sztemp);
		if(TimeDelta>=400)
		{
			sFDBL()=false;
		}
		if(sFDBL()==true)
		{
			sFDBL()=false;
			return false;
		}
		//::ReleaseCapture();
		POINT EndPoint;
		EndPoint.x=LOWORD(lParam);
		EndPoint.y=HIWORD(lParam);
		HDC hdc=GetDC(getMHwnd());
		CBaseShape::eSHAPE& eGState=eGgetGState();
		switch(eGState)
			{
			case CBaseShape::eLine:
			{
				if(m_pen_color==RGB(0,0,0))
					m_pen_color=RGB(255,0,0);
				HPEN hpentemp=CreatePen(PS_SOLID,2,RGB(255,0,0));
				
				CLine* pline=new CLine(sPoint(),EndPoint);//类的指针成功的解决了这个问题
				pline->mSetPen(m_pen_style,m_pen_width,m_pen_color);
				pline->mSetHwnd(getMHwnd());
				pline->mDrawItSelf(&hdc);
				//DeleteObject(tempHPen);//删除临时用的HPEN，因为GDI对象最终都要求被删除
				vpShape.push_back(pline);
				sNeedToSave()=true;
				break;
			}
			case CBaseShape::eRect:
				{
					if(m_pen_color==RGB(0,0,0))
						m_pen_color=RGB(255,0,0);
					CRect* prect=new CRect(sPoint(),EndPoint);
					prect->mSetBrush(m_brush_style,m_brush_color,m_brush_hatch);
					prect->mSetPen(m_pen_style,m_pen_width,m_pen_color);
					prect->mSetHwnd(getMHwnd());
					vpShape.push_back(prect);
					sNeedToSave()=true;
					prect->mDrawItSelf(&hdc);

					break;
				}
			case CBaseShape::eEllipse:
				{
					if(m_pen_color==RGB(0,0,0))
						m_pen_color=RGB(255,0,0);
					CEllipse* pellipse=new CEllipse(sPoint(),EndPoint);
					pellipse->mSetBrush(m_brush_style,m_brush_color,m_brush_hatch);
					pellipse->mSetPen(m_pen_style,m_pen_width,m_pen_color);
					pellipse->mSetHwnd(getMHwnd());
					vpShape.push_back(pellipse);
					sNeedToSave()=true;
					pellipse->mDrawItSelf(&hdc);
					break;
				}
			case CBaseShape::ePolygon:
				{
					SetCursor(LoadCursor(NULL,IDC_CROSS));//不要给它搞“特权”--已经解决
					if(tempPolygon.m_pointCounts()!=0)
					{
						OutputDebugStringA("draw polygon's lines\n");
						if(m_pen_color==RGB(0,0,0))
							m_pen_color=RGB(255,0,0);
						LOGPEN lp;
						lp.lopnColor=m_pen_color;
						lp.lopnStyle=m_pen_style;
						lp.lopnWidth.x=m_pen_width;
						HPEN temphpen=CreatePenIndirect(&lp);
						SelectObject(hdc,temphpen);
						MoveToEx(hdc,sPoint().x,sPoint().y,NULL);
						LineTo(hdc,EndPoint.x,EndPoint.y);
					}
					sPoint()=EndPoint;
					//如果此前产生了一个“MOVELDOWN”的消息，那么就处理，将该点信息也追加到tempPolygon中
					if(sFDBL2()==true)
						SetCursor(LoadCursor(NULL,IDC_ARROW));
					if(fMoveLDown()==true)
					{
						tempPolygon.mPolygonBuildItSelf(EndPoint);
						fMoveLDown()=false;
					}
				}
				
				break;
			default:
				break;
		}
		ReleaseDC(getMHwnd(),hdc);
		return true;
	}

	void OnLDBLCLK(WPARAM wParam,LPARAM lParam)//消息处理函数,处理ON_DBLCLCK
	{
		OutputDebugStringA("double click\n");
		HDC hdc=GetDC(getMHwnd());
		CBaseShape::eSHAPE& eGState=eGgetGState();
		switch(eGState)
		{
		case CBaseShape::ePolygon:
			{
				if(m_pen_color==RGB(0,0,0))
					m_pen_color=RGB(255,0,0);
				tempPolygon.mSetPen(m_pen_style,m_pen_width,m_pen_color);
				tempPolygon.mSetBrush(m_brush_style,m_brush_color,m_brush_hatch);
				tempPolygon.mSetHwnd(getMHwnd());
				tempPolygon.mDrawItSelf(&hdc);
				CPolygon* pPolygon=new CPolygon(tempPolygon);//拷贝构造，至此绘图部分全部完成了
				vpShape.push_back(pPolygon);
				sNeedToSave()=true;
				tempPolygon.m_ClearItSelf();
				////
				////用同一个sFDBL()会有风险
				sFDBL2()=true;

				SetCursor(LoadCursor(NULL,IDC_ARROW));
				ClipMouse(false);
				////
				break;
			}
		default:
			break;
		}
		if(fSelect()==1)
		{
			if(fFinalSelect()==false && sSelectedShapeCount()>=1)
			{
				//OnPaint(wParam,lParam);
				InvalidateRect(getMHwnd(),NULL,true);
				fFinalSelect()=true;//最终选定
			}
			else
			{
				fFinalSelect()=false;//再多敲一次双击，就取消选中
				OnSelect();          //然后立刻恢复成原来的状态
				//::InvalidateRect(this->getMHwnd(),NULL,true);
			}
		}
		if(fSelectAll()==true)
		{
			fSelectAll()=false;
			TCHAR sztemp_status[64];
			wsprintf(sztemp_status, L"状态栏:  选择");
			SendMessage(hStatusWnd, SB_SETTEXT, 0, (LPARAM)(LPSTR)sztemp_status);
			wsprintf(sztemp_status, L"双击选中，再次双击可以释放选择");
			SendMessage(hStatusWnd, SB_SETTEXT, 1, (LPARAM)(LPSTR)sztemp_status);
			OnSelect();
		}
		sFDBL()=true;
		ReleaseDC(getMHwnd(),hdc);
	}

	virtual void OnMove(LPARAM lParam)//消息处理函数，处理WM_MOUSEMOVE
	{
		//状态栏上显示坐标位置
		TCHAR sztemp_status[32];
		wsprintf(sztemp_status, L"鼠标位置: %d, %d", LOWORD(lParam), HIWORD(lParam));
		SendMessage(hStatusWnd, SB_SETTEXT, 3, (LPARAM)(LPSTR)sztemp_status);
		CBaseShape::eSHAPE& eGState=eGgetGState();
		switch(eGState)
		{
		case CBaseShape::ePolygon:
			{
				ClipMouse(true);
				SetCursor(LoadCursor(NULL,IDC_CROSS));
				HDC hdc=GetDC(getMHwnd());
				ZoomAndHdc(&hdc);
				SetROP2(hdc,R2_NOTXORPEN);
				if(m_pen_color==RGB(0,0,0))
					m_pen_color=RGB(255,0,0);

				////检查是否对双击事件进行了处理，如果处理过了就不画了
				if(sFDBL2()==false)
				{
					if(tempPolygon.m_pointCounts()!=0)
					{

						LOGPEN lp;
						lp.lopnColor=m_pen_color;
						lp.lopnStyle=m_pen_style;
						lp.lopnWidth.x=m_pen_width;
						HPEN temphpen=CreatePenIndirect(&lp);
						SelectObject(hdc,temphpen);
						MoveToEx(hdc,sPoint().x,sPoint().y,NULL);
						LineTo(hdc,EndPLastTime().x,EndPLastTime().y);
						POINT EndPoint;
						EndPoint.x=LOWORD(lParam);
						EndPoint.y=HIWORD(lParam);
						EndPLastTime().x=EndPoint.x;
						EndPLastTime().y=EndPoint.y;
						MoveToEx(hdc,sPoint().x,sPoint().y,NULL);
						LineTo(hdc,EndPLastTime().x,EndPLastTime().y);
					}
				}
				else
				{
					SetCursor(LoadCursor(NULL,IDC_ARROW));
					ClipMouse(false);
				}
				ReleaseDC(getMHwnd(),hdc);
				break;
			}
		default:
			break;
		}
		if(fSelect()==true)//使得变成SELECT状态
		{
			if(fFinalSelect()==false)
			{
				OnSelect();
			}
		}
	}

	void OnMoveLDown(WPARAM wParam,LPARAM lParam)//消息处理函数，处理WM_MOUSEMOVE且MK_LBUTTON
	{
		ClipMouse(true);
		//状态栏上显示坐标位置
		TCHAR sztemp_status[32];
		wsprintf(sztemp_status, L"鼠标位置: %d, %d", LOWORD(lParam), HIWORD(lParam));
		SendMessage(hStatusWnd, SB_SETTEXT, 3, (LPARAM)(LPSTR)sztemp_status);
		CBaseShape::eSHAPE& eGState=eGgetGState();
		switch(eGState)
		{
		case CBaseShape::eLine:
			{
				SetCursor(LoadCursor(NULL,IDC_CROSS));
				HDC hdc=GetDC(getMHwnd());
				SetROP2(hdc,R2_NOTXORPEN);//极为巧妙的方式,先把上次画的“同一条”直线擦掉，然后再画现在的
										  //能够实现这点的技术保证是“光栅运算（ROP2）”

				if(m_pen_color==RGB(0,0,0))
					m_pen_color=RGB(255,0,0);
				HPEN hpentemp=CreatePen(PS_SOLID,2,m_pen_color);

				////
				CLine tempLine(sPoint(),EndPLastTime());
				tempLine.mSetPen(m_pen_style,m_pen_width,m_pen_color);
				tempLine.mOnlyDrawItSelf(&hdc);
				POINT EndPoint;
				EndPoint.x=LOWORD(lParam);
				EndPoint.y=HIWORD(lParam);
				EndPLastTime().x=EndPoint.x;
				EndPLastTime().y=EndPoint.y;
				tempLine.SetTo(EndPoint);
				tempLine.mOnlyDrawItSelf(&hdc);
				ReleaseDC(getMHwnd(),hdc);
				break;
			}
		case CBaseShape::eRect:
			{
				SetCursor(LoadCursor(NULL,IDC_CROSS));
				HDC hdc=GetDC(getMHwnd());
				SetROP2(hdc,R2_NOTXORPEN);

				if(m_pen_color==RGB(0,0,0))
					m_pen_color=RGB(255,0,0);
				
				CRect rect_temp(sPoint(),EndPLastTime());
														//放心大胆的去用这个实例，每次构造一个新实例时地址还是原来的
														//就和同一个实例的效果一样，不用担心会产生内存泄漏
				/*char sztemp[128];
				wsprintfA(sztemp,"the address of the rect_temp is %x Ox\n",&rect_temp);
				OutputDebugStringA(sztemp);*///验证了我的想法，每次地址值都一样
				rect_temp.mSetBrush(m_brush_style,m_brush_color,m_brush_hatch);
				rect_temp.mSetPen(m_pen_style,m_pen_width,m_pen_color);
				rect_temp.mSetHwnd(getMHwnd());
				rect_temp.mOnlyDrawItSelf(&hdc);
				POINT EndPoint;
				EndPoint.x=LOWORD(lParam);
				EndPoint.y=HIWORD(lParam);
				EndPLastTime().x=EndPoint.x;
				EndPLastTime().y=EndPoint.y;
				rect_temp.SetTo(EndPoint);
				rect_temp.mOnlyDrawItSelf(&hdc);
				ReleaseDC(getMHwnd(),hdc);
				break;
			}
		case CBaseShape::eEllipse:
			{
				SetCursor(LoadCursor(NULL,IDC_CROSS));
				HDC hdc=GetDC(getMHwnd());
				SetROP2(hdc,R2_NOTXORPEN);
				if(m_pen_color==RGB(0,0,0))
					m_pen_color=RGB(255,0,0);
				CEllipse ellipse_temp(sPoint(),EndPLastTime());
				ellipse_temp.mSetBrush(m_brush_style,m_brush_color,m_brush_hatch);
				ellipse_temp.mSetPen(m_pen_style,m_pen_width,m_pen_color);
				ellipse_temp.mSetHwnd(getMHwnd());
				ellipse_temp.mOnlyDrawItSelf(&hdc);
				POINT EndPoint;
				EndPoint.x=LOWORD(lParam);
				EndPoint.y=HIWORD(lParam);
				EndPLastTime().x=EndPoint.x;
				EndPLastTime().y=EndPoint.y;
				ellipse_temp.SetTo(EndPoint);
				ellipse_temp.mOnlyDrawItSelf(&hdc);
				ReleaseDC(getMHwnd(),hdc);
				break;
			}
		case CBaseShape::ePolygon:
			{
				SetCursor(LoadCursor(NULL,IDC_CROSS));
				HDC hdc=GetDC(getMHwnd());
				SetROP2(hdc,R2_NOTXORPEN);

				if(m_pen_color==RGB(0,0,0))
					m_pen_color=RGB(255,0,0);

				////检查是否对双击时间进行了处理，如果处理过了就不画了
				if(sFDBL2()==false)
				{
					if(tempPolygon.m_pointCounts()!=0)
					{
						LOGPEN lp;
						lp.lopnColor=m_pen_color;
						lp.lopnStyle=m_pen_style;
						lp.lopnWidth.x=m_pen_width;
						HPEN temphpen=CreatePenIndirect(&lp);
						SelectObject(hdc,temphpen);
						MoveToEx(hdc,sPoint().x,sPoint().y,NULL);
						LineTo(hdc,EndPLastTime().x,EndPLastTime().y);
						POINT EndPoint;
						EndPoint.x=LOWORD(lParam);
						EndPoint.y=HIWORD(lParam);
						EndPLastTime().x=EndPoint.x;
						EndPLastTime().y=EndPoint.y;
						MoveToEx(hdc,sPoint().x,sPoint().y,NULL);
						LineTo(hdc,EndPLastTime().x,EndPLastTime().y);
					}
				}
				else
				{
					sFDBL2()=false;
				}
				fMoveLDown()=true;
				ReleaseDC(getMHwnd(),hdc);
				break;
			}
		default:
			break;
		}
		if(fSelect()==true)//使得变成SELECT状态
		{
			OnSelect();
			eGgetGState()=CBaseShape::eNull;
		}
	}

	void OnNew()//消息处理函数，用于新建文档，类似于word
	{
		CNewWnd* pmywndnew=new CNewWnd;
		pmywndnew->MyRegisterClass(m_hinstance);
		pmywndnew->Create(m_hinstance,true);
		pmywndnew->Initialize();
		pmywndnew->m_set_pen(PS_SOLID,2,RGB(0,0,255));
		pmywndnew->m_set_brush(BS_NULL,HS_DIAGCROSS,RGB(0,0,255));
		pmywndnew->UpdateWindow();
		pmywndnew->ShowWindow();
	}

	bool OnOpen()//消息处理函数，打开文件
	{
		szFileName[0]='\0';
		::GetWindowText(getMHwnd(),szTitleName,260);
		if(!popfile.PopFileOpenDlg (getMHwnd(),szFileName, szTitleName))
			return false;
		SetWindowText(getMHwnd(),szFileName);
		//////////////////////////////////////////////////////////////////////////
		std::ifstream infile(szFileName);
		if(!infile)
		{
			MessageBoxW(NULL,L"文件打不开\n",L"画图板：提示",MB_OK);
		}
		else
		{
			//这个地方需要改进，当现有的文件没有保存的时候载入新文件会覆盖现有文件，应当出现友情提示
			vpShape.clear();
			int intshape;
			while(infile>>intshape)
			{
				CBaseShape::eSHAPE tempshape;
				tempshape=(CBaseShape::eSHAPE)intshape;
				switch(tempshape)
				{
				case CBaseShape::eEllipse:
					{
						CEllipse* pellipse=new CEllipse;
						pellipse->mOpenItSelf(infile);
						vpShape.push_back(pellipse);
						break;
					}
				case CBaseShape::eRect:
					{
						CRect* prect=new CRect;
						prect->mOpenItSelf(infile);
						vpShape.push_back(prect);
						break;
					}
				case CBaseShape::eLine:
					{
						CLine* pline=new CLine;
						pline->mOpenItSelf(infile);
						vpShape.push_back(pline);
						break;
					}
				case CBaseShape::ePolygon:
					{
						CPolygon* pPolygon=new CPolygon;
						pPolygon->mOpenItSelf(infile);
						vpShape.push_back(pPolygon);
						break;
					}
				default:
					break;
				}
			}
			infile.close();
			InvalidateRect(getMHwnd(),NULL,true);
		}
		return true;
	}

	virtual void OnPaint(WPARAM wParam,LPARAM lParam)//消息处理函数，WM_PAINT
	{
		////没有封装的画图程序
		PAINTSTRUCT ps;
		HDC hdc=BeginPaint(getMHwnd(),&ps);
		if(!vpShape.empty())
		{
			ZoomAndHdc(&hdc);
			//OutputDebugStringA("not empty\n");
			std::vector<CBaseShape*>::iterator it=vpShape.begin();
			for(;it!=vpShape.end();it++)
			{
				(*it)->mOnlyDrawItSelf(&hdc);
			}
		}
		::EndPaint(getMHwnd(),&ps);
	}

	bool OnSave()//消息处理函数，保存文件
	{
		::GetWindowText(getMHwnd(),szTitleName,260);
		lstrcpy(szFileName,szTitleName);
		if(!popfile.PopFileSaveDlg(getMHwnd(),szFileName, szTitleName))
			return false;
		SetWindowText(getMHwnd(),szFileName);
		//////////////////////////////////////////////////////////////////////////
		if(!vpShape.empty())
		{
			std::ofstream outfile(szFileName);
			//outfile<<vpShape.size()<<std::endl;
			std::vector<CBaseShape*>::iterator it=vpShape.begin();
			for(;it!=vpShape.end();it++)
			{
				(*it)->mSaveItSelf(outfile);
			}
			outfile.close();
		}
		sNeedToSave()=false;
		return true;
	}

	bool OnSaveAs()//消息处理函数，另存文件
	{
		::GetWindowText(getMHwnd(),szTitleName,260);
		lstrcpy(szFileName,szTitleName);
		if(!popfile.PopFileSaveDlg(getMHwnd(),szFileName, szTitleName))
			return false;
		SetWindowText(getMHwnd(),szFileName);
		//////////////////////////////////////////////////////////////////////////
		if(!vpShape.empty())
		{
			std::ofstream outfile(szFileName);
			//outfile<<vpShape.size()<<std::endl;
			std::vector<CBaseShape*>::iterator it=vpShape.begin();
			for(;it!=vpShape.end();it++)
			{
				(*it)->mSaveItSelf(outfile);
			}
			outfile.close();
		}
		sNeedToSave()=false;
		return true;
	}

	void OnSelect()//消息处理函数，选择图形
	{
		if(fSelectAll()==false)
		{
			HDC hdc=GetDC(getMHwnd());
			POINT cursorPosition;
			GetCursorPos(&cursorPosition);
			ScreenToClient(getMHwnd(),&cursorPosition);
			if(!vpShape.empty())
			{
				sSelectedShapeCount()=0;
				std::vector<CBaseShape*>::iterator it=vpShape.begin();
				double tempdouble=1;
				for(;it!=vpShape.end();it++)
				{
					if(tempdouble>((*it)->m_CalCulateDistance(&hdc,cursorPosition)))
						tempdouble=(*it)->m_CalCulateDistance(&hdc,cursorPosition);
				}
				for(it=vpShape.begin();it!=vpShape.end();it++)
				{
					(*it)->mJudgeItSelf(&hdc,cursorPosition,tempdouble);
					if((*it)->mGetFSelected()==1)
						sSelectedShapeCount()++;
				}
			}
			ReleaseDC(getMHwnd(),hdc);
		}
	}

	void OnSelectAll()//消息处理函数，全选图形
	{
		if(!vpShape.empty())
		{
			sSelectedShapeCount()=0;
			std::vector<CBaseShape*>::iterator it=vpShape.begin();
			for(;it!=vpShape.end();it++)
			{
				(*it)->SetFSelected(1);
				sSelectedShapeCount()++;
			}
		}
		fSelect()=true;
		fSelectAll()=true;
		InvalidateRect(getMHwnd(),NULL,true);
	}

	void OnSelectColor()//消息处理函数，选择颜色，Charles Petzold代码
	{
		static CHOOSECOLOR cc;
		static COLORREF crCustColors[16];

		cc.lStructSize=sizeof(CHOOSECOLOR);
		cc.hwndOwner=getMHwnd();
		cc.hInstance=NULL;
		cc.rgbResult=RGB(0,0,0);
		cc.lpCustColors=crCustColors;
		cc.Flags=CC_RGBINIT|CC_FULLOPEN;
		cc.lCustData=0;
		cc.lpfnHook=NULL;
		cc.lpTemplateName=NULL;
		if(ChooseColor(&cc))
		{
			m_brush_color=m_pen_color=cc.rgbResult;
		}
	}

	virtual void OnSize(WPARAM wParam,LPARAM lParam)//消息处理函数，WM_SIZE
	{
		m_Width=LOWORD(lParam);
		m_Height=HIWORD(lParam);
		if(wParam==SIZE_MAXIMIZED)
		{
			OutputDebugStringA("MAX\n");
			sTimeLast()=GetMessageTime();
			sFDBL()=true;
		}
		//移动状态栏
		MoveWindow(hStatusWnd,0,0,LOWORD(lParam),HIWORD(lParam),true);
		//可能和STATUSWND的属性也有关系，不管Y的坐标是0还是20，都是在父窗口的下端
	}

	static bool& sFDBL()//取用静态变量,对双击消息是否得到处理进行记录
	{
		static bool s_fdbclk=false;
		return s_fdbclk;
	}

    static bool& sFDBL2()//取用静态变量,对双击消息是否得到处理进行记录,不同的是记录的是POLYGON时发生的双击消息
	{
		static bool s_fdbclk=false;
		return s_fdbclk;
	}

	static UINT& sMenuCheckState1()//取用静态变量，checkmenu,为图形用的
	{
		static UINT s_menu;
		return s_menu;
	}

	static UINT& sMenuCheckState2()//取用静态变量，checkmenu,为线宽用的
	{
		static UINT s_menu=ID_LINE1;
		return s_menu;
	}

	static UINT& sMenuCheckState3()//取用静态变量，checkmenu,为填充用的
	{
		static UINT s_menu=ID_FILL_NULL;
		return s_menu;
	}

	static UINT& sMenuCheckState4()//取用静态变量，checkmenu,为线型用的
	{
		static UINT s_menu=ID_PS_SOLID;
		return s_menu;
	}

	static bool& sNeedToSave()//取用静态变量，判断是否需要保存
	{
		static bool s_needSave=false;
		return s_needSave;
	}

	static int& sNotSelectedShapeCount()//取用静态变量，没有被选中的图形的数量
	{
		static int s_NotSelected=1;
		return s_NotSelected;
	}

	static POINT& sPoint()//取用静态变量，保存点的坐标
	{
		static POINT s_Point;
		return s_Point;
	}

	static int& sSelectedShapeCount()//取用静态变量，判断选中的图形的数量
	{
		static int s_count=0;
		return s_count;
	}

	static long& sTimeLast()//取用静态变量，记录上一次LBUTTONUP的时间
	{
		static long s_timeLast=0;
		return s_timeLast;
	}

	static float& sZoom()//取用静态变量，当前放大倍数
	{
		static float s_zoom=1;
		return s_zoom;
	}

	virtual LRESULT CALLBACK WndProc(UINT message, WPARAM wParam, LPARAM lParam)//窗口过程
	{
		HMENU hmenu=GetMenu(getMHwnd());
		switch(message)
		{
		case WM_SIZE:
			OnSize(wParam,lParam);
			break;
		case WM_CLOSE:
			{
				OnExit();
				break;
			}
		case WM_COMMAND://  WM_COMMAND	- process the application menu
			int wmId, wmEvent;
			wmId    = LOWORD(wParam);
			wmEvent = HIWORD(wParam);
			fSelect()=false;//普通的按键都是非SELECT的
			// Parse the menu selections:
			switch (wmId)
			{
			case IDM_NEW:
				{
					OnNew();
					break;
				}
			case ID_SELECT_ALL:
				{
					TCHAR sztemp_status[64];
					wsprintf(sztemp_status, L"状态栏:  选择");
					SendMessage(hStatusWnd, SB_SETTEXT, 0, (LPARAM)(LPSTR)sztemp_status);
					wsprintf(sztemp_status, L"全选，再次双击可以释放选择");
					SendMessage(hStatusWnd, SB_SETTEXT, 1, (LPARAM)(LPSTR)sztemp_status);
					OnSelectAll();
					eGgetGState()=CBaseShape::eNull;
					CheckMenuItem(hmenu,sMenuCheckState1(),MF_UNCHECKED);//把原来的菜单上的对勾去掉
					break;
				}
			case ID_DELETE:
				{
					fSelect()=true;
					OnDelete();
					OnDeleteAll();
					break;
				}
			case IDM_ABOUT:
				
				DialogBox(NULL, MAKEINTRESOURCE(IDD_ABOUTBOX), getMHwnd(), CBaseWnd::About);
				break;
			case IDM_EXIT:
				OnExit();
				break;
			case IDM_LINE:
				{
					TCHAR sztemp_status[64];
					wsprintf(sztemp_status, L"状态栏:  直线");
					SendMessage(hStatusWnd, SB_SETTEXT, 0, (LPARAM)(LPSTR)sztemp_status);
					eGgetGState()=CBaseShape::eLine;
					CheckMenuItem(hmenu,sMenuCheckState1(),MF_UNCHECKED);//把原来的菜单上的对勾去掉
					sMenuCheckState1()=IDM_LINE;
					CheckMenuItem(hmenu,sMenuCheckState1(),MF_CHECKED);//把现在的对勾加上
					break;
				}
				
			case IDR_RECTANGLE:
				{
					TCHAR sztemp_status[64];
					wsprintf(sztemp_status, L"状态栏:  矩形");
					SendMessage(hStatusWnd, SB_SETTEXT, 0, (LPARAM)(LPSTR)sztemp_status);
					eGgetGState()=CBaseShape::eRect;
					CheckMenuItem(hmenu,sMenuCheckState1(),MF_UNCHECKED);//把原来的菜单上的对勾去掉
					sMenuCheckState1()=IDR_RECTANGLE;
					CheckMenuItem(hmenu,sMenuCheckState1(),MF_CHECKED);//把现在的对勾加上
					break;
				}
			case IDM_ELLIPSE:
				{
					TCHAR sztemp_status[64];
					wsprintf(sztemp_status, L"状态栏:  椭圆");
					SendMessage(hStatusWnd, SB_SETTEXT, 0, (LPARAM)(LPSTR)sztemp_status);
					eGgetGState()=CBaseShape::eEllipse;
					CheckMenuItem(hmenu,sMenuCheckState1(),MF_UNCHECKED);//把原来的菜单上的对勾去掉
					sMenuCheckState1()=IDM_ELLIPSE;
					CheckMenuItem(hmenu,sMenuCheckState1(),MF_CHECKED);//把现在的对勾加上
					break;
				}
			case IDM_POLYGON:
				{
					TCHAR sztemp_status[64];
					wsprintf(sztemp_status, L"状态栏:  多边形");
					SendMessage(hStatusWnd, SB_SETTEXT, 0, (LPARAM)(LPSTR)sztemp_status);
					eGgetGState()=CBaseShape::ePolygon;
					CheckMenuItem(hmenu,sMenuCheckState1(),MF_UNCHECKED);//把原来的菜单上的对勾去掉
					sMenuCheckState1()=IDM_POLYGON;
					CheckMenuItem(hmenu,sMenuCheckState1(),MF_CHECKED);//把现在的对勾加上
					break;
				}
			case IDM_SELECTCOLOR:
				OnSelectColor();
				break;
			case IDM_SAVE:
				OnSave();
				break;
			case IDM_SAVEAS:
				OnSaveAs();
				break;
			case IDM_OPEN:
				OnOpen();
				break;
			case ID_LINE1:
				{
					m_pen_width=1;
					CheckMenuItem(hmenu,sMenuCheckState2(),MF_UNCHECKED);//把原来的菜单上的对勾去掉
					sMenuCheckState2()=ID_LINE1;
					CheckMenuItem(hmenu,sMenuCheckState2(),MF_CHECKED);//把现在的对勾加上
					break;
				}
			case ID_LINE2:
				{
					m_pen_width=2;
					CheckMenuItem(hmenu,sMenuCheckState2(),MF_UNCHECKED);//把原来的菜单上的对勾去掉
					sMenuCheckState2()=ID_LINE2;
					CheckMenuItem(hmenu,sMenuCheckState2(),MF_CHECKED);//把现在的对勾加上
					break;
				}
			case ID_LINE3:
				{
					m_pen_width=3;
					CheckMenuItem(hmenu,sMenuCheckState2(),MF_UNCHECKED);//把原来的菜单上的对勾去掉
					sMenuCheckState2()=ID_LINE3;
					CheckMenuItem(hmenu,sMenuCheckState2(),MF_CHECKED);//把现在的对勾加上
					break;
				}
			case ID_LINE4:
				{
					m_pen_width=4;
					CheckMenuItem(hmenu,sMenuCheckState2(),MF_UNCHECKED);//把原来的菜单上的对勾去掉
					sMenuCheckState2()=ID_LINE4;
					CheckMenuItem(hmenu,sMenuCheckState2(),MF_CHECKED);//把现在的对勾加上
					break;
				}
			case ID_LINE5:
				{
					m_pen_width=5;
					CheckMenuItem(hmenu,sMenuCheckState2(),MF_UNCHECKED);//把原来的菜单上的对勾去掉
					sMenuCheckState2()=ID_LINE5;
					CheckMenuItem(hmenu,sMenuCheckState2(),MF_CHECKED);//把现在的对勾加上
					break;
				}
			case ID_PS_SOLID:
				{
					m_pen_style=PS_SOLID;
					CheckMenuItem(hmenu,sMenuCheckState4(),MF_UNCHECKED);//把原来的菜单上的对勾去掉
					sMenuCheckState4()=ID_PS_SOLID;
					CheckMenuItem(hmenu,sMenuCheckState4(),MF_CHECKED);//把现在的对勾加上
					break;
				}
			case ID_PS_DASH:
				{
					m_pen_style=PS_DASH;
					CheckMenuItem(hmenu,sMenuCheckState4(),MF_UNCHECKED);//把原来的菜单上的对勾去掉
					sMenuCheckState4()=ID_PS_DASH;
					CheckMenuItem(hmenu,sMenuCheckState4(),MF_CHECKED);//把现在的对勾加上
					break;
				}
			case ID_PS_DOT:
				{
					m_pen_style=PS_DOT;
					CheckMenuItem(hmenu,sMenuCheckState4(),MF_UNCHECKED);//把原来的菜单上的对勾去掉
					sMenuCheckState4()=ID_PS_DOT;
					CheckMenuItem(hmenu,sMenuCheckState4(),MF_CHECKED);//把现在的对勾加上
					break;
				}
			case ID_PS_DASHDOT:
				{
					m_pen_style=PS_DASHDOT;
					CheckMenuItem(hmenu,sMenuCheckState4(),MF_UNCHECKED);//把原来的菜单上的对勾去掉
					sMenuCheckState4()=ID_PS_DASHDOT;
					CheckMenuItem(hmenu,sMenuCheckState4(),MF_CHECKED);//把现在的对勾加上
					break;
				}
			case ID_PS_DASHDOTDOT:
				{
					m_pen_style=PS_DASHDOTDOT;
					CheckMenuItem(hmenu,sMenuCheckState4(),MF_UNCHECKED);//把原来的菜单上的对勾去掉
					sMenuCheckState4()=ID_PS_DASHDOTDOT;
					CheckMenuItem(hmenu,sMenuCheckState4(),MF_CHECKED);//把现在的对勾加上
					break;
				}
			case ID_FILL_NULL:
				{
					m_brush_style=BS_NULL;
					CheckMenuItem(hmenu,sMenuCheckState3(),MF_UNCHECKED);//把原来的菜单上的对勾去掉
					sMenuCheckState3()=ID_FILL_NULL;
					CheckMenuItem(hmenu,sMenuCheckState3(),MF_CHECKED);//把现在的对勾加上
					break;
				}
			case ID_FILL_HATCH:
				{
					m_brush_style=BS_HATCHED;
					m_brush_hatch=HS_DIAGCROSS;
					CheckMenuItem(hmenu,sMenuCheckState3(),MF_UNCHECKED);//把原来的菜单上的对勾去掉
					sMenuCheckState3()=ID_FILL_HATCH;
					CheckMenuItem(hmenu,sMenuCheckState3(),MF_CHECKED);//把现在的对勾加上
					break;
				}
			case IDM_SELECT:
				{
					TCHAR sztemp_status[64];
					wsprintf(sztemp_status, L"状态栏:  选择");
					SendMessage(hStatusWnd, SB_SETTEXT, 0, (LPARAM)(LPSTR)sztemp_status);
					wsprintf(sztemp_status, L"双击选中，再次双击可以释放选择");
					SendMessage(hStatusWnd, SB_SETTEXT, 1, (LPARAM)(LPSTR)sztemp_status);
					fSelect()=true;
					OnSelect();
					eGgetGState()=CBaseShape::eNull;
					CheckMenuItem(hmenu,sMenuCheckState1(),MF_UNCHECKED);//把原来的菜单上的对勾去掉
					break;
				}
			case ID_ZOOMIN://放大
				{
					sZoom()*=2;
					InvalidateRect(getMHwnd(),NULL,true);
					TCHAR sztemp_status[64];
					int iOutput=(int)(100*sZoom());
					wsprintf(sztemp_status, L"视图:  %d％",iOutput);
					SendMessage(hStatusWnd, SB_SETTEXT, 4, (LPARAM)(LPSTR)sztemp_status);
					break;
				}
			case ID_ZOOMOUT://缩小
				{
					sZoom()*=0.5;
					InvalidateRect(getMHwnd(),NULL,true);
					TCHAR sztemp_status[64];
					int iOutput=(int)(100*sZoom());
					wsprintf(sztemp_status, L"视图:  %d％",iOutput);
					SendMessage(hStatusWnd, SB_SETTEXT, 4, (LPARAM)(LPSTR)sztemp_status);
					break;
				}
			case ID_ZOOMORIG://原尺寸
				{
					sZoom()=1;
					InvalidateRect(getMHwnd(),NULL,true);
					TCHAR sztemp_status[64];
					int iOutput=(int)(100*sZoom());
					wsprintf(sztemp_status, L"视图:  %d％",iOutput);//浮点数必须要转换成INT才能正确输出？
					SendMessage(hStatusWnd, SB_SETTEXT, 4, (LPARAM)(LPSTR)sztemp_status);
					break;
				}
			default:
				return DefWindowProc(getMHwnd(), message, wParam, lParam);
			}
			break;
		case WM_KEYDOWN:
			if(wParam==VK_DELETE)
			{
				OnDelete();
				OnDeleteAll();
				break;
			}
		case WM_PAINT:
			OnPaint(wParam,lParam);
			break;
		case WM_LBUTTONDOWN:
			OnLButtonDown(wParam,lParam);
			break;
		case WM_LBUTTONUP:
			OnLButtonUp(wParam,lParam);
			break;
		case WM_LBUTTONDBLCLK:
			OnLDBLCLK(wParam,lParam);
			break;//如果自己处理不好会很麻烦,因为DBCLK会产生如下信息WM_LBUTTONDOWN,WM_LBUTTONUP,WM_LBUTTONDBLCLK,WM_LBUTTONUP
		case WM_MOUSEMOVE:
			if(wParam==MK_LBUTTON)
			{
				OnMoveLDown(wParam,lParam);
			}
			else
				OnMove(lParam);
			break;
		case WM_COPYDATA:
			{
				OnCopyData(wParam,lParam);
				break;
			}
		case WM_DESTROY:
			OnDestroy();
			break;
		default:
			return OnDefault(message,wParam,lParam);
		}
		return 0;
	}

	void ZoomAndHdc(HDC *hdc)//让HDC随着ZOOM变化
	{
		if(sZoom()<=1.0002 && sZoom()>=0.9998)
		{
		}
		else if(sZoom()>1.002)
		{
			RECT rect;
			::GetClientRect(getMHwnd(),&rect);
			SetMapMode(*hdc,MM_ANISOTROPIC);
			SetWindowExtEx(*hdc,rect.right/(int)sZoom(),rect.bottom/(int)sZoom(),NULL);//这样是放大2倍

			SetViewportExtEx(*hdc,rect.right,rect.bottom,NULL);
			//SetViewportOrgEx(hdc,(rect.right-rect.left)/2,(rect.bottom-rect.top)/2,NULL);
			SetViewportOrgEx(*hdc,0,0,NULL);
		}
		else
		{
			RECT rect;
			::GetClientRect(getMHwnd(),&rect);
			SetMapMode(*hdc,MM_ANISOTROPIC);
			SetWindowExtEx(*hdc,((float)rect.right)/sZoom(),((float)rect.bottom)/sZoom(),NULL);//这样是缩小2倍

			SetViewportExtEx(*hdc,rect.right,rect.bottom,NULL);
			SetViewportOrgEx(*hdc,0,0,NULL);
		}
	}

private:
	std::vector <CBaseShape*> vpShape;
	COLORREF m_pen_color;//线的颜色
	UINT     m_pen_style; //线型
	int      m_pen_width; //现宽
	COLORREF m_brush_color;//画刷的颜色
	UINT     m_brush_style; //画刷的类型
	LONG     m_brush_hatch;	//画刷的填充纹路
	CPolygon tempPolygon;
	HWND     hStatusWnd;//状态栏的句柄
	CPopFile popfile;//用于打开保存文件的对话框
};