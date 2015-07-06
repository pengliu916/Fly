//--------------------------------------------------------------------------------------
// File: Fly.cpp
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//--------------------------------------------------------------------------------------
#include "DXUT.h"
#include "DXUTcamera.h"
#include "DXUTgui.h"
#include "DXUTsettingsdlg.h"
#include "resource.h"
#include <cstdlib>
#include <ctime>
#include <string>
#include <sstream>

::CFirstPersonCamera						camera;
float													Acceleration				=		0.8f;
float													Velocity						=		0.0f;
float													Displacement 			=		0.0f;
float													GTime							=		0.0f;
int														GState							=		0;

//--------------------------------------------------------------------------------------
// Geometry Class
//--------------------------------------------------------------------------------------
class Geometry
{
public:
	bool									alive;
	float									speed;
	float									speedR;
	float									radius;
	::ID3DXEffect*						effect;
	::D3DXHANDLE					hTech;
	::D3DXHANDLE					hMatrix;
	::D3DXHANDLE					hCMask;
	::LPD3DXMESH					mesh;
	::LPDIRECT3DTEXTURE9		texture;
	::D3DXMATRIX					mTran;
	::D3DXVECTOR3					position;
	FLOAT									CM[4];
	float									SR[3];
	Geometry(){}
	void GeometryB(ID3DXEffect*  E, LPD3DXMESH  M,
					LPDIRECT3DTEXTURE9  T,float S,float R,float Sr,int C)
	{
		alive			=	true;
		effect		=	E;
		texture	=	T;
		speed		=	S;
		speedR	=	Sr;
		radius		=	R;
		hTech		=	effect->GetTechniqueByName("Box");
		hMatrix	=	effect->GetParameterByName( NULL,"matViewProjection" );
		hCMask	=	effect->GetParameterByName( NULL,"ColorMask" );
		mesh		=	M;
		D3DXMatrixIdentity(&mTran);
		::D3DXMatrixTranslation(&mTran,0.0f,-50.0f,0.0f);
		
		CM[0]=0.0f;
		CM[1]=0.0f;
		CM[2]=0.0f;
		CM[3]=1.0f;
		for(int k=0;k<3;k++)
		{
			SR[k]=(((float)(rand()%RAND_MAX))/(float)RAND_MAX-0.5f)*20;
		}
		switch(C)
		{
		case 0:
			CM[0] =1.0f;
			CM[1] =0.0f;
			CM[2] =((float)(rand()%RAND_MAX))/(float)RAND_MAX;
			break;
		case 1:
			CM[0] =1.0f;
			CM[2] =0.0f;;
			CM[1] =((float)(rand()%RAND_MAX))/(float)RAND_MAX;
			break;
		case 2:
			CM[1] =1.0f;
			CM[0] =0.0f;
			CM[2] =((float)(rand()%RAND_MAX))/(float)RAND_MAX;
			break;
		case 3:
			CM[1] =1.0f;
			CM[2] =0.0f;
			CM[0] =((float)(rand()%RAND_MAX))/(float)RAND_MAX;
			break;
		case 4:
			CM[2] =1.0f;
			CM[0] =0.0f;
			CM[1] =((float)(rand()%RAND_MAX))/(float)RAND_MAX;
			break;
		case 5:
			CM[2] =1.0f;
			CM[1] =0.0f;
			CM[0] =((float)(rand()%RAND_MAX))/(float)RAND_MAX;
			break;
		case 6:
			CM[2] =1.0f;
			CM[1] =1.0f;
			CM[0] =1.0f;
			break;
		}
			
	}

	void		Animate(float time)
	{
		::D3DXMATRIX	mR1,mR2,mR3,mT;
		::D3DXMatrixRotationX(&mR1,time*SR[0]/2);
		::D3DXMatrixRotationY(&mR2,time*SR[1]/2);
		::D3DXMatrixRotationZ(&mR3,time*SR[2]/2);
		position.x=radius*sin(time*speedR);
		position.y=(float)((int)((time*speed )*500)%10000)/100.0f-50.0f;
		if(position.y<30)
			alive=true;
		position.z=radius*cos(time*speedR);
		::D3DXMatrixTranslation(&mT,position.x,position.y,position.z);
		mTran=mR1*mR2*mR3*mT;
	}
	void		Draw(D3DXMATRIX* m)
	{
		HRESULT hr;
		UINT					cPasses;
		::D3DXMATRIX	mt=mTran**m;
		V(effect->SetMatrix(hMatrix,&mt));
		V(effect->SetFloatArray(hCMask,CM,4));
		V(effect->SetTexture("Texture",texture));
		V(effect->Begin(&cPasses,0));
		V(effect->BeginPass(0));
		V(mesh->DrawSubset(0));
		V(effect->EndPass());
		V(effect->End());
	}
};

class MainGeometry : public Geometry
{
public:
	::D3DXMATRIX				mRotateZ;
	::D3DXHANDLE				hRotateZ;
	::D3DXMATRIX				mRotateX;
	::D3DXHANDLE				hRotateX;
	::D3DXHANDLE				hState;
	::D3DXHANDLE				hTime;
	float								process;
	float								process1;
	float								acc;
	int									state;
	MainGeometry()
	{
	}
	void		Init()
	{
		::D3DXMATRIX	mR1,mT;
		::D3DXMatrixRotationX(&mR1,1.571);
		::D3DXMatrixTranslation(&mT,0,45.0f,0);
		process=0.0f;
		process1=0.0f;
		acc=0.0f;
		hRotateZ	=	effect->GetParameterByName( NULL,"matRotateZ" );
		hRotateX=	effect->GetParameterByName( NULL,"matRotateX" );
		hTime= effect->GetParameterByName(NULL,"time");
		hState=effect->GetParameterByName(NULL,"state");
		position.x=0;
		position.y=45;
		position.z=0;
		mTran=mR1*mT;
	}
	void		Animate(float inTime)
	{
		::D3DXVECTOR3   mainTran=camera.mainTran*2.0f;
		::D3DXMATRIX	mT;
		position.x+=mainTran.x;
		position.y+=mainTran.y;
		position.z+=mainTran.z;
		::D3DXMatrixTranslation(&mT,mainTran.x,mainTran.y,mainTran.z);
		if(mainTran.x>0)
		{
			if(process<1)
				process+=0.1;
		}else if(mainTran.x<0){
			if(process>-1)
				process-=0.1;
		}else if(mainTran.x==0){
			if(process>0)
				process-=0.1;
			if(process<0)
				process+=0.1;
		}
		if(mainTran.z>0)
		{
			if(process1<1)
				process1+=0.1;
		}else if(mainTran.z<0){
			if(process1>-1)
				process1-=0.1;
		}else if(mainTran.z==0){
			if(process1>0)
				process1-=0.1;
			if(process1<0)
				process1+=0.1;
		}
		::D3DXMatrixRotationZ(&mRotateZ,process*-0.3);
		::D3DXMatrixRotationX(&mRotateX,process1*-0.3);
		mTran*=mT;
		state=GState;
		effect->SetFloat(hTime,inTime);
	}

	void collide(Geometry &box)
	{
		if(box.position.y>=42 && box.alive)
			{
				::D3DXVECTOR3 D=position-box.position;
				float distance=D.x*D.x+D.y*D.y+D.z*D.z;
				if(distance<=5)
				{
					box.alive=false;
					//GState++;
					state=GState;
					/*if(state>3)
						state=0;*/
				}
			}
	}

	void		Draw(D3DXMATRIX* m)
	{
		HRESULT hr;
		UINT					cPasses;
		::D3DXMATRIX	mt=mTran**m;
		V(effect->SetMatrix(hMatrix,&mt));
		V(effect->SetMatrix(hRotateZ,&mRotateZ));
		V(effect->SetMatrix(hRotateX,&mRotateX));
		switch(state)
		{
		case 0:
			CM[0]=1;CM[1]=1;CM[2]=1;
			break;
		case 1:
			CM[0]=1;CM[1]=1;CM[2]=0;
			break;
		case 2:
			CM[0]=1;CM[1]=0;CM[2]=0;
			break;
		}
		V(effect->SetFloat(hState,state));
		V(effect->SetFloatArray(hCMask,CM,4));
		V(effect->SetTexture("Texture",texture));
		V(effect->Begin(&cPasses,0));
		V(effect->BeginPass(0));
		V(mesh->DrawSubset(0));
		V(effect->EndPass());
		V(effect->End());
	}
};

//--------------------------------------------------------------------------------------
// Global Variable
//--------------------------------------------------------------------------------------
::LPD3DXMESH									CubeMesh			=		NULL;
::LPD3DXMESH									TunnelMesh		=		NULL;
::LPD3DXMESH									screenMesh		=		NULL;
::LPD3DXMESH									boxMesh				=		NULL;
::LPD3DXMESH									MainMesh			=		NULL;
::LPDIRECT3DTEXTURE9						boxTexture[5];	
::LPDIRECT3DTEXTURE9						EffectTextures[7]	;
::LPDIRECT3DTEXTURE9						MainTextures	;
::LPDIRECT3DCUBETEXTURE9			SkyTexture;
::IDirect3DSurface9*							pRT[6]			;
::IDirect3DSurface9*							pBackBuffer		=		NULL;
::ID3DXEffect*										pSkyEffect			=		NULL;
::ID3DXEffect*										pMainEffect		=		NULL;
::ID3DXEffect*										pMGEffect			=		NULL;
::ID3DXEffect*										pBoxEffect			=		NULL;
::D3DXHANDLE									hTech					=		NULL;
::D3DXHANDLE									hSkyTech				=		NULL;
::D3DXHANDLE									hBoxTech			=		NULL;
::D3DXHANDLE									hTime					=		NULL;
::D3DXHANDLE									hDist					=		NULL;
::D3DXHANDLE									hDisplacement 	=		NULL;
::D3DXHANDLE									hGlowFactor		=		NULL;
::D3DXHANDLE									hBlurFactor		=		NULL;
::D3DXHANDLE									hSBlurFactor		=		NULL;
::D3DXHANDLE									hWordViewProj	=		NULL;
::D3DXHANDLE									hSViewProj			=		NULL;
::D3DXHANDLE									hPosition			=		NULL;
::D3DXHANDLE									hBoxProj				=		NULL;
::D3DXHANDLE									hVPinvWidth		=		NULL;
::D3DXHANDLE									hVPinvHeight		=		NULL;

float													GlowFactor			=		0.5f;
float													BlurFactor			=		0.5f;
float													SBlurFactor			=		1.0f;
float													angleOfEye			=		1.145;
float													fAspectRatio		=		0;
float													camerX				=		0.0f;
int														score					=		0;
const int												numOfObject		=		30;
bool													speedUp				=		false;
Geometry											box[numOfObject];
MainGeometry											main;
::D3DXMATRIX									mainTran;
::CDXUTDialogResourceManager		dlgManager;
::CDXUTDialog									HUD;

//--------------------------------------------------------------------------------------
// HUD control IDs
//--------------------------------------------------------------------------------------
#define IDC_STATIC_HELP						1
//#define IDC_TOGGLEFULLSCREEN    1
//#define IDC_TOGGLEREF           3
//#define IDC_CHANGEDEVICE        4

//--------------------------------------------------------------------------------------
// Forward declarations 
//--------------------------------------------------------------------------------------
bool			CALLBACK IsDeviceAcceptable( D3DCAPS9* pCaps, D3DFORMAT AdapterFormat, D3DFORMAT BackBufferFormat, bool bWindowed,void* pUserContext );
bool			CALLBACK ModifyDeviceSettings( DXUTDeviceSettings* pDeviceSettings, void* pUserContext );
HRESULT		CALLBACK OnCreateDevice( IDirect3DDevice9* pd3dDevice, const D3DSURFACE_DESC* pBackBufferSurfaceDesc,void* pUserContext );
HRESULT		CALLBACK OnResetDevice( IDirect3DDevice9* pd3dDevice, const D3DSURFACE_DESC* pBackBufferSurfaceDesc,void* pUserContext );
void				CALLBACK OnFrameMove( double fTime, float fElapsedTime, void* pUserContext );
void				CALLBACK OnFrameRender( IDirect3DDevice9* pd3dDevice, double fTime, float fElapsedTime, void* pUserContext );
LRESULT		CALLBACK MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, bool* pbNoFurtherProcessing,void* pUserContext );
void				CALLBACK KeyboardProc( UINT nChar, bool bKeyDown, bool bAltDown, void* pUserContext );
void				CALLBACK OnGUIEvent( UINT nEvent, int nControlID, CDXUTControl* pControl, void* pUserContext );
void				CALLBACK OnLostDevice( void* pUserContext );
void				CALLBACK OnDestroyDevice( void* pUserContext );
void				InitApp();

//--------------------------------------------------------------------------------------
// Initial the Application
//--------------------------------------------------------------------------------------
void InitApp()
{
	HUD.Init( &dlgManager );
	HUD.SetCallback( OnGUIEvent );
	HUD.SetFont( 1,L"Comic Sans MS",26,FW_BOLD );
	HUD.AddStatic(IDC_STATIC_HELP,L"0",0,0,200,60);
	HUD.GetStatic( IDC_STATIC_HELP )->SetTextColor( D3DCOLOR_ARGB( 255, 200, 200,200 ) );
	HUD.GetControl(IDC_STATIC_HELP)->GetElement( 0 )->iFont = 1;
}

//--------------------------------------------------------------------------------------
// Rejects any D3D9 devices that aren't acceptable to the app by returning false
//--------------------------------------------------------------------------------------
bool CALLBACK IsD3D9DeviceAcceptable( D3DCAPS9* pCaps, D3DFORMAT AdapterFormat, D3DFORMAT BackBufferFormat,
                                      bool bWindowed, void* pUserContext )
{
    // Typically want to skip back buffer formats that don't support alpha blending
    IDirect3D9* pD3D = DXUTGetD3D9Object();
    if( FAILED( pD3D->CheckDeviceFormat( pCaps->AdapterOrdinal, pCaps->DeviceType,
                                         AdapterFormat, D3DUSAGE_QUERY_POSTPIXELSHADER_BLENDING,
                                         D3DRTYPE_TEXTURE, BackBufferFormat ) ) )
        return false;

    // No fallback, so need ps3.0
    if( pCaps->PixelShaderVersion < D3DPS_VERSION( 3, 0 ) )
        return false;

	 // No fallback, so need vs3.0
    if( pCaps->VertexShaderVersion < D3DVS_VERSION( 3, 0 ) )
        return false;

    // No fallback, so need to support render target
    if( FAILED( pD3D->CheckDeviceFormat( pCaps->AdapterOrdinal, pCaps->DeviceType,
                                         AdapterFormat, D3DUSAGE_RENDERTARGET,
                                         D3DRTYPE_TEXTURE, BackBufferFormat ) ) )
    {
        return false;
    }

    // No fallback, so need to support D3DFMT_G16R16F or D3DFMT_A16B16G16R16F render target
    if( FAILED( pD3D->CheckDeviceFormat( pCaps->AdapterOrdinal, pCaps->DeviceType,
                                         AdapterFormat, D3DUSAGE_RENDERTARGET,
                                         D3DRTYPE_TEXTURE, D3DFMT_G16R16F ) ) )
    {
        if( FAILED( pD3D->CheckDeviceFormat( pCaps->AdapterOrdinal, pCaps->DeviceType,
                                             AdapterFormat, D3DUSAGE_RENDERTARGET,
                                             D3DRTYPE_TEXTURE, D3DFMT_A16B16G16R16F ) ) )
        {
            return false;
        }
    }
    return true;
}

//--------------------------------------------------------------------------------------
// Before a device is created, modify the device settings as needed
//--------------------------------------------------------------------------------------
bool CALLBACK ModifyDeviceSettings( DXUTDeviceSettings* pDeviceSettings, void* pUserContext )
{
    return true;
}


//--------------------------------------------------------------------------------------
// Create any D3D9 resources that will live through a device reset (D3DPOOL_MANAGED)
// and aren't tied to the back buffer size
//--------------------------------------------------------------------------------------
HRESULT CALLBACK OnD3D9CreateDevice( IDirect3DDevice9* pd3dDevice, const D3DSURFACE_DESC* pBackBufferSurfaceDesc,
                                     void* pUserContext )
{
	HRESULT hr;

	V_RETURN( dlgManager.OnD3D9CreateDevice( pd3dDevice ) );

	/*Load the Cubelmesh*/
	if( FAILED( D3DXLoadMeshFromX( L"SkyCube.x", D3DXMESH_MANAGED, 
                                   pd3dDevice, NULL, 
                                   NULL, NULL,NULL, 
                                   &CubeMesh ) ) )
    {
        MessageBox(NULL, L"Could not find SkyCube.x", L"File error", MB_OK);
        return E_FAIL;
    }

		/*Load the Tunnelmesh*/
	if( FAILED( D3DXLoadMeshFromX( L"Wall.x", D3DXMESH_MANAGED, 
                                   pd3dDevice, NULL, 
                                   NULL, NULL,NULL, 
                                   &TunnelMesh ) ) )
    {
        MessageBox(NULL, L"Could not find Wall.x", L"File error", MB_OK);
        return E_FAIL;
    }

	/*Load the screen mesh*/
	if( FAILED( D3DXLoadMeshFromX( L"Screen.x", D3DXMESH_MANAGED, 
                                   pd3dDevice, NULL, 
                                   NULL, NULL,NULL, 
                                   &screenMesh ) ) )
    {
        MessageBox(NULL, L"Could not find Screen.x", L"File error", MB_OK);
        return E_FAIL;
    }

	/*Load the box mesh*/
	if( FAILED( D3DXLoadMeshFromX( L"Cube.x", D3DXMESH_MANAGED, 
                                   pd3dDevice, NULL, 
                                   NULL, NULL,NULL, 
                                   &boxMesh ) ) )
    {
        MessageBox(NULL, L"Could not find Cube.x", L"File error", MB_OK);
        return E_FAIL;
    }

	/*Load the box mesh*/
	if( FAILED( D3DXLoadMeshFromX( L"Main.x", D3DXMESH_MANAGED, 
                                   pd3dDevice, NULL, 
                                   NULL, NULL,NULL, 
                                   &MainMesh ) ) )
    {
        MessageBox(NULL, L"Could not find Cube.x", L"File error", MB_OK);
        return E_FAIL;
    }

	/*Load the Texture*/
	if( FAILED( D3DXCreateTextureFromFile( pd3dDevice,
								  L"Wall.bmp",
								  &EffectTextures[6])) )
    {
        MessageBox(NULL, L"Could not find Wall.bmp", L"File error", MB_OK);
        return E_FAIL;
    }

	/*Load the Texture*/
	if( FAILED( D3DXCreateTextureFromFile( pd3dDevice,
								  L"Main.bmp",
								  &MainTextures)) )
    {
        MessageBox(NULL, L"Could not find Main.bmp", L"File error", MB_OK);
        return E_FAIL;
    }

	for(int i=0;i<5;i++)
	{
		/*Load the boxTexture*/
		std::wstringstream ss;
		ss<<L"Cube"<<i<<L".bmp";
	
		if( FAILED( D3DXCreateTextureFromFile( pd3dDevice,
										ss.str().c_str(),
									  &boxTexture[i])) )
		{
			MessageBox(NULL, L"Could not find Cube.bmp", L"File error", MB_OK);
			return E_FAIL;
		}
	}

	if( FAILED( D3DXCreateCubeTextureFromFile( pd3dDevice,
								  L"SkyCubemap.dds",
								  &SkyTexture)) )
    {
        MessageBox(NULL, L"Could not find SkyCubemap.dds", L"File error", MB_OK);
        return E_FAIL;
    }

	/*Load the Skyeffect file*/
	if( FAILED( D3DXCreateEffectFromFile(pd3dDevice,L"sky.fx",NULL,NULL,
		D3DXSHADER_DEBUG,NULL,&pSkyEffect,NULL) ) )
    {
        MessageBox(NULL, L"Could not find sky.fx", L"File error", MB_OK);
        return E_FAIL;
    }

	/*Load the effect file*/
	if( FAILED( D3DXCreateEffectFromFile(pd3dDevice,L"SenceEffect_Glow.fx",NULL,NULL,
		D3DXSHADER_DEBUG,NULL,&pMainEffect,NULL) ) )
    {
        MessageBox(NULL, L"Could not find SenceEffect_Glow.fx", L"File error", MB_OK);
        return E_FAIL;
    }

	/*Load the boxeffect file*/
	if( FAILED( D3DXCreateEffectFromFile(pd3dDevice,L"ObjectEffect.fx",NULL,NULL,
		D3DXSHADER_DEBUG,NULL,&pBoxEffect,NULL) ) )
    {
        MessageBox(NULL, L"Could not find ObjectEffect.fx", L"File error", MB_OK);
        return E_FAIL;
    }

	/*Load the MGeffect file*/
	if( FAILED( D3DXCreateEffectFromFile(pd3dDevice,L"MainEffect.fx",NULL,NULL,
		D3DXSHADER_DEBUG,NULL,&pMGEffect,NULL) ) )
    {
        MessageBox(NULL, L"Could not find MainEffect.fx", L"File error", MB_OK);
        return E_FAIL;
    }


	for(int i=0;i<numOfObject;i++){
		box[i].GeometryB(pBoxEffect,boxMesh,boxTexture[rand()%5],((float)(rand()%RAND_MAX)/(float)RAND_MAX+0.5f)*3,
																								((float)(rand()%RAND_MAX)/(float)RAND_MAX+0.3)*5,
																								((float)(rand()%RAND_MAX)/(float)RAND_MAX-0.5f)*5,rand()%6);
	}
	main.GeometryB(pMGEffect,MainMesh,MainTextures,0,0,0,6);
	main.Init();
	
		
		/*Initial the camera*/
	camera.SetScalers( 0.01f, 10.0f );
	camera.SetDrag( true );
	::D3DXVECTOR3		vEyePosition(0.0f,50.0f,0.0f);
	::D3DXVECTOR3		vLookAt(0.0f,0.0f,0.1f);
	camera.SetViewParams(&vEyePosition,&vLookAt);

    return S_OK;
}


//--------------------------------------------------------------------------------------
// Create any D3D9 resources that won't live through a device reset (D3DPOOL_DEFAULT) 
// or that are tied to the back buffer size 
//--------------------------------------------------------------------------------------
HRESULT CALLBACK OnD3D9ResetDevice( IDirect3DDevice9* pd3dDevice, const D3DSURFACE_DESC* pBackBufferSurfaceDesc,
                                    void* pUserContext )
{
	HRESULT hr;

	/*Reset the dialogManagers*/
	V_RETURN( dlgManager.OnD3D9ResetDevice() );

	/*Set and reset the dialogs*/
	HUD.SetLocation(0,0);
	HUD.SetSize(pBackBufferSurfaceDesc->Width,pBackBufferSurfaceDesc->Height);
	HUD.GetControl(IDC_STATIC_HELP)->SetLocation(pBackBufferSurfaceDesc->Width/2-100,pBackBufferSurfaceDesc->Height-60);

	for(int i=0;i<6;i++){
		/*Initial the screenTexture*/
		V_RETURN(D3DXCreateTexture(pd3dDevice,pBackBufferSurfaceDesc->Width,pBackBufferSurfaceDesc->Height,
		1,D3DUSAGE_RENDERTARGET,::D3DFMT_A8B8G8R8,
								D3DPOOL_DEFAULT,&EffectTextures[i]));

		/*Bound the texture to the surface*/
		V_RETURN(EffectTextures[i]->GetSurfaceLevel(0,&pRT[i]));
	}

	/*Get the BackBuffer*/
	V_RETURN(pd3dDevice->GetRenderTarget(0,&pBackBuffer));

	/*Bound the variable to the handles*/
	hTech					=		pMainEffect->GetTechniqueByName("TechMain");
	hWordViewProj	=		pMainEffect->GetParameterByName( NULL,"matViewProjection" );
	hTime					=		pMainEffect->GetParameterByName( NULL,"Time" );
	hDisplacement 	=		pMainEffect->GetParameterByName( NULL,"Displacement" );
	hGlowFactor		=		pMainEffect->GetParameterByName( NULL,"glow_factor" );
	hBlurFactor		=		pMainEffect->GetParameterByName( NULL,"blur_factor" );
	hSBlurFactor		=		pMainEffect->GetParameterByName( NULL,"Sblur_factor" );
	hVPinvWidth		=		pMainEffect->GetParameterByName( NULL,"viewport_inv_width" );
	hVPinvHeight		=		pMainEffect->GetParameterByName( NULL,"viewport_inv_height" );
	hSkyTech				=		pSkyEffect->GetTechniqueByName("Scene");
	hSViewProj			=		pSkyEffect->GetParameterByName( NULL,"matViewProjection" );
	hPosition			=		pSkyEffect->GetParameterByName( NULL,"view_position" );	
	hDist					=		pSkyEffect->GetParameterByName( NULL,"dist" );
	
	/*Bound the Texture to the Effect*/
	V_RETURN(pMainEffect->SetTexture("WallTexture_Tex",EffectTextures[6]));
	V_RETURN(pMainEffect->SetTexture("Wall_Tex",EffectTextures[0]));
	V_RETURN(pMainEffect->SetTexture("Glow_H_Tex",EffectTextures[1]));
	V_RETURN(pMainEffect->SetTexture("Glow_V_Tex",EffectTextures[2]));
	V_RETURN(pMainEffect->SetTexture("ToBeBlured_Tex",EffectTextures[3]));
	V_RETURN(pMainEffect->SetTexture("Blured_Tex",EffectTextures[4]));
	V_RETURN(pMainEffect->SetTexture("Bluring_Tex",EffectTextures[5]));
	V_RETURN(pSkyEffect->SetTexture("Environment_Tex",SkyTexture));

	V_RETURN(pMainEffect->SetFloat(hVPinvWidth,(float)1.0f/pBackBufferSurfaceDesc->Width));
	V_RETURN(pMainEffect->SetFloat(hVPinvHeight,(float)1.0f/pBackBufferSurfaceDesc->Height));
	
	fAspectRatio	=	pBackBufferSurfaceDesc->Width	/	(float)pBackBufferSurfaceDesc->Height;
	camera.SetProjParams(angleOfEye,fAspectRatio,0.1f,5000.0f);
	if(pMainEffect)
		V_RETURN(pMainEffect->OnResetDevice());

    return S_OK;
}


//--------------------------------------------------------------------------------------
// Handle updates to the scene.  This is called regardless of which D3D API is used
//--------------------------------------------------------------------------------------
void CALLBACK OnFrameMove( double fTime, float fElapsedTime, void* pUserContext )
{
	if(GState==3)
	{
		GTime=fTime;
		return;
	}
	fTime-=GTime;
	WCHAR wszOutput[1024];
	if(speedUp)
		score+=fElapsedTime*1500;
	else
		score+=fElapsedTime*1000;
	swprintf_s( wszOutput, 1024, L"SCORE :  %08d",score );		
			HUD.GetStatic( IDC_STATIC_HELP )->SetText( wszOutput );
	if(speedUp && Velocity<=1)
	{
		Velocity+=Acceleration*fElapsedTime;	
		if(Acceleration>0.005)
			Acceleration-=0.0055*fElapsedTime;
	}
	if((!speedUp) && Velocity>=0)
	{
		Velocity-=Acceleration*fElapsedTime;	
		if(Acceleration>0.005)
			Acceleration+=0.0055*fElapsedTime;
	}
	angleOfEye=1.145+0.755*Velocity;
	camera.SetProjParams(angleOfEye,fAspectRatio,0.1f,5000.0f);
	SBlurFactor=1+0.04*Velocity;
	BlurFactor=0.5-0.3*Velocity;

	camera.FrameMove(fElapsedTime);
	pMainEffect->SetFloat(hTime,fTime);
	pMainEffect->SetFloat(hGlowFactor,GlowFactor);
	pMainEffect->SetFloat(hBlurFactor,BlurFactor);
	pMainEffect->SetFloat(hSBlurFactor,SBlurFactor);
	main.Animate(fTime);
	for(int i=0;i<numOfObject;i++)
	{
		box[i].Animate(fTime);
		main.collide(box[i]);
	}
	
}


//--------------------------------------------------------------------------------------
// Render the scene using the D3D9 device
//--------------------------------------------------------------------------------------
void CALLBACK OnD3D9FrameRender( IDirect3DDevice9* pd3dDevice, double fTime, float fElapsedTime, void* pUserContext )
{
    HRESULT hr;
	::D3DXMATRIX	mWorld;
	::D3DXMATRIX	mView;
	::D3DXMATRIX	mProj;
	::D3DXMATRIX	mWorldViewProj;
	::D3DXVECTOR4 mViewPos;
	::D3DXVECTOR3 mViewPos3;
	UINT					iPass,cPasses;

	mWorld					=	*camera.GetWorldMatrix();
	mView						=	*camera.GetViewMatrix();
	mProj						=	*camera.GetProjMatrix();
	mWorldViewProj	=	 mView * mProj;
	mViewPos3=*camera.GetEyePt();
	mViewPos.x=mViewPos3.x;
	mViewPos.y=mViewPos3.y;
	mViewPos.z=mViewPos3.z;
	mViewPos.w=1;

	V(pMainEffect->SetMatrix(hWordViewProj,&mWorldViewProj));
	V(pSkyEffect->SetMatrix(hSViewProj,&mWorldViewProj));
	V(pSkyEffect->SetVector(hPosition,&mViewPos));
	V(pd3dDevice->SetRenderTarget(0,pRT[0]));

    // Clear the render target and the zbuffer 
    V( pd3dDevice->Clear( 0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_ARGB( 0, 0, 0, 0 ), 1.0f, 0 ) );

    // Render the scene
   if( SUCCEEDED( pd3dDevice->BeginScene() ) )
    {
		V(pSkyEffect->SetTechnique(hSkyTech));
		V(pSkyEffect->Begin(&cPasses,0));
		for(float d=5;d>0;d--)
		{
			for( iPass = 0 ; iPass < cPasses ; iPass++)
			{
				V(pSkyEffect->SetFloat(hDist,d*d*d*d*d/1000));
				V(pSkyEffect->BeginPass(iPass));
				V(CubeMesh->DrawSubset(0));
				V(pSkyEffect->EndPass());
			}
		}
		V(pSkyEffect->End());

		V(pMainEffect->SetTechnique(hTech));
		V(pMainEffect->Begin(&cPasses,0));
	
		V(pMainEffect->BeginPass(0));
		V(TunnelMesh->DrawSubset(0));
		V(pMainEffect->EndPass());
		V(pMainEffect->End());

		for(int i=0;i<numOfObject;i++)
		box[i].Draw(&mWorldViewProj);
		main.Draw(&mWorldViewProj);
		
		V(pMainEffect->Begin(&cPasses,0));
		V(pd3dDevice->SetRenderTarget(0,pRT[1]));
		V( pd3dDevice->Clear( 0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_ARGB(0, 0, 0, 0 ), 1.0f, 0 ) );
		V(pMainEffect->BeginPass(1));
		V(screenMesh->DrawSubset(0));
		V(pMainEffect->EndPass());

		V(pd3dDevice->SetRenderTarget(0,pRT[2]));
		V( pd3dDevice->Clear( 0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_ARGB( 0, 0, 0, 0 ), 1.0f, 0 ) );
		V(pMainEffect->BeginPass(2));
		V(screenMesh->DrawSubset(0));
		V(pMainEffect->EndPass());

		V(pd3dDevice->SetRenderTarget(0,pRT[3]));
		V( pd3dDevice->Clear( 0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_ARGB( 0, 0, 0, 0 ), 1.0f, 0 ) );
		V(pMainEffect->BeginPass(3));
		V(screenMesh->DrawSubset(0));
		V(pMainEffect->EndPass());

		V(pd3dDevice->SetRenderTarget(0,pRT[5]));
		V(pMainEffect->BeginPass(4));
		V(screenMesh->DrawSubset(0));
		V(pMainEffect->EndPass());

		V(pd3dDevice->SetRenderTarget(0,pRT[4]));
		V(pMainEffect->BeginPass(5));
		V(screenMesh->DrawSubset(0));
		V(pMainEffect->EndPass());

		V(pd3dDevice->SetRenderTarget(0,pBackBuffer));
		V( pd3dDevice->Clear( 0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_ARGB(0, 0, 0, 0 ),0.0f, 0 ) );
		V(pMainEffect->BeginPass(6));
		V(screenMesh->DrawSubset(0));
		V(pMainEffect->EndPass());
		
		V(pMainEffect->End());
		//V( HUD.OnRender( fElapsedTime ) );
        V( pd3dDevice->EndScene() );
    }
}


//--------------------------------------------------------------------------------------
// Handle messages to the application 
//--------------------------------------------------------------------------------------
LRESULT CALLBACK MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam,
                          bool* pbNoFurtherProcessing, void* pUserContext )
{
	 camera.HandleMessages( hWnd, uMsg, wParam, lParam );
    return 0;
}


//--------------------------------------------------------------------------------------
// Release D3D9 resources created in the OnD3D9ResetDevice callback 
//--------------------------------------------------------------------------------------
void CALLBACK OnD3D9LostDevice( void* pUserContext )
{
	if(pMainEffect)
		pMainEffect->OnLostDevice();
	if(pBoxEffect)
		pBoxEffect->OnLostDevice();
	if(pMGEffect)
		pMGEffect->OnLostDevice();
	if(pSkyEffect)
		pSkyEffect->OnLostDevice();

	dlgManager.OnD3D9LostDevice();

	for(int i=0;i<5;i++){
		SAFE_RELEASE(pRT[i]);
	}
	SAFE_RELEASE(pBackBuffer);
}


//--------------------------------------------------------------------------------------
// Release D3D9 resources created in the OnD3D9CreateDevice callback 
//--------------------------------------------------------------------------------------
void CALLBACK OnD3D9DestroyDevice( void* pUserContext )
{
	for(int i=0;i<7;i++){
		SAFE_RELEASE(EffectTextures[i]);
	}
	for(int i=0;i<5;i++){
		SAFE_RELEASE(pRT[i]);
	}
	SAFE_RELEASE(pBackBuffer);
	SAFE_RELEASE(TunnelMesh);
	SAFE_RELEASE(screenMesh);
	SAFE_RELEASE(pMainEffect);
	SAFE_RELEASE(pBoxEffect);
	SAFE_RELEASE(pSkyEffect);
	SAFE_RELEASE(pMGEffect);
	SAFE_RELEASE(boxMesh);
	SAFE_RELEASE(CubeMesh);
	SAFE_RELEASE(MainMesh);
	for(int i=0;i<5;i++)
		SAFE_RELEASE(boxTexture[i]);
	SAFE_RELEASE(MainTextures);
	SAFE_RELEASE(SkyTexture);
    dlgManager.OnD3D9DestroyDevice();
}

//--------------------------------------------------------------------------------------
// Handles the Keyboard events
//--------------------------------------------------------------------------------------
void CALLBACK KeyboardProc( UINT nChar, bool bKeyDown, bool bAltDown, void* pUserContext )
{
    if( bKeyDown )
    {
       switch( nChar )
        {
			case VK_SPACE:
            {
				speedUp=!speedUp;
				Acceleration=0.8;
				break;
            }

			case VK_F4:
            {
				 DXUTToggleFullScreen(); break;
                break;
            }

            case 'R':
            {
				GState=0;
				score=0;
				speedUp=false;
				Velocity=0;
                break;
            }

			case 'H':
            {
				angleOfEye-=0.02;
				camera.SetProjParams(angleOfEye,fAspectRatio,0.1f,5000.0f);
                break;
            }

			case 'Y':
            {
				angleOfEye+=0.02;
				camera.SetProjParams(angleOfEye,fAspectRatio,0.1f,5000.0f);
                break;
            }
            
			 case 'J':
            {
				if(BlurFactor<0.95)
					BlurFactor+=0.01;
                break;
            }

			 case 'O':
            {
				if(GlowFactor>0.01)
					GlowFactor-=0.01;
                break;
            }

			case 'L':
            {
				if(GlowFactor<0.99)
					GlowFactor+=0.01;
                break;
            }

			 case 'U':
            {
				if(BlurFactor>0.15)
					BlurFactor-=0.01;
                break;
            }

			 case 'I':
            {
				if(SBlurFactor<1.2f)
					SBlurFactor+=0.01;
                break;
            }

			 case 'K':
            {
				if(SBlurFactor>1.0f)
					SBlurFactor-=0.01;
                break;
            }
        }
    }
}


//--------------------------------------------------------------------------------------
// Handles the GUI events
//--------------------------------------------------------------------------------------
void CALLBACK OnGUIEvent( UINT nEvent, int nControlID, CDXUTControl* pControl, void* pUserContext )
{
	  /*switch( nControlID )
    {
        case IDC_TOGGLEFULLSCREEN:
            DXUTToggleFullScreen(); break;
        case IDC_TOGGLEREF:
            DXUTToggleREF(); break;
        case IDC_CHANGEDEVICE:
            settingsDlg.SetActive( !settingsDlg.IsActive() ); break;
    }*/
}
//--------------------------------------------------------------------------------------
// Initialize everything and go into a render loop
//--------------------------------------------------------------------------------------
INT WINAPI wWinMain( HINSTANCE, HINSTANCE, LPWSTR, int )
{
    // Enable run-time memory check for debug builds.
#if defined(DEBUG) | defined(_DEBUG)
    _CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
#endif

    // Set the callback functions
    DXUTSetCallbackD3D9DeviceAcceptable( IsD3D9DeviceAcceptable );
    DXUTSetCallbackD3D9DeviceCreated( OnD3D9CreateDevice );
    DXUTSetCallbackD3D9DeviceReset( OnD3D9ResetDevice );
    DXUTSetCallbackD3D9FrameRender( OnD3D9FrameRender );
    DXUTSetCallbackD3D9DeviceLost( OnD3D9LostDevice );
    DXUTSetCallbackD3D9DeviceDestroyed( OnD3D9DestroyDevice );
    DXUTSetCallbackDeviceChanging( ModifyDeviceSettings );
    DXUTSetCallbackMsgProc( MsgProc );
	DXUTSetCallbackKeyboard( KeyboardProc );
    DXUTSetCallbackFrameMove( OnFrameMove );

    // TODO: Perform any application-level initialization here
	InitApp();
    // Initialize DXUT and create the desired Win32 window and Direct3D device for the application
    DXUTInit( true, true ); // Parse the command line and show msgboxes
    DXUTSetHotkeyHandling( true, true, true );  // handle the default hotkeys
    DXUTSetCursorSettings( true, true ); // Show the cursor and clip it when in full screen
    DXUTCreateWindow( L"Fly" );
    DXUTCreateDevice( true,800,600 );

    // Start the render loop
    DXUTMainLoop();

    // TODO: Perform any application-level cleanup here

    return DXUTGetExitCode();
}


