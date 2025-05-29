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
    float4 svpos : SV_POSITION; // �ϊ����ꂽ���W
    float4 color : COLOR;       // �ϊ����ꂽ�F
};

VSOutput VS_Main(VSInput vin)
{
    float3 position = vin.pos;
    float3 displaced = position;
    
    for (int i = 0; i < WAVE_COUNT;++i)
    {
        float amp = amplitude[i].x;
        float2 dir = normalize(direction[i].xy);
        float len = waveLength[i].x;
        float s = speed[i].x;
        
        // �g��
        float freq = 2.0f * 3.14159f /len;  
        // �p���x
        float angular = freq * s;
        float d = dot(dir, position.xz);
        float phase = (freq * d) + (time * angular);
        
        displaced.x += dir.x * (amp * cos(phase));
        displaced.z += dir.y * (amp * cos(phase));
        displaced.y += amp * sin(phase);
    }
    
    // ���[���h���W�ɕϊ�
    float4 worldPos = mul(world, float4(displaced,1.0f)); 
    // �r���[���W�ɕϊ�
    float4 viewPos = mul(view, worldPos); 
    // ���e�ϊ�
    float4 projPos = mul(proj, viewPos);        

    VSOutput vout;
    
    vout.svpos = projPos;       // ���e�ϊ����ꂽ���W���s�N�Z���V�F�[�_�[�ɓn��
    vout.color = vin.color;     // ���_�F�����̂܂܃s�N�Z���V�F�[�_�[�ɓn��
    
    return vout;
}