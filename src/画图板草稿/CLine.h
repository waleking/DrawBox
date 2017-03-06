#include "CBaseShap.h"

class CLine:public CBaseShape
{
public:
	CLine()
	{
		from.x=0;
		from.y=0;
		to.x=0;
		to.y=0;
		shape=eLine;
		hBrush=(HBRUSH)GetStockObject(NULL_BRUSH);
		hPen=(HPEN)GetStockObject(BLACK_PEN);//每个都用HPEN是不合理的，会造成内存的浪费
											//改进方法是将GDI OBJECT也保存成类
											//然后建一个MAP，将它们和SHAPE类绑定，但这又会造成一个问题
											//保存文件时，必须要把MAP和GDI OBJECT类同时保存，加大工作量
											//不知这两个方案对比下来，哪个更好？
											//现在有答案了：hpen只有4个字节和INT一样大，不必担心不删除HPEN会影响内存
											//放心的去做！！！
		fShapeSelected=0;
	};
	CLine(POINT a,POINT b)
	{
		CBaseShape::SetFrom(a);
		CBaseShape::SetTo(b);
		shape=eLine;
		//DeleteObject(hp);//不应当被删除,原因是hp没有真正的被写到hPen,到最后hPen是unused
		fShapeSelected=0;
	}
	virtual void mDrawItSelf(HDC* hdc)
	{
		//CEllipse* pellipse=new CEllipse;
		COPYDATASTRUCT cpdata;

		cpdata.cbData=sizeof(CLine);//发送的数据大小,类创建的对象的大小都是一样的吗？
		cpdata.dwData=CLine::eLine;//与目的句柄对接的信息
		cpdata.lpData=this;//要发送的数据
		HWND hwnd=::FindWindowW(TEXT("cn_fengzhuang_try"),NULL);//至于这两个参数的意义有待考察
		//还要判断一下是否和原来的HWND相同
		//GetObjectHwnd();
		TCHAR* windowNum=new TCHAR[3];
		TCHAR windowName[64]=TEXT("画图板文档");
		TCHAR windowNameStd[]=TEXT("画图板文档");//要与之比较的标准窗口名称
		TCHAR windowNameToCampare[260];//获得的当前窗口的名字
		GetWindowText(m_shape_to_hwnd,windowNameToCampare,260);
		bool fSend=true;//记录是否发送数据，将可以发送数据的窗口限定在“画图板文档系列”
		for(int index=1;index<6;index++)
		{
			if(windowNameToCampare[index]!=windowNameStd[index])
				fSend=false;
		}
		if(fSend==true)
		{
			for(int index=1;index<=98;index++)
			{
				if(index<=9)
				{
					*(windowNum)=48+index;//加48是要转化为char型或者TCHAR型
					*(windowNum+1)=0;
					*(windowNum+2)=0;
				}
				else if(index<=99)
				{
					*(windowNum)=(index/10)+48;//得到高位
					*(windowNum+1)=index%10+48;//得到低位
					*(windowNum+2)=0;
				}
				for(int j=0;j<64;j++)
				{
					windowName[j]=0;
				}
				windowName[0]=TEXT('画');
				windowName[1]=TEXT('图');
				windowName[2]=TEXT('板');
				windowName[3]=TEXT('文');
				windowName[4]=TEXT('档');
				lstrcat(windowName,windowNum);
				HWND tempHwnd=FindWindow(TEXT("cn_fengzhuang_try"),windowName);
				if(tempHwnd!=m_shape_to_hwnd && tempHwnd!=NULL)
				{
					SendMessage(tempHwnd,WM_COPYDATA,0,(LPARAM)&cpdata);
				}
			}
		}
		SelectObject(*hdc,hPen);
		MoveToEx(*hdc,from.x,from.y,NULL);
		LineTo(*hdc,to.x,to.y);
	}

	virtual void mOnlyDrawItSelf(HDC* hdc)
	{
		if(fShapeSelected==0)
		{
			SelectObject(*hdc,hPen);
			MoveToEx(*hdc,from.x,from.y,NULL);
			LineTo(*hdc,to.x,to.y);
		}
		else if(fShapeSelected==1)
		{
			LOGPEN penStruct;
			if(penColor!=RGB(255,0,0))
			{
				penStruct.lopnColor=RGB(255,0,0);
			}
			else
				penStruct.lopnColor=RGB(0,0,255);
			penStruct.lopnStyle=PS_DASHDOT;
			penStruct.lopnWidth.x=1;
			HPEN pen=CreatePenIndirect(&penStruct);
			SelectObject(*hdc,pen);
			MoveToEx(*hdc,from.x,from.y,NULL);
			LineTo(*hdc,to.x,to.y);
		}
		
	}
	virtual void mSaveItSelf(std::ofstream& outfile)
	{
		if(!outfile)
		{
			OutputDebugStringA("文件不能被写入或创建\n");
		}
		else
		{
			outfile<<shape<<std::endl;
			outfile<<from.x<<" "<<from.y<<" "<<to.x<<" "<<to.y<<std::endl;
			outfile<<penStyle<<" "<<penWidth<<" "<<penColor<<std::endl;
		}
		OutputDebugStringA("ellipse save itself\n");
	}

	virtual void mOpenItSelf(std::ifstream& infile)
	{
		infile>>from.x;
		infile>>from.y;
		infile>>to.x;
		infile>>to.y;
		infile>>penStyle;
		infile>>penWidth;
		infile>>penColor;
		mSetPen(penStyle,penWidth,penColor);
	}

	virtual double m_CalCulateDistance(HDC * hdc,POINT pt)
	{
		float distance;//形式上的DISTANCE
		double delta;
		if((abs(from.y-to.y))>(abs(from.x-to.x)))//斜率会大于1，有可能很难选中，所以改变方程形式
		{
			k=((float)(to.x-from.x))/((float)(to.y-from.y));
			b=from.x-k*from.y;
			distance=(k*pt.y+b-pt.x);
		}
		else
		{
			k=((float)(to.y-from.y))/((float)(to.x-from.x));
			b=from.y-k*from.x;
			distance=(k*pt.x+b-pt.y);
		}
		delta=distance;
		return abs(delta);
	}

	
	virtual void mJudgeItSelf(HDC * hdc,POINT pt,double stddelta)
	{
		float distance;//形式上的DISTANCE
		double delta;
		if((abs(from.y-to.y))>(abs(from.x-to.x)))//斜率会大于1，有可能很难选中，所以改变方程形式
		{
			distance=(k*pt.y+b-pt.x);
		}
		else
		{
			distance=(k*pt.x+b-pt.y);
		}
		delta=distance;
		if(delta<=100&&delta>=-100 && (abs(delta)<=stddelta))
		{
			fShapeSelected=1;
			mDrawItBySelect(hdc);
		}
		else
		{	
			mDrawItSelf(hdc);//没有选中，那么应该去除刚才的痕迹，重新画
			fShapeSelected=0;
		}
	}

	virtual void mDrawItBySelect(HDC* hdc)
	{
		LOGPEN penStruct;
		if(penColor!=RGB(255,0,0))
		{
			penStruct.lopnColor=RGB(255,0,0);
		}
		else
			penStruct.lopnColor=RGB(0,0,255);
		penStruct.lopnStyle=PS_SOLID;
		penStruct.lopnWidth.x=penWidth;
		HPEN pen=CreatePenIndirect(&penStruct);
		SelectObject(*hdc,pen);
		MoveToEx(*hdc,from.x,from.y,NULL);
		LineTo(*hdc,to.x,to.y);
	}

private:
	float k;//斜率
	float b;//截距

};