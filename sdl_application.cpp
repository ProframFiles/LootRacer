#include "sdl_application.hpp"
#include "akj_ogl.h"
#include "SDL.h"
#include "StaticEnvironment.hpp"
#include "DrawnStringFactory.hpp"

#if (defined _WIN32 || defined WIN32)
	#define snprintf _snprintf 
#endif
#include <stdio.h>
#include "MeshPool.hpp"
#include "VertexArray.hpp"
#include <string>
#include "BSPTree.h"
#include "akjFont.hpp"
#include "cs314_math.hpp"
#include "DrawableObject.hpp"
#include "AudioMixer.hpp"

cSDLGLContext::cSDLGLContext(int init_width, int init_height)
	:mErrorMessageBuffer(kErrorBufferSize, '\0')
{
	putenv(strdup("SDL_VIDEO_CENTERED=1")); 

	P4_ASSERT( SDL_Init( SDL_INIT_VIDEO ) >= 0 ); 
	SDL_GL_SetAttribute( SDL_GL_ACCELERATED_VISUAL, 1 );
	#ifndef _DEBUG
		SDL_GL_SetAttribute( SDL_GL_SWAP_CONTROL, 1 );
	#else
		SDL_GL_SetAttribute( SDL_GL_SWAP_CONTROL, 1 );
	#endif // _DEBUG

	const int bpp = 8;
	P4_ASSERT( SDL_SetVideoMode( init_width, init_height,  bpp, SDL_OPENGL | SDL_NOFRAME ) != NULL );

	akj::glInit();

	glEnable( GL_NORMALIZE ); 
	glEnable( GL_TEXTURE_2D);
	glEnable( GL_DEPTH_TEST );

	glViewport( 0, 0, init_width, init_height );
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glDisable(GL_CULL_FACE);

	glEnable (GL_BLEND);
    glBlendEquation(GL_FUNC_ADD);
    glBlendFuncSeparate (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

cSDLGLContext::~cSDLGLContext()
{
	p4::Log::Debug("About to call SDL_Quit");
	SDL_Quit();
}

cSDLApplication::~cSDLApplication()
{
}

cSDLApplication::cSDLApplication(cSDLGLContext& context)
	:mGLContext(context)
	,mIsDone(false)
	,mTotalFrameCount(0)
	,mElapsedFrames(0)
	,mSmoothedFrameTime(1/60.0)
	,mLastFrameTime(1/60.0)
	,mLevelElapsedTime(0.0f)
	,mElapsedEventTime(0.0f)
	,mIsPlayingLevel(false)
{

	mUsingVsync = mGLContext.UsingVSync();
	mTimedItems.reserve(256);
	SetConfig();
	p4::Log::Info("Initializing audio");
	InitAudio();
	SDL_WM_SetCaption( "P4: Loading Shaders!", "testgl" );
	p4::Log::Info("Loading Shaders");
	LoadShaders();
	SDL_WM_SetCaption( "P4: Loading Meshes!", "testgl" );
	p4::Log::Info("Loading Meshes");
	LoadMeshes();
	SDL_WM_SetCaption( "P4: Loading Textures!", "testgl" );
	p4::Log::Info("Loading Textures");
	LoadTextures();
	InitStringFactory();
	CreateDebugDisplay();
	SDL_WM_SetCaption( "P4: Assembling Static Objects!", "testgl" );
	p4::Log::Info("Assembling static objects");
	AssembleStaticObjects();
	SDL_WM_SetCaption( "P4: Creating Nav Mesh!", "testgl" );
	p4::Log::Info("Creating Nav Mesh");
	CreateCollisionStructures();
	SDL_WM_SetCaption( "P4: Game On!", "testgl" );

	mPlayer.reset(new cPlayer(mStaticEnvironment.get()));
	// now initialize individual objects
	mTimedItems.push_back(mPlayer.get());
	mStaticObjectPool->SetPlayer( mPlayer.get());
	mStaticObjectPool->SetLightManager( &mLightManager);
	CreateLoot();

	cDrawableObject* arrow = mDynamicObjectPool->GetDrawableObject("Arrow");
	arrow->SetDepthFunction(GL_ALWAYS);
	MoveToBackOfDrawingOrder(arrow);
	SDL_WM_GrabInput( SDL_GRAB_ON );
	SDL_ShowCursor(0);
	mIsMouseCaptured = true;
	
	StopLevel();
	mTimer.Start();
	mAbsoluteTimer.Start();
	mFrameTimer.Start();
}


void cSDLApplication::SetConfig()
{
	mFixedFrameTime = 1.0f/60.0f;
	//five minute level time?
	mLevelTime = 300.0;
	mMinTickTime = 1.0f/240.0f;
	// quarter arc per pixel
	mMouseSensitivity = cCoord2(0.01f, 0.01f);
	mStrafeRate = 10.0f;
	mAccelRate = 15.0f;
	mAspectRatio = 1.77778f;
	mFOV = 60.0f;
	SetProjection();
	glClearColor(0.0f, 0.0f, 0.1f, 1.0 );
	
}

void cSDLApplication::ToggleCaptureMouse()
{
	if(mIsMouseCaptured){
		SDL_WM_GrabInput( SDL_GRAB_OFF );
		SDL_ShowCursor(1);
	}
	else{
		SDL_WM_GrabInput( SDL_GRAB_ON );
		SDL_ShowCursor(0);
	}
	mIsMouseCaptured = !mIsMouseCaptured;
}

void cSDLApplication::SetProjection()
{
	glMatrixMode( GL_PROJECTION );
	glLoadMatrixf(ProjectionMatrix(mFOV, mAspectRatio, 0.2f, 301.0f).Data());
	glMatrixMode( GL_MODELVIEW );
}

void cSDLApplication::HandleEvents()
{
	SDL_Event event;
	/* Check if there's a pending event. */
	while( SDL_PollEvent( &event ) ) {
		HandleSingleEvent(event);
	}
}

int cSDLApplication::CheckErrors(){
	int err_num = mGLContext.CheckErrors();
	if(err_num > 0){
		mIsDone = true;
	}
	return err_num;
}

int cSDLGLContext::CheckErrors()
{
	/* Check for error conditions. */
	int error_num = 0;
	error_num += akj::glCheckAllErrors(__FILE__, __LINE__);
	const char* sdl_error = SDL_GetError( );
	if( sdl_error[0] != '\0' ) {
		p4::Log::Error( "testgl: SDL error '%s'\n", sdl_error);
		error_num++;
		SDL_ClearError();
	}
	return error_num;
}

void cSDLGLContext::SwapBuffers()
{
	SDL_GL_SwapBuffers();
}

bool cSDLGLContext::ToggleVSync()
{
	if(UsingVSync()){
		SDL_GL_SetAttribute( SDL_GL_SWAP_CONTROL, 0 );
		return false;
	}
	else{
		SDL_GL_SetAttribute( SDL_GL_SWAP_CONTROL, 1 );
		return true;
	}
}

bool cSDLGLContext::UsingVSync()
{
	int frame_control = 0;
	SDL_GL_GetAttribute( SDL_GL_SWAP_CONTROL, &frame_control );
	return frame_control>0;
}

void cSDLApplication::HandleSingleEvent( SDL_Event& event )
{
	switch( event.type ) {
	    case SDL_ACTIVEEVENT:
			/* See what happened */
			if ( event.active.state & SDL_APPACTIVE ) {
				p4::Log::Debug(  "app %s active focus ", event.active.gain ? "gained" : "lost");
			} else if ( event.active.state & SDL_APPMOUSEFOCUS ) {
				p4::Log::Debug(  "app %s mouse focus ", event.active.gain ? "gained" : "lost");
			} else if ( event.active.state & SDL_APPINPUTFOCUS ) {
				p4::Log::Debug(  "app %s input focus ", event.active.gain ? "gained" : "lost");
			}
			break;
		case SDL_MOUSEBUTTONDOWN:
			if(event.button.button==SDL_BUTTON_LEFT){
				mPlayer->AddForwardVelocity(mAccelRate);
			}
			else if(event.button.button==SDL_BUTTON_RIGHT)
			{
				mPlayer->Jump();
				mAudioMixer->PlayAudio(std::string("jump"));
			}
			break;
		case SDL_MOUSEBUTTONUP:
			if(event.button.button==SDL_BUTTON_LEFT){
				mPlayer->AddForwardVelocity(-mAccelRate);
			}
			break;
		case SDL_MOUSEMOTION:{
			static bool firstEvent = true;
			if(!firstEvent)
			{
				mPlayer->RotateLeftRight(-mMouseSensitivity.x*event.motion.xrel);
				mPlayer->TiltUpDown(-mMouseSensitivity.y*event.motion.yrel);
			}
			else{
				firstEvent = false;
			}
			break;
		}
			

	    case SDL_KEYDOWN:
			p4::Log::Debug("key '%s' pressed", SDL_GetKeyName(event.key.keysym.sym));
			switch (event.key.keysym.sym){
			case SDLK_ESCAPE:
				mIsDone = true;
				break;
			case SDLK_RETURN:
				StartLevel();
				break;
			case SDLK_1:
				mAudioMixer->PlayAudio(0,0);
				break;
			case SDLK_2:
				mAudioMixer->PlayAudio(0,1);
				break;
			case SDLK_3:
				mAudioMixer->PlayAudio(0,2);
				break;
			case SDLK_v:
				mFOV -= 1.0;
				SetProjection();
				break;
			case SDLK_f:
				mFOV += 1.0;
				SetProjection();
				break;
			case SDLK_SPACE:
				if(mIsPlayingLevel){
					mPlayer->Jump();
					mAudioMixer->PlayAudio(std::string("jump"));
				}
				else{
					StartLevel();
				}
				break;
			case SDLK_r:
				ToggleCaptureMouse();
				break;
			case SDLK_w:
				mPlayer->AddForwardVelocity(mAccelRate);
				break;
			case SDLK_a:
				mPlayer->AddLateralVelocity(-mStrafeRate);
				break;
			case SDLK_s:
				mPlayer->AddForwardVelocity(-mAccelRate);
				break;
			case SDLK_d:
				mPlayer->AddLateralVelocity(mStrafeRate);
				break;
			default:
				break;
			}
			
			break;
		case SDL_KEYUP:
			p4::Log::Debug("key '%s' releassed", SDL_GetKeyName(event.key.keysym.sym));
			switch (event.key.keysym.sym){
			case SDLK_w:
				mPlayer->AddForwardVelocity(-mAccelRate);
				break;
			case SDLK_a:
				mPlayer->AddLateralVelocity(mStrafeRate);
				break;
			case SDLK_s:
				mPlayer->AddForwardVelocity(mAccelRate);
				break;
			case SDLK_d:
				mPlayer->AddLateralVelocity(-mStrafeRate);
				break;
			default:
				break;
			}
		
			break;
	    case SDL_QUIT:
			mIsDone = true;
			break;
	}
}

void cSDLApplication::HandleTimedItems()
{
	float elapsed_time = mUsingVsync ? mFixedFrameTime : static_cast<float>(mElapsedEventTime);

	// don't do anything if enough time hasn't passed 
	if(elapsed_time < mMinTickTime){
		return;
	}
	mElapsedEventTime = 0.0;
	if(mIsPlayingLevel){
		mLevelElapsedTime += elapsed_time;
	}

	mTimer.Start();
	mElapsedFrames = 0;
	for (size_t i = 0; i < mTimedItems.size(); i++)
	{
		iTimedItem& item = *mTimedItems.at(i); 
		item.Update(elapsed_time);
		if(item.IsDone()){
			mTimedItems.at(i) = mTimedItems.at(mTimedItems.size()-1);
			mTimedItems.pop_back();
			item.SetInactive();
		}
	}
	mAudioMixer->SetListenerPosition(mPlayer->GetCOMPosition());
}


void cSDLApplication::DrawAll()
{
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glDepthFunc(GL_LEQUAL);
	mPlayer->SetPOV();
	for (iDrawable* drawable: mDrawnItems)
	{
		glPushMatrix();
		drawable->Draw();
		glPopMatrix();
		CheckErrors();
	}
	glClear(GL_DEPTH_BUFFER_BIT);
	mDrawnStringFactory->Draw();
	akj::glCheckAllErrors(__FILE__,__LINE__);
}

void cSDLApplication::LoadShaders()
{
	try
	{
		{
			cShaderDescription shader_desc("text shader");
			shader_desc.AddFragmentFile("../shaders/text_shader.frag").AddVertexFile("../shaders/text_shader.vert");
			mShaderPool.LoadShader(shader_desc);
		}
		{
			cShaderDescription shader_desc("test");
			shader_desc.AddFragmentFile("../shaders/default_object.frag").AddVertexFile("../shaders/default_object.vert");
			mShaderPool.LoadShader(shader_desc);
		}
		{
			cShaderDescription shader_desc("base plane shader");
			shader_desc.AddFragmentFile("../shaders/default_object.frag").AddVertexFile("../shaders/base_plane.vert");
			mShaderPool.LoadShader(shader_desc);
		}
		{
			cShaderDescription shader_desc("decal shader");
			shader_desc.AddFragmentFile("../shaders/decal_shader.frag").AddVertexFile("../shaders/decal_shader.vert");
			mShaderPool.LoadShader(shader_desc);
			akj::glCheckAllErrors(__FILE__, __LINE__);
		}
	}
	catch (const std::runtime_error& e)
	{
		p4::Log::Error("Exception while loading shaders::\n\t\"%s\"", e.what());
		mIsDone = true;
	}
}

void cSDLApplication::LoadMeshes()
{
	try{
		mMeshPool.LoadObjFile("../geometry/level_base.obj");
		mMeshPool.LoadObjFile("../geometry/dynamicHedra.obj");
		mMeshPool.LoadObjFile("../geometry/square.obj");
	}
	catch (const std::runtime_error& e)
	{
		p4::Log::Error("Exception while loading .obj files:\n\t\"%s\"", e.what());
		mIsDone = true;
	}
}

void cSDLApplication::LoadTextures()
{
	try{
		mTexturePool.LoadTexture("default dm", "../textures/gray128.tga");
		mTexturePool.LoadTexture("default nm", "../textures/default_nm.tga");
		mTexturePool.LoadTexture("default lm", "../textures/default_lm.tga");
		mTexturePool.LoadTexture("Profile", "../textures/gradient_text.tga", false);
		mTexturePool.LoadTexture("Font", "../fonts/DroidSansMono.sdf_font.png", false);
		mTexturePool.LoadTexture("base dm", "../textures/unstable_weave__dm.tga");
		mTexturePool.LoadTexture("base nm", "../textures/unstable_weave__nm.tga");
		mTexturePool.LoadTexture("base lm", "../textures/unstable_weave__lm.tga");
		mTexturePool.LoadTexture("clay dm", "../textures/clay__dm.tga");
		mTexturePool.LoadTexture("clay nm", "../textures/clay__nm.tga");
		mTexturePool.LoadTexture("clay lm", "../textures/clay__lm.tga");
		//mTexturePool.LoadTexture("dodger blue dm", "../textures/dodger128.tga");
		mTexturePool.LoadTexture("cyberglow dm", "../textures/blocky__dm.tga");
		mTexturePool.LoadTexture("cyberglow nm", "../textures/blocky__nm.tga");
		mTexturePool.LoadTexture("cyberglow lm", "../textures/blocky__lm.tga");
		mTexturePool.LoadTexture("candyswirl dm", "../textures/candy_swirl__dm.tga");
		mTexturePool.LoadTexture("candyswirl nm", "../textures/candy_swirl__nm.tga");
		mTexturePool.LoadTexture("candyswirl lm", "../textures/candy_swirl__lm.tga");
		mTexturePool.LoadTexture("metal storm dm", "../textures/metal_storm__dm.tga");
		mTexturePool.LoadTexture("metal storm nm", "../textures/metal_storm__nm.tga");
		mTexturePool.LoadTexture("metal storm lm", "../textures/metal_storm__lm.tga");
		//mTexturePool.LoadTexture("pink dm", "../textures/pink128.tga");
		mTexturePool.LoadTexture("fence dm", "../textures/fence__dm.tga");
		mTexturePool.LoadTexture("fence nm", "../textures/fence__nm.tga");
		mTexturePool.LoadTexture("fence lm", "../textures/fence__lm.tga");
		mTexturePool.LoadTexture("gouged scarred dm", "../textures/gouged_scarred__dm.tga");
		mTexturePool.LoadTexture("gouged scarred nm", "../textures/gouged_scarred__nm.tga");
		mTexturePool.LoadTexture("arrow decal shape", "../textures/arrow_out_small.png");
		mTexturePool.LoadTexture("gouged scarred lm", "../textures/gouged_scarred__lm.tga");
		mTexturePool.LoadTexture("arrow decal profile", "../textures/gradient_arrow.tga");
		mTexturePool.LoadTexture("sun decal shape", "../textures/sun_out_small.png");
		mTexturePool.LoadTexture("loot high", "../textures/gradient_sun.tga");
		mTexturePool.LoadTexture("loot low", "../textures/gradient_low.tga");
		mTexturePool.LoadTexture("loot medium", "../textures/gradient_sun.tga");
	}
	catch (const std::runtime_error& e)
	{
		p4::Log::Error("Exception while loading textures: %s", e.what());
		mIsDone = true;
	}
}
void cSDLApplication::AssembleStaticObjects()
{
	mStaticObjectPool.reset(new cObjectPool(mMeshPool, mShaderPool, mTexturePool));
	mDynamicObjectPool.reset(new cObjectPool(mMeshPool, mShaderPool, mTexturePool));
	
	try{
		mStaticObjectPool->CreateDrawnObject("BasePlane","BasePlane", "base plane shader", "base");
		mStaticObjectPool->CreateDrawnObject("EastFence","EastFence", "base plane shader", "fence");
		mStaticObjectPool->CreateDrawnObject("WestFence","WestFence", "base plane shader", "fence");
		mStaticObjectPool->CreateDrawnObject("NorthFence","NorthFence", "base plane shader", "fence");
		mStaticObjectPool->CreateDrawnObject("SouthFence","SouthFence", "base plane shader", "fence");
		mStaticObjectPool->CreateDrawnObject("Pyramid001","Pyramid001", "test", "gouged scarred");
		mStaticObjectPool->CreateDrawnObject("Tube001","Tube001", "test", "candyswirl");
		mStaticObjectPool->CreateDrawnObject("Spindle001", "Spindle001", "test", "clay");
		mStaticObjectPool->CreateDrawnObject("Tube002", "Tube002", "test", "candyswirl");
		mStaticObjectPool->CreateDrawnObject("GeoSphere001", "GeoSphere001", "test", "cyberglow");
		mStaticObjectPool->CreateDrawnObject("GeoSphere003", "GeoSphere003", "test", "metal storm");
		mStaticObjectPool->CreateDrawnObject("Box001", "Box001", "test", "cyberglow");
		mStaticObjectPool->CreateDrawnObject("Box002", "Box002", "test", "metal storm");
		mStaticObjectPool->CreateDrawnObject("StarSphere", "StarSphere", "test", "base");
		mStaticObjectPool->PopulateDrawables(mDrawnItems);

		//mDynamicObjectPool->CreateDrawnObject("DynamicHedra", "DynamicHedra", "test", "dodger blue");
		//mDynamicObjectPool->CreateDrawnObject("center of mass", "DynamicHedra", "test", "pink");



		std::vector<std::string> suffixes;
		suffixes.push_back(std::string(" shape"));
		suffixes.push_back(std::string(" profile"));
		mDynamicObjectPool->CreateDrawnObject("Arrow", "Square", "decal shader", "arrow decal", suffixes);
		
		std::vector<std::string> textures;
		textures.push_back(std::string("sun decal shape"));
		textures.push_back(std::string("loot high"));
		mDynamicObjectPool->CreateDrawnObject("Loot::Sun::High", "Square", "decal shader", "", textures);
		textures.at(1) = std::string("loot low");
		mDynamicObjectPool->CreateDrawnObject("Loot::Sun::Low", "Square", "decal shader", "", textures);

		mDynamicObjectPool->PopulateDrawables(mDrawnItems);
	}
	catch (const std::runtime_error& e)
	{
		p4::Log::Error("Exception while loading textures: %s", e.what());
		mIsDone = true;
	}
}
void cSDLApplication::Run()
{

	CheckErrors();
	while(!IsDone()){
		HandleEvents();
		HandleTimedItems();
		DoGameplay();
		UpdateHUD();
		DrawAll();
		mGLContext.SwapBuffers();
		

		mTotalFrameCount++;
		mElapsedFrames++;
		mLastFrameTime = mFrameTimer.Read();
		mElapsedEventTime += mLastFrameTime;
		mSmoothedFrameTime = mSmoothedFrameTime*0.995 + 0.005*mLastFrameTime;
		mFrameTimer.Start();
	}
}

void cSDLApplication::CreateCollisionStructures()
{
	std::vector<akj::cVertexArray*> static_meshes;
	static_meshes.push_back(mMeshPool.GetMesh("Box001"));
	static_meshes.push_back(mMeshPool.GetMesh("Tube001"));
	static_meshes.push_back(mMeshPool.GetMesh("StarSphere"));
	static_meshes.push_back(mMeshPool.GetMesh("GeoSphere003"));
	static_meshes.push_back(mMeshPool.GetMesh("Box002"));
	static_meshes.push_back(mMeshPool.GetMesh("Pyramid001"));
	static_meshes.push_back(mMeshPool.GetMesh("Tube002"));
	static_meshes.push_back(mMeshPool.GetMesh("GeoSphere001"));
	static_meshes.push_back(mMeshPool.GetMesh("Spindle001"));

	akj::cVertexArray* base_plane = mMeshPool.GetMesh("BasePlane");
	mStaticEnvironment.reset(new cStaticEnvironment(static_meshes,base_plane));
	
}

void cSDLApplication::InitStringFactory()
{
	akj::cGLShader* text_shader = mShaderPool.GetShader("text shader");
	P4_ASSERT(text_shader != NULL);
	mDrawnStringFactory.reset(new cDrawnStringFactory(text_shader));
	akj::cGLTexture* font_texture = mTexturePool.GetTexture("Font");
	akj::cGLTexture* profile_texture = mTexturePool.GetTexture("Profile");
	P4_ASSERT(font_texture != NULL);
	P4_ASSERT(profile_texture != NULL);
	mDrawnStringFactory->LoadFont(std::string("../fonts/DroidSansMono.sdf_font.packed"), font_texture, profile_texture);
}

void cSDLApplication::CreateDebugDisplay()
{
	mFrameRateText = mDrawnStringFactory->CreateDrawnString("FrameRate Result", cCoord2(0.0f, 0.0f), cCoord3(1.0f, 0.0f,1.0f));
	mPlayerScoreText = mDrawnStringFactory->CreateDrawnString("Player Score", cCoord2(0.0f, 0.0f), cCoord3(1.0f, 0.0f,1.0f));
	mTimeRemainingText = mDrawnStringFactory->CreateDrawnString("Time Remaining", cCoord2(0.0f, 0.0f), cCoord3(1.0f, 0.0f,1.0f));
	mRayCastText = mDrawnStringFactory->CreateDrawnString("RayCast Result", cCoord2(-1.0f, 0.725f), cCoord3(0.0f, 1.0f,1.0f));
	mYDistanceText = mDrawnStringFactory->CreateDrawnString("GroundPlane", cCoord2(-1.0f, 0.45f), cCoord3(0.7f, 0.7f,0.4f));
	mXDistanceText = mDrawnStringFactory->CreateDrawnString("GroundPlaneX", cCoord2(-1.0f, 0.45f), cCoord3(0.2f, 0.7f,1.0f));
	mZDistanceText = mDrawnStringFactory->CreateDrawnString("GroundPlaneZ", cCoord2(-1.0f, 0.45f), cCoord3(0.0f, 0.7f,0.4f));
	
}

void cSDLApplication::UpdateHUD()
{
	std::string score("Score: ");
	std::string time("Time Remaining: ");
	std::string FPS("FPS: ");
	std::string V("  V: ");
	std::string X("  X: ");
	std::string Y("  Y: ");
	std::string Z("  Z: ");

	const std::string framerate_text = std::to_string(1.0/mSmoothedFrameTime);
	const std::string closest_string = std::to_string(mPlayer->GetCOMVelocity().length());
	cCoord3 heading = mLightManager.mArrowLight.XYZ();//(mPlayer->GetHeadingVector()).normalized();
	const std::string y_string = std::to_string(heading.y);
	const std::string x_string = std::to_string(heading.x);
	const std::string z_string = std::to_string(heading.z);
	const std::string score_string = std::to_string(mPlayer->GetScore());
	const float time_remaining = static_cast<float>( GreaterOf(mLevelTime - mLevelElapsedTime, 0.0));
	const float remaining_fraction = time_remaining/mLevelTime;
	const std::string time_remaining_string = std::to_string(time_remaining);
	
	
	mTimeRemainingText->SetText(time + time_remaining_string, cCoord2(-0.3f, 0.825f), remaining_fraction*cCoord3(0.0f, 1.0f,0.0f) +(1.0f-remaining_fraction)*cCoord3(1.0f, 0.0f,0.0f));
	mPlayerScoreText->SetText(score + score_string,  cCoord2(-0.2f, 0.725f),cCoord3(0.8f, 0.8f,0.0f) );
	mFrameRateText->SetText(FPS + framerate_text, cCoord2(-1.0f, 0.825f), cCoord3(1.0f, 0.0f,1.0f));
	mRayCastText->SetText(V + closest_string, cCoord2(-1.0f, 0.725f), 0.5f*(heading+cCoord3(1.0)));
	mXDistanceText->SetText(X + x_string, cCoord2(-1.0f, 0.625f), cCoord3(0.9f, 0.2f,0.2f));
	if(mPlayer->IsOnGround()){
		mYDistanceText->SetText(Y + y_string, cCoord2(-1.0f, 0.525f), cCoord3(0.2f, 0.9f,0.2f));
	}
	else{
		mYDistanceText->SetText(Y + y_string, cCoord2(-1.0f, 0.525f), cCoord3(0.6f, 0.6f,0.2f));
	}
	mZDistanceText->SetText(Z + z_string, cCoord2(-1.0f, 0.425f), cCoord3(0.2f, 0.2f,0.9f));
}

void cSDLApplication::CreateLoot()
{
	mLootVec.reserve(32);
	cDrawableObject* target_high = mDynamicObjectPool->GetDrawableObject("Loot::Sun::High");
	cDrawableObject* target_low = mDynamicObjectPool->GetDrawableObject("Loot::Sun::Low");
	cCoord3 pos;
	pos = cCoord3(20.0f, 40.0f, -70.0f);
	mLightManager.mOrangeLightVec.push_back(cCoord4(pos, 1.6f));
	tPositionalAudioPtr audio_handle = mAudioMixer->GetSampleHandle("loot static", pos);
	mLootVec.emplace_back(new cLootObject(target_high, 10, &mLightManager.mOrangeLightVec.back(), std::move(audio_handle)));
	mLootVec.back()->GetDrawable()->SetPosition(pos);
	
	pos = cCoord3(20.0f, 20.0f, 10.0f);
	mLightManager.mOrangeLightVec.push_back(cCoord4(pos, 1.6f));
	audio_handle = mAudioMixer->GetSampleHandle("loot static", pos);
	mLootVec.emplace_back(new cLootObject(target_high, 10, &mLightManager.mOrangeLightVec.back(), std::move(audio_handle)));
	mLootVec.back()->GetDrawable()->SetPosition(pos);
	
	pos = cCoord3(-45.0f, 70.0f, 45.0f);
	mLightManager.mOrangeLightVec.push_back(cCoord4(pos, 1.60f));
	audio_handle = mAudioMixer->GetSampleHandle("loot static", pos);
	mLootVec.emplace_back(new cLootObject(target_high, 10, &mLightManager.mOrangeLightVec.back(), std::move(audio_handle)));
	mLootVec.back()->GetDrawable()->SetPosition(pos);
	
	pos = cCoord3(10.0f, 35.0f, -40.0f);
	mLightManager.mBlueLightVec.push_back(cCoord4(pos, 1.6f));
	audio_handle = mAudioMixer->GetSampleHandle("loot static", pos);
	mLootVec.emplace_back(new cLootObject(target_low, 3, &mLightManager.mBlueLightVec.back(), std::move(audio_handle)));
	mLootVec.back()->GetDrawable()->SetPosition(pos);
	
	pos = cCoord3(80.0f, 25.0f, -10.0f);
	mLightManager.mBlueLightVec.push_back(cCoord4(pos, 1.6f));
	audio_handle = mAudioMixer->GetSampleHandle("loot static", pos);
	mLootVec.emplace_back(new cLootObject(target_low, 3, &mLightManager.mBlueLightVec.back(), std::move(audio_handle)));
	mLootVec.back()->GetDrawable()->SetPosition(pos);

	pos = cCoord3(-95.0f, 0.5f, -95.0f);
	mLightManager.mBlueLightVec.push_back(cCoord4(pos, 1.6f));
	audio_handle = mAudioMixer->GetSampleHandle("loot static", pos);
	mLootVec.emplace_back(new cLootObject(target_low, 3, &mLightManager.mBlueLightVec.back(), std::move(audio_handle)));
	mLootVec.back()->GetDrawable()->SetPosition(pos);
	
	for (std::unique_ptr<cLootObject>& loot: mLootVec)
	{
		mDrawnItems.push_back(loot.get());
	}
}

void cSDLApplication::DoGameplay()
{

	cDrawableObject* target = mDynamicObjectPool->GetDrawableObject("Arrow");
	const float target_distance = mPlayer->GetTargetLocation().length();
	if(mPlayer->JustHitGround()){
		int sound_index = mAudioMixer->PlayAudio(std::string("ground"));
		mAudioMixer->AdjustSampleVolume(sound_index, std::abs(mPlayer->GetCOMVelocity().y)/50.0);
	}

	if(target_distance < 3.0f ||target_distance > 500.0f || !mIsPlayingLevel){
		target->SetVisibility(false);
		mLightManager.mArrowLight.w = 0.0;
	}
	else{
		target->SetVisibility(true);
		
		target->SetOrientation(cUnitQuat( cCoord3::YAxis(),mPlayer->GetTargetOrientation()));
		cCoord3 arrow_bounce = 0.9f*std::abs(std::cos(static_cast<float>(7.0f*mLevelElapsedTime)))*mPlayer->GetTargetOrientation();
		target->RotateToFace(3.0f*mLevelElapsedTime);
		target->SetPosition(mPlayer->GetTargetLocation() + arrow_bounce);
		target->SetScale(cCoord3(1.6f+0.05f*target_distance));
		mLightManager.mArrowLight = cCoord4( mPlayer->GetTargetLocation() + arrow_bounce +2.0f*mPlayer->GetTargetOrientation(), 1.0f);

	}


	cCoord3 player_pos = mPlayer->GetPosition();
	for (std::unique_ptr<cLootObject>&  loot: mLootVec)
	{
		loot->GetDrawable()->SetScale(cCoord3(2.0f));
		loot->GetDrawable()->SetOrientation(cCoord3::YAxis());
		loot->GetDrawable()->RotateToFace(std::fmod(static_cast<float>(mLevelElapsedTime*(2.0+0.001*loot->GetDrawable()->GetPosition().y)),2.0f*AKJ_PIf));
		if(mIsPlayingLevel && loot->IsTouching(player_pos)){
			loot->SetInactive();
			mPlayer->AddToScore(loot->GetValue());
			mAudioMixer->PlayAudio(std::string("got loot"));
		}
	}
	if(mLevelElapsedTime >= mLevelTime){
		StopLevel();
	}
}

void cSDLApplication::StartLevel()
{
	mLevelElapsedTime = 0.0;
	mIsPlayingLevel = true;
	mTitleString.reset();
	mTitleSubString.reset();
	for (std::unique_ptr<cLootObject>&  loot: mLootVec)
	{
		loot->SetActive();
		mPlayer->AddToScore(loot->GetValue());
	}
	mPlayer->SetScore(0);
	mLightManager.mAmbientLight = cCoord4(0.25f);
	glClearColor(0.1f, 0.1f, 0.15f, 1.0 );
	mAudioMixer->PlayAudio(std::string("level start"));
}

void cSDLApplication::StopLevel()
{
	mTitleString = mDrawnStringFactory->CreateDrawnString("Loot Racer!", cCoord2(-0.2f, 0.0f), cCoord3(0.5f, 0.5f,0.0f));
	mTitleSubString = mDrawnStringFactory->CreateDrawnString("Press Space To Start!", cCoord2(-0.4f, -0.2f), cCoord3(0.5f, 0.5f,0.0f));
	mTitleString->SetScale(cCoord2(5.0f, 5.0f));
	//mLevelElapsedTime = 0.0;
	mIsPlayingLevel = false;
	mLightManager.mAmbientLight = cCoord4(0.0f);
	for (std::unique_ptr<cLootObject>&  loot: mLootVec)
	{
		loot->SetInactive();
	}
	glClearColor(0.0f, 0.0f, 0.1f, 1.0 );
}

void cSDLApplication::MoveToBackOfDrawingOrder( iDrawable* pointer )
{
	P4_ASSERT(pointer != NULL);
	int index = 0;
	for (iDrawable* drawable: mDrawnItems)
	{
		if(pointer == drawable){
			break;
		}
		index ++;
	}
	P4_ASSERT(index != mDrawnItems.size());
	for (size_t i = index; i < mDrawnItems.size()-1; i++)
	{
		mDrawnItems.at(i)=mDrawnItems.at(i+1);
	}
	mDrawnItems.back()=pointer;
}

void cSDLApplication::InitAudio()
{
	mAudioMixer.reset(new cAudioMixer(44100, sizeof(short), 2, 512));
	mAudioMixer->LoadAudioFile(std::string("test"), std::string("../audio/test.wav"));
	mAudioMixer->LoadAudioFile(std::string("loot static"), std::string("../audio/test.wav"));
	mAudioMixer->LoadAudioFile(std::string("jump"), std::string("../audio/poyo_16.wav"));
	mAudioMixer->LoadAudioFile(std::string("object bounce"), std::string("../audio/slingshot_16.wav"));
	mAudioMixer->LoadAudioFile(std::string("ground"), std::string("../audio/elastic_16.wav"));
	mAudioMixer->LoadAudioFile(std::string("got loot"), std::string("../audio/cordial_16.wav"));
	mAudioMixer->LoadAudioFile(std::string("level start"), std::string("../audio/ominous_16.wav"));
}




#if (defined _WIN32 || defined WIN32)
	#undef snprintf
#endif
