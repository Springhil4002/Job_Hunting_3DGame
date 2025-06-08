cbuffer LightParams : register(b2)
{
    float3 lightDir;    // ���C�g�̕���
    float pad0;         // �p�f�B���O
    float4 lightColor;  // ���C�g�̐F
}

struct VSOutput
{
    float4 svpos    : SV_POSITION;  // ���_�V�F�[�_�[���痈�����W
    float4 color    : COLOR;        // ���_�V�F�[�_�[���痈���F
    float2 uv       : TEXCOORD;     // ���_�V�F�[�_�[���炫��UV
    float3 normal   : NORMAL;       // ���_�V�F�[�_�[���炫���@��
};

Texture2D tex : register(t0);       // �e�N�X�`��
SamplerState smp : register(s0);    // �T���v���[

float4 PS_Main(VSOutput pin) : SV_TARGET
{
    float3 N= normalize(pin.normal);
    float3 L = normalize(-lightDir);    // ���C�g����
    
    // �����o�[�g����
    float NdotL = max(dot(N, L), 0.0f);
    
    float4 texColor = tex.Sample(smp, pin.uv);
    float4 diffuse = texColor * pin.color * lightColor * NdotL;
    
    return diffuse;
}