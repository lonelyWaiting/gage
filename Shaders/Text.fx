Texture2D font_sampler;
SamplerState font_sampler_state; 

struct APP_TO_VS
{
  	float2 Pos : POSITION;
  	float2 TexCoord : TEXCOORD0;
};

struct VS_TO_PS 
{
	  float4 Pos : SV_Position;
	  float2 TexCoord : TEXCOORD1;
};

void TextVS(APP_TO_VS In, uint instance_id : SV_InstanceID, out VS_TO_PS Out)
{
    Out.Pos = float4(In.Pos, 0, 1);
    Out.TexCoord = In.TexCoord;
}

void TextPS(VS_TO_PS In, out float4 OutColor : SV_Target)
{
//    OutColor = float4(1,0,0,1);
      OutColor = font_sampler.Sample(font_sampler_state, In.TexCoord).aaaa;
      clip(OutColor.a - 0.01f);
}
