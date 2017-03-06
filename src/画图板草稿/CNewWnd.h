#include "CBaseWnd.h"

#include "CLine.h"
#include "CRect.h"
#include "CEllipse.h"
#include "CPolygon.h"
#include "CPopFile.h"
//#include <GdiPlus.h>
//#include <GdiPlusMetaFile.h> ϣ�����ͼƬ
#include <vector>
#include <commctrl.h>
#pragma comment(lib,"comctl32.lib") 

class CNewWnd:public CBaseWnd
{

public:
	void ClipMouse(bool trap)//����ס�����
	{
		RECT rcClip;//��걻Լ��������
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

	static CBaseShape::eSHAPE& eGgetGState()//ȡ�þ�̬����������ʾ��ǰ��ͼ��״̬
	{
		static CBaseShape::eSHAPE s_eGState=CBaseShape::eNull;
		return s_eGState;
	}

	static POINT& EndPLastTime()//ȡ�þ�̬��������ʾ�ϴ����λ��
	{
		static POINT sEndLast;
		return sEndLast;
	}

	static bool& fFinalSelect()//ȡ�þ�̬��������ʾͼ���Ƿ�����ѡ��
	{
		static bool f_select=false;
		return f_select;
	}

	static bool& fMoveLDown()//ȡ�þ�̬��������¼��ǰһ����Ϣ�Ƿ���up����֪POLYGON��LBUTTONUP
	{
		static bool s_fMoveLDown=false;
		return s_fMoveLDown;
	}

	static bool& fSelect()//ȡ�þ�̬�������ж��Ƿ���SELECT��
	{
		static bool f_Select=false;
		return f_Select;
	}

	static bool& fSelectAll()//ȡ�þ�̬�������ж��Ƿ���ȫѡ��
	{
		static bool f_select;
		return f_select;
	}

	void Initialize()//��ʼ������
	{
		////���ò˵���checkmenu
		HMENU hmenu=GetMenu(getMHwnd());
		CheckMenuItem(hmenu,IDM_LINE,MF_CHECKED);
		sMenuCheckState1()=IDM_LINE;
		CheckMenuItem(hmenu,ID_LINE2,MF_CHECKED);
		sMenuCheckState2()=ID_LINE2;
		CheckMenuItem(hmenu,ID_FILL_NULL,MF_CHECKED);
		sMenuCheckState3()=ID_FILL_NULL;
		CheckMenuItem(hmenu,ID_PS_SOLID,MF_CHECKED);
		sMenuCheckState4()=ID_PS_SOLID;
		
		eGgetGState()=CBaseShape::eLine;//Ĭ���ǻ���
		////״̬��
		hStatusWnd=::CreateStatusWindow(WS_CHILD|WS_VISIBLE,L"״̬��",getMHwnd(),NULL);
		int pint[5]={100,360,520,640,-1};//�ֳ�5������
		::SendMessage(hStatusWnd,SB_SETPARTS,5,(LPARAM)pint);//��3�������Ƿָ����������
		TCHAR sztemp_status[64];
		wsprintf(sztemp_status, L"״̬��:  ֱ��");
		SendMessage(hStatusWnd, SB_SETTEXT, 0, (LPARAM)(LPSTR)sztemp_status);
		wsprintf(sztemp_status, L"��ͼ:  100��");
		SendMessage(hStatusWnd, SB_SETTEXT, 4, (LPARAM)(LPSTR)sztemp_status);/**/
		/*wsprintf(sztemp_status, L"����:    ʵ��   ��� 2   ��ɫ");
		SendMessage(hStatusWnd, SB_SETTEXT, 1, (LPARAM)(LPSTR)sztemp_status);
		wsprintf(sztemp_status, L"��ˢ:    �����    ��ɫ");
		SendMessage(hStatusWnd, SB_SETTEXT, 2, (LPARAM)(LPSTR)sztemp_status);*/

		//popfile�ĳ�ʼ��
		popfile.PopFileInitialize(getMHwnd());
	}

	void m_set_brush(UINT s,long h,COLORREF cr)//���û�ˢ
	{
		m_brush_color=cr;
		m_brush_style=s;
		m_brush_hatch=h;
	}

	void m_set_pen(UINT ps,int w,COLORREF cr)//���û���
	{
		m_pen_style=ps;
		m_pen_width=w;
		m_pen_color=cr;
	}

	void OnCopyData(WPARAM wParam, LPARAM lParam)//��Ϣ����WM_COPYDATA
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
				CPolygon* ptemppolygon=(CPolygon*)(pcpData->lpData);//���������⣬������ȷ��ö���θ��������
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

	int OnDelete()//��Ϣ������������IDM_DELETE��VK_DELETE,�ݹ������
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
							//vpShape.erase(ittemp);//����óȻɾ��ԭ����ָ�룬��Ϊ����Ҫ������
						}
					}
					if(ittemp!=vpShape.end())
					{
						vpShape.erase(ittemp);
						OnDelete();//�ݹ����
					}
				}
		InvalidateRect(getMHwnd(),NULL,true);
		fFinalSelect()=false;
		OnSelect();
		return 1;
	}

	void OnDeleteAll()//��Ϣ������������IDM_DELETE��VK_DELETE,�ǵݹ�����ͣ�ȫ��ɾ��
	{
		if(fSelectAll()==true)
			if(!vpShape.empty())
			{
				vpShape.erase(vpShape.begin(),vpShape.end());
			}
	}

	virtual void OnExit()//��Ϣ������,�����˳�����Ϣ
	{
		TCHAR sztemp[512]=L"�Ƿ񱣴�� ";
		GetWindowText(getMHwnd(),szTitleName,260);
		lstrcat(sztemp,szTitleName);
		TCHAR sztemp2[]=L" ���޸�";
		lstrcat(sztemp,sztemp2);
		
		if(sNeedToSave()==true)
		{
			int answer=MessageBox(getMHwnd(),sztemp,L"��ͼ�壭��ʾ",MB_YESNOCANCEL);
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


	void OnLButtonDown(WPARAM wParam,LPARAM lParam)//��Ϣ��������WM_LBUTTONDOWN
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

	bool OnLButtonUp(WPARAM wParam,LPARAM lParam)//��Ϣ������������WM_LBUTTONUP
	{
		//��DBLCLK�Ĵ���̫�����ˡ�����
		//��Ҫ�鿴��Ϣ���У�Ȼ���ж�ʱ����������������������һʧ������
		OutputDebugStringA("up\n");
		ClipMouse(false);//�ɿ������Ĳ���
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
				
				CLine* pline=new CLine(sPoint(),EndPoint);//���ָ��ɹ��Ľ�����������
				pline->mSetPen(m_pen_style,m_pen_width,m_pen_color);
				pline->mSetHwnd(getMHwnd());
				pline->mDrawItSelf(&hdc);
				//DeleteObject(tempHPen);//ɾ����ʱ�õ�HPEN����ΪGDI�������ն�Ҫ��ɾ��
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
					SetCursor(LoadCursor(NULL,IDC_CROSS));//��Ҫ�����㡰��Ȩ��--�Ѿ����
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
					//�����ǰ������һ����MOVELDOWN������Ϣ����ô�ʹ������õ���ϢҲ׷�ӵ�tempPolygon��
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

	void OnLDBLCLK(WPARAM wParam,LPARAM lParam)//��Ϣ������,����ON_DBLCLCK
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
				CPolygon* pPolygon=new CPolygon(tempPolygon);//�������죬���˻�ͼ����ȫ�������
				vpShape.push_back(pPolygon);
				sNeedToSave()=true;
				tempPolygon.m_ClearItSelf();
				////
				////��ͬһ��sFDBL()���з���
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
				fFinalSelect()=true;//����ѡ��
			}
			else
			{
				fFinalSelect()=false;//�ٶ���һ��˫������ȡ��ѡ��
				OnSelect();          //Ȼ�����ָ̻���ԭ����״̬
				//::InvalidateRect(this->getMHwnd(),NULL,true);
			}
		}
		if(fSelectAll()==true)
		{
			fSelectAll()=false;
			TCHAR sztemp_status[64];
			wsprintf(sztemp_status, L"״̬��:  ѡ��");
			SendMessage(hStatusWnd, SB_SETTEXT, 0, (LPARAM)(LPSTR)sztemp_status);
			wsprintf(sztemp_status, L"˫��ѡ�У��ٴ�˫�������ͷ�ѡ��");
			SendMessage(hStatusWnd, SB_SETTEXT, 1, (LPARAM)(LPSTR)sztemp_status);
			OnSelect();
		}
		sFDBL()=true;
		ReleaseDC(getMHwnd(),hdc);
	}

	virtual void OnMove(LPARAM lParam)//��Ϣ������������WM_MOUSEMOVE
	{
		//״̬������ʾ����λ��
		TCHAR sztemp_status[32];
		wsprintf(sztemp_status, L"���λ��: %d, %d", LOWORD(lParam), HIWORD(lParam));
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

				////����Ƿ��˫���¼������˴������������˾Ͳ�����
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
		if(fSelect()==true)//ʹ�ñ��SELECT״̬
		{
			if(fFinalSelect()==false)
			{
				OnSelect();
			}
		}
	}

	void OnMoveLDown(WPARAM wParam,LPARAM lParam)//��Ϣ������������WM_MOUSEMOVE��MK_LBUTTON
	{
		ClipMouse(true);
		//״̬������ʾ����λ��
		TCHAR sztemp_status[32];
		wsprintf(sztemp_status, L"���λ��: %d, %d", LOWORD(lParam), HIWORD(lParam));
		SendMessage(hStatusWnd, SB_SETTEXT, 3, (LPARAM)(LPSTR)sztemp_status);
		CBaseShape::eSHAPE& eGState=eGgetGState();
		switch(eGState)
		{
		case CBaseShape::eLine:
			{
				SetCursor(LoadCursor(NULL,IDC_CROSS));
				HDC hdc=GetDC(getMHwnd());
				SetROP2(hdc,R2_NOTXORPEN);//��Ϊ����ķ�ʽ,�Ȱ��ϴλ��ġ�ͬһ����ֱ�߲�����Ȼ���ٻ����ڵ�
										  //�ܹ�ʵ�����ļ�����֤�ǡ���դ���㣨ROP2����

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
														//���Ĵ󵨵�ȥ�����ʵ����ÿ�ι���һ����ʵ��ʱ��ַ����ԭ����
														//�ͺ�ͬһ��ʵ����Ч��һ�������õ��Ļ�����ڴ�й©
				/*char sztemp[128];
				wsprintfA(sztemp,"the address of the rect_temp is %x Ox\n",&rect_temp);
				OutputDebugStringA(sztemp);*///��֤���ҵ��뷨��ÿ�ε�ֵַ��һ��
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

				////����Ƿ��˫��ʱ������˴������������˾Ͳ�����
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
		if(fSelect()==true)//ʹ�ñ��SELECT״̬
		{
			OnSelect();
			eGgetGState()=CBaseShape::eNull;
		}
	}

	void OnNew()//��Ϣ�������������½��ĵ���������word
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

	bool OnOpen()//��Ϣ�����������ļ�
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
			MessageBoxW(NULL,L"�ļ��򲻿�\n",L"��ͼ�壺��ʾ",MB_OK);
		}
		else
		{
			//����ط���Ҫ�Ľ��������е��ļ�û�б����ʱ���������ļ��Ḳ�������ļ���Ӧ������������ʾ
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

	virtual void OnPaint(WPARAM wParam,LPARAM lParam)//��Ϣ��������WM_PAINT
	{
		////û�з�װ�Ļ�ͼ����
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

	bool OnSave()//��Ϣ�������������ļ�
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

	bool OnSaveAs()//��Ϣ������������ļ�
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

	void OnSelect()//��Ϣ��������ѡ��ͼ��
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

	void OnSelectAll()//��Ϣ��������ȫѡͼ��
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

	void OnSelectColor()//��Ϣ��������ѡ����ɫ��Charles Petzold����
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

	virtual void OnSize(WPARAM wParam,LPARAM lParam)//��Ϣ��������WM_SIZE
	{
		m_Width=LOWORD(lParam);
		m_Height=HIWORD(lParam);
		if(wParam==SIZE_MAXIMIZED)
		{
			OutputDebugStringA("MAX\n");
			sTimeLast()=GetMessageTime();
			sFDBL()=true;
		}
		//�ƶ�״̬��
		MoveWindow(hStatusWnd,0,0,LOWORD(lParam),HIWORD(lParam),true);
		//���ܺ�STATUSWND������Ҳ�й�ϵ������Y��������0����20�������ڸ����ڵ��¶�
	}

	static bool& sFDBL()//ȡ�þ�̬����,��˫����Ϣ�Ƿ�õ�������м�¼
	{
		static bool s_fdbclk=false;
		return s_fdbclk;
	}

    static bool& sFDBL2()//ȡ�þ�̬����,��˫����Ϣ�Ƿ�õ�������м�¼,��ͬ���Ǽ�¼����POLYGONʱ������˫����Ϣ
	{
		static bool s_fdbclk=false;
		return s_fdbclk;
	}

	static UINT& sMenuCheckState1()//ȡ�þ�̬������checkmenu,Ϊͼ���õ�
	{
		static UINT s_menu;
		return s_menu;
	}

	static UINT& sMenuCheckState2()//ȡ�þ�̬������checkmenu,Ϊ�߿��õ�
	{
		static UINT s_menu=ID_LINE1;
		return s_menu;
	}

	static UINT& sMenuCheckState3()//ȡ�þ�̬������checkmenu,Ϊ����õ�
	{
		static UINT s_menu=ID_FILL_NULL;
		return s_menu;
	}

	static UINT& sMenuCheckState4()//ȡ�þ�̬������checkmenu,Ϊ�����õ�
	{
		static UINT s_menu=ID_PS_SOLID;
		return s_menu;
	}

	static bool& sNeedToSave()//ȡ�þ�̬�������ж��Ƿ���Ҫ����
	{
		static bool s_needSave=false;
		return s_needSave;
	}

	static int& sNotSelectedShapeCount()//ȡ�þ�̬������û�б�ѡ�е�ͼ�ε�����
	{
		static int s_NotSelected=1;
		return s_NotSelected;
	}

	static POINT& sPoint()//ȡ�þ�̬����������������
	{
		static POINT s_Point;
		return s_Point;
	}

	static int& sSelectedShapeCount()//ȡ�þ�̬�������ж�ѡ�е�ͼ�ε�����
	{
		static int s_count=0;
		return s_count;
	}

	static long& sTimeLast()//ȡ�þ�̬��������¼��һ��LBUTTONUP��ʱ��
	{
		static long s_timeLast=0;
		return s_timeLast;
	}

	static float& sZoom()//ȡ�þ�̬��������ǰ�Ŵ���
	{
		static float s_zoom=1;
		return s_zoom;
	}

	virtual LRESULT CALLBACK WndProc(UINT message, WPARAM wParam, LPARAM lParam)//���ڹ���
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
			fSelect()=false;//��ͨ�İ������Ƿ�SELECT��
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
					wsprintf(sztemp_status, L"״̬��:  ѡ��");
					SendMessage(hStatusWnd, SB_SETTEXT, 0, (LPARAM)(LPSTR)sztemp_status);
					wsprintf(sztemp_status, L"ȫѡ���ٴ�˫�������ͷ�ѡ��");
					SendMessage(hStatusWnd, SB_SETTEXT, 1, (LPARAM)(LPSTR)sztemp_status);
					OnSelectAll();
					eGgetGState()=CBaseShape::eNull;
					CheckMenuItem(hmenu,sMenuCheckState1(),MF_UNCHECKED);//��ԭ���Ĳ˵��ϵĶԹ�ȥ��
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
					wsprintf(sztemp_status, L"״̬��:  ֱ��");
					SendMessage(hStatusWnd, SB_SETTEXT, 0, (LPARAM)(LPSTR)sztemp_status);
					eGgetGState()=CBaseShape::eLine;
					CheckMenuItem(hmenu,sMenuCheckState1(),MF_UNCHECKED);//��ԭ���Ĳ˵��ϵĶԹ�ȥ��
					sMenuCheckState1()=IDM_LINE;
					CheckMenuItem(hmenu,sMenuCheckState1(),MF_CHECKED);//�����ڵĶԹ�����
					break;
				}
				
			case IDR_RECTANGLE:
				{
					TCHAR sztemp_status[64];
					wsprintf(sztemp_status, L"״̬��:  ����");
					SendMessage(hStatusWnd, SB_SETTEXT, 0, (LPARAM)(LPSTR)sztemp_status);
					eGgetGState()=CBaseShape::eRect;
					CheckMenuItem(hmenu,sMenuCheckState1(),MF_UNCHECKED);//��ԭ���Ĳ˵��ϵĶԹ�ȥ��
					sMenuCheckState1()=IDR_RECTANGLE;
					CheckMenuItem(hmenu,sMenuCheckState1(),MF_CHECKED);//�����ڵĶԹ�����
					break;
				}
			case IDM_ELLIPSE:
				{
					TCHAR sztemp_status[64];
					wsprintf(sztemp_status, L"״̬��:  ��Բ");
					SendMessage(hStatusWnd, SB_SETTEXT, 0, (LPARAM)(LPSTR)sztemp_status);
					eGgetGState()=CBaseShape::eEllipse;
					CheckMenuItem(hmenu,sMenuCheckState1(),MF_UNCHECKED);//��ԭ���Ĳ˵��ϵĶԹ�ȥ��
					sMenuCheckState1()=IDM_ELLIPSE;
					CheckMenuItem(hmenu,sMenuCheckState1(),MF_CHECKED);//�����ڵĶԹ�����
					break;
				}
			case IDM_POLYGON:
				{
					TCHAR sztemp_status[64];
					wsprintf(sztemp_status, L"״̬��:  �����");
					SendMessage(hStatusWnd, SB_SETTEXT, 0, (LPARAM)(LPSTR)sztemp_status);
					eGgetGState()=CBaseShape::ePolygon;
					CheckMenuItem(hmenu,sMenuCheckState1(),MF_UNCHECKED);//��ԭ���Ĳ˵��ϵĶԹ�ȥ��
					sMenuCheckState1()=IDM_POLYGON;
					CheckMenuItem(hmenu,sMenuCheckState1(),MF_CHECKED);//�����ڵĶԹ�����
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
					CheckMenuItem(hmenu,sMenuCheckState2(),MF_UNCHECKED);//��ԭ���Ĳ˵��ϵĶԹ�ȥ��
					sMenuCheckState2()=ID_LINE1;
					CheckMenuItem(hmenu,sMenuCheckState2(),MF_CHECKED);//�����ڵĶԹ�����
					break;
				}
			case ID_LINE2:
				{
					m_pen_width=2;
					CheckMenuItem(hmenu,sMenuCheckState2(),MF_UNCHECKED);//��ԭ���Ĳ˵��ϵĶԹ�ȥ��
					sMenuCheckState2()=ID_LINE2;
					CheckMenuItem(hmenu,sMenuCheckState2(),MF_CHECKED);//�����ڵĶԹ�����
					break;
				}
			case ID_LINE3:
				{
					m_pen_width=3;
					CheckMenuItem(hmenu,sMenuCheckState2(),MF_UNCHECKED);//��ԭ���Ĳ˵��ϵĶԹ�ȥ��
					sMenuCheckState2()=ID_LINE3;
					CheckMenuItem(hmenu,sMenuCheckState2(),MF_CHECKED);//�����ڵĶԹ�����
					break;
				}
			case ID_LINE4:
				{
					m_pen_width=4;
					CheckMenuItem(hmenu,sMenuCheckState2(),MF_UNCHECKED);//��ԭ���Ĳ˵��ϵĶԹ�ȥ��
					sMenuCheckState2()=ID_LINE4;
					CheckMenuItem(hmenu,sMenuCheckState2(),MF_CHECKED);//�����ڵĶԹ�����
					break;
				}
			case ID_LINE5:
				{
					m_pen_width=5;
					CheckMenuItem(hmenu,sMenuCheckState2(),MF_UNCHECKED);//��ԭ���Ĳ˵��ϵĶԹ�ȥ��
					sMenuCheckState2()=ID_LINE5;
					CheckMenuItem(hmenu,sMenuCheckState2(),MF_CHECKED);//�����ڵĶԹ�����
					break;
				}
			case ID_PS_SOLID:
				{
					m_pen_style=PS_SOLID;
					CheckMenuItem(hmenu,sMenuCheckState4(),MF_UNCHECKED);//��ԭ���Ĳ˵��ϵĶԹ�ȥ��
					sMenuCheckState4()=ID_PS_SOLID;
					CheckMenuItem(hmenu,sMenuCheckState4(),MF_CHECKED);//�����ڵĶԹ�����
					break;
				}
			case ID_PS_DASH:
				{
					m_pen_style=PS_DASH;
					CheckMenuItem(hmenu,sMenuCheckState4(),MF_UNCHECKED);//��ԭ���Ĳ˵��ϵĶԹ�ȥ��
					sMenuCheckState4()=ID_PS_DASH;
					CheckMenuItem(hmenu,sMenuCheckState4(),MF_CHECKED);//�����ڵĶԹ�����
					break;
				}
			case ID_PS_DOT:
				{
					m_pen_style=PS_DOT;
					CheckMenuItem(hmenu,sMenuCheckState4(),MF_UNCHECKED);//��ԭ���Ĳ˵��ϵĶԹ�ȥ��
					sMenuCheckState4()=ID_PS_DOT;
					CheckMenuItem(hmenu,sMenuCheckState4(),MF_CHECKED);//�����ڵĶԹ�����
					break;
				}
			case ID_PS_DASHDOT:
				{
					m_pen_style=PS_DASHDOT;
					CheckMenuItem(hmenu,sMenuCheckState4(),MF_UNCHECKED);//��ԭ���Ĳ˵��ϵĶԹ�ȥ��
					sMenuCheckState4()=ID_PS_DASHDOT;
					CheckMenuItem(hmenu,sMenuCheckState4(),MF_CHECKED);//�����ڵĶԹ�����
					break;
				}
			case ID_PS_DASHDOTDOT:
				{
					m_pen_style=PS_DASHDOTDOT;
					CheckMenuItem(hmenu,sMenuCheckState4(),MF_UNCHECKED);//��ԭ���Ĳ˵��ϵĶԹ�ȥ��
					sMenuCheckState4()=ID_PS_DASHDOTDOT;
					CheckMenuItem(hmenu,sMenuCheckState4(),MF_CHECKED);//�����ڵĶԹ�����
					break;
				}
			case ID_FILL_NULL:
				{
					m_brush_style=BS_NULL;
					CheckMenuItem(hmenu,sMenuCheckState3(),MF_UNCHECKED);//��ԭ���Ĳ˵��ϵĶԹ�ȥ��
					sMenuCheckState3()=ID_FILL_NULL;
					CheckMenuItem(hmenu,sMenuCheckState3(),MF_CHECKED);//�����ڵĶԹ�����
					break;
				}
			case ID_FILL_HATCH:
				{
					m_brush_style=BS_HATCHED;
					m_brush_hatch=HS_DIAGCROSS;
					CheckMenuItem(hmenu,sMenuCheckState3(),MF_UNCHECKED);//��ԭ���Ĳ˵��ϵĶԹ�ȥ��
					sMenuCheckState3()=ID_FILL_HATCH;
					CheckMenuItem(hmenu,sMenuCheckState3(),MF_CHECKED);//�����ڵĶԹ�����
					break;
				}
			case IDM_SELECT:
				{
					TCHAR sztemp_status[64];
					wsprintf(sztemp_status, L"״̬��:  ѡ��");
					SendMessage(hStatusWnd, SB_SETTEXT, 0, (LPARAM)(LPSTR)sztemp_status);
					wsprintf(sztemp_status, L"˫��ѡ�У��ٴ�˫�������ͷ�ѡ��");
					SendMessage(hStatusWnd, SB_SETTEXT, 1, (LPARAM)(LPSTR)sztemp_status);
					fSelect()=true;
					OnSelect();
					eGgetGState()=CBaseShape::eNull;
					CheckMenuItem(hmenu,sMenuCheckState1(),MF_UNCHECKED);//��ԭ���Ĳ˵��ϵĶԹ�ȥ��
					break;
				}
			case ID_ZOOMIN://�Ŵ�
				{
					sZoom()*=2;
					InvalidateRect(getMHwnd(),NULL,true);
					TCHAR sztemp_status[64];
					int iOutput=(int)(100*sZoom());
					wsprintf(sztemp_status, L"��ͼ:  %d��",iOutput);
					SendMessage(hStatusWnd, SB_SETTEXT, 4, (LPARAM)(LPSTR)sztemp_status);
					break;
				}
			case ID_ZOOMOUT://��С
				{
					sZoom()*=0.5;
					InvalidateRect(getMHwnd(),NULL,true);
					TCHAR sztemp_status[64];
					int iOutput=(int)(100*sZoom());
					wsprintf(sztemp_status, L"��ͼ:  %d��",iOutput);
					SendMessage(hStatusWnd, SB_SETTEXT, 4, (LPARAM)(LPSTR)sztemp_status);
					break;
				}
			case ID_ZOOMORIG://ԭ�ߴ�
				{
					sZoom()=1;
					InvalidateRect(getMHwnd(),NULL,true);
					TCHAR sztemp_status[64];
					int iOutput=(int)(100*sZoom());
					wsprintf(sztemp_status, L"��ͼ:  %d��",iOutput);//����������Ҫת����INT������ȷ�����
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
			break;//����Լ������û���鷳,��ΪDBCLK�����������ϢWM_LBUTTONDOWN,WM_LBUTTONUP,WM_LBUTTONDBLCLK,WM_LBUTTONUP
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

	void ZoomAndHdc(HDC *hdc)//��HDC����ZOOM�仯
	{
		if(sZoom()<=1.0002 && sZoom()>=0.9998)
		{
		}
		else if(sZoom()>1.002)
		{
			RECT rect;
			::GetClientRect(getMHwnd(),&rect);
			SetMapMode(*hdc,MM_ANISOTROPIC);
			SetWindowExtEx(*hdc,rect.right/(int)sZoom(),rect.bottom/(int)sZoom(),NULL);//�����ǷŴ�2��

			SetViewportExtEx(*hdc,rect.right,rect.bottom,NULL);
			//SetViewportOrgEx(hdc,(rect.right-rect.left)/2,(rect.bottom-rect.top)/2,NULL);
			SetViewportOrgEx(*hdc,0,0,NULL);
		}
		else
		{
			RECT rect;
			::GetClientRect(getMHwnd(),&rect);
			SetMapMode(*hdc,MM_ANISOTROPIC);
			SetWindowExtEx(*hdc,((float)rect.right)/sZoom(),((float)rect.bottom)/sZoom(),NULL);//��������С2��

			SetViewportExtEx(*hdc,rect.right,rect.bottom,NULL);
			SetViewportOrgEx(*hdc,0,0,NULL);
		}
	}

private:
	std::vector <CBaseShape*> vpShape;
	COLORREF m_pen_color;//�ߵ���ɫ
	UINT     m_pen_style; //����
	int      m_pen_width; //�ֿ�
	COLORREF m_brush_color;//��ˢ����ɫ
	UINT     m_brush_style; //��ˢ������
	LONG     m_brush_hatch;	//��ˢ�������·
	CPolygon tempPolygon;
	HWND     hStatusWnd;//״̬���ľ��
	CPopFile popfile;//���ڴ򿪱����ļ��ĶԻ���
};