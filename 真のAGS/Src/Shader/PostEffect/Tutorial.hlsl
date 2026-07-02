#include "../Common/Pixel/PixelShader2DHeader.hlsli"

// 定数バッファ：スロット4番目(b4と書く)
cbuffer cbParam : register(b4)
{
	float4 g_color;
}

float4 main(PS_INPUT PSInput) : SV_TARGET
{
    return float4(1,0,0,1);
}
