cbuffer Transform : register(b0)
{
    float4x4 world;     // ���[���h�s��
    float4x4 view;      // �r���[�s��
    float4x4 proj;      // ���e�s��
    
    float time;     // �g�̂��߂̎���
}

#define WAVE_COUNT (4)

cbuffer GerstnerParams : register(b1)
{
    float4 amplitude[WAVE_COUNT];   // (x = �U��, yzw = �\��)
    float4 direction[WAVE_COUNT];   // (x,y = �����x�N�g��, zw = �\��)
    float4 waveLength[WAVE_COUNT];  // (x = �g��, yzw = �\��)
    float4 speed[WAVE_COUNT];       // (x = �g�̑��x, yzw = �\��)
}

struct VSInput
{
    float3 pos      : POSITION;  // ���_���W
    float3 normal   : NORMAL;    // �@��
    float2 uv       : TEXCOORD;  // UV
    float3 tangent  : TANGENT;   // �ڋ��
    float4 color    : COLOR;     // ���_�F
};

struct VSOutput
{
    float4 svpos    : SV_POSITION;  // �ϊ����ꂽ���W
    float4 color    : COLOR;        // �ϊ����ꂽ�F
    float2 uv       : TEXCOORD;     // �ϊ����ꂽUV
    float3 normal   : NORMAL;       // �ϊ����ꂽ�@��
};

VSOutput VS_Main(VSInput vin)
{
    float3 localPos = vin.pos;
    float3 normal_WS = normalize(mul((float3x3) world, vin.normal));
    
    // ���[�J�����W�����[���h���W�ɕϊ�
    float4 worldPos4 = mul(world, float4(localPos, 1.0f));
    float3 worldPos = worldPos4.xyz;
    
    // �g�ɂ��ψʃx�N�g��(�����l�[��)
    float3 waveOffset = float3(0.0f, 0.0f, 0.0f);
    
    // Gerstner�g�̌v�Z(���[���h���W�x�[�X)
    for (int i = 0; i < WAVE_COUNT;++i)
    {
        float amp = amplitude[i].x;
        float2 dir = normalize(direction[i].xy);
        float len = waveLength[i].x;
        float spd = speed[i].x;

        float freq = 2.0f * 3.14159f / len;
        float angular = freq * spd;
        
        float d = dot(dir, worldPos.xz);
        float phase = (freq * d) + (time * angular);
        
        // Gerstner�g�̕ψʂ����Z
        waveOffset.x += dir.x * (amp * cos(phase));
        waveOffset.z += dir.y * (amp * cos(phase));
        waveOffset.y += amp * sin(phase);
    }
    
    // ���[���h���W�ɔg�̕ψʂ����Z
    worldPos += waveOffset;
    // �r���[�ϊ�
    float4 viewPos = mul(view, float4(worldPos, 1.0f));
    // �v���W�F�N�V�����ϊ�
    float4 projPos = mul(proj, viewPos);
    
    VSOutput vout;
    vout.svpos = projPos;
    vout.color = vin.color;
    vout.uv = vin.uv;
    vout.normal = normal_WS;
    return vout;
}