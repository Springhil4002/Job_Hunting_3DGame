#include "ModelManager.h"
#include "Debug_New.h"

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
        printf("ModelManager:既存モデル %ls を再利用します\n",_filePath.c_str());
        return it->second;
    }
    printf("ModelManager:未知モデル %ls 保存されていません\n", _filePath.c_str());
    return nullptr;
}

std::shared_ptr<ModelData> ModelManager::LoadModel(const std::wstring& _filePath)
{
    // 既に読み込まれているか確認
    auto it = m_models.find(_filePath);
    if (it != m_models.end())
    {
        printf("ModelManager:既存モデル %ls を再利用します\n", _filePath.c_str());
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
        printf("ModelManager:モデル読み込み失敗\n");
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
            printf("ModelManager:頂点バッファの作成失敗\n");
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
            printf("ModelManager:インデックスバッファの作成失敗\n");
            return nullptr;
        }
        modelData->indexBuffers.push_back(ib);
    }
    printf("ModelManager:新規モデル %ls を読み込み成功\n", _filePath.c_str());
    m_models[_filePath] = modelData;
    return modelData;
}