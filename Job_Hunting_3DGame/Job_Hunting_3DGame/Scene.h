#pragma once

class Scene 
{
private:
	float rotateY = 0.0f;
public:
	bool Init();	// ����������
	
	void Update();	// �X�V����
	void Draw();	// �`�揈��
};

extern Scene* g_Scene;