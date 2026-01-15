#pragma once
#include "Sound.h"

class SoundManager
{
private:
	Sound m_Sound;		// サウンドのインスタンス
public:
	HRESULT Init();		// 初期化処理
	void UnInit();		// 終了処理

	void Play(SOUND_LABEL _label);			// サウンド再生
	void Stop(SOUND_LABEL _label);			// サウンド停止
	void Resume(SOUND_LABEL _label);		// サウンドの再生再開
};