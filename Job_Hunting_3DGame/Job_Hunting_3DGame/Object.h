#pragma once
#include <math.h>
#include <DirectXMath.h>
#include <unordered_map>
#include <string>
#include "System/CommonTypes.h"
#include "Tags.h"

class Object {
protected:
	// SRT情報(姿勢情報)
	XMFLOAT3 m_Position = { 0.0f,0.0f,0.0f };
	XMFLOAT3 m_Rotation = { 0.0f,0.0f,0.0f };
	XMFLOAT3 m_Scale	= { 1.0f,1.0f,1.0f };
public:
	// タグのインスタンス
	Tags m_tags;

	Object() = default;
	~Object() = default;

	/// @brief オブジェクトクローン関数
	/// @return オブジェクトのポインタを返します
	virtual Object* clone() const = 0;	

	/// @brief 初期化処理
	virtual bool Init() { return true; };
	/// @brief 更新処理
	virtual void Update() {};
	/// @brief 描画処理
	virtual void Draw()   {};
	/// @brief 終了処理
	virtual void Uninit() {};

	/// @brief 座標取得関数
	/// @return 座標を返します
	DirectX::XMVECTOR GetPos() const;
	/// @brief 回転取得関数
	/// @return 回転を返します
	DirectX::XMVECTOR GetRota() const;
	/// @brief スケール取得関数
	/// @return スケールを返します
	DirectX::XMVECTOR GetScale() const;

	/// @brief 座標設定関数
	/// @param _pos 例:DirectX::XMVECTOR(0.0f,0.0f,0.0f)
	void SetPos(DirectX::XMVECTOR _pos);
	/// @brief 回転設定関数
	/// @param _rota 例:DirectX::XMVECTOR(0.0f,0.0f,0.0f)
	void SetRota(DirectX::XMVECTOR _rota);
	/// @brief スケール設定関数
	/// @param _scale 例:DirectX::XMVECTOR(0.0f,0.0f,0.0f)
	void SetScale(DirectX::XMVECTOR _scale);
};