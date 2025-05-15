#pragma once

class Scene 
{
private:
	float rotateY = 0.0f;
public:
	bool Init();	// ‰Šú‰»ˆ—
	
	void Update();	// XVˆ—
	void Draw();	// •`‰æˆ—
};

extern Scene* g_Scene;