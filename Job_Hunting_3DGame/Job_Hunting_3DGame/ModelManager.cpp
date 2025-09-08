#include "ModelManager.h"
#include "Debug_New.h"

ModelManager& ModelManager::GetInstance()
{
    static ModelManager instance;
    return instance;
}

// モデルを取得（まだ読み込まれていなければ読み込む）
std::shared_ptr<ModelData> ModelManager::GetModel(const std::wstring& _filePath)
{
    // 既に読み込まれているか確認
    auto it = m_models.find(_filePath);
    if (it != m_models.end())
    {
        printf("ModelManager:既存モデルを返します\n");
        return it->second; // 登録済みのモデルを返す
    }

    printf("ModelManager:新規モデルを読み込みます\n");
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

    m_models[_filePath] = modelData;
    return modelData;
}