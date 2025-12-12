cbuffer Transform : register(b0)
{
    float4x4 world;
    float4x4 view;
    float4x4 proj;
    float3 cameraPos;
    float pad;
};

cbuffer LightData : register(b1)
{
    float3 lightDir; // ライト方向 
    float envStrength; // 環境光の強さ
    float4 lightColor; // ライトカラー
};

struct PS_IN
{
    float4 svpos : SV_POSITION; // 変換された座標
    float4 color : COLOR; // 変換された色
    float2 uv : TEXCOORD; // UV
    float3 worldPos : POSITION; // 位置
    float3 worldNormal : NORMAL; // 法線
};

SamplerState smp : register(s0);    // サンプラー
Texture2D _MainTex : register(t0);  // テクスチャ

float4 PS_Main(PS_IN pin) : SV_TARGET
{
    // テクスチャカラー取得
    float4 baseColor = _MainTex.Sample(smp, pin.uv);
    // 法線と光の方向を取得
    float3 normal = normalize(pin.worldNormal);
    float3 lightVec = -lightDir;
    // ディヒューズの計算
    float diffuseIntensity = saturate(dot(normal, lightVec));
    // 視線ベクトル
    float3 viewVec = normalize(cameraPos - pin.worldPos);
    // 反射ベクトル
    float3 reflectVec = normalize(reflect(-lightVec, normal));
    // スペキュラ
    float specularPower = 20.0f;
    float specularIntensity = pow(saturate(dot(reflectVec, viewVec)), specularPower);
    float3 specularColor = lightColor.rgb;
    float3 specular = specularColor * specularIntensity;
    // アンビエント
    float3 ambient = lightColor.rgb * envStrength;
    // ディヒューズ
    float3 diffuse = lightColor.rgb * diffuseIntensity;
    // ライティング計算
    float3 lighting = ambient + diffuse + specular;
    // ベースのカラーにライティングを合成
    float4 finalColor = float4(baseColor.rgb * lighting, baseColor.a);
    return finalColor;
}