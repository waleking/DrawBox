#include <fstream>
#include <math.h>
#include <vector>
class CBaseShape
{
public:
	enum eSHAPE{eNull,eLine,eRect,eEllipse,ePolygon};//表明图形的状态
	CBaseShape()//构造函数
	{
		from.x=0;
		from.y=0;
		to.x=0;
		to.y=0;
		shape=eNull;
		fShapeSelected=0;
	};
	
	virtual void SetFSelected(int b)//表明是否选中
	{
		fShapeSelected=b;
	}

	virtual double m_CalCulateDistance(HDC * hdc,POINT pt)//计算点到图形的距离
	{
		return 0;
	}

	virtual void SetFrom(POINT p)//设置FROM这个点
	{
		from.x=p.x;
		from.y=p.y;
	}
	virtual	POINT Get_From()//得到FROM这个点
	{
		return from;
	}
	virtual POINT Get_To()//得到TO这个点
	{
		return to;
	}
	virtual void SetTo(POINT p)//设置TO这个点
	{
		to.x=p.x;
		to.y=p.y;
	}
	virtual void SetShape(eSHAPE eshape)//得到图形的状态
	{
		shape=eshape;
	}
	virtual eSHAPE GetShap()//设置画笔各种属性
	{
		return shape;
	}

	virtual void mSetPen(UINT pStyle,int pWidth,COLORREF pColor)//设置画笔各种属性
	{
		LOGPEN lp;
		POINT pt;
		penWidth=pt.x=pWidth;
		lp.lopnColor=penColor=pColor;
		lp.lopnStyle=penStyle=pStyle;
		lp.lopnWidth=pt;
		hPen=CreatePenIndirect(&lp);
	}

	virtual void mSetPen()//设置画笔的重载函数
	{
		LOGPEN lp;
		lp.lopnColor=penColor;
		lp.lopnStyle=penStyle;
		lp.lopnWidth.x=penWidth;
		hPen=CreatePenIndirect(&lp);
	}

	virtual void mSetBrush(UINT bStyle,COLORREF bColor,LONG bHatch)//设置画刷各种属性
	{
		LOGBRUSH lb;
		brushColor=lb.lbColor=bColor;
		brushHatch=lb.lbHatch=bHatch;
		brushStyle=lb.lbStyle=bStyle;
		hBrush=CreateBrushIndirect(&lb);
	}

	virtual void mSetBrush()//设置画刷的重载函数
	{
		LOGBRUSH lb;
		lb.lbColor=brushColor;
		lb.lbHatch=brushHatch;
		lb.lbStyle=brushStyle;
		hBrush=CreateBrushIndirect(&lb);
	}

	virtual void mSetHwnd(HWND hwnd)//获得相关联的窗口句柄
	{
		m_shape_to_hwnd=hwnd;
	}

	virtual void mDrawItSelf(HDC* hdc)//绘制，并且发送消息
	{
		OutputDebugStringA("base shape draw itself!\n");
	};

	virtual void mSaveItSelf(std::ofstream& outfile)//保存
	{
		if(!outfile)
		{
			OutputDebugStringA("文件不存在\n");
		}
		else
		{
			outfile<<"success\n";
		}
		OutputDebugStringA("base shape save itself\n");
	}

	virtual void mOpenItSelf(std::ifstream& infile)//打开
	{

	}

	virtual void mJudgeItSelf(HDC*,POINT,double)//判断是否该被选中
	{
		OutputDebugStringA("it is selected!\n");
	}

	virtual int mGetFSelected()//判断是否选中
	{
		return fShapeSelected;
	}
	virtual void mOnlyDrawItSelf(HDC* hdc)//绘制，不发送消息
	{
	}

	void ErrorExit() //返回错误编号和具体信息的代码
	{ 
		TCHAR szBuf[80]; 
		LPVOID lpMsgBuf;
		DWORD dw = GetLastError(); 

		FormatMessage(
			FORMAT_MESSAGE_ALLOCATE_BUFFER | 
			FORMAT_MESSAGE_FROM_SYSTEM,
			NULL,
			dw,
			MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
			(LPTSTR) &lpMsgBuf,
			0, NULL );

		wsprintf(szBuf, 
			L"failed with error %d: %s", 
			dw, lpMsgBuf); 

		MessageBox(NULL, szBuf, L"Error", MB_OK); 

		LocalFree(lpMsgBuf);
		ExitProcess(dw); 
	}

protected:
	POINT from;
	POINT to;
	eSHAPE shape;
	HBRUSH hBrush;//储存填充区域的信息
	HPEN hPen;    //储存边框的信息,实际是个句柄
	UINT     penStyle; //线型
	int    penWidth; //现宽
	COLORREF penColor; //线的颜色
	UINT     brushStyle; //画刷的类型
	COLORREF brushColor;	//画刷的颜色
	LONG     brushHatch;	//画刷的填充纹路
	int     fShapeSelected;//图形是否选中,如果是0那么就可以检查，如果是－1，说明已经“有人抢先”，如果是1说明自己占优
	HWND     m_shape_to_hwnd;//从图形到句柄
	//std::vector<HWND> vHwndToSendM;//接受信息的目的窗口
									 //最后放弃了这个很难控制的变量，因为它带来的“麻烦”太多了！
};