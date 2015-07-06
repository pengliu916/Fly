texture Texture;
float4x4 matViewProjection : ViewProjection;
float4x4 matRotateZ;
float4x4 matRotateX;
float4 ColorMask;
float  time;
float  state;
struct VS_INPUT 
{
   float4 Position : POSITION0;
   float2 TexCoord : TEXCOORD0;
};

struct VS_OUTPUT 
{
   float4 Position : POSITION0;
   float2 TexCoord : TEXCOORD0;
   float Depth:TEXCOORD1;
};

VS_OUTPUT vs_main( VS_INPUT Input )
{
   VS_OUTPUT Output;
   Output.Position = mul( mul(mul(Input.Position*float4(1,1,1,1.5),matRotateX),matRotateZ), matViewProjection );
   Output.TexCoord = Input.TexCoord;
   Output.Depth = Input.Position.y+50;
   return( Output );
}

sampler Texture0 = sampler_state
{
   Texture = (Texture);
};

float4 ps_main(float2 TexCoord : TEXCOORD0,
               float Depth : TEXCOORD1) : COLOR0
{   
   float4 Color=tex2D(Texture0,TexCoord)*Depth/100;
   Color=Color*ColorMask*2;
      Color= Color*(cos(time*6)*state+3.3)/(state+1);

      return Color;
}

technique Box
{
   pass RBox
   {
      CULLMODE = NONE;
      ALPHABLENDENABLE = TRUE;
      ALPHATESTENABLE = TRUE;
      ALPHAFUNC = GREATER;

      VertexShader = compile vs_3_0 vs_main();
      PixelShader = compile ps_3_0 ps_main();
   }
}
