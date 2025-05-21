#pragma once
#include <math.h>
#include <DirectXMath.h>
#include <unordered_map>
#include <string>
#include "System/CommonTypes.h"
//#include "Tags.h"

class Object {
protected:
	// SRT情報(姿勢情報)
	XMFLOAT3 m_Position = { 0.0f,0.0f,0.0f };
	XMFLOAT3 m_Rotation = { 0.0f,0.0f,0.0f };
	XMFLOAT3 m_Scale	= { 1.0f,1.0f,1.0f };
public:
	// タグのインスタンス
	//Tags tags;

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

	DirectX::XMVECTOR GetPos() const;
	DirectX::XMVECTOR GetRota() const;
	DirectX::XMVECTOR GetScale() const;

	void SetPos(DirectX::XMVECTOR _pos);
	void SetRota(DirectX::XMVECTOR _rota);
	void SetScale(DirectX::XMVECTOR _scale);
};