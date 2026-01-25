#pragma once
#include "UI.h"
#include <array>

class UI_Score : public UI
{
private:
	struct UV_Rect
	{
		float x0;
		float y0;
		float x1;
		float y1;
	};
	int m_Score = 0;
	std::array<UV_Rect, 11> m_UV_Numbers = {};
	// 8桁スコア表示用の配列
	std::array<std::unique_ptr<UI>, 8> m_ScoreDigits;
	// "SCORE"表示用のUI
	std::unique_ptr<UI> m_ScoreUI;
public:
	std::unique_ptr<Object> clone() const override;

	/// @brief 初期化関数
	/// @param _cameraUI	2Dカメラ 
	/// @param _width		横幅
	/// @param _height		縦幅
	/// @param _numTexPath	数字テクスチャ
	/// @param _unitTexPath SCOREテクスチャ
	/// @return 生成の成否
	bool Init(Camera2D* _cameraUI, float _width, float _height,
		const std::wstring& _numTexPath,
		const std::wstring& _scoreTexPath);
	/// @brief 更新関数
	void Update() override;
	/// @brief 描画関数
	void Draw() override;
	/// @brief 終了関数
	void UnInit() override;

	/// @brief 特定の数値をUV座標に変換して設定する関数
	void SetDigit(UI* _digit, int _number);
};