#include "VSShadow.hlsli"
#include "VSBase.hlsli"
// the sinature of the main function in the vertex shader must be the same
VSOut main(float3 pos : Position3D, float3 fn : Normal, float3 tan : Tangent, float2 tc : Tex2D)
{
    VSOut vso;
    vso.tc = tc;
    vso.ViewPos = (float3)mul(float4(pos, 1.0f), modelCamView);
    vso.viewNorm = mul(fn, (float3x3) modelCamView);
    vso.tan = mul(tan, (float3x3) modelCamView);
    vso.pos = mul(float4(pos, 1.0f), modelProjView);
    const float4 vertexWorldPos = mul(float4(pos, 1.0f), modelWorldView);
    vso.shadowCamPos = ToShadowScreenSpace(float4(pos, 1.0f));
    return vso;
}