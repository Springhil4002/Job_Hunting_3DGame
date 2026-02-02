struct PS_IN
{
    float4 pos   : SV_POSITION; // 変換された座標
    float4 color : COLOR;       // 変換された色
    float2 uv    : TEXCOORD;    // 変換されたUV
};

Texture2D tex : register(t0);
SamplerState smp : register(s0);

float4 PS_Main(PS_IN pin) : SV_Target
{
    // テクスチャカラー取得
    float4 texColor = tex.Sample(smp, pin.uv);
    float4 finalColor;
    finalColor.rgb = pin.color.rgb;
    finalColor.a = texColor.a * pin.color.a;
    
    // 頂点カラーと掛け合わせ
    return finalColor;
}