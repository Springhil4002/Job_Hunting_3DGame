#pragma once
#include <Windows.h>
#include <cstdint>
#include "System/NonCopyable.h"

const UINT WINDOW_WIDTH = 1920;
const UINT WINDOW_HEIGHT = 1080;

class Application :public NonCopyable
{
private:
public:
	void Run(const TCHAR* _appName);	// アプリケーション実行関数
};

