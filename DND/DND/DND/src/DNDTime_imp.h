//////////////////////////////////////////////////////////////////////////
//name:		DNDTime_imp
//author:	Lveyou
//data:		17-07-27

//other:
//17-07-27: ʱ��ʵ�� - Lveyou
//////////////////////////////////////////////////////////////////////////

#ifndef _DND_TIME_IMP_H_
#define _DND_TIME_IMP_H_


#include "DNDTime.h"
#include "DNDUser.h"
#include <windows.h>
#include <ctime>

namespace DND
{
	class Time_imp : public Time
	{
		friend class Game;
	public:
		//example: "14:47:39"
		virtual String GetHMSString() override;
		virtual UINT32 GetFPS() override;
		virtual UINT32 GetRealFPS() override;
		virtual double GetDelta() override;
	private:
		Time_imp();
		
		UINT64 _start;
		UINT64 _loopStart;
		
		double _real_delta;			//(s), ��һ֡����ʱ��
		UINT32 _real_fps;			//��ʵ֡��
		double _delta;				//(s), ��һ֡����ʱ��
		UINT32 _fps;				//���õ�֡��
		double _except_render;		//����Ļ�ͼʱ�䣨CPU->GPU->��ʾ����
		//����ı��� ��Ҫ������֮֡��ʱ���
		LARGE_INTEGER _freq;//cpuʱ��Ƶ�ʣ�һ����ٴ�ʱ������
		LARGE_INTEGER _last;//ǰһ���ۼ�ʱ������
		LARGE_INTEGER _current;//��ǰ���ۼ�ʱ������
		void _update_current();
		void _set_last();
		double _get_cl_delta();
	};
}

#endif