#pragma once
#include "BaseScene.h"
#include "PlayerController.h"

static bool runningTimer = false;
static std::chrono::steady_clock::time_point startTime;
static std::chrono::milliseconds elapsedTime(0);

class TitleScene : public BaseScene
{
private:
	HWND hwnd;
	Camera* camera;
	PlayerController* playerCtrl;	
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
	void Update(float _deltaTime) override;
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

	void ImGui_Prop();
	void ImGui_PlayerController();
	void ImGui_WaterMesh();
	void ImGui_Timer();
};