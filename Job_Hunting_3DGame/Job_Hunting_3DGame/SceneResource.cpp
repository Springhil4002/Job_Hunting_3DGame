#include "SceneResource.h"
#include "Debug_New.h"
#include "Debug_Msg.h"

SceneResourceList GetSceneResourceList(SCENE_ID _ID)
{
	SceneResourceList list;

	switch(_ID)
	{
	case SCENE_ID_TITLE:
		list.textures = {
			{ L"Assets/Texture/Color_Black.png",	 TEX_TYPE::TEX_TYPE_TEXTURE2D},		// プレイヤー
			{ L"Assets/Texture/Color_Blue.png",		 TEX_TYPE::TEX_TYPE_TEXTURE2D},		// プレイヤー
			{ L"Assets/Texture/Color_White.png",	 TEX_TYPE::TEX_TYPE_TEXTURE2D},		// プレイヤー
			{ L"Assets/Texture/Color_Yellow.png",	 TEX_TYPE::TEX_TYPE_TEXTURE2D},		// プレイヤー
			{ L"Assets/Texture/Color_Red.png",		 TEX_TYPE::TEX_TYPE_TEXTURE2D},		// プレイヤー、ゴール
			{ L"Assets/Texture/Particle_Splash.png", TEX_TYPE::TEX_TYPE_TEXTURE2D },	// 水しぶきパーティクル
			{ L"Assets/Texture/SkyBox.dds",			 TEX_TYPE::TEX_TYPE_CUBEMAP },		// スカイボックス
			{ L"Assets/Texture/SeaMesh_Normal.png",  TEX_TYPE::TEX_TYPE_TEXTURE2D },	// 水面メッシュ(法線マップ1)
			{ L"Assets/Texture/SeaMesh_Normal_2.png", TEX_TYPE::TEX_TYPE_TEXTURE2D },	// 水面メッシュ(法線マップ2)
			{ L"Assets/Texture/UI_Title_Logo.png",	 TEX_TYPE::TEX_TYPE_TEXTURE2D },	// UI,ロゴ(Wave Blue)
			{ L"Assets/Texture/UI_Title_Start.png",	 TEX_TYPE::TEX_TYPE_TEXTURE2D },	// UI,タイトルスタート
		};
		list.modelPaths = {
			{ L"Assets/Player/Player.FBX" },	// モデル(水上バイク)
		};
		break;
	case SCENE_ID_GAME:
		list.textures = {
			{ L"Assets/Texture/UI_Number.png",			TEX_TYPE::TEX_TYPE_TEXTURE2D },	// UI,数字(0,1,2,3,4,5,6,7,8,9,:)
			{ L"Assets/Texture/UI_Go.png",				TEX_TYPE::TEX_TYPE_TEXTURE2D },	// UI,ロゴ(GO)
			{ L"Assets/Texture/UI_Km.png",				TEX_TYPE::TEX_TYPE_TEXTURE2D },	// UI,単位(Km)
			{ L"Assets/Texture/UI_Score.png",			TEX_TYPE::TEX_TYPE_TEXTURE2D },	// UI,単位(Score)
			{ L"Assets/Texture/UI_Game_controller.png", TEX_TYPE::TEX_TYPE_TEXTURE2D },	// UI,操作方法
		};
		list.modelPaths = {
			{ L"Assets/Goal/GoalGate.fbx" },	// モデル(鳥居)
		};
		break;
	case SCENE_ID_RESULT:
		list.textures = {
			{ L"Assets/Texture/Result_Bg.png",			TEX_TYPE::TEX_TYPE_TEXTURE2D },	// リザルト背景
			{ L"Assets/Texture/UI_MaxSpeed.png",		TEX_TYPE::TEX_TYPE_TEXTURE2D },	// UI,上限速度
			{ L"Assets/Texture/UI_Acceleration.png",	TEX_TYPE::TEX_TYPE_TEXTURE2D },	// UI,加速度
			{ L"Assets/Texture/UI_ScoreUp.png",			TEX_TYPE::TEX_TYPE_TEXTURE2D },	// UI,スコア増加
			{ L"Assets/Texture/UI_GoalCreate.png",		TEX_TYPE::TEX_TYPE_TEXTURE2D },	// UI,ゴール生成数
			{ L"Assets/Texture/UI_Level.png",			TEX_TYPE::TEX_TYPE_TEXTURE2D },	// UI,レベル
			{ L"Assets/Texture/UI_Cost.png",			TEX_TYPE::TEX_TYPE_TEXTURE2D },	// UI,コスト
			{ L"Assets/Texture/UI_Upgrade_Idle.png",	TEX_TYPE::TEX_TYPE_TEXTURE2D },	// UI,強化(黒)
			{ L"Assets/Texture/UI_CurrentSelect.png",	TEX_TYPE::TEX_TYPE_TEXTURE2D },	// UI,選択中の枠
			{ L"Assets/Texture/UI_Title.png",			TEX_TYPE::TEX_TYPE_TEXTURE2D },	// UI,タイトルへ
			{ L"Assets/Texture/UI_Replay.png",			TEX_TYPE::TEX_TYPE_TEXTURE2D },	// UI,リプレイ

		};
		list.modelPaths = {
		};
		break;
	default:
		DEBUG_LOG(L"SceneResource:SCENE_IDエラー");
		break;
	}
	return list;
}