cbuffer ShadowBuffer : register(b1)
{
     matrix shadowView;
};

float4 ToShadowScreenSpace(const in float4 vertex_world_pos)
{
    return mul(vertex_world_pos, shadowView);
}