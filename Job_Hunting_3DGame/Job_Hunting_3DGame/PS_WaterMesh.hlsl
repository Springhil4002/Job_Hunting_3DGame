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

struct VSOutput
{
    float4 svpos    : SV_POSITION;  // 頂点シェーダーから来た座標
    float4 color    : COLOR;        // 頂点シェーダーから来た色
    float2 uv       : TEXCOORD;     // 頂点シェーダーからきたUV
    float3 normal   : NORMAL;       // 頂点シェーダーからきた法線
    float3 worldPos : POSITION1;    // 頂点シェーダーから来たワールド空間座標(スペキュラ計算用)
};

Texture2D tex : register(t0);       // テクスチャ
SamplerState smp : register(s0);    // サンプラー

float4 PS_Main(VSOutput pin) : SV_TARGET
{
    float3 N = normalize(pin.normal); // 法線(ワールド空間)
    float3 L = normalize(-lightDir); // ライト方向(ワールド空間)
    float3 V = normalize(cameraPos - pin.worldPos); // 視線ベクトル
    float3 R = reflect(-L, N); // 反射ベクトル
    
    // ハーフランバート
    float NdotL = dot(N, L);
    NdotL = saturate(NdotL * 0.5f + 0.5f); // 明るさの最低値が0.5
    
    // 鏡面反射
    float specPower = 64.0f;
    float specIntensity = 0.8f;
    float spec = pow(max(dot(R, V), 0.0f), specPower);
    
    // Fresnel効果（Schlick近似）
    float fresnelPower = 5.0f;
    float baseReflectivity = 0.02f; // 水の表現のため
    float fresnel = baseReflectivity + 
                    (1.0 - baseReflectivity) * 
                    pow(1.0 - saturate(dot(N, V)), 
                    fresnelPower);
    // テクスチャカラー
    float4 texColor = tex.Sample(smp, pin.uv);
    
    // ライティング
    float4 ambient = 0.1f * texColor;
    float4 diffuse = texColor * pin.color * lightColor * NdotL;
    float4 specular = fresnel * specIntensity * spec * lightColor;
    
    return ambient + diffuse + specular;
}