#pragma once

class NonCopyable {
public:
    /// @brief �f�t�H���g�R���X�g���N�^�B
    NonCopyable() = default;

    // �R�s�[�R���X�g���N�^�A�R�s�[������Z�q�̋֎~
    NonCopyable(const NonCopyable&) = delete;
    NonCopyable& operator=(const NonCopyable&) = delete;
};
