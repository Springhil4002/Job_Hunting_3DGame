#pragma once
#include <stdio.h>
#include <set>
#include "System/Input.h"
#include "PrototypeManager.h"

#define MAX_OBJECT (1)

class BaseScene
{
protected:
	// シーンのインスタンス
	static BaseScene* sceneInstance;	
	// オブジェクトを管理するインスタンス
	std::set<Object*> objectInstance;
public:
	// 入力系インスタンス
	static Input input;
	
	// オブジェクト生成に関するインスタンス
	PrototypeManager* prototypeManager = new PrototypeManager;

	/// @brief コンストラクタ
	BaseScene() {
		delete sceneInstance;
		sceneInstance = this;
	}

	/// @brief デストラクタ
	virtual ~BaseScene() {
		sceneInstance = nullptr;
		delete prototypeManager;
	}

	/// @brief 初期化処理
	virtual void Init() = 0;
	/// @brief 更新処理
	virtual void Update() = 0;
	/// @brief 描画処理
	virtual void Draw() = 0;
	/// @brief 終了処理
	virtual void Uninit() = 0;

	/// @brief 入力取得処理関数
	static void Input();

	/// @brief オブジェクトの生成関数
	/// @param _ObjectID 生成したいオブジェクトの識別子
	/// @return 生成したいオブジェクトを返します
	virtual Object* CreateObj(const std::string& _ObjectID) = 0;

	/// @brief 現在のシーンインスタンスを取得する関数
	/// @return 現在のシーンインスタンスを返します
	static BaseScene* GetInstance();

	/// @brief 現時点の存在している全てのオブジェクトを取得する関数
	/// @return 現時点の存在している全てのオブジェクトを返します
	std::set<Object*>* GetObjects();

	/// @brief 特定の型をもつオブジェクトを取得する関数
	/// @tparam T 取得したい型
	/// @return 取得したい型をもつ全てのオブジェクト
	template <class T>
	std::vector<T*> GetObjects()
	{
		// 取得したい型をもつオブジェクトを格納する配列
		std::vector<T*> objects;

		// 存在している全てのオブジェクトから探索
		for (auto& obj : objectInstance)
		{
			// 型一致なら配列に追加
			if (T* castedObj = dynamic_cast<T*>(obj)) {
				objects.push_back(castedObj);
			}
		}
		return objects;
	}
};

