#include "SceneResource.h"
#include "Debug_New.h"

SceneResourceList GetSceneResourceList(SCENE_ID _ID)
{
	SceneResourceList list;

	switch(_ID)
	{
	case SCENE_ID_TITLE:
		list.textures = {
			{ L"Assets/Texture/Color_Black.png",	 TEX_TYPE::TEX_TYPE_TEXTURE2D },
			{ L"Assets/Texture/Title.png",			 TEX_TYPE::TEX_TYPE_TEXTURE2D },
		};
		list.modelPaths = {
			{},
			{},
		};
		break;
	case SCENE_ID_GAME:
		list.textures = {
			{ L"Assets/Texture/SkyDome.dds",		 TEX_TYPE::TEX_TYPE_TEXTURE2D },
			{ L"Assets/Texture/Player.png",			 TEX_TYPE::TEX_TYPE_TEXTURE2D },
			{ L"Assets/Texture/Color_Red.png",		 TEX_TYPE::TEX_TYPE_TEXTURE2D },
			{ L"Assets/Texture/SkyDome.dds",		 TEX_TYPE::TEX_TYPE_CUBEMAP },
			{ L"Assets/Texture/Particle_Splash.png", TEX_TYPE::TEX_TYPE_TEXTURE2D },
			{ L"Assets/Texture/hogehoge.png",		 TEX_TYPE::TEX_TYPE_TEXTURE2D },
		};
		list.modelPaths = {
			{ L"Assets/Player/Player.FBX" },
			{ L"Assets/Goal/GoalGate.fbx" },
		};
		break;
	case SCENE_ID_RESULT:
		list.textures = {
			{},
			{},
		};
		list.modelPaths = {
			{},
			{},
		};
		break;
	default:
		printf("SceneResource:SCENE_IDÉGÉâÅ[\n");
		break;
	}
	return list;
}