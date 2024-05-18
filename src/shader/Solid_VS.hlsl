#include "VSBase.hlsli"

float4 main(float3 pos : Position3D) : SV_POSITION
{
    return mul(float4(pos, 1.0f), modelProjView);
}