cbuffer Transform : register(b0)
{
    float4x4 world;     // ワールド行列
    float4x4 view;      // ビュー行列
    float4x4 proj;      // 投影行列
    float time;         // 時間
    float3 cameraPos;   // カメラ位置
    float pad1;         // パディング
};

cbuffer LightParams : register(b2)
{
    float3 lightDir;    // ライト方向
    float envStrength;  // 環境光の強さ
    float4 lightColor;  // ライトカラー
};

cbuffer WaterParams : register(b3)
{
    float2 fbmScale;        // FBMノイズのスケール(粗さ)
    float fbmGain;          // FBMのゲイン(オクターブ間の振幅減少率)
    float fbmLacunarity;    // FBMのラキューナリティ(オクターブ間の周波数の増加率)
    int fbmOctaves;         // FBMのオクターブ数(ディテールのレベル)
    float normalPerturb;    // 法線摂動の強さ
    float foamThreshold;    // 泡が発生する閾値
    float foamIntensity;    // 泡の色の強さ
    float4 shallowColor;    // 浅い部分の水の色
    float4 deepColor;       // 深い部分の色
};

struct PS_IN
{
    float4 svpos : SV_POSITION;     // 変換された座標
    float4 color : COLOR;           // 変換されたカラー
    float2 uv : TEXCOORD;           // 変換されたUV
    float3 normal : NORMAL;         // Gerstner波で変位したワールド空間の法線
    float3 worldPos : POSITION1;    // Gerstner波で変位したワールド空間の座標
};

SamplerState smp : register(s0);        // サンプラー
TextureCube skyCube : register(t3);     // 環境マップ

// 簡易乱数関数
float hash(float2 p)
{
    p = frac(p * float2(123.34, 456.21));
    p += dot(p, p + 78.233);
    return frac(p.x * p.y);
}

// パーリンノイズ関数(補間を使って連続的なノイズを生成)  
float noise(float2 p)
{
    float2 i = floor(p);    // 整数部
    float2 f = frac(p);     // 小数部
    
    // 四隅のハッシュ値を計算
    float a = hash(i);
    float b = hash(i + float2(1, 0));
    float c = hash(i + float2(0, 1));
    float d = hash(i + float2(1, 1));
    
    // 補間曲線
    float2 u = f * f * (3.0 - 2.0 * f);
    // x→y方向に線形補間
    return lerp(lerp(a, b, u.x), lerp(c, d, u.x), u.y);
}

// FBMノイズ関数(複数のオクターブを合成してディテールを調整)
float fbm(float2 p, int octaves, float lacunarity, float gain)
{
    float value = 0.0;
    float amp = 0.5;    // 初期振幅
    float2 q = p;       // 初期座標
    [unroll]
    for (int i = 0; i < 10; ++i)
    {
        if (i >= octaves)
            break;
        // ノイズを加算
        value += amp * noise(q);
        // 周波数をあげる
        q = q * lacunarity + float2(37.1, 17.7);
        // 振幅を減衰させる
        amp *= gain;
    }
    return value;
}

float4 PS_Main(PS_IN pin) : SV_TARGET
{
    // ライティング用ベクトルを準備
    float3 N = normalize(pin.normal);               // 頂点シェーダーから渡された法線
    float3 L = normalize(-lightDir);                // ライトベクトル
    float3 V = normalize(cameraPos - pin.worldPos); // 視線ベクトル
    
    float NdotL = saturate(dot(N, L));              

    // FBMノイズの座標とアニメーション
    float2 st = pin.worldPos.xz * fbmScale + time * 0.05;
    int oct = max(1, fbmOctaves);
    
    // ノイズを3つ生成
    float n0 = fbm(st, oct, fbmLacunarity, fbmGain);                // U方向の摂動用ノイズ
    float n1 = fbm(st * 2.31 + 11.7, oct, fbmLacunarity, fbmGain);  // V方向の摂動用ノイズ
    float nH = fbm(st * 4.71 + 31.1, oct, fbmLacunarity, fbmGain);  // 泡立ち用ノイズ

    // 接空間を構築
    float3 up = float3(0, 1, 0);                // ワールド空間のY軸
    float3 T = normalize(cross(up, N));         // 接線
    T = (all(T == 0)) ? float3(1, 0, 0) : T;    
    float3 B = normalize(cross(N, T));          // 従法線
    
    // ノイズをブレンドして、摂動ベクトルを生成
    float3 perturb = normalize(T * (n0 - 0.5) + B * (n1 - 0.5));
    // 元の法線に摂動ベクトルを加算して正規化
    N = normalize(N + perturb * normalPerturb);
    
    // フレネル反射
    float NdotV = saturate(dot(N, V));
    const float F0 = 0.02;
    float fresnel = F0 + (1.0 - F0) * pow(1.0 - NdotV, 5.0);
    // 最小反射率の設定
    fresnel = max(fresnel, 0.04);
     
    // スペキュラ反射
    float3 Rl = reflect(-L, N);     // ライトの反射ベクトル
    float3 Rv = reflect(-V, N);     // 視線ベクトルの反射ベクトル
    // スペキュラ計算
    float spec = pow(saturate(dot(Rl, V)), 240.0);
    float4 specular = fresnel * spec * lightColor;
    
    // 深度による深度ファクター
    float slope = 1.0 - dot(N, float3(0, 1, 0));
    float depthFactor = saturate(pow(1.0 - NdotV, 1.5));
    
    // 深い色と浅い色を補間
    float4 waterBase = lerp(deepColor, shallowColor, depthFactor);
    
    // 深度ファクターに応じた色吸収
    float3 absorbCoeff = float3(2.0, 1.2, 0.4);
    waterBase.rgb *= exp(-absorbCoeff * depthFactor);
    
    // ディヒューズとアンビエント
    float4 ambient = float4(0.08, 0.10, 0.12, 1.0);
    float4 diffuse = waterBase * (NdotL * 0.6);
           
    // 環境光
    float4 envColor = skyCube.Sample(smp, Rv);
    // フレネルと環境光の寄与をブレンド
    float envMix = saturate(envStrength + fresnel * 0.8);
    envMix = max(envMix, 0.15);     // 最小反射率

    // ベースライティング
    float4 lighting = ambient + diffuse + specular;
    // ベースライティングに環境反射をブレンド
    float4 baseMix = lerp(lighting, envColor, envMix);
           
    // 泡立ちの設定
    float foam = saturate((slope * 0.6 + (nH - foamThreshold)) * (1.0 / max(1e-3, 1.0 - foamThreshold)));
    foam *= foamIntensity;
    float4 withFoam = lerp(baseMix, float4(1, 1, 1, 1), foam);
    
    // フォグの設定
    float dist = length(cameraPos - pin.worldPos);
    float fogT = saturate((dist - 150.0) / 800.0);
    float4 fogColor = deepColor;
    
    float4 finalColor = lerp(withFoam, fogColor, fogT);
    return finalColor;
};