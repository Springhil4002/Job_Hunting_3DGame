#pragma once
#include <stdexcept>
#include <unordered_map>
#include <string>
#include <vector>
#include <memory>
#include "AssimpLoader.h"
#include "System/SharedStruct.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"

struct ModelData
{
    std::vector<Mesh> meshes;
    std::vector<std::shared_ptr<VertexBuffer>> vertexBuffers;
    std::vector<std::shared_ptr<IndexBuffer>> indexBuffers;
};

class ModelManager
{
private:
    /// @brief コンストラクタ
    ModelManager() = default;
    /// @brief デストラクタ
    ~ModelManager() = default;
    // コピーコンストラクタと代入演算子を削除してシングルトンを強制
    ModelManager(const ModelManager&) = delete;
    ModelManager& operator=(const ModelManager&) = delete;
    /// @brief モデルデータを管理するマップ
    std::unordered_map<std::wstring, std::shared_ptr<ModelData>> m_models;
public:
    static ModelManager& GetInstance()
    {
        static ModelManager instance;
        return instance;
    }

    // モデルを取得（まだ読み込まれていなければ読み込む）
    std::shared_ptr<ModelData> GetModel(const std::wstring& _filePath)
    {
		// 既に読み込まれているか確認
        auto it = m_models.find(_filePath);
        if (it != m_models.end())
        {
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

        m_models[_filePath] = modelData;
        return modelData;
    }

};
