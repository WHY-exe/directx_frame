cbuffer SolidColorCBuf : register(b3)
{
    float4 solid_color;
}
float4 main() : SV_TARGET
{
	return solid_color;
}