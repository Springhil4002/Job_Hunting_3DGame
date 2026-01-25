#pragma once
#include "BaseScene.h"
#include "Camera2D.h"
#include "Shop.h"

class ResultScene : public BaseScene
{
private:
	HWND hwnd;
	Camera* camera;
	Camera2D* uiCamera;
	std::unique_ptr<Shop> m_Shop;
public:
	/// @brief コンストラクタ
	ResultScene() = default;
	ResultScene(Camera* _camera, Camera2D* _uiCamera, HWND _hwnd) 
	{ Init(_camera, _uiCamera,_hwnd); }
	/// @brief デストラクタ
	~ResultScene() = default;

	/// @brief オブジェクトの生成関数
	/// @param _objectID 生成したいオブジェクトの型
	/// @return 生成したオブジェクトを返します
	Object* CreateObj(const std::string& _objectID) override;

	/// @brief 初期化処理
	void Init(Camera* _camera, Camera2D* _uiCamera, HWND _hwnd);
	/// @brief 更新処理
	void Update(float _deltaTime) override;
	/// @brief 描画処理
	void Draw() override;
	/// @brief 終了処理
	void Uninit() override;
	/// @brief デバッグ用:カメラ移動入力処理
	void Update_Input();
	/// @brief デバッグ用:マウスの回転更新処理
	void Update_MouseRotate(float _sensi);
	/// @brief ImGuiの描画処理
	void Draw_ImGui() override;

	void ImGui_Prop();

	void ImGui_Shop();
};