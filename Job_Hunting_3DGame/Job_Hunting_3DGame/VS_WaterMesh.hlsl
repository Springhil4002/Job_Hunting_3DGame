cbuffer Transform : register(b0)
{
    float4x4 world;     // ���[���h�s��
    float4x4 view;      // �r���[�s��
    float4x4 proj;      // ���e�s��
    float time;         // ����
    float3 cameraPos;   // �J�����ʒu
    float pad1;         // �p�f�B���O
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
    float3 worldPos : POSITION1;    // �ϊ����ꂽ���[���h��ԍ��W(�X�y�L�����v�Z�p)
};

VSOutput VS_Main(VSInput vin)
{
    float3 localPos = vin.pos;
    
    // ���[�J�����W�����[���h���W�ɕϊ�
    float4 worldPos4 = mul(world, float4(localPos, 1.0f));
    float3 worldPos = worldPos4.xyz;
    
    // �g�ɂ��ψʃx�N�g��(�����l�[��)
    float3 waveOffset = float3(0.0f, 0.0f, 0.0f);
    
    // �@���p�̕Δ����x�N�g��
    float3 tangentX = float3(1, 0, 0);  // X�����̒P�ʃx�N�g��(���[���h���)
    float3 tangentZ = float3(0, 0, 1);  // Z�����̒P�ʃx�N�g��(���[���h���)
    
    float3 dx = float3(0, 0, 0);    // X�����̌X��
    float3 dz = float3(0, 0, 0);    // Z�����̌X��
    
    // Gerstner�g�̌v�Z(���[���h���W�x�[�X)
    for (int i = 0; i < WAVE_COUNT; ++i)
    {
        // ���_�̈ʒu���v�Z
        float amp = amplitude[i].x;
        float2 dir = normalize(direction[i].xy);
        float len = waveLength[i].x;
        float spd = speed[i].x;

        float freq = 2.0f * 3.14159f / len;
        float angular = freq * spd;
        float d = dot(dir, worldPos.xz);
        float phase = (freq * d) + (time * angular);
        float cosPhase = cos(phase);
        float sinPhase = sin(phase);
        
        // Gerstner�g�̕ψʂ����Z
        waveOffset.x += dir.x * (amp * cosPhase);
        waveOffset.z += dir.y * (amp * cosPhase);
        waveOffset.y += amp * sinPhase;
        
        // �@���Čv�Z�p:�Δ���
        float3 D = float3(dir.x, freq * amp * cosPhase, dir.y);
        dx += D * dot(dir, tangentX.xz);
        dz += D * dot(dir, tangentZ.xz);
    }
    
    // ���[���h���W�ɔg�̕ψʂ����Z
    worldPos += waveOffset;
    
    // �ڃx�N�g���̌����Ŗ@�����v�Z(�Δ���)
    float3 binormal = float3(0, 1, 0) - dx; // y�����𒆐S�ɌX���C��
    float3 tangent = float3(0, 1, 0) - dz;
    float3 normal = normalize(cross(tangent, binormal));

    // �@�������[���h�s��ŕϊ�
    float3 normal_WS = normalize(mul((float3x3) world, normal));
    
    // �r���[�ϊ��ƃv���W�F�N�V�����ϊ�
    float4 viewPos = mul(view, float4(worldPos, 1.0f));
    float4 projPos = mul(proj, viewPos);
    
    VSOutput vout;
    vout.svpos = projPos;
    vout.color = vin.color;
    vout.uv = vin.uv;
    vout.normal = normal_WS;
    vout.worldPos = worldPos;
    return vout;
}