#pragma once
#include "UI.h"
#include "PlayerController.h"
#include <array>

class UI_Speed : public UI
{
private:
	struct UV_Rect 
	{
		float x0;
		float y0;
		float x1;
		float y1;
	};

	// 数字の配列
	std::array<UV_Rect, 11> m_UV_Numbers = {};
	// 3桁速度表示用の配列
	std::array<std::unique_ptr<UI>, 3> m_SpeedDigits;
	// 速度単位表示用のUI
	std::unique_ptr<UI> m_UnitUI;
	// 参照用のPlayerController変数
	PlayerController* m_PlayerController = nullptr;
public:
	/// @brief クローン関数
	std::unique_ptr<Object> clone() const override;

	/// @brief 初期化関数
	/// @param _cameraUI　カメラUI 
	/// @param _width	横幅
	/// @param _height	縦幅
	/// @param _numTexPath	数字テクスチャ
	/// @param _unitTexPath 単位テクスチャ
	/// @return 初期化成功の成否
	bool Init(Camera2D* _cameraUI, float _width, float _height,
		const std::wstring& _numTexPath,
		const std::wstring& _unitTexPath);
	/// @brief 更新関数
	void Update() override;
	/// @brief 描画関数
	void Draw() override;
	/// @brief 終了関数
	void UnInit() override;

	/// @brief 参照用PlayerControllerを設定する関数
	void Set_PlayerController(PlayerController* _playerController);
	/// @brief 特定の数値をUV座標に変換して設定する関数
	void SetDigit(UI* _digit, int _number);
};