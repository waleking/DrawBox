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
		hPen=(HPEN)GetStockObject(BLACK_PEN);//ÿ������HPEN�ǲ�����ģ�������ڴ���˷�
											//�Ľ������ǽ�GDI OBJECTҲ�������
											//Ȼ��һ��MAP�������Ǻ�SHAPE��󶨣������ֻ����һ������
											//�����ļ�ʱ������Ҫ��MAP��GDI OBJECT��ͬʱ���棬�Ӵ�����
											//��֪�����������Ա��������ĸ����ã�
											//�����д��ˣ�hpenֻ��4���ֽں�INTһ���󣬲��ص��Ĳ�ɾ��HPEN��Ӱ���ڴ�
											//���ĵ�ȥ��������
		fShapeSelected=0;
	};
	CLine(POINT a,POINT b)
	{
		CBaseShape::SetFrom(a);
		CBaseShape::SetTo(b);
		shape=eLine;
		//DeleteObject(hp);//��Ӧ����ɾ��,ԭ����hpû�������ı�д��hPen,�����hPen��unused
		fShapeSelected=0;
	}
	virtual void mDrawItSelf(HDC* hdc)
	{
		//CEllipse* pellipse=new CEllipse;
		COPYDATASTRUCT cpdata;

		cpdata.cbData=sizeof(CLine);//���͵����ݴ�С,�ഴ���Ķ���Ĵ�С����һ������
		cpdata.dwData=CLine::eLine;//��Ŀ�ľ���Խӵ���Ϣ
		cpdata.lpData=this;//Ҫ���͵�����
		HWND hwnd=::FindWindowW(TEXT("cn_fengzhuang_try"),NULL);//���������������������д�����
		//��Ҫ�ж�һ���Ƿ��ԭ����HWND��ͬ
		//GetObjectHwnd();
		TCHAR* windowNum=new TCHAR[3];
		TCHAR windowName[64]=TEXT("��ͼ���ĵ�");
		TCHAR windowNameStd[]=TEXT("��ͼ���ĵ�");//Ҫ��֮�Ƚϵı�׼��������
		TCHAR windowNameToCampare[260];//��õĵ�ǰ���ڵ�����
		GetWindowText(m_shape_to_hwnd,windowNameToCampare,260);
		bool fSend=true;//��¼�Ƿ������ݣ������Է������ݵĴ����޶��ڡ���ͼ���ĵ�ϵ�С�
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
			OutputDebugStringA("�ļ����ܱ�д��򴴽�\n");
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
		float distance;//��ʽ�ϵ�DISTANCE
		double delta;
		if((abs(from.y-to.y))>(abs(from.x-to.x)))//б�ʻ����1���п��ܺ���ѡ�У����Ըı䷽����ʽ
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
		float distance;//��ʽ�ϵ�DISTANCE
		double delta;
		if((abs(from.y-to.y))>(abs(from.x-to.x)))//б�ʻ����1���п��ܺ���ѡ�У����Ըı䷽����ʽ
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
			mDrawItSelf(hdc);//û��ѡ�У���ôӦ��ȥ���ղŵĺۼ������»�
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
	float k;//б��
	float b;//�ؾ�

};