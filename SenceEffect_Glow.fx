texture WallTexture_Tex;
texture Wall_Tex : RenderColorTarget;
texture Glow_H_Tex : RenderColorTarget;
texture Glow_V_Tex : RenderColorTarget;
texture ToBeBlured_Tex : RenderColorTarget;
texture Blured_Tex : RenderColorTarget;
texture Bluring_Tex : RenderColorTarget;
float viewport_inv_width : ViewportWidthInverse;
float viewport_inv_height : ViewportHeightInverse;
float Time : Time0_X;
float4x4 matViewProjection : ViewProjection;
float glow_factor=0.5;
float blur_factor;
float Sblur_factor;
float Displacement =1.0f;

struct VS_INPUT 
{
   float4 Position : POSITION0;
   float2 TexCoord : TEXCOORD0;
};

struct VS_OUTPUT 
{
   float4 Pos: POSITION;
   float2 texCoord: TEXCOORD0;
};

//--------------------------------------------------------------//
// Wall_Pass
//--------------------------------------------------------------//
struct WP_VS_OUTPUT 
{
   float4 Position : POSITION0;
   float2 TexCoord : TEXCOORD0;
   float Depth:TEXCOORD1;
};

WP_VS_OUTPUT Wall_Pass_vs_main( VS_INPUT Input )
{
   WP_VS_OUTPUT Output;
   Output.Position = mul( Input.Position, matViewProjection );
   Output.TexCoord = Input.TexCoord;
   Output.Depth = Input.Position.y+50;
   return( Output );
}

sampler WallTexture = sampler_state
{
   Texture = (WallTexture_Tex);
};

float4 Wall_Pass_ps_main(float2 TexCoord : TEXCOORD0,
               float Depth : TEXCOORD1) : COLOR0
{   
   float4 Color=tex2D(WallTexture,float2(TexCoord.x+Time+Displacement,TexCoord.y))*Depth*Depth/10000;
   return Color;
}




//--------------------------------------------------------------//
// Glow_H_Pass
//--------------------------------------------------------------//

VS_OUTPUT Glow_H_Pass_vs_main(float4 Pos: POSITION)
{
   VS_OUTPUT Out;
   Out.Pos = float4(Pos.xy, 0, 1);
   Out.texCoord.x = 0.5 * (1 + Pos.x + viewport_inv_width);
   Out.texCoord.y = 0.5 * (1 - Pos.y +viewport_inv_height);
   return Out;
}


sampler Wall = sampler_state
{
   Texture = (Wall_Tex);
   ADDRESSU = CLAMP;
   ADDRESSV = CLAMP;
};
float4 Glow_H_Pass_ps_main(float2 texCoord: TEXCOORD0) : COLOR 
{
   float4 color = float4(0,0,0,0);
   for(int i=-25;i<=25;i++)
   {
      float4 col = tex2D(Wall,texCoord+
             float2(i*viewport_inv_width,0))/51;
      color += col;
   }
   return color;
}

//--------------------------------------------------------------//
// Glow_V_Pass
//--------------------------------------------------------------//

VS_OUTPUT Glow_V_Pass_vs_main(float4 Pos: POSITION)
{
   VS_OUTPUT Out;
   Out.Pos = float4(Pos.xy, 0, 1);
   Out.texCoord.x = 0.5 * (1 + Pos.x + viewport_inv_width);
   Out.texCoord.y = 0.5 * (1 - Pos.y +viewport_inv_height);
   return Out;
}


float4 Glow_V_Pass_ps_main(float2 texCoord: TEXCOORD0) : COLOR 
{
   float4 color = float4(0,0,0,0);
   for(int i=-25;i<=25;i++)
   {
      float4 col = tex2D(Wall,texCoord+
             float2(0,i*viewport_inv_height))/51;
      color += col;
   }
   return color;
}

//--------------------------------------------------------------//
// Altogether
//--------------------------------------------------------------//

VS_OUTPUT Altogether_vs_main(float4 Pos: POSITION)
{
   VS_OUTPUT Out;
   Out.Pos = float4(Pos.xy, 0, 1);
   Out.texCoord.x = 0.5 * (1 + Pos.x + viewport_inv_width);
   Out.texCoord.y = 0.5 * (1 - Pos.y +viewport_inv_height);
   return Out;
}

sampler OriWall = sampler_state
{
   Texture = (Wall_Tex);
   ADDRESSU = CLAMP;
   ADDRESSV = CLAMP;
};
sampler Glow_H = sampler_state
{
   Texture = (Glow_H_Tex);
};
sampler Glow_V = sampler_state
{
   Texture = (Glow_V_Tex);
};

float4 Altogether_ps_main(float2 texCoord: TEXCOORD0) : COLOR 
{
   return tex2D(OriWall,texCoord)*glow_factor+
	(tex2D(Glow_H,texCoord)+tex2D(Glow_V,texCoord))*(1-glow_factor)/2;
}

//--------------------------------------------------------------//
// Blur
//--------------------------------------------------------------//

VS_OUTPUT Blur_vs_main(float4 Pos: POSITION)
{
   VS_OUTPUT Out;
   Out.Pos = float4(Pos.xy, 0, 1);
   Out.texCoord.x = 0.5 * (1 + Pos.x + viewport_inv_width);
   Out.texCoord.y = 0.5 * (1 - Pos.y +viewport_inv_height);
   return Out;
}
sampler ToBeBlured = sampler_state
{
   Texture = (ToBeBlured_Tex);
   ADDRESSU = CLAMP;
   ADDRESSV = CLAMP;
   MINFILTER = LINEAR;
   MIPFILTER = LINEAR;
   MAGFILTER = LINEAR;
};
sampler Blured = sampler_state
{
   Texture = (Blured_Tex);
   MINFILTER = LINEAR;
   MIPFILTER = LINEAR;
   MAGFILTER = LINEAR;
};

float4 Blur_ps_main(float2 texCoord: TEXCOORD0) : COLOR 
{
   float4 col1 = tex2D(ToBeBlured, texCoord);
   float4 col2 = tex2D(Blured, texCoord);
   return col1*blur_factor+col2*(1-blur_factor);
}

//--------------------------------------------------------------//
// Blured
//--------------------------------------------------------------//

VS_OUTPUT Blured_vs_main(float4 Pos: POSITION)
{
   VS_OUTPUT Out;
   Out.Pos = float4(Pos.xy*Sblur_factor, 0, 1);
   Out.texCoord.x = 0.5 * (1 + Pos.x+viewport_inv_width);
   Out.texCoord.y = 0.5 * (1 - Pos.y +viewport_inv_height);
   return Out;
}

sampler Bluring = sampler_state
{
   Texture = (Bluring_Tex);
   MINFILTER = LINEAR;
   MIPFILTER = LINEAR;
   MAGFILTER = LINEAR;
};

float4 Blured_ps_main(float2 texCoord: TEXCOORD0) : COLOR 
{
   return tex2D(Bluring,  texCoord);
}

//--------------------------------------------------------------//
// Present
//--------------------------------------------------------------//
struct PP_VS_OUTPUT 
{
   float4 Pos: POSITION;
   float2 texCoord: TEXCOORD0;
};

PP_VS_OUTPUT Present_vs_main(float4 Pos: POSITION)
{
   PP_VS_OUTPUT Out;
   Out.Pos = float4(Pos.xy, 0, 1);
   Out.texCoord.x = 0.5 * (1 + Pos.x+viewport_inv_width);
   Out.texCoord.y = 0.5 * (1 - Pos.y +viewport_inv_height);
   return Out;
}

float4 Present_ps_main(float2 texCoord: TEXCOORD0) : COLOR 
{
   return tex2D(Blured,  texCoord);
}

//--------------------------------------------------------------//
// Technique Section
//--------------------------------------------------------------//
technique TechMain
{
   pass Wall_Pass
   {
      CULLMODE = NONE;
      ALPHABLENDENABLE = TRUE;
      ALPHATESTENABLE = TRUE;
      ALPHAFUNC = GREATER;

      VertexShader = compile vs_3_0 Wall_Pass_vs_main();
      PixelShader = compile ps_3_0 Wall_Pass_ps_main();
   }

   pass Glow_H_Pass
   {
      CULLMODE = NONE;
      ZENABLE = FALSE;
      ZWRITEENABLE = FALSE;

      VertexShader = compile vs_3_0 Glow_H_Pass_vs_main();
      PixelShader = compile ps_3_0 Glow_H_Pass_ps_main();
   }

   pass Glow_V_Pass
   {
      CULLMODE = NONE;
      ZENABLE = FALSE;
      ZWRITEENABLE = FALSE;

      VertexShader = compile vs_3_0 Glow_V_Pass_vs_main();
      PixelShader = compile ps_3_0 Glow_V_Pass_ps_main();
   }

   pass Altogether
   {
      CULLMODE = NONE;
      ZENABLE = FALSE;
      ZWRITEENABLE = FALSE;

      VertexShader = compile vs_3_0 Altogether_vs_main();
      PixelShader = compile ps_3_0 Altogether_ps_main();
   }

   pass Blur
   {
      CULLMODE = NONE;
      ZWRITEENABLE = FALSE;
      ZENABLE = FALSE;

      VertexShader = compile vs_3_0 Blur_vs_main();
      PixelShader = compile ps_3_0 Blur_ps_main();
   }

   pass Blured
   {
      CULLMODE = NONE;
      ZWRITEENABLE = FALSE;
      ZENABLE = FALSE;

      VertexShader = compile vs_3_0 Blured_vs_main();
      PixelShader = compile ps_3_0 Blured_ps_main();
   }

   pass Present
   {
      CULLMODE = NONE;
      ZWRITEENABLE = FALSE;
      ZENABLE = FALSE;

      VertexShader = compile vs_3_0 Present_vs_main();
      PixelShader = compile ps_3_0 Present_ps_main();
   }
}

