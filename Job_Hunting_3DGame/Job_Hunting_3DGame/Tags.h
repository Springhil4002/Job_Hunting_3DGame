#pragma once
#include<string>
#include<set>
#include<vector>
// 符号を付与、削除、検索できる
class Tags
{
private:
	// タグを格納する配列
	std::set<std::string> m_tags;
public:	
	/// @brief コンストラクタ
	Tags() = default;
	/// @brief デストラクタ
	~Tags() = default;

	std::set<std::string>::const_iterator begin() const;
	std::set<std::string>::const_iterator end() const;

	/// @brief タグの追加関数
	/// @param _tag 追加したいタグ名
	void AddTag(const std::string _tag);
	/// @brief タグの指定削除関数
	/// @param _tag 消したいタグ名
	void RemoveTag(const std::string _tag);		
	/// @brief タグを探す関数
	/// @param _tag 探したいタグ名
	/// @return 合致するタグのオブジェクトの数を返します
	bool SearchTag(const std::string _tag) const;	
	/// @brief タグの全削除
	void ClearTags();								
};

