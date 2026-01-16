#pragma once
#include "Object.h"
#include "Model3D.h"
#include "WaterMesh.h"
#include "SeaMesh.h"
#include "SkyBox.h"
#include "Player.h"
#include "Goal.h"
#include "UI.h"
#include "UI_Fade.h"
#include "UI_Flash.h"
#include "UI_Timer.h"
#include "UI_Speed.h"

class PrototypeManager
{
private:
	std::unordered_map<std::string, std::unique_ptr<Object>> prototypes;
public:
	~PrototypeManager() = default;

	/// @brief 新規プロトタイプ追加関数
	/// @param _key プロトタイプの識別用文字列
	/// @param _prototype 追加するプロトタイプの型
	void AddPrototype(const std::string& _key, std::unique_ptr<Object> _prototype)
	{
		prototypes[_key] = std::move(_prototype);
	}

	/// @brief 特定のプロトタイプのクローン作成処理
	/// @param _objectID 作成したいクローンの型
	/// @return 作成したクローンを返します
	std::unique_ptr<Object> Create(const std::string& _objectID)
	{
		auto it = prototypes.find(_objectID);
		if (it != prototypes.end())
		{
			// clone()でunique_ptrを返す
			return it->second->clone();
		}
		return nullptr;
	}
};