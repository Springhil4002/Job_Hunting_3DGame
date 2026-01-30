#pragma once
#include <unordered_map>
#include <memory>
#include <string>
#include <cassert>
#include "RootSignature.h"
#include "RootSignature_Model3D.h"
#include "RootSignature_Player.h"
#include "RootSignature_Goal.h"
#include "RootSignature_WaterMesh.h"
#include "RootSignature_SeaMesh.h"
#include "RootSignature_Splash.h"
#include "RootSignature_DebugSphere.h"
#include "RootSignature_SkyBox.h"
#include "RootSignature_UI.h"
#include "Debug_Msg.h"

enum class Root_Type
{
    ROOT_TYPE_PLAYER,
    ROOT_TYPE_GOAL,
    ROOT_TYPE_WATERMESH,
    ROOT_TYPE_SEAMESH,
    ROOT_TYPE_MODEL3D,
    ROOT_TYPE_SPLASH,
    ROOT_TYPE_SPHERE,
    ROOT_TYPE_SKYBOX,    
    ROOT_TYPE_UI,
};

class RootSignatureManager
{
private:
    RootSignatureManager() = default;
    ~RootSignatureManager() = default;
    RootSignatureManager(const RootSignatureManager&) = delete;
    RootSignatureManager& operator=(const RootSignatureManager&) = delete;

    // ルートシグネチャを名前で管理
    std::unordered_map<std::string, std::shared_ptr<RootSignature>> m_RootSignatures;

public:
    static RootSignatureManager& GetInstance()
    {
        static RootSignatureManager instance;
        return instance;
    }

    template<typename T>
    std::shared_ptr<T> GetRootSignature(const std::string& _name)
    {
        auto it = m_RootSignatures.find(_name);
        if (it != m_RootSignatures.end())
        {
            DEBUG_LOG(L"RootSignatureManager:ルートシグネチャを再利用します");
            return std::dynamic_pointer_cast<T>(it->second);
        }

        auto root = std::make_shared<T>();
        if (!root->IsValid())
        {
            DEBUG_LOG_ERROR(L"RootSignatureManager:ルートシグネチャの生成に失敗");
            return nullptr;
        }

        m_RootSignatures[_name] = root;
        DEBUG_LOG(L"RootSignatureManager:ルートシグネチャを新規作成します");
        return root;
    }

    static std::string ToString(Root_Type _type)
    {
        switch (_type)
        {
        case Root_Type::ROOT_TYPE_PLAYER:     return "Player";
        case Root_Type::ROOT_TYPE_GOAL:       return "Goal";
        case Root_Type::ROOT_TYPE_WATERMESH:  return "WaterMesh";
        case Root_Type::ROOT_TYPE_SEAMESH:    return "SeaMesh";
        case Root_Type::ROOT_TYPE_MODEL3D:    return "Model3D";
        case Root_Type::ROOT_TYPE_SPLASH:     return "Splash";
        case Root_Type::ROOT_TYPE_SPHERE:     return "Sphere";
        case Root_Type::ROOT_TYPE_SKYBOX:     return "SkyBox";
        case Root_Type::ROOT_TYPE_UI:         return "UI";
        default: return "Unknown";
        }
    }

    // 型ごとの Get 関数
    std::shared_ptr<RootSignature_Player>      GetRoot_Player()         { return GetRootSignature<RootSignature_Player>(ToString(Root_Type::ROOT_TYPE_PLAYER)); }
    std::shared_ptr<RootSignature_Goal>        GetRoot_Goal()           { return GetRootSignature<RootSignature_Goal>(ToString(Root_Type::ROOT_TYPE_GOAL)); }
    std::shared_ptr<RootSignature_WaterMesh>   GetRoot_WaterMesh()      { return GetRootSignature<RootSignature_WaterMesh>(ToString(Root_Type::ROOT_TYPE_WATERMESH)); }
	std::shared_ptr<RootSignature_SeaMesh>     GetRoot_SeaMesh()        { return GetRootSignature<RootSignature_SeaMesh>(ToString(Root_Type::ROOT_TYPE_SEAMESH)); }
    std::shared_ptr<RootSignature_Model3D>     GetRoot_Model3D()        { return GetRootSignature<RootSignature_Model3D>(ToString(Root_Type::ROOT_TYPE_MODEL3D)); }
    std::shared_ptr<RootSignature_Splash>      GetRoot_Splash()         { return GetRootSignature<RootSignature_Splash>(ToString(Root_Type::ROOT_TYPE_SPLASH)); }
    std::shared_ptr<RootSignature_DebugSphere> GetRoot_DebugSphere()    { return GetRootSignature<RootSignature_DebugSphere>(ToString(Root_Type::ROOT_TYPE_SPHERE)); }
    std::shared_ptr<RootSignature_SkyBox>      GetRoot_SkyBox()         { return GetRootSignature<RootSignature_SkyBox>(ToString(Root_Type::ROOT_TYPE_SKYBOX)); }
    std::shared_ptr<RootSignature_UI>          GetRoot_UI()             { return GetRootSignature<RootSignature_UI>(ToString(Root_Type::ROOT_TYPE_UI)); }

    // enum で取得
    std::shared_ptr<RootSignature> GetRoot(Root_Type _type)
    {
        switch (_type)
        {
        case Root_Type::ROOT_TYPE_PLAYER:      return GetRoot_Player();
        case Root_Type::ROOT_TYPE_GOAL:        return GetRoot_Goal();
        case Root_Type::ROOT_TYPE_WATERMESH:   return GetRoot_WaterMesh();
		case Root_Type::ROOT_TYPE_SEAMESH:     return GetRoot_SeaMesh();
        case Root_Type::ROOT_TYPE_MODEL3D:     return GetRoot_Model3D();
        case Root_Type::ROOT_TYPE_SPLASH:      return GetRoot_Splash();
        case Root_Type::ROOT_TYPE_SPHERE:      return GetRoot_DebugSphere();
        case Root_Type::ROOT_TYPE_SKYBOX:      return GetRoot_SkyBox();
        case Root_Type::ROOT_TYPE_UI:          return GetRoot_UI();
        default:
            DEBUG_LOG(L"RootSignatureManager:不明なRoot_Typeです");
            assert(false);
            return nullptr;
        }
    }

    void Debug_RootSignatureList()
    {
        DEBUG_LOG(L"RootSignatureManager:現在の作成済みルートシグネチャ一覧");
        for (const auto& [_name, _] : m_RootSignatures)
        {
            printf("%s\n", _name.c_str());
        }
    }
};