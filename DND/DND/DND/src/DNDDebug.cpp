#include "DNDDebug.h"
#include "DNDError.h"
#include "DNDValue.h"
#include "DNDGame.h"
#include "DNDTime.h"
#include <windows.h>
#include <iostream>

namespace DND
{



	void Debug::WriteLine(const String& str, int level)
	{
		assert(_debugger && L"ERROR_00008");
		_debugger->WriteLine(str, level);
	}

	void Debug::Write(const String& str, int level)
	{
		assert(_debugger && L"ERROR_00008");
		_debugger->Write(str, level);
	}

	void Debug::WriteEndl(int level)
	{
		assert(_debugger && L"ERROR_00008");
		_debugger->WriteEndl(level);
	}

	void Debug::SetDebugger(Debugger* debugger)
	{
		assert(debugger && L"ERROR_00007");
		_debugger = debugger;
	}


	Debugger* Debug::_debugger = NULL;
	//do nothing
	void DebuggerEmpty::WriteLine(const String& str, int level)
	{

	}

	void DebuggerEmpty::Write(const String& str, int level)
	{

	}

	void DebuggerEmpty::WriteEndl(int level)
	{

	}


	void DebuggerConsole::WriteLine(const String& str, int level)
	{
		if(_bTimeStamp)
		{
			Write(String(L"[") + Game::Get()->time->GetHMSString() + L"]", DebugLevel::TIME);
			Write(L"=", DebugLevel::BLANK);
		}
			
		Write(str, level);
		WriteEndl(level);
	}

	void DebuggerConsole::Write(const String& str, int level)
	{
		switch(level)
		{
		case DebugLevel::BLANK:
			SetConsoleTextAttribute(_handleOutput,
				FOREGROUND_INTENSITY |
				BACKGROUND_INTENSITY);
			break;
		case DebugLevel::TIME:
			SetConsoleTextAttribute(_handleOutput,
				BACKGROUND_GREEN | BACKGROUND_RED | BACKGROUND_BLUE |
				BACKGROUND_INTENSITY);
			break;
		case DebugLevel::INFO:
			SetConsoleTextAttribute(_handleOutput,
				FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_BLUE |
				BACKGROUND_BLUE |
				FOREGROUND_INTENSITY);
			break;
		case DebugLevel::NOTICE:
			SetConsoleTextAttribute(_handleOutput,
				FOREGROUND_GREEN |
				BACKGROUND_GREEN |
				FOREGROUND_INTENSITY);
			break;
		case DebugLevel::WARN:
			SetConsoleTextAttribute(_handleOutput,
				FOREGROUND_GREEN | FOREGROUND_RED |
				BACKGROUND_GREEN | BACKGROUND_RED |
				FOREGROUND_INTENSITY);
			break;
		case DebugLevel::ERR:
			SetConsoleTextAttribute(_handleOutput,
				FOREGROUND_RED |
				BACKGROUND_RED |
				FOREGROUND_INTENSITY);
			break;
		}
		std::wcout << str.GetWcs();
		
	}

	void DebuggerConsole::WriteEndl(int level)
	{
		std::wcout << std::endl;
	}

	DebuggerConsole::DebuggerConsole()
	{
		dnd_assert(AllocConsole(), ERROR_00009);
		FILE* stream;
		freopen_s(&stream, "CON", "r", stdin);//�ض���������
		freopen_s(&stream, "CON", "w", stdout);//�ض���������

		_get_console_hwnd();
		_handleOutput = GetStdHandle(STD_OUTPUT_HANDLE);
		_bTimeStamp = true;
	}

	void DebuggerConsole::_get_console_hwnd()
	{
		SetConsoleTitle(DEFAULT_CONSOLE_TITLE.GetWcs());
		//����̨���ڴ���
		for (int i = 0; i < 100; i++)
		{
			_consoleHwnd = FindWindow(NULL, DEFAULT_CONSOLE_TITLE.GetWcs());
			if (_consoleHwnd)
				break;
			Sleep(100);
		}

		dnd_assert(_consoleHwnd, ERROR_00010);
	}

	DebuggerConsole::~DebuggerConsole()
	{
		FreeConsole();
	}

	void DebuggerConsole::SetTimeStamp(bool open)
	{
		_bTimeStamp = open;
	}

	void DebuggerConsole::TempWrite(const String& str, int level)
	{
		UINT32 len = str.GetLength();
		Write(str, level);
		Write(String(L'\b', len), level);
	}

}