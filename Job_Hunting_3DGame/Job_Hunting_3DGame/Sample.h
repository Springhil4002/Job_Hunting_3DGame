#pragma once

class Sample 
{
private:
public:
	bool Init();	// 初期化処理
	
	void Update();	// 更新処理
	void Draw();	// 描画処理	
};

extern Sample* g_Sample;