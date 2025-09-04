#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <stdio.h>
#include <cstdlib>
#include <crtdbg.h>
#include "App.h"
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)

int main(int _argc, wchar_t** _argv, wchar_t** _envp)
{
	// リークチェック有効化
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	
	Application app;
	app.Run(TEXT("就職作品_HAL大阪_春岡大和"));

	return 0;
}