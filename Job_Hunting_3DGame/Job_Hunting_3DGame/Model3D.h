#pragma once
#include "Object.h"

class Model3D : public Object
{
private:
public:
	// クローンメソッド
	Object* clone() const override;

	/// @brief 初期化処理
	/// @return 初期化処理の成否を返します
	bool Init()		override;
	/// @brief 更新処理
	void Update()	override;
	/// @brief 描画処理
	void Draw()		override;
	/// @brief 終了処理
	void Uninit()	override;
};

extern Model3D* g_Model3D;