cbuffer Transform : register(b0)
{
    float4x4 world;     // ワールド行列
    float4x4 view;      // ビュー行列
    float4x4 proj;      // 投影行列
    float time;         // 時間
    float3 cameraPos;   // カメラ位置
    float pad1;         // パディング
}

cbuffer LightParams : register(b2)
{
    float3 lightDir;    // ライトの方向
    float pad0;         // パディング
    float4 lightColor;  // ライトの色
}

struct PS_IN
{
    float4 svpos    : SV_POSITION;  // 頂点シェーダーから来た座標
    float4 color    : COLOR;        // 頂点シェーダーから来た色
    float2 uv       : TEXCOORD;     // 頂点シェーダーからきたUV
    float3 normal   : NORMAL;       // 頂点シェーダーからきた法線
    float3 worldPos : POSITION1;    // 頂点シェーダーから来たワールド空間座標(スペキュラ計算用)
};

Texture2D tex : register(t0);           // テクスチャ
SamplerState smp : register(s0);        // サンプラー
TextureCube skyCube : register(t3);     // キューブマップ

float4 PS_Main(PS_IN pin) : SV_TARGET
{
    float3 N = normalize(pin.normal); // 法線(ワールド空間)
    float3 L = normalize(-lightDir); // ライト方向(ワールド空間)
    float3 V = normalize(cameraPos - pin.worldPos); // 視線ベクトル
    float3 R = reflect(-L, N); // 反射ベクトル
    
    // ハーフランバート
    float NdotL = saturate(dot(N, L));
    NdotL = lerp(0.8f, 1.0f, NdotL); // 明るさの最低値が0.5
    
    // 鏡面反射
    float specPower = 32.0f;
    float specIntensity = 1.0f;
    float spec = pow(max(dot(R, V), 0.0f), specPower);
    
    // Fresnel効果（Schlick近似）
    float fresnelPower = 5.0f;
    float baseReflectivity = 0.1f; // 水の表現のため
    float fresnel = baseReflectivity +
                    (1.0 - baseReflectivity) *
                    pow(1.0 - saturate(dot(N, V)),
                    fresnelPower);
    // ライティング
    float4 ambient = float4(0.5f,0.6f,0.8f,1.0f);
    float4 diffuse = pin.color * lightColor * NdotL * 0.7f;
    //float4 diffuse = 0.0f;
    float4 specular = fresnel * specIntensity * spec * lightColor;
    
    float4 lighting = ambient + diffuse + specular;
    
    float3 reflectDir = reflect(-V, N); // 視線ベクトルに反射方向
    float4 envColor = skyCube.Sample(smp,reflectDir);
    float envStrength = 0.4f;
    
    float4 finalColor = lerp(lighting, envColor, envStrength);
    
    return finalColor;
}