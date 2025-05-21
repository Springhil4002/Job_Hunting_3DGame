#pragma once
#include "BaseScene.h"

class ResultScene : public BaseScene
{
private:
public:
	/// @brief コンストラクタ
	ResultScene() { Init(); };
	/// @brief デストラクタ
	~ResultScene() {};

	/// @brief オブジェクトの生成関数
	/// @param _objectID 生成したいオブジェクトの型
	/// @return 生成したオブジェクトを返します
	Object* CreateObj(const std::string& _objectID) override;

	/// @brief 初期化処理
	void Init() override;
	/// @brief 更新処理
	void Update() override;
	/// @brief 描画処理
	void Draw() override;
	/// @brief 終了処理
	void Uninit() override;
};
