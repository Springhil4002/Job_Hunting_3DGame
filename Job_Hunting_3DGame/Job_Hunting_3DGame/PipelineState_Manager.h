#pragma once
#include <memory>
#include <unordered_map>
#include <string>
#include "PipelineState_General.h"
#include "PipelineState_Splash.h"
#include "PipelineState_SkyBox.h"
#include "PipelineState_DebugSphere.h"
#include "PipelineState_UI.h"

enum class PSO_Type
{
	PSO_TYPE_PLAYER,
	PSO_TYPE_GOAL,
	PSO_TYPE_WATERMESH,
	PSO_TYPE_MODEL3D,
	PSO_TYPE_SPLASH,
	PSO_TYPE_SPHERE,
	PSO_TYPE_SKYBOX,
	PSO_TYPE_UI,
};

class PipelineState_Manager
{
private:
	PipelineState_Manager() {}
	~PipelineState_Manager() {}

	PipelineState_Manager(const PipelineState_Manager&) = delete;
	PipelineState_Manager& operator=(const PipelineState_Manager&) = delete;

	// 各種PSOをマップ保持
	// 汎用PSO(Player,WaterMesh,Goal)
	std::unordered_map<std::string, std::unique_ptr<PipelineState_General>> m_PSO_General;
	// パーティクル(水しぶき)PSO		
	std::unordered_map<std::string, std::unique_ptr<PipelineState_Splash>> m_PSO_Splash;	
	// 球体メッシュ用PSO
	std::unordered_map<std::string, std::unique_ptr<PipelineState_DebugSphere>> m_PSO_DebugSphere;	
	// スカイドーム用PSO
	std::unordered_map<std::string, std::unique_ptr<PipelineState_SkyBox>> m_PSO_SkyBox;
	// UI用PSO
	std::unordered_map<std::string, std::unique_ptr<PipelineState_UI>> m_PSO_UI;

	/// @brief 名前でパイプラインステートを取得、無ければ生成するテンプレート関数
	/// @tparam T 特定のPSOの型
	/// @param _psoMap 特定のPSOマップ
	/// @param _key 名前
	/// @return 名前に対応しているPSO
	template <typename T>
	T* GetPSOCreate(std::unordered_map<std::string, std::unique_ptr<T>>& _psoMap, const std::string& _key)
	{
		auto it = _psoMap.find(_key);
		// キーに対応するPSOが存在するか確認
		if (it != _psoMap.end())
		{
			printf("PSO_Manager:既存のPSOを返します\n");
			return it->second.get();
		}
			
		// 無ければ作成して追加
		auto newPso = std::make_unique<T>();
		T* psoPtr = newPso.get();
		_psoMap[_key] = std::move(newPso);
		printf("PSO_Manager:新規のPSOを生成しました\n");
		return psoPtr;
	}

public:
	/// @brief インスタンス取得関数
	/// @return PSO_Managerのインスタンス
	static PipelineState_Manager& GetInstance()
	{
		static PipelineState_Manager instance;
		return instance;
	}
	
	static std::string ToString(PSO_Type _type)
	{
		switch(_type)
		{
		case PSO_Type::PSO_TYPE_PLAYER:		return "Player";
		case PSO_Type::PSO_TYPE_GOAL:		return "Goal";
		case PSO_Type::PSO_TYPE_WATERMESH:	return "WaterMesh";
		case PSO_Type::PSO_TYPE_MODEL3D:	return "Model3D";
		case PSO_Type::PSO_TYPE_SPLASH:		return "Splash";
		case PSO_Type::PSO_TYPE_SPHERE:		return "Sphere";
		case PSO_Type::PSO_TYPE_SKYBOX:		return "SkyBox";
		case PSO_Type::PSO_TYPE_UI:			return "UI";
		default: return "Unknown";
		}
	}
	// 各種PSOの取得関数 
	PipelineState_General* GetPSO_General(const std::string& _key) { return GetPSOCreate(m_PSO_General, _key); }
	PipelineState_Splash* GetPSO_Splash(const std::string& _key) { return GetPSOCreate(m_PSO_Splash, _key); }
	PipelineState_DebugSphere* GetPSO_DebugSphere(const std::string& _key) { return GetPSOCreate(m_PSO_DebugSphere, _key); }
	PipelineState_SkyBox* GetPSO_SkyBox(const std::string& _key) { return GetPSOCreate(m_PSO_SkyBox, _key); }
	PipelineState_UI* GetPSO_UI(const std::string _key) { return GetPSOCreate(m_PSO_UI, _key); }
	
	PipelineState_General* GetPSO_General(PSO_Type _type) { return GetPSO_General(ToString(_type)); }
	PipelineState_Splash* GetPSO_Splash(PSO_Type _type) { return GetPSO_Splash(ToString(_type)); }
	PipelineState_DebugSphere* GetPSO_DebugSphere(PSO_Type _type) { return GetPSO_DebugSphere(ToString(_type)); }
	PipelineState_SkyBox* GetPSO_SkyBox(PSO_Type _type) { return GetPSO_SkyBox(ToString(_type)); }
	PipelineState_UI* GetPSO_UI(PSO_Type _type) { return GetPSO_UI(ToString(_type)); }
};