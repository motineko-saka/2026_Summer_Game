#include "../Common/Pixel/PixelShader2DHeader.hlsli"
 
// 定数バッファ：スロット4番目(b4と書く)
cbuffer cbParam : register(b4)
{
    float4 g_color;
}
 
float4 main(PS_INPUT PSInput) : SV_TARGET
{
 
	// UV座標とテクスチャを参照して、最適な色を取得する
    float4 srcCol = tex.Sample(texSampler, PSInput.uv);
    if (srcCol.a < 0.01f)
    {
		// 描画しない(アルファテスト)
        discard;
    }
    float4 dstCol = srcCol;
    
    // モノクロ処理
    float luminance = dot(srcCol.rgb, float3(0.0, 0.0, 1.0));
    float3 gray = float3(luminance, luminance, luminance);

    // g_color.rgb で色を付けられる。g_color.a はティント強度（0=グレー, 1=完全に g_color に乗算）
    float3 tinted = gray * g_color.rgb;
    float3 result = lerp(gray, tinted, saturate(g_color.a));

    dstCol = float4(result, srcCol.a);

    return dstCol;
}