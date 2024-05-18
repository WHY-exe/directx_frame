cbuffer TranformCbuf
{
    matrix viewProj;	
};

struct VSOut
{
    float3 worldPos : Position;
    float4 pos : SV_Position;
};

VSOut main(float3 pos : Position3D)
{
    VSOut vso;
    vso.worldPos = pos;
    vso.pos = mul(float4(pos, 0.0f), viewProj);
    vso.pos.z = vso.pos.w;
    return vso;
}