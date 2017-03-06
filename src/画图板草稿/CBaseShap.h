#include <fstream>
#include <math.h>
#include <vector>
class CBaseShape
{
public:
	enum eSHAPE{eNull,eLine,eRect,eEllipse,ePolygon};//����ͼ�ε�״̬
	CBaseShape()//���캯��
	{
		from.x=0;
		from.y=0;
		to.x=0;
		to.y=0;
		shape=eNull;
		fShapeSelected=0;
	};
	
	virtual void SetFSelected(int b)//�����Ƿ�ѡ��
	{
		fShapeSelected=b;
	}

	virtual double m_CalCulateDistance(HDC * hdc,POINT pt)//����㵽ͼ�εľ���
	{
		return 0;
	}

	virtual void SetFrom(POINT p)//����FROM�����
	{
		from.x=p.x;
		from.y=p.y;
	}
	virtual	POINT Get_From()//�õ�FROM�����
	{
		return from;
	}
	virtual POINT Get_To()//�õ�TO�����
	{
		return to;
	}
	virtual void SetTo(POINT p)//����TO�����
	{
		to.x=p.x;
		to.y=p.y;
	}
	virtual void SetShape(eSHAPE eshape)//�õ�ͼ�ε�״̬
	{
		shape=eshape;
	}
	virtual eSHAPE GetShap()//���û��ʸ�������
	{
		return shape;
	}

	virtual void mSetPen(UINT pStyle,int pWidth,COLORREF pColor)//���û��ʸ�������
	{
		LOGPEN lp;
		POINT pt;
		penWidth=pt.x=pWidth;
		lp.lopnColor=penColor=pColor;
		lp.lopnStyle=penStyle=pStyle;
		lp.lopnWidth=pt;
		hPen=CreatePenIndirect(&lp);
	}

	virtual void mSetPen()//���û��ʵ����غ���
	{
		LOGPEN lp;
		lp.lopnColor=penColor;
		lp.lopnStyle=penStyle;
		lp.lopnWidth.x=penWidth;
		hPen=CreatePenIndirect(&lp);
	}

	virtual void mSetBrush(UINT bStyle,COLORREF bColor,LONG bHatch)//���û�ˢ��������
	{
		LOGBRUSH lb;
		brushColor=lb.lbColor=bColor;
		brushHatch=lb.lbHatch=bHatch;
		brushStyle=lb.lbStyle=bStyle;
		hBrush=CreateBrushIndirect(&lb);
	}

	virtual void mSetBrush()//���û�ˢ�����غ���
	{
		LOGBRUSH lb;
		lb.lbColor=brushColor;
		lb.lbHatch=brushHatch;
		lb.lbStyle=brushStyle;
		hBrush=CreateBrushIndirect(&lb);
	}

	virtual void mSetHwnd(HWND hwnd)//���������Ĵ��ھ��
	{
		m_shape_to_hwnd=hwnd;
	}

	virtual void mDrawItSelf(HDC* hdc)//���ƣ����ҷ�����Ϣ
	{
		OutputDebugStringA("base shape draw itself!\n");
	};

	virtual void mSaveItSelf(std::ofstream& outfile)//����
	{
		if(!outfile)
		{
			OutputDebugStringA("�ļ�������\n");
		}
		else
		{
			outfile<<"success\n";
		}
		OutputDebugStringA("base shape save itself\n");
	}

	virtual void mOpenItSelf(std::ifstream& infile)//��
	{

	}

	virtual void mJudgeItSelf(HDC*,POINT,double)//�ж��Ƿ�ñ�ѡ��
	{
		OutputDebugStringA("it is selected!\n");
	}

	virtual int mGetFSelected()//�ж��Ƿ�ѡ��
	{
		return fShapeSelected;
	}
	virtual void mOnlyDrawItSelf(HDC* hdc)//���ƣ���������Ϣ
	{
	}

	void ErrorExit() //���ش����ź;�����Ϣ�Ĵ���
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
	HBRUSH hBrush;//��������������Ϣ
	HPEN hPen;    //����߿����Ϣ,ʵ���Ǹ����
	UINT     penStyle; //����
	int    penWidth; //�ֿ�
	COLORREF penColor; //�ߵ���ɫ
	UINT     brushStyle; //��ˢ������
	COLORREF brushColor;	//��ˢ����ɫ
	LONG     brushHatch;	//��ˢ�������·
	int     fShapeSelected;//ͼ���Ƿ�ѡ��,�����0��ô�Ϳ��Լ�飬����ǣ�1��˵���Ѿ����������ȡ��������1˵���Լ�ռ��
	HWND     m_shape_to_hwnd;//��ͼ�ε����
	//std::vector<HWND> vHwndToSendM;//������Ϣ��Ŀ�Ĵ���
									 //��������������ѿ��Ƶı�������Ϊ�������ġ��鷳��̫���ˣ�
};