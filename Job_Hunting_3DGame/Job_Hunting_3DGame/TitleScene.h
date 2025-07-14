#pragma once
#include "BaseScene.h"

class TitleScene : public BaseScene
{
private:
	HWND hwnd;
	Camera* camera;
	const int gridX = 256;
	const int gridZ = 256;
	const int gridMeshSize = 512;
public:
	/// @brief コンストラクタ
	TitleScene() = default;
	TitleScene(Camera* _camera, HWND _hwnd) { Init(_camera, _hwnd); }
	/// @brief デストラクタ
	~TitleScene() = default;

	/// @brief オブジェクトの生成関数
	/// @param _objectID 生成したいオブジェクトの型
	/// @return 生成したオブジェクトを返します
	Object* CreateObj(const std::string& _objectID) override;

	/// @brief 初期化処理
	void Init() {};
	void Init(Camera* _camera,HWND _hwnd);
	/// @brief 更新処理
	void Update() override;
	/// @brief 描画処理
	void Draw() override;
	/// @brief 終了処理
	void Uninit() override;
	/// @brief 入力処理
	void Update_Input();
	/// @brief マウスの回転更新処理
	void Update_MouseRotate(float _sensi);
	/// @brief ImGuiの描画処理
	void Draw_ImGui() override;
};