#include "SceneResource.h"
#include "Debug_New.h"

SceneResourceList GetSceneResourceList(SCENE_ID _ID)
{
	SceneResourceList list;

	switch(_ID)
	{
	case SCENE_ID_TITLE:
		list.textures = {
			{ L"Assets/Texture/Color_Black.png",	TEX_TYPE::TEX_TYPE_TEXTURE2D },
			{ L"Assets/Texture/Title_Bg.png",		TEX_TYPE::TEX_TYPE_TEXTURE2D },
			{ L"Assets/Texture/Title_Logo.png",		TEX_TYPE::TEX_TYPE_TEXTURE2D },
			{ L"Assets/Texture/Title_Start.png",	TEX_TYPE::TEX_TYPE_TEXTURE2D },
			{ L"Assets/Texture/Game_controller.png", TEX_TYPE::TEX_TYPE_TEXTURE2D },
			{ L"Assets/Texture/SkyDome.dds",		 TEX_TYPE::TEX_TYPE_CUBEMAP },
			{ L"Assets/Texture/SeaMesh_Normal.png",  TEX_TYPE::TEX_TYPE_TEXTURE2D },
			{ L"Assets/Texture/SeaMesh_Normal_2.png", TEX_TYPE::TEX_TYPE_TEXTURE2D },
		};
		list.modelPaths = {
		};
		break;
	case SCENE_ID_GAME:
		list.textures = {
			{ L"Assets/Texture/SkyDome.dds",		 TEX_TYPE::TEX_TYPE_TEXTURE2D },
			{ L"Assets/Texture/Player.png",			 TEX_TYPE::TEX_TYPE_TEXTURE2D },
			{ L"Assets/Texture/Color_Red.png",		 TEX_TYPE::TEX_TYPE_TEXTURE2D },
			{ L"Assets/Texture/SkyDome.dds",		 TEX_TYPE::TEX_TYPE_CUBEMAP },
			{ L"Assets/Texture/Particle_Splash.png", TEX_TYPE::TEX_TYPE_TEXTURE2D },
			{ L"Assets/Texture/Game_if.png",		 TEX_TYPE::TEX_TYPE_TEXTURE2D },
			{ L"Assets/Texture/Number.png",			 TEX_TYPE::TEX_TYPE_TEXTURE2D},
			{ L"Assets/Texture/GO.png",				 TEX_TYPE::TEX_TYPE_TEXTURE2D },
		};
		list.modelPaths = {
			{ L"Assets/Player/Player.FBX" },
			{ L"Assets/Goal/GoalGate.fbx" },
		};
		break;
	case SCENE_ID_RESULT:
		list.textures = {
			{ L"Assets/Texture/hogehoge.png",		 TEX_TYPE::TEX_TYPE_TEXTURE2D },
			{ L"Assets/Texture/Result_Bg.png",		 TEX_TYPE::TEX_TYPE_TEXTURE2D },
			{ L"Assets/Texture/Result_Logo.png",	 TEX_TYPE::TEX_TYPE_TEXTURE2D },
			{ L"Assets/Texture/Result_ToTitle.png",	 TEX_TYPE::TEX_TYPE_TEXTURE2D },
			{},
		};
		list.modelPaths = {
		};
		break;
	default:
		printf("SceneResource:SCENE_IDÉGÉâÅ[\n");
		break;
	}
	return list;
}