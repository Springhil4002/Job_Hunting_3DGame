#pragma once
#include "UI.h"
#include <vector>
#include <string>

class UI_Shop : public UI
{
private:
public:
	/// @brief クローン関数
	std::unique_ptr<Object> clone() const override;

	/// @brief 更新関数
	void Update() override;
	/// @brief 描画関数
	void Draw() override;
	/// @brief 終了関数
	void UnInit() override;
};

