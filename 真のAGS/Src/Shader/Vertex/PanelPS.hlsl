// VS/PS共通
#include "../Common/VertexToPixelHeader.hlsli"
 
// IN
#define PS_INPUT VertexToPixelLit
 
// PS
#include "../Common/Pixel/PixelShader3DHeader.hlsli"

cbuffer cbParam : register(b4)
{
    float4 g_diff_color;
}
 
float4 main(PS_INPUT PSInput) : SV_TARGET0
{
    float4 color;
    
	// テクスチャーの色を取得
    color = diffuseMapTexture.Sample(diffuseMapSampler, PSInput.uv);
    
    
    if (color.a < 0.01f)
    {
        discard;
    }

    return float4((color.rgb * g_diff_color.rgb), 1.0f);
}