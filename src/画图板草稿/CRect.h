#ifndef CBaseShape_H

#define CBaseShap_H
#endif
class CRect:public CBaseShape
{
public:
	CRect()
	{
		from.x=0;
		from.y=0;
		to.x=0;
		to.y=0;
		shape=eRect;
		hBrush=(HBRUSH)GetStockObject(NULL_BRUSH);
		hPen=(HPEN)GetStockObject(BLACK_PEN);
		fShapeSelected=0;
	};
	CRect(POINT a,POINT b)
	{
		CBaseShape::SetFrom(a);
		CBaseShape::SetTo(b);
		shape=eRect;
		fShapeSelected=0;
	};

	virtual void mOnlyDrawItSelf(HDC* hdc)
	{
		if(fShapeSelected==0)
		{
			SelectObject(*hdc,hPen);
			SelectObject(*hdc,hBrush);
			Rectangle(*hdc,from.x,from.y,to.x,to.y);
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
			SelectObject(*hdc,hBrush);
			Rectangle(*hdc,from.x,from.y,to.x,to.y);
		}
	}

	virtual void mDrawItSelf(HDC* hdc)
	{
		COPYDATASTRUCT cpdata;

		cpdata.cbData=sizeof(CRect);//���͵����ݴ�С,�ഴ���Ķ���Ĵ�С����һ������
		cpdata.dwData=CBaseShape::eRect;//��Ŀ�ľ���Խӵ���Ϣ
		cpdata.lpData=this;//Ҫ���͵�����
		HWND hwnd=::FindWindowW(TEXT("cn_fengzhuang_try"),NULL);
		TCHAR* windowNum=new TCHAR[3];
		TCHAR windowName[64]=TEXT("��ͼ���ĵ�");

		for(int index=1;index<=98;index++)
		{
			if(index<=9)
			{
				*(windowNum)=48+index;//��48��Ҫת��Ϊchar�ͻ���TCHAR��
				*(windowNum+1)=0;
				*(windowNum+2)=0;
			}
			else if(index<=99)
			{
				*(windowNum)=(index/10)+48;//�õ���λ
				*(windowNum+1)=index%10+48;//�õ���λ
				*(windowNum+2)=0;
			}
			for(int j=0;j<64;j++)
			{
				windowName[j]=0;
			}
			windowName[0]=TEXT('��');
			windowName[1]=TEXT('ͼ');
			windowName[2]=TEXT('��');
			windowName[3]=TEXT('��');
			windowName[4]=TEXT('��');
			lstrcat(windowName,windowNum);
			HWND tempHwnd=FindWindow(TEXT("cn_fengzhuang_try"),windowName);
			if(tempHwnd!=m_shape_to_hwnd && tempHwnd!=NULL)
			{
				SendMessage(tempHwnd,WM_COPYDATA,0,(LPARAM)&cpdata);
			}
		}
		SelectObject(*hdc,hPen);
		SelectObject(*hdc,hBrush);
		Rectangle(*hdc,from.x,from.y,to.x,to.y);
	}

	virtual void mSaveItSelf(std::ofstream& outfile)
	{
		if(!outfile)
		{
			OutputDebugStringA("�ļ����ܱ�д��򴴽�\n");
		}
		else
		{
			outfile<<shape<<std::endl;
			outfile<<from.x<<" "<<from.y<<" "<<to.x<<" "<<to.y<<std::endl;
			outfile<<penStyle<<" "<<penWidth<<" "<<penColor<<std::endl;
			outfile<<brushStyle<<" "<<brushColor<<" "<<brushHatch<<std::endl;
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
		infile>>brushStyle;
		infile>>brushColor;
		infile>>brushHatch;
		mSetPen(penStyle,penWidth,penColor);
		mSetBrush(brushStyle,brushColor,brushHatch);
	}

	virtual double m_CalCulateDistance(HDC * hdc,POINT pt)
	{
		double delta;
		float distanceX,distanceY;//��ʽ�ϵ�DISTANCE
		if(abs(pt.x-from.x)>abs(pt.x-to.x))
		{
			distanceX=(float)abs(pt.x-to.x);
		}
		else
		{
			distanceX=(float)abs(pt.x-from.x);
		}
		if(abs(pt.y-from.y)>abs(pt.y-to.y))
		{
			distanceY=(float)abs(pt.y-to.y);
		}
		else
		{
			distanceY=(float)abs(pt.y-from.y);
		}
		if(distanceX>distanceY)
			delta=distanceY;
		else
			delta=distanceX;
		return abs(delta-1);
	}

	virtual void mJudgeItSelf(HDC * hdc,POINT pt,double stddelta)
	{

		double delta;
		float distanceX,distanceY;//��ʽ�ϵ�DISTANCE
		if(abs(pt.x-from.x)>abs(pt.x-to.x))
		{
			distanceX=(float)abs(pt.x-to.x);
		}
		else
		{
			distanceX=(float)abs(pt.x-from.x);
		}
		if(abs(pt.y-from.y)>abs(pt.y-to.y))
		{
			distanceY=(float)abs(pt.y-to.y);
		}
		else
		{
			distanceY=(float)abs(pt.y-from.y);
		}
		if(distanceX>distanceY)
			delta=distanceY;
		else
			delta=distanceX;
		if(delta<=5&&delta>=0.1 && (abs(delta-1)<=stddelta))
		{
			fShapeSelected=1;
			mDrawItBySelect(hdc);
			OutputDebugStringA("true!\n");
		}
		else
		{	
			mDrawItSelf(hdc);//û��ѡ�У���ôӦ��ȥ���ղŵĺۼ������»�
			fShapeSelected=0;
			OutputDebugStringA("false!\n");
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
		SelectObject(*hdc,hBrush);
		Rectangle(*hdc,from.x,from.y,to.x,to.y);
	}
};