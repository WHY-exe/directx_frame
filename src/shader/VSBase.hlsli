cbuffer CBuf : register(b0)
{
    matrix modelWorldView;
    matrix modelCamView;
    matrix modelProjView;
};

struct VSOut
{
    float2 tc : Tex2D;
    float3 ViewPos : ViewPos;
    float3 viewNorm : ViewNormal;
    float3 tan : Tangent;   
    float4 shadowCamPos : ShadowPosition;
    float4 pos : SV_Position;
};