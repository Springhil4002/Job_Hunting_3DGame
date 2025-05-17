cbuffer Transform : register(b0)
{
    float4x4 World;     // ワールド行列
    float4x4 View;     // ビュー行列
    float4x4 Proj;     // 投影行列
}

struct VS_Input
{
    float3 pos      : POSITION;     // 頂点座標
    float3 normal   : NORMAL;       // 法線
    float2 uv       : TEXCOORD;     // UV
    float3 tangent  : TANGENT;      // 接空間
    float4 color    : COLOR;        // 頂点カラー
};

struct VS_Output
{
    float4 svpos : SV_POSITION;     // 変換された座標
    float4 color : COLOR;           // 変換された色
    float2 uv    : TEXCOORD;        // UV  
};

VS_Output vert(VS_Input input)
{
    VS_Output output = (VS_Output) 0; // アウトプット構造体を定義する
    
    float4 localPos = float4(input.pos, 1.0f);  // 頂点座標
    float4 worldPos = mul(World, localPos);     // ワールド座標に変換
    float4 viewPos = mul(View, worldPos);       // ビュー座標に変換
    float4 projPos = mul(Proj, viewPos);        // 投影変換
    
    output.svpos = projPos;     // 投影変換された座標をピクセルシェーダーに渡す
    output.color = input.color; // 頂点カラーをそのままピクセルシェーダーに渡す
    output.uv = input.uv;       // UVを入力から渡す
    
    return output;
}