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
    /// @brief モデルマネージャーのインスタンス取得関数
    /// @return モデルマネージャーのインスタンスを返します
    static ModelManager& GetInstance();
    /// @brief 指定したファイルパスから保存したモデルデータを取得する関数
    /// @param _filePath 取得したいモデルデータファイルのパス
    /// @return 取得したいモデルデータ
    std::shared_ptr<ModelData> GetModel(const std::wstring& _filePath);
    /// @brief 指定したファイルパスのモデルデータを読み込み・保存する関数
    /// @param _filePath 読み込み・保存したいモデルデータのファイルパス
    /// @return 指定したモデルデータ
    std::shared_ptr<ModelData> LoadModel(const std::wstring& _filePath);
};