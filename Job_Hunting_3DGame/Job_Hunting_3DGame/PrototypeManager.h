#pragma once
#include "Object.h"
#include "Model3D.h"
#include "WaterMesh.h"

class PrototypeManager
{
private:
	std::unordered_map<std::string, Object*> prototypes;
public:
	/// @brief 新規プロトタイプ追加関数
	/// @param _key プロトタイプの識別用文字列
	/// @param _prototype 追加するプロトタイプの型
	void AddPrototype(const std::string& _key, Object* _prototype)
	{
		prototypes[_key] = _prototype;
	}

	/// @brief 特定のプロトタイプのクローン作成処理
	/// @param _objectID 作成したいクローンの型
	/// @return 作成したクローンの型を返します
	Object* Create(const std::string& _objectID)
	{
		// 該当するプロトタイプの登録をチェック
		if (prototypes.find(_objectID) != prototypes.end())
		{
			// クローン生成
			return prototypes[_objectID]->clone();
		}
		// 該当するプロトタイプがない
		return nullptr;
	}
};