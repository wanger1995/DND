//////////////////////////////////////////////////////////////////////////
//name:		DNDText
//author:	Lveyou
//data:		17-08-15

//other:
//17-08-15: �ı��ؼ� Lveyou
//////////////////////////////////////////////////////////////////////////

#ifndef _DND_TEXT_H_
#define _DND_TEXT_H_

#include "DNDDLL.h"
#include "DNDString.h"
#include "DNDCoor.h"
#include "DNDColor.h"

namespace DND
{
	const int TEXT_ALIGN_LEFT = 0;
	const int TEXT_ALIGN_HCENTER = 1;
	const int TEXT_ALIGN_RIGHT = 2;

	const int TEXT_ALIGN_BUTTOM = 2;
	const int TEXT_ALIGN_VCENTER = 1;
	const int TEXT_ALIGN_TOP = 0;

	class Sprite;
	class DLL_API Text
	{
	public:
		virtual void Render() = 0;
		virtual void RenderFrame() = 0;
		virtual void SetString(const String& str) = 0;
		virtual void SetStringFast(const String& str) = 0;
		virtual void RegisterString(const String& str) = 0;
		virtual String GetString() = 0;
		virtual Coor* GetCoor() = 0;
		virtual Color GetColor() = 0;
		virtual void SetOrder(int order) = 0;
		virtual int GetOrder() = 0;
		virtual bool IsPickup() = 0;
		virtual void SetAlignHorizontal(int align = TEXT_ALIGN_LEFT) = 0;
		virtual void SetAlignVertical(int align = TEXT_ALIGN_TOP) = 0;
		virtual void SetColor(Color color) = 0;
		virtual void SetPitchRow(float row = 1.0f) = 0;
		virtual void SetMaxW(unsigned w) = 0;
		virtual unsigned GetH() = 0;
		//���棺���������������صľ��飬���ܻ�����ʧЧ
		virtual Sprite* GetPickup(unsigned& n) = 0;
		virtual Sprite* GetSprite(unsigned n) = 0;//

		virtual Text* Clone() = 0;
		virtual ~Text() = 0;
	};


}

#endif