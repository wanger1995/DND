#include "DNDSystem_imp.h"
#include "DNDError.h"
#include "DNDGame.h"
#include "DNDValue.h"
#include "DNDFont.h"
#include "DNDCoor.h"

#include<Shlobj.h>
#pragma comment(lib,"Shell32.lib")

namespace DND
{

	void System_imp::_create_window()
	{
		dnd_assert(!_hWnd ,ERROR_00001);
		//窗口类
		WNDCLASS wc;
		wc.style = CS_HREDRAW | CS_VREDRAW;
		wc.lpfnWndProc = Game::_window_proc; //消息处理函数
		wc.cbClsExtra = 0;
		wc.cbWndExtra = 0;
		wc.hInstance = _hInstance;//实例句柄
		wc.hIcon = (HICON)::LoadImage(NULL, L"game.ico", IMAGE_ICON, 0, 0, LR_DEFAULTSIZE | LR_LOADFROMFILE);
		wc.hCursor = LoadCursor(0, IDC_ARROW);
		wc.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);//背景白色
		wc.lpszMenuName = 0;
		wc.lpszClassName = L"DNDWindowClass";//窗口类名

		dnd_assert(RegisterClass(&wc), ERROR_00002);
		
		//创建窗口
		_hWnd = CreateWindow(
			L"DNDWindowClass",
			L"DNDFirstWindow",
			WS_POPUP,
			-100,
			-100,
			100,
			100,
			0 /*parent hwnd*/, 0 /* menu */, _hInstance, 0 /*extra*/);

		dnd_assert(_hWnd, ERROR_00003);

		ShowWindow(_hWnd, SW_HIDE);
		//UpdateWindow(_hWnd);
		//SetFocus(_hWnd);

	}

	System_imp::System_imp()
	{
		_windowTitle = DEFAULT_WINDOW_TITLE;
		_windowStyle = DEFAULT_WINDOW_STYLE;
		_windowSize = DEFAULT_WINDOW_SIZE;
		SetWindowCenter();

		_hWnd = 0;
		_hInstance = GetModuleHandle(0);
		_foucs = true;
		_font = new Font;

		//exe路径
		wchar_t exe_path[MAX_PATH] = { 0 };
		GetModuleFileName(0, exe_path, MAX_PATH);
		_exePath = exe_path;
		_exeName = exe_path;

		int pos = _exePath.FindEnd(L'\\') + 1;

		_exePath.CutTail(pos);//不去除/
		_exeName.CutHead(pos);

		
	}

	void System_imp::SetWindowTitle(const String& title)
	{
		_windowTitle = title;
		SetWindowText(_hWnd, title.GetWcs());
	}

	void System_imp::SetWindowCenter()
	{
		Size s = GetDesktopSize();
		_windowPoint.x = (s.w - _windowSize.w) / 2;
		_windowPoint.y = (s.h - _windowSize.h) / 2;
	}

	void System_imp::SetWindowStyle(DWORD style)
	{
		_windowStyle = style;
	}

	void System_imp::SetWindowSize(Size size)
	{
		_windowSize = size;
	}

	Size System_imp::GetWindowSize()
	{
		return _windowSize;
	}

	HWND System_imp::GetWindowHwnd()
	{
		return _hWnd;
	}

	bool System_imp::IsFoucs()
	{
		return _foucs;
	}

	void System_imp::ApplyWindow()
	{
		RECT rect = {0, 0, _windowSize.w, _windowSize.h};

		AdjustWindowRect(&rect, _windowStyle, false);

		SetWindowLong(_hWnd, GWL_STYLE, _windowStyle);

		MoveWindow(_hWnd,
			_windowPoint.x,
			_windowPoint.y,
			rect.right - rect.left,
			rect.bottom - rect.top,
			true);

		SetWindowText(_hWnd, _windowTitle.GetWcs());

		ShowWindow(_hWnd, SW_SHOW);
		/*UpdateWindow(_hWnd);
		SetFocus(_hWnd);*/
	}

	void System_imp::SetVsync(bool vsync /*= true*/)
	{
		Game::Get()->_dx->_vsync = vsync;
	}

	void System_imp::SetWindowPoint(Point point)
	{
		_windowPoint = point;
	}

	Size System_imp::GetDesktopSize()
	{
		return Size(GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN));;
	}

	void System_imp::RenderDot(Vector2 pos, Color color /*= Color::WHITE*/)
	{
		Game::Get()->_dx->_gfxSimple->_add_dot(XMFLOAT3(pos.a, pos.b, 0),
			XMFLOAT4(color.r(), color.g(), color.b(), color.a()));
	}

	void System_imp::RenderLine(Vector2 p1, Vector2 p2, Color color /*= Color::WHITE*/)
	{
		Game::Get()->_dx->_gfxSimple->_add_line(
			XMFLOAT3(p1.a, p1.b, 0),
			XMFLOAT3(p2.a, p2.b, 0),
			XMFLOAT4(color.r(), color.g(), color.b(), color.a()));
	}

	bool System_imp::LoadFontFile(const String& name, const String& path, int mode /*= 0*/)
	{
		return _font->_load_font_file(name, path, mode);
	}



	String System_imp::GetChooseFolder(const String& title, String root)
	{
		wchar_t buffer[MAX_PATH] = { 0 };

		BROWSEINFO bi = { 0 };
		if (root == L"")
		{
			bi.pidlRoot = NULL;
		}
		else
		{
			//获取想要的根目录
			LPSHELLFOLDER pShellFolder = NULL;
			ULONG nCharsParsed = 0;

			SHGetDesktopFolder(&pShellFolder);

			wchar_t temp[MAX_PATH] = { 0 };
			//root.Replace_Char(L'/', L'\\');
			root.GetWideCharStr(temp, MAX_PATH);
			//第三个参数
			pShellFolder->ParseDisplayName(NULL, NULL, temp, &nCharsParsed, (LPITEMIDLIST*)&(bi.pidlRoot), NULL);


			pShellFolder->Release();
		}

		bi.hwndOwner = _hWnd;//拥有着窗口句柄，为NULL表示对话框是非模态的，实际应用中一般都要有这个句柄
		bi.pszDisplayName = buffer;//接收文件夹的缓冲区
		bi.lpszTitle = title.GetWcs();//标题
		bi.ulFlags = BIF_NEWDIALOGSTYLE;
		LPITEMIDLIST idl = SHBrowseForFolder(&bi);


		if (SHGetPathFromIDList(idl, buffer))
		{
			String ret = buffer;
			return ret + L"\\";
		}
		else
		{
			return String();
		}
	}


	bool System_imp::GetChooseFile(const WCHAR* filter, String& ipath_name, String& iname)
	{
		
		wchar_t path[MAX_PATH] = { 0 };
		wchar_t name[MAX_PATH] = { 0 };
		OPENFILENAME openFileName = { 0 };
		openFileName.lStructSize = sizeof(OPENFILENAME);
		openFileName.nMaxFile = MAX_PATH;  //这个必须设置，不设置的话不会出现打开文件对话框 
		openFileName.lpstrFilter = filter;
			//L"文本文件(*.png)\0*.png\0所有文件(*.*)\0*.*\0\0";
		openFileName.lpstrFile = path;
		openFileName.lpstrFileTitle = name;
		openFileName.nMaxFileTitle = MAX_PATH;
		openFileName.nFilterIndex = 1;
		openFileName.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

		
		if (GetOpenFileName(&openFileName))
		{
			

			SetCurrentDirectory(_exePath.GetWcs());

			ipath_name = openFileName.lpstrFile;
			iname = openFileName.lpstrFileTitle;
			
			

			return true;
		}

	
		return false;
	}

	void System_imp::CopyAFile(const String& source, const String& target)
	{
		CopyFile(source.GetWcs(), target.GetWcs(), FALSE);
	}



	bool System_imp::GetPathFileFirst(const String& path, String& name)
	{

		_find = ::FindFirstFile(path.GetWcs(), &_findData);

		if (INVALID_HANDLE_VALUE == _find)
		{
			return false;
		}

		name = _findData.cFileName;
		return true;
	}

	bool System_imp::GetPathFileNext(const String& path, String& name)
	{
		while (FindNextFile(_find, &_findData))
		{
			name = _findData.cFileName;
			return true;
		}
		return false;
	}

	String System_imp::GetExePath()
	{
		return _exePath;
	}

	String System_imp::GetExeName()
	{
		return _exeName;
	}

	void System_imp::SetShowCursor(bool show)
	{
		ShowCursor(show);
	}

	System_imp::~System_imp()
	{
		delete _font;
	}

	void System_imp::RenderCircle(Vector2 pos, float radius, UINT32 count, Color color /*= Color::WHITE*/)
	{
		float rad_dt = 3.1415926f * 2 / count;
		Vector2 p1 = Vector2(radius, 0);
		for(UINT32 i = 1;i < count; ++i)
		{
			Vector2 p2;
			p2.a = radius * cos(rad_dt * i);
			p2.b = radius * sin(rad_dt * i);

			RenderLine(p1 + pos, p2 + pos, color);
			p1 = p2;
		}
		RenderLine(p1 + pos, Vector2(radius, 0) + pos, color);
	}

	void System_imp::RenderCoor(Coor* coor)
	{
		Vector2 p = Vector2();
		Vector2 x = p + Vector2(32.0f, 0);
		Vector2 y = p + Vector2(0, 32.0f);
		p = coor->ThisToWorld(p);
		x = coor->ThisToWorld(x);
		y = coor->ThisToWorld(y);

		RenderLine(p, x, Color::RED);
		RenderLine(p, y, Color::GREEN);
	}

	Point System_imp::GetWindowPoint()
	{
		return _windowPoint;
	}
	








}

