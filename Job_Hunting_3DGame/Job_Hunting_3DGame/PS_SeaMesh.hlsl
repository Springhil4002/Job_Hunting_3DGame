cbuffer Transform : register(b0)
{
    float4x4 world;
    float4x4 view;
    float4x4 proj;
    float time;
    float3 cameraPos;
};

struct PS_IN
{
    float4 svpos : SV_POSITION; 
    float4 color : COLOR;
    float2 uv    : TEXCOORD;
    
    float3 worldPos     : POSITION1;
    float3 worldNormal  : NORMAL1;
    
    float3 worldTanget   : TANGENT1;
    float3 worldBinormal : BINORMAL1;
};

SamplerState smp : register(s0); 
TextureCube cubeTex : register(t0);
Texture2D normalTex : register(t1);
Texture2D normalTex2 : register(t2);

float4 PS_Main(PS_IN pin) : SV_TARGET
{
    // UVスクロール
    float2 scrollUV1 = pin.uv;
    float2 scrollUV2 = pin.uv;
    
    // 波1:斜め下方向
    scrollUV1.x += time * 0.005f;
    scrollUV1.y += time * 0.005f;
    // 波2:斜め上方向、スケールの調整
    scrollUV2.x += time * 0.005f;
    scrollUV2.y -= time * 0.005f;
    scrollUV2 *= 0.5f;
    
    // 各波のノーマルマップ取得
    float4 normalColor1 = normalTex.Sample(smp, scrollUV1);
    float4 normalColor2 = normalTex2.Sample(smp, scrollUV2);
    // ノーマルマップの法線範囲を-1～1の範囲に変換
    float3 normalMap1 = normalColor1.xyz * 2.0f - 1.0f;
    float3 normalMap2 = normalColor2.xyz * 2.0f - 1.0f;
    
    // ノーマルマップの合成
    float3 normalMap = normalize(normalMap1 + normalMap2);
    normalMap.z = 1.0f;
    float3 normalMap_Normalized = normalize(normalMap);
    
    // TBN行列の作成
    float3 tangent  = normalize(pin.worldTanget);
    float3 binormal = normalize(pin.worldBinormal);
    float3 normal   = normalize(pin.worldNormal);
    // タンジェント空間からワールド空間に変換
    float3x3 TBN = float3x3(tangent, binormal, normal);
    // ノーマルマップの法線をTBN行列で変換、ワールド空間の法線を取得
    float3 N = mul(normalMap_Normalized, TBN);
    
    // 視線ベクトル
    float3 viewVec = normalize(cameraPos.xyz - pin.worldPos);
    // 反射ベクトル
    float3 R = reflect(viewVec, N);
    
    // 視線ベクトルと法線の内積の絶対値取得
    float VdotN = saturate(dot(viewVec, N));
    
    // フレネル反射の計算
    float F0 = 0.02f;
    float F = F0 + (1.0f - F0) * pow(1.0f - VdotN, 5.0f);
    // 反射色と屈折色のブレンド
    // 反射色(環境マップの色)
    float3 adjustedR = float3(R.x, R.z, -R.y);
    // キューブマップのサンプリング
    float4 reflectionColor = cubeTex.Sample(smp, adjustedR);
    // 屈折色の計算(海の色)
    float4 refractionColor = float4(0.3f, 0.5f, 0.7f, 1.0f);
    
    // フレネル反射に基づいて色を補間してブレンド
    float4 finalColor = lerp(refractionColor, reflectionColor, F);
    
    return finalColor; 
}