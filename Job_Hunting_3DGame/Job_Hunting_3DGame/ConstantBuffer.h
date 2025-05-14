#pragma once
#include <d3dx12.h>
#include "ComPtr.h"

class ConstantBuffer
{
private:
    // 定数バッファ生成に成功したかのフラグ
    bool m_IsValid = false; 
    // 定数バッファ
    ComPtr<ID3D12Resource> m_pBuffer; 
    // 定数バッファビューの設定
    D3D12_CONSTANT_BUFFER_VIEW_DESC m_Desc; 
    void* m_pMappedPtr = nullptr;

    // コピー禁止
    ConstantBuffer(const ConstantBuffer&) = delete;
    void operator = (const ConstantBuffer&) = delete;
public:
    /// @brief  コンストラクタで定数バッファを生成
    /// @param size バッファサイズ
    ConstantBuffer(size_t _size); 
    /// @brief バッファ生成に成否判定処理
    /// @return バッファ生成の成否を返します
    bool IsValid(); 

    /// @brief バッファのGPU上のアドレス取得処理
    /// @return バッファのGPU上のアドレスを返します
    D3D12_GPU_VIRTUAL_ADDRESS GetAddress() const; 
    /// @brief 定数バッファビュー取得処理
    /// @return 定数バッファビューを返す
    D3D12_CONSTANT_BUFFER_VIEW_DESC ViewDesc(); 
    /// @brief 定数バッファにマッピングされたポインタを取得
    /// @return 定数バッファにマッピングされたポインタを返す
    void* GetPtr() const; 

    // テンプレート
    template<typename T>
    T* GetPtr()
    {
        return reinterpret_cast<T*>(GetPtr());
    }
};
