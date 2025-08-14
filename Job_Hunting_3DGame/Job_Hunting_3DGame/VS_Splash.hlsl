cbuffer Transform : register(b0)
{
    float4x4 world; // ワールド行列（通常使わない、インスタンスごとに処理）
    float4x4 view; // ビュー行列
    float4x4 proj; // プロジェクション行列
    float time;
    float3 cameraPos;
    float pad1;
};

struct VS_Input
{
    // 頂点データ（例：四角ポリゴンのローカル座標）
    float3 pos : POSITION;
    float3 normal : NORMAL;
    float2 uv : TEXCOORD;
    float3 tangent : TANGENT;
    float4 color : COLOR;

    // インスタンスデータ
    float3 instancePos : INSTANCEPOS;
    float instanceScale : INSTANCESCALE;
    float4 instanceColor : INSTANCECOLOR;
};

struct VS_Out
{
    float4 pos : SV_POSITION;
    float4 color : COLOR;
    float2 uv : TEXCOORD;
};

VS_Out VS_Main(VS_Input vin)
{
    VS_Out vout;

    // ビュー行列の回転部分を取り出し（Z軸は除外）
    float3x3 viewRotation = (float3x3) view;
    float3x3 billboardRotation = transpose(viewRotation); // ビルボード用に逆行列（回転行列の逆は転置）

    // パーティクルのローカル位置を回転させてビルボード化
    float3 localPos = mul(billboardRotation, vin.pos * vin.instanceScale);

    // ワールド空間の最終位置 = インスタンス位置 + 回転済みローカル位置
    float3 worldPos = vin.instancePos + localPos;

    // ビュー・プロジェクション変換
    float4 viewPos = mul(view, float4(worldPos, 1.0f));
    vout.pos = mul(proj, viewPos);

    // UVとインスタンス色を渡す
    vout.uv = vin.uv;
    vout.color = vin.instanceColor;

    return vout;
}
