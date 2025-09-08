#pragma once
#include "PipelineState_General.h"
#include "PipelineState_Splash.h"
#include "PipelineState_SkyDomeMesh.h"
#include "PipelineState_DebugSphere.h"
#include <memory>
#include <unordered_map>
#include <string>

enum class PSO_Type
{
	PSO_TYPE_GENERAL,
	PSO_TYPE_SPLASH,
	PSO_TYPE_SPHERE,
	PSO_TYPE_SKYDOME,
	PSO_TYPE_WATERMESH
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
	std::unordered_map<std::string, std::unique_ptr<PipelineState_SkyDomeMesh>> m_PSO_SkyDomeMesh;

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
	/// @brief 全てPSOの生成がされてかチェックする関数
	/// @param _key PSOの使用者の識別キー
	/// @param _type PSOの種類
	/// @return 生成の成否
	bool IsPSOCreated(const std::string& _key, const PSO_Type _type)
	{
		switch(_type)
		{
		case PSO_Type::PSO_TYPE_GENERAL: return m_PSO_General.count(_key) > 0;
		case PSO_Type::PSO_TYPE_SPLASH: return m_PSO_Splash.count(_key) > 0;
		case PSO_Type::PSO_TYPE_SPHERE: return m_PSO_DebugSphere.count(_key) > 0;
		case PSO_Type::PSO_TYPE_SKYDOME: return m_PSO_SkyDomeMesh.count(_key) > 0;
		default: return false;
		}
	}
	// 各種PSOの取得関数 
	PipelineState_General* GetPSO_General(const std::string& _key) { return GetPSOCreate(m_PSO_General, _key); }
	PipelineState_Splash* GetPSO_Splash(const std::string& _key) { return GetPSOCreate(m_PSO_Splash, _key); }
	PipelineState_DebugSphere* GetPSO_DebugSphere(const std::string& _key) { return GetPSOCreate(m_PSO_DebugSphere, _key); }
	PipelineState_SkyDomeMesh* GetPSO_SkyDomeMesh(const std::string& _key) { return GetPSOCreate(m_PSO_SkyDomeMesh, _key); }
};