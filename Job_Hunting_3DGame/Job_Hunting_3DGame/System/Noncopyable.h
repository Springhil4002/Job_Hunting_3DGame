#pragma once

class NonCopyable {
public:
    /// @brief デフォルトコンストラクタ。
    NonCopyable() = default;

    // コピーコンストラクタ、コピー代入演算子の禁止
    NonCopyable(const NonCopyable&) = delete;
    NonCopyable& operator=(const NonCopyable&) = delete;
};
