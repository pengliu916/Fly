float4x4 matViewProjection : ViewProjection;
float4 view_position : ViewPosition;
float dist;
struct VS_OUTPUT 
{
   float4 Pos: POSITION;
   float3 dir: TEXCOORD0;
};

VS_OUTPUT vs_main(float4 Pos: POSITION)
{
   VS_OUTPUT Out;
   Out.Pos = mul( float4(Pos.xyz*dist+float3(0.0f,-50,0), 1),matViewProjection);
   Out.dir = Pos.xyz*dist;
   return Out;
}

float Exposure_Level = float( 1.04 );
texture Environment_Tex;
sampler Environment = sampler_state
{
   Texture = (Environment_Tex);
   MAGFILTER = LINEAR;
   MINFILTER = LINEAR;
   MIPFILTER = LINEAR;
   ADDRESSU = WRAP;
   ADDRESSV = WRAP;
   ADDRESSW = WRAP;
};
float4 ps_main(float3 dir: TEXCOORD0) : COLOR 
{
   float4 color = texCUBE(Environment, dir);
   if(color.r<0.5)
   color.a	= 0.0f;
   return color ;
}



technique Scene
{
   pass Environment
   {
      CULLMODE = NONE;
      ALPHABLENDENABLE = TRUE;
      ALPHATESTENABLE = TRUE;
      ALPHAFUNC = GREATER;

      VertexShader = compile vs_2_0 vs_main();
      PixelShader = compile ps_2_0 ps_main();
   }

}

