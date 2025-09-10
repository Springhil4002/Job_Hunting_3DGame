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
#include "RootSignature_Splash.h"
#include "RootSignature_DebugSphere.h"
#include "RootSignature_SkyDomeMesh.h"

enum class Root_Type
{
    ROOT_TYPE_PLAYER,
    ROOT_TYPE_GOAL,
    ROOT_TYPE_WATERMESH,
    ROOT_TYPE_MODEL3D,
    ROOT_TYPE_SPLASH,
    ROOT_TYPE_SPHERE,
    ROOT_TYPE_SKYDOME
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
            printf("RootSignatureManager:%s のルートシグネチャを再利用します\n", _name.c_str());
            return std::dynamic_pointer_cast<T>(it->second);
        }

        auto root = std::make_shared<T>();
        if (!root->IsValid())
        {
            printf("RootSignatureManager:%s の生成に失敗\n", _name.c_str());
            return nullptr;
        }

        m_RootSignatures[_name] = root;
        printf("RootSignatureManager:%s を新規生成\n", _name.c_str());
        return root;
    }

    static std::string ToString(Root_Type _type)
    {
        switch (_type)
        {
        case Root_Type::ROOT_TYPE_PLAYER:     return "Player";
        case Root_Type::ROOT_TYPE_GOAL:       return "Goal";
        case Root_Type::ROOT_TYPE_WATERMESH:  return "WaterMesh";
        case Root_Type::ROOT_TYPE_MODEL3D:    return "Model3D";
        case Root_Type::ROOT_TYPE_SPLASH:     return "Splash";
        case Root_Type::ROOT_TYPE_SPHERE:     return "Sphere";
        case Root_Type::ROOT_TYPE_SKYDOME:    return "SkyDome";
        default: return "Unknown";
        }
    }

    // 型ごとの Get 関数
    std::shared_ptr<RootSignature_Player>      GetRoot_Player()         { return GetRootSignature<RootSignature_Player>(ToString(Root_Type::ROOT_TYPE_PLAYER)); }
    std::shared_ptr<RootSignature_Goal>        GetRoot_Goal()           { return GetRootSignature<RootSignature_Goal>(ToString(Root_Type::ROOT_TYPE_GOAL)); }
    std::shared_ptr<RootSignature_WaterMesh>   GetRoot_WaterMesh()      { return GetRootSignature<RootSignature_WaterMesh>(ToString(Root_Type::ROOT_TYPE_WATERMESH)); }
    std::shared_ptr<RootSignature_Model3D>     GetRoot_Model3D()        { return GetRootSignature<RootSignature_Model3D>(ToString(Root_Type::ROOT_TYPE_MODEL3D)); }
    std::shared_ptr<RootSignature_Splash>      GetRoot_Splash()         { return GetRootSignature<RootSignature_Splash>(ToString(Root_Type::ROOT_TYPE_SPLASH)); }
    std::shared_ptr<RootSignature_DebugSphere> GetRoot_DebugSphere()    { return GetRootSignature<RootSignature_DebugSphere>(ToString(Root_Type::ROOT_TYPE_SPHERE)); }
    std::shared_ptr<RootSignature_SkyDomeMesh> GetRoot_SkyDome()        { return GetRootSignature<RootSignature_SkyDomeMesh>(ToString(Root_Type::ROOT_TYPE_SKYDOME)); }

    // enum で取得
    std::shared_ptr<RootSignature> GetRoot(Root_Type _type)
    {
        switch (_type)
        {
        case Root_Type::ROOT_TYPE_PLAYER:      return GetRoot_Player();
        case Root_Type::ROOT_TYPE_GOAL:        return GetRoot_Goal();
        case Root_Type::ROOT_TYPE_WATERMESH:   return GetRoot_WaterMesh();
        case Root_Type::ROOT_TYPE_MODEL3D:     return GetRoot_Model3D();
        case Root_Type::ROOT_TYPE_SPLASH:      return GetRoot_Splash();
        case Root_Type::ROOT_TYPE_SPHERE:      return GetRoot_DebugSphere();
        case Root_Type::ROOT_TYPE_SKYDOME:     return GetRoot_SkyDome();
        default:
            printf("RootSignatureManager:不明なRoot_Typeです\n");
            assert(false);
            return nullptr;
        }
    }

    void Debug_RootSignatureList()
    {
        printf("RootSignatureManager: 現在の生成済みルートシグネチャ一覧\n");
        for (const auto& [_name, _] : m_RootSignatures)
        {
            printf("%s\n", _name.c_str());
        }
    }
};