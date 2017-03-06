#ifndef CBaseShape_H

#define CBaseShap_H
#endif//ͷ�ļ�����ӷ�����Ҫѧϰ

class CEllipse:public CBaseShape
{
public:
	CEllipse()
	{
		from.x=0;
		from.y=0;
		to.x=0;
		to.y=0;
		shape=eEllipse;
		hBrush=(HBRUSH)GetStockObject(NULL_BRUSH);
		hPen=(HPEN)GetStockObject(BLACK_PEN);
		fShapeSelected=0;
	};
	CEllipse(POINT a,POINT b)
	{
		CBaseShape::SetFrom(a);
		CBaseShape::SetTo(b);
		shape=eEllipse;
		fShapeSelected=0;
	};

	virtual void mDrawItSelf(HDC* hdc)
	{
		//CEllipse* pellipse=new CEllipse;
		COPYDATASTRUCT cpdata;

		cpdata.cbData=sizeof(CEllipse);//���͵����ݴ�С,�ഴ���Ķ���Ĵ�С����һ������
		cpdata.dwData=CBaseShape::eEllipse;//��Ŀ�ľ���Խӵ���Ϣ
		cpdata.lpData=this;//Ҫ���͵�����
		HWND hwnd=::FindWindowW(TEXT("cn_fengzhuang_try"),NULL);//���������������������д�����
		//��Ҫ�ж�һ���Ƿ��ԭ����HWND��ͬ
		//GetObjectHwnd();
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
				//vHwndToSendM.push_back(tempHwnd);//��Ӧ�ú�ԭ���ıȽ�һ��
			}
		}
		
		//std::vector<HWND> ::iterator it=vHwndToSendM.begin();
		//for(;it!=vHwndToSendM.end();it++)
		//{
		//	try
		//	{
		//		::SendMessage((*it),WM_COPYDATA,0,(LPARAM)&cpdata);//�ҵ����������Ĵ��ڣ�������Ϣ
		//														//Ϊʲô���д�
		//	}
		//	
		//	catch(char* str)
		//	{
		//		char sztemp[128];
		//		wsprintfA(sztemp,"Exception raised: %s\n",str);
		//		OutputDebugStringA(sztemp);
		//	}
		//	ErrorExit();
		//}
		//����vHwndToSendM����Ļ������ҷ�������
			
		
		SelectObject(*hdc,hPen);
		SelectObject(*hdc,hBrush);
		Ellipse(*hdc,from.x,from.y,to.x,to.y);
		char sztemp[128];
		wsprintfA(sztemp,"pen's width is%d\n",penWidth);
		OutputDebugStringA(sztemp);
	}



	virtual void mOnlyDrawItSelf(HDC* hdc)
	{
		if(fShapeSelected==0)
		{
			SelectObject(*hdc,hPen);
			SelectObject(*hdc,hBrush);
			Ellipse(*hdc,from.x,from.y,to.x,to.y);
			char sztemp[128];
			wsprintfA(sztemp,"pen's width is%d\n",penWidth);
			OutputDebugStringA(sztemp);
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
			Ellipse(*hdc,from.x,from.y,to.x,to.y);
		}
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

	virtual double m_CalCulateDistance(HDC * hdc,POINT pt)
	{
		if(from.x>to.x)
		{
			m_X_half=((float)from.x-(float)to.x)/2;//�����᲻�᲻��ȷ��INT������������
			mCenter.x=(long)(to.x+m_X_half);
		}
		else
		{
			m_X_half=((float)to.x-(float)from.x)/2;
			mCenter.x=(long)(from.x+m_X_half);
		}
		if(from.y>to.y)
		{
			m_Y_half=((float)from.y-(float)to.y)/2;
			mCenter.y=(long)(to.y+m_Y_half);
		}
		else
		{
			m_Y_half=((float)to.y-(float)from.y)/2;
			mCenter.y=(long)(from.y+m_Y_half);
		}
		float distanceX,distanceY;//��ʽ�ϵ�DISTANCE
		distanceX=(float)(pt.x-mCenter.x);
		distanceY=(float)(pt.y-mCenter.y);
		double delta=(pow(distanceX,2))/(pow(m_X_half,2))+(pow(distanceY,2))/(pow(m_Y_half,2));
		return abs(delta-1);
	}

	virtual void mJudgeItSelf(HDC * hdc,POINT pt,double stddelta)
	{
		
		float distanceX,distanceY;//��ʽ�ϵ�DISTANCE
		distanceX=(float)(pt.x-mCenter.x);
		distanceY=(float)(pt.y-mCenter.y);
		double delta=(pow(distanceX,2))/(pow(m_X_half,2))+(pow(distanceY,2))/(pow(m_Y_half,2));
		if(delta<=1.1&&delta>=0.9 && (abs(delta-1)<=stddelta))
		{
			fShapeSelected=1;
			mDrawItBySelect(hdc);
		}
		else
		{	
			mDrawItSelf(hdc);//û��ѡ�У���ôӦ��ȥ���ղŵĺۼ������»�
			fShapeSelected=0;
		}
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
		//��HPEN��HBRUSH��������ʾPEN��BRUSH��������Ϣ����ʵֻ��һ����������´��������������ʹ�ࡣ����
		////���ڿ��Ա���ʹ���
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
		/*penStruct.lopnColor=penColor;
		penStruct.lopnStyle=penStyle;*/
		penStruct.lopnWidth.x=penWidth;
		HPEN pen=CreatePenIndirect(&penStruct);
		SelectObject(*hdc,pen);
		SelectObject(*hdc,hBrush);
		Ellipse(*hdc,from.x,from.y,to.x,to.y);
		char sztemp[128];
		wsprintfA(sztemp,"pen's width is%d\n",penWidth);
		OutputDebugStringA(sztemp);
	}

private:
	POINT mCenter;
	float   m_X_half;
	float   m_Y_half;
};