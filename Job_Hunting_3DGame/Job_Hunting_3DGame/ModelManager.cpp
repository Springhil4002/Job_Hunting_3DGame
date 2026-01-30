#include "ModelManager.h"
#include "Debug_New.h"
#include "Debug_Msg.h"

ModelManager& ModelManager::GetInstance()
{
    static ModelManager instance;
    return instance;
}

std::shared_ptr<ModelData> ModelManager::GetModel(const std::wstring& _filePath)
{
    auto it = m_models.find(_filePath);
    if (it != m_models.end())
    {
        DEBUG_LOG(L"ModelManager:既存モデル ({}) を再利用します ", _filePath);
        return it->second;
    }
    DEBUG_LOG(L"ModelManager:未知モデル ({}) 保存されていません ", _filePath);
    return nullptr;
}

std::shared_ptr<ModelData> ModelManager::LoadModel(const std::wstring& _filePath)
{
    // 既に読み込まれているか確認
    auto it = m_models.find(_filePath);
    if (it != m_models.end())
    {
        DEBUG_LOG(L"ModelManager:既存モデル ({}) を再利用します ", _filePath);
        return it->second; // 登録済みのモデルを返す
    }

    // 無いなら新規登録
    auto modelData = std::make_shared<ModelData>();
    ImportSettings importSetting = {
        _filePath.c_str(),
        modelData->meshes,
        false,
        false };
    AssimpLoader loader;
    if (!loader.Load(importSetting))
    {
        DEBUG_LOG_ERROR(L"ModelManager:モデル読み込み失敗 ({})", _filePath);
        return nullptr;
    }

    for (auto& mesh : modelData->meshes)
    {
        // 頂点バッファの作成
        auto vb = std::make_shared<VertexBuffer>(
            sizeof(Vertex) * mesh.Vertices.size(),
            sizeof(Vertex),
            mesh.Vertices.data()
        );
        if (!vb->IsValid())
        {
            DEBUG_LOG_ERROR(L"ModelManager:頂点バッファの作成失敗");
            return nullptr;
        }
        modelData->vertexBuffers.push_back(vb);

        // インデックスバッファの作成
        auto ib = std::make_shared<IndexBuffer>(
            sizeof(uint32_t) * mesh.Indices.size(),
            mesh.Indices.data()
        );
        if (!ib->IsValid())
        {
            DEBUG_LOG_ERROR(L"ModelManager:インデックスバッファの作成失敗");
            return nullptr;
        }
        modelData->indexBuffers.push_back(ib);
    }
    DEBUG_LOG(L"ModelManager:新規モデル ({}) 読み込み成功", _filePath);
    m_models[_filePath] = modelData;
    return modelData;
}