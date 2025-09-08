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

// モデルデータ構造体
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
    // インスタンス取得関数
    static ModelManager& GetInstance();
    // モデルを取得（まだ読み込まれていなければ読み込む）
    std::shared_ptr<ModelData> GetModel(const std::wstring& _filePath);
};
