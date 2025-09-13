#pragma once
#include <stdio.h>
#include <set>
#include "System/Input.h"
#include "PrototypeManager.h"

#define MAX_OBJECT (65)

class BaseScene
{
protected:
	// シーンのインスタンス
	static BaseScene* sceneInstance;	
	// オブジェクトを管理するインスタンス
	std::vector<std::unique_ptr<Object>> objectInstance;
	// 入力系インスタンス
	static Input input;
	// オブジェクト生成に関するインスタンス
	std::unique_ptr<PrototypeManager> prototypeManager;
	// 画面横幅
	float screenWidth = 1920.0f;
	// 画面縦幅
	float screenHeight = 1080.0f;
public:
	/// @brief コンストラクタ
	BaseScene() {
		sceneInstance = this;
		prototypeManager = std::make_unique<PrototypeManager>();
	}

	/// @brief デストラクタ
	virtual ~BaseScene() {
		sceneInstance = nullptr;
		objectInstance.clear();
	}

	/// @brief 初期化処理
	virtual void Init() = 0;
	/// @brief 更新処理
	virtual void Update(float _deltaTime) = 0;
	/// @brief 描画処理
	virtual void Draw() = 0;
	/// @brief 終了処理
	virtual void Uninit() = 0;
	/// @brief ImGuiの描画処理
	virtual void Draw_ImGui() = 0;

	/// @brief 入力取得処理関数
	static void Input();

	/// @brief オブジェクトの生成関数
	/// @param _ObjectID 生成したいオブジェクトの識別子
	/// @return 生成したいオブジェクトを返します
	virtual Object* CreateObj(const std::string& _ObjectID) = 0;

	/// @brief 現在のシーンインスタンスを取得する関数
	/// @return 現在のシーンインスタンスを返します
	static BaseScene* GetInstance();

	/// @brief シーン内のオブジェクトの全取得関数
	/// @return シーン内で追加した全体のオブジェクトを返します
	std::vector<std::unique_ptr<Object>>& GetAllObjects();

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
			if (T* obj = dynamic_cast<T*>(obj.get())) {
				objects.push_back(obj);
			}
		}
		return objects;
	}

	template <class T>
	T* FindByTag(const std::string& _tag)
	{
		for (auto& obj : objectInstance)
		{
			if (obj->m_tags.SearchTag(_tag))
			{
				return dynamic_cast<T*>(obj.get());
			}
		}
		return nullptr;
	}

	template<typename T>
	T* FindByType()
	{
		for (auto& obj : objectInstance)
		{
			if (auto casted = dynamic_cast<T*>(obj.get()))
			{
				return casted;
			}
		}
		return nullptr;
	}
};