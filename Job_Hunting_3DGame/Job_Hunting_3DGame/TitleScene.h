#pragma once
#include "BaseScene.h"

class TitleScene : public BaseScene
{
private:
	Camera* camera;
	const int GRID_SIZE = 8;
	const float SPACE = 2.0f;
	const float OFFSET = (GRID_SIZE - 1) * SPACE * 0.5f;
public:
	/// @brief コンストラクタ
	TitleScene() = default;
	TitleScene(Camera* _camera) { Init(_camera); }
	/// @brief デストラクタ
	~TitleScene() = default;

	/// @brief オブジェクトの生成関数
	/// @param _objectID 生成したいオブジェクトの型
	/// @return 生成したオブジェクトを返します
	Object* CreateObj(const std::string& _objectID) override;

	/// @brief 初期化処理
	void Init() {};
	void Init(Camera* _camera);
	/// @brief 更新処理
	void Update() override;
	/// @brief 描画処理
	void Draw() override;
	/// @brief 終了処理
	void Uninit() override;
};