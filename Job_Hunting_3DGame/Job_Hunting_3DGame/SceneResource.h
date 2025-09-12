#pragma once
#include <string>
#include <vector>
#include "SCENE_ID.h"

enum class TEX_TYPE 
{ 
	TEX_TYPE_TEXTURE2D,
	TEX_TYPE_CUBEMAP
};

struct SceneTexture
{
	std::wstring path;
	TEX_TYPE type;
};

struct SceneResourceList 
{
	std::vector<SceneTexture> textures;
	std::vector<std::wstring> modelPaths;
};

/// @brief シーン内で使うリソース一覧を取得する関数
/// @param _ID シーン識別ID
/// @return リソース一覧
SceneResourceList GetSceneResourceList(SCENE_ID _ID);