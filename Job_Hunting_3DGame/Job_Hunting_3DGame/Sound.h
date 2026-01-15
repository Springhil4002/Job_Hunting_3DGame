#pragma once
#include <xaudio2.h>
#include <vector>

enum SOUND_LABEL {
	// BGM
	SOUND_LABEL_BGM_001 = 0,	
	SOUND_LABEL_BGM_002,		
	SOUND_LABEL_BGM_003,

	// SE
	SOUND_LABEL_SE_001,
	SOUND_LABEL_SE_002,
	SOUND_LABEL_SE_003,

	SOUND_LABEL_MAX		// サウンドファイルの総数
};

class Sound
{
private:
	struct SoundParam
	{
		LPCSTR fileName;	// サウンドファイル
		bool loopFlag;		// ループフラグ BGMならtrue,SEならfalse
	};

	// 登録するサウンドファイルのパス
	SoundParam m_SoundParams[SOUND_LABEL_MAX] =
	{
		{"Assets/Sound/BGM/"		,true},
		{"Assets/Sound/BGM/"		,true},
		{"Assets/Sound/BGM/"		,true},

		{"Assets/Sound/SE/"			,true},
		{"Assets/Sound/SE/"			,true},
		{"Assets/Sound/SE/"			,true},
	};

	// XAudioインターフェースへのポインタ
	IXAudio2* m_pXAudio2 = NULL;
	// マスターボイスへのポインタ
	IXAudio2MasteringVoice* m_pMasteringVoice = NULL;
	// 各サウンドのソースボイスへのポインタ
	IXAudio2SourceVoice* m_pSourceVoice[SOUND_LABEL_MAX] = {};
	// 各サウンドのWAVフォーマット情報
	WAVEFORMATEXTENSIBLE m_WavFx[SOUND_LABEL_MAX] = {};
	// 各サウンドのバッファ情報
	XAUDIO2_BUFFER m_Buffer[SOUND_LABEL_MAX] = {};
	// 各サウンドのデータバッファ
	std::vector<BYTE> m_DataBuffer[SOUND_LABEL_MAX] = {};
	
	// 指定されたチャンク識別子を持つデータチャンクをファイル内から見つける
	HRESULT FindChunk(HANDLE, DWORD, DWORD&, DWORD&);
	// 指定された位置からデータチャンクを読み取る
	HRESULT ReadChunkData(HANDLE, void*, DWORD, DWORD);
public:
	// 初期化処理
	HRESULT Init();
	// 終了処理
	void UnInit();

	// 引数で指定したサウンドを再生する
	void Play(SOUND_LABEL _label);
	// 引数で指定したサウンドを停止する
	void Stop(SOUND_LABEL _label);
	// 引数で指定したサウンドの再生を再開する
	void Resume(SOUND_LABEL _label);
};