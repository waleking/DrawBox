#ifndef CBaseShape_H

#define CBaseShap_H
#endif
//#include <vector>
class CPolygon:public CBaseShape
{
public:
	CPolygon()
	{
		shape=ePolygon;
		hBrush=(HBRUSH)GetStockObject(NULL_BRUSH);
		hPen=(HPEN)GetStockObject(BLACK_PEN);
		fShapeSelected=0;
	};
	void mPolygonBuildItSelf(POINT pTemp)
	{
		vP.push_back(pTemp);
	}
	virtual void mDrawItSelf(HDC* hdc)
	{
		COPYDATASTRUCT cpdata;

		cpdata.cbData=sizeof(*this);//发送的数据大小,类创建的对象的大小都是一样的吗？
		cpdata.dwData=CBaseShape::ePolygon;//与目的句柄对接的信息
		cpdata.lpData=this;//要发送的数据
		HWND hwnd=::FindWindowW(TEXT("cn_fengzhuang_try"),NULL);
		TCHAR* windowNum=new TCHAR[3];
		TCHAR windowName[64]=TEXT("画图板文档");

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

		SelectObject(*hdc,hPen);
		SelectObject(*hdc,hBrush);
		std::vector<POINT>::iterator it=vP.begin();
		int counts_pts=(int)vP.size();
		int index=0;
		POINT* pts=new POINT[counts_pts];//我对这一句很不放心
		for(;it!=vP.end();it++)
		{
			pts[index++]=(*it);//这句也是
		}
		if(counts_pts>2)
		{
			Polygon((*hdc),pts,counts_pts);
		}
		else
		{
			OutputDebugStringA("the number of the polygon is less than 3!\n");
		}
		OutputDebugStringA("Polygon draws it self\n");
	}

	virtual void mOnlyDrawItSelf(HDC* hdc)
	{
		SelectObject(*hdc,hPen);
		SelectObject(*hdc,hBrush);
		std::vector<POINT>::iterator it=vP.begin();
		int counts_pts=(int)vP.size();
		int index=0;
		POINT* pts=new POINT[counts_pts];
		for(;it!=vP.end();it++)
		{
			pts[index++]=(*it);
		}
		if(counts_pts>2)
		{
			Polygon((*hdc),pts,counts_pts);
		}
		else
		{
			OutputDebugStringA("the number of the polygon is less than 3!\n");
		}
		OutputDebugStringA("Polygon draws it self\n");
	}

	int m_pointCounts()
	{
		return (int)vP.size();
	}

	void m_ClearItSelf()
	{
		vP.clear();
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
			outfile<<vP.size()<<" "<<penStyle<<" "<<penWidth<<" "<<penColor<<std::endl;
			outfile<<brushStyle<<" "<<brushColor<<" "<<brushHatch<<std::endl;
			std::vector<POINT>::iterator it=vP.begin();
			for(;it!=vP.end();it++)
			{
				outfile<<(*it).x<<" "<<(*it).y<<std::endl;
			}
		}
		OutputDebugStringA("ellipse save itself\n");
	}

	virtual void mOpenItSelf(std::ifstream& infile)
	{
		int counts=0;
		infile>>counts;
		infile>>penStyle;
		infile>>penWidth;
		infile>>penColor;
		infile>>brushStyle;
		infile>>brushColor;
		infile>>brushHatch;
		for(int i=0;i<counts;i++)
		{
			POINT* pt=new POINT;
			infile>>pt->x;
			infile>>pt->y;
			vP.push_back((*pt));
		}
		mSetPen(penStyle,penWidth,penColor);
		mSetBrush(brushStyle,brushColor,brushHatch);
	}
	virtual void mJudgeItSelf()
	{
		OutputDebugStringA("polygon is selected!\n");
	}
private:
	std::vector<POINT> vP;
};