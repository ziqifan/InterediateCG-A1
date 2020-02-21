#include "Game.h"
#include "ResourceManager.h"
#include "TextureCube.h"
#include "UI.h"

#include <vector>
#include <string>
#include <fstream>
#include <random>



Game::Game()
{
	updateTimer = new Timer();
}

Game::~Game()
{
	delete updateTimer;
}

int activeToonRamp = 0;
bool toonActive = false;

void Game::initializeGame()
{
	/////////////////////////////////////Bind Warm 3D Texture////////////////////////////////////////////
	LUTpath = "../assets/textures/3DCUBE/WARM.cube";
	std::ifstream LUTfile(LUTpath.c_str());
	while (!LUTfile.eof())
	{
		std::string LUTline;
		getline(LUTfile, LUTline);
		if (LUTline.empty()) continue;
		RGB line;
		if (sscanf_s(LUTline.c_str(), "%f %f %f", &line.r, &line.g, &line.b) == 3) LUT.push_back(line);
	}
	glEnable(GL_TEXTURE_3D);

	glGenTextures(1, &colorWarm);
	glBindTexture(GL_TEXTURE_3D, colorWarm);

	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_REPEAT);

	glTexImage3D(GL_TEXTURE_3D, 0, GL_RGB, 64, 64, 64, 0, GL_RGB, GL_FLOAT, &LUT[0]);

	glBindTexture(GL_TEXTURE_3D, 0);
	glDisable(GL_TEXTURE_3D);

	LUT.clear();

	/////////////////////////////////////Bind Cool 3D Texture////////////////////////////////////////////
	LUTpath = "../assets/textures/3DCUBE/COOL.cube";
	std::ifstream LUTfile1(LUTpath.c_str());
	while (!LUTfile1.eof())
	{
		std::string LUTline;
		getline(LUTfile1, LUTline);
		if (LUTline.empty()) continue;
		RGB line;
		if (sscanf_s(LUTline.c_str(), "%f %f %f", &line.r, &line.g, &line.b) == 3) LUT.push_back(line);
	}
	glEnable(GL_TEXTURE_3D);

	glGenTextures(1, &colorCool);
	glBindTexture(GL_TEXTURE_3D, colorCool);

	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_REPEAT);

	glTexImage3D(GL_TEXTURE_3D, 0, GL_RGB, 64, 64, 64, 0, GL_RGB, GL_FLOAT, &LUT[0]);

	glBindTexture(GL_TEXTURE_3D, 0);
	glDisable(GL_TEXTURE_3D);

	LUT.clear();

	/////////////////////////////////////Bind Custom 3D Texture////////////////////////////////////////////
	LUTpath = "../assets/textures/3DCUBE/CUSTOM.cube";
	std::ifstream LUTfile2(LUTpath.c_str());
	while (!LUTfile2.eof())
	{
		std::string LUTline;
		getline(LUTfile2, LUTline);
		if (LUTline.empty()) continue;
		RGB line;
		if (sscanf_s(LUTline.c_str(), "%f %f %f", &line.r, &line.g, &line.b) == 3) LUT.push_back(line);
	}
	glEnable(GL_TEXTURE_3D);

	glGenTextures(1, &colorCustom);
	glBindTexture(GL_TEXTURE_3D, colorCustom);

	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_REPEAT);

	glTexImage3D(GL_TEXTURE_3D, 0, GL_RGB, 64, 64, 64, 0, GL_RGB, GL_FLOAT, &LUT[0]);

	glBindTexture(GL_TEXTURE_3D, 0);
	glDisable(GL_TEXTURE_3D);

	LUT.clear();

	////////////////////////////////////////////////////////////////////////////////////////////////////

	ShaderProgram::initDefault();
	FrameBuffer::initFrameBuffers();

	frameBuffer.addDepthTarget();
	frameBuffer.addColorTarget(GL_RGBA8);
	frameBuffer.init(windowWidth, windowHeight);

	meshSphere.initMeshSphere(32U, 32U);
	meshSkybox.initMeshSphere(32U, 32U, true);
	meshLight.initMeshSphere(6U, 6U);
	
	shaderBasic.load("shader.vert", "shader.frag");
	shaderTexture.load("shader.vert", "shaderTexture.frag");
	shaderTextureAlphaDiscard.load("shader.vert", "shaderTextureAlphaDiscard.frag");
	shaderSky.load("shaderSky.vert", "shaderSky.frag");
	shaderGrayscale.load("PassThrough.vert", "Post/GreyscalePost.frag");

	ResourceManager::Shaders.push_back(&shaderBasic);
	ResourceManager::Shaders.push_back(&shaderTexture);
	ResourceManager::Shaders.push_back(&shaderTextureAlphaDiscard);
	ResourceManager::Shaders.push_back(&shaderSky);
	ResourceManager::Shaders.push_back(&shaderGrayscale);


	uniformBufferCamera.allocateMemory(sizeof(mat4) * 2);
	uniformBufferCamera.bind(0);
	uniformBufferTime.allocateMemory(sizeof(float));
	uniformBufferTime.bind(1);
	uniformBufferLightScene.allocateMemory(sizeof(vec4));
	uniformBufferLightScene.bind(2);
	
	light.init();
	light._UBO.bind(3);

	uniformBufferToon.allocateMemory(sizeof(int));
	uniformBufferToon.bind(5);
	uniformBufferToon.sendBool(false, 0);

	//////////////////Below are boolean values////////////////////////////////////
	ambLight  .allocateMemory(sizeof(int));
	speLight  .allocateMemory(sizeof(int));
	rimLight  .allocateMemory(sizeof(int));
	diffLight .allocateMemory(sizeof(int));
	diffRamp  .allocateMemory(sizeof(int));
	specRamp  .allocateMemory(sizeof(int));
	gradWarm  .allocateMemory(sizeof(int));
	gradCool  .allocateMemory(sizeof(int));
	gradCustom.allocateMemory(sizeof(int));

	ambLight  .bind(11);
	speLight  .bind(12);
	rimLight  .bind(13);
	diffLight .bind(14);
	diffRamp  .bind(15);
	specRamp  .bind(16);
	gradWarm  .bind(17);
	gradCool  .bind(18);
	gradCustom.bind(19);

	ambLight  .sendBool(false, 0);
	speLight  .sendBool(false, 0);
	rimLight  .sendBool(false, 0);
	diffLight .sendBool(false, 0);
	diffRamp  .sendBool(false, 0);
	specRamp  .sendBool(false, 0);
	gradWarm  .sendBool(false, 0);
	gradCool  .sendBool(false, 0);
	gradCustom.sendBool(false, 0);

	//////////////////////////////////////////////////////////////////////////

	uniformBufferLightScene.sendVector(vec3(0.2f), 0);

	Texture* texBlack = new Texture("black.png");
	Texture* texYellow = new Texture("yellow.png");
	Texture* texEarthAlbedo = new Texture("earth.jpg");
	Texture* texEarthEmissive = new Texture("earthEmissive.png");
	Texture* texEarthSpecular = new Texture("earthSpec.png");
	Texture* texMoonAlbedo = new Texture("8k_moon.jpg");
	Texture* texSaturnAlbedo = new Texture("fatYoshi.png");

	//nLoad toon texture ramp
	textureToonRamp.push_back(new Texture("blueramp.png", false));
	textureToonRamp[0]->setWrapParameters(GL_CLAMP_TO_EDGE);
	textureToonRamp[0]->sendTexParameters();

	textureToonRamp.push_back(new Texture("nice.png", false));
	textureToonRamp[1]->setWrapParameters(GL_CLAMP_TO_EDGE);
	textureToonRamp[1]->sendTexParameters();

	std::vector<Texture*> texEarth = { texEarthAlbedo, texEarthEmissive, texEarthSpecular };
	std::vector<Texture*> texSun = { texBlack, texYellow, texBlack };
	std::vector<Texture*> texMoon = { texMoonAlbedo, texBlack, texBlack };
	std::vector<Texture*> texSaturn = { texSaturnAlbedo, texBlack, texBlack };

	goSun = GameObject(&meshSphere, texSun);
	goEarth = GameObject(&meshSphere, texEarth);
	goMoon = GameObject(&meshSphere, texMoon);
	goSaturn = GameObject(&meshSphere, texSaturn);

	std::vector<std::string> skyboxTex;
	skyboxTex.push_back("snow/hangingstone_ft.tga");
	skyboxTex.push_back("snow/hangingstone_bk.tga");
	skyboxTex.push_back("snow/hangingstone_dn.tga");
	skyboxTex.push_back("snow/hangingstone_up.tga");
	skyboxTex.push_back("snow/hangingstone_rt.tga");
	skyboxTex.push_back("snow/hangingstone_lf.tga");
	goSkybox = GameObject(&meshSkybox, new TextureCube(skyboxTex));
	goSkybox.setShaderProgram(&shaderSky);

	ResourceManager::addEntity(&goSun);
	ResourceManager::addEntity(&goEarth);
	ResourceManager::addEntity(&goMoon);
	ResourceManager::addEntity(&goSaturn);

	goSun.setLocalPos(vec3(0, 3, -4));
	goEarth.setLocalPos(vec3(-2, 0, 0));
	goMoon.setLocalPos(vec3(-1, 0, -1));
	goSaturn.setLocalPos(vec3(-2, 0, -3));

	std::uniform_real_distribution<float> randomPositionX(-100.0f, 100.0f);
	std::uniform_real_distribution<float> randomPositionY(-100.0f, 100.0f);
	std::uniform_real_distribution<float> randomPositionZ(-100.0f, -10.0f);
	std::uniform_real_distribution<float> randomRotation(0.0f, 360.0f);
	std::uniform_real_distribution<float> randomScale(0.5f, 4.0f);
	std::default_random_engine generator(std::_Random_device());

	goSun.setScale(1.50f);
	goEarth.setScale(0.50f);
	goMoon.setScale(0.25f);
	goSaturn.setScale(1.0f);

	goSun.setLocalRotY(180);
	goSaturn.setLocalRotY(90);

	goSun			.setShaderProgram(&shaderTexture);
	goEarth			.setShaderProgram(&shaderTexture);
	goMoon			.setShaderProgram(&shaderTexture);
	goSaturn		.setShaderProgram(&shaderTexture);
	   	 
	// These Render flags can be set once at the start (No reason to waste time calling these functions every frame).
	// Tells OpenGL to respect the depth of the scene. Fragments will not render when they are behind other geometry.
	glEnable(GL_DEPTH_TEST); 
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	   
	// Basic clear color used by glClear().
	glClearColor(0, 0, 0, 0); // Black.

	// Setup our main scene objects...
	float aspect = static_cast<float>(windowWidth) / static_cast<float>(windowHeight);
	camera.perspective(90.0f, aspect, 0.05f, 1000.0f);
	camera.setLocalPos(vec3(0.0f, 4.0f, 5.0f));
	camera.setLocalRotX(-15.0f);
}

void Game::update()
{
	// update our clock so we have the delta time since the last update
	updateTimer->tick();

	float deltaTime = updateTimer->getElapsedTimeSeconds();
	TotalGameTime += deltaTime;

#pragma region movementCode
	float cameraSpeedMult = 2.0f;
	float cameraRotateSpeed = 90.0f;
	if (input.shiftL || input.shiftR)
	{
		cameraSpeedMult *= 4.0f;
	}

	if (input.ctrlL || input.ctrlR)
	{
		cameraSpeedMult *= 0.5f;
	}

	if (input.moveUp)
	{
		camera.m_pLocalPosition.y += cameraSpeedMult * deltaTime;
	}
	if (input.moveDown)
	{
		camera.m_pLocalPosition.y -= cameraSpeedMult * deltaTime;
	}
	if (input.moveForward)
	{
		camera.m_pLocalPosition -= camera.m_pLocalRotation.GetForward() * cameraSpeedMult * deltaTime;
	}
	if (input.moveBackward)
	{
		camera.m_pLocalPosition += camera.m_pLocalRotation.GetForward() * cameraSpeedMult * deltaTime;
	}
	if (input.moveRight)
	{
		camera.m_pLocalPosition += camera.m_pLocalRotation.GetRight() *  cameraSpeedMult * deltaTime;
	}
	if (input.moveLeft)
	{
		camera.m_pLocalPosition -= camera.m_pLocalRotation.GetRight() * cameraSpeedMult * deltaTime;
	}
	if (input.rotateUp)
	{
		camera.m_pLocalRotationEuler.x += cameraRotateSpeed * deltaTime;
		camera.m_pLocalRotationEuler.x = min(camera.m_pLocalRotationEuler.x, 85.0f);
	}
	if (input.rotateDown)
	{
		camera.m_pLocalRotationEuler.x -= cameraRotateSpeed * deltaTime;
		camera.m_pLocalRotationEuler.x = max(camera.m_pLocalRotationEuler.x, -85.0f);
	}
	if (input.rotateRight)
	{
		camera.m_pLocalRotationEuler.y -= cameraRotateSpeed * deltaTime;
	}
	if (input.rotateLeft)
	{
		camera.m_pLocalRotationEuler.y += cameraRotateSpeed * deltaTime;
	}
#pragma endregion movementCode

	// Give the earth some motion over time.
	goEarth.setLocalRotY(TotalGameTime * 15.0f);

	light.position = camera.getView() * vec4(goSun.getWorldPos(), 1.0f);
	light.update(deltaTime);
	// Give our Transforms a chance to compute the latest matrices
	camera.update(deltaTime);
	for (Transform* object : ResourceManager::Transforms)
	{
		object->update(deltaTime);
	}
	goSkybox.update(deltaTime);
	// Movement
	goEarth.setLocalPos(vec3 (5.0f * sin(TotalGameTime* 100.0f * 3.14f / 180.0f), 0.0f, 5.0f * cos(TotalGameTime* 100.0f * 3.14f / 180.0f)) + goSun.getLocalPos());
	goSaturn.setLocalPos(vec3(8.0f * sin(TotalGameTime* 30.0f * 3.14f / 180.0f), 0.0f, 8.0f * cos(TotalGameTime* 30.0f * 3.14f / 180.0f)) + goSun.getLocalPos());
	goMoon.setLocalPos(vec3(1.4f * sin(TotalGameTime* 80.0f * 3.14f / 180.0f), 0.0f, 1.4f * cos(TotalGameTime* 80.0f * 3.14f / 180.0f)) + goEarth.getLocalPos());

	uniformBufferTime.sendFloat(TotalGameTime, 0);
}

void Game::draw()
{
	glClear(GL_DEPTH_BUFFER_BIT);

	// Bind ramp
	textureToonRamp[0]->bind(31);
	textureToonRamp[1]->bind(32);

	// Bind 3D textures
	glActiveTexture(GL_TEXTURE0 + 24);
	glBindTexture(GL_TEXTURE_3D, colorWarm);
	glActiveTexture(GL_TEXTURE0 + 25);
	glBindTexture(GL_TEXTURE_3D, colorCool);
	glActiveTexture(GL_TEXTURE0 + 26);
	glBindTexture(GL_TEXTURE_3D, colorCustom);

	frameBuffer.clear();
	frameBuffer.bind();

	uniformBufferCamera.sendMatrix(camera.getLocalToWorld(), sizeof(mat4));
	goSkybox.draw();
	glClear(GL_DEPTH_BUFFER_BIT);
	
	uniformBufferCamera.sendMatrix(camera.getProjection(), 0);
	uniformBufferCamera.sendMatrix(camera.getView(), sizeof(mat4));

	shaderTexture.bind();
	shaderTexture.unbind();
		
	camera.render();
	
	frameBuffer.unbind();//stop drawing to framebuffer

	//Bind shader and color of the framebuffer texture and draw it to full screen quad
	shaderGrayscale.bind();
	frameBuffer.bindColorAsTexture(0, 0);
	glViewport(0, 0, windowWidth, windowHeight);
	frameBuffer.drawFSQ();
	frameBuffer.unbindTexture(0);
	shaderGrayscale.unbind();

	if(guiEnabled)
		GUI();	

	// Commit the Back-Buffer to swap with the Front-Buffer and be displayed on the monitor.
	glutSwapBuffers();
}

void Game::GUI()
{
	UI::Start(windowWidth, windowHeight);
		
	// TODO: Add imgui controls to texture ramps
	if (ImGui::Checkbox("Toon Shading Active", &toonActive))
	{
		uniformBufferToon.sendBool(toonActive, 0);
	}
	if (ImGui::DragInt("Toon Ramp Selection", &activeToonRamp))
	{
		activeToonRamp = Clamp(activeToonRamp, 0, (int)textureToonRamp.size() - 1);
	}

	static vec3 lightPosition = goSun.getLocalPos();
	if (ImGui::DragFloat3("Light Position", &lightPosition[0], 1))
	{
		goSun.setLocalPos(lightPosition);
	}

	if (ImGui::SliderFloat3("Attenuation", &light.constantAtten, 0.0f, 1.0f, "%.6f", 4.0f))
	{
		
	}

	ImGui::Text("Radius: %f", light.radius);

	UI::End();
}

void Game::keyboardDown(unsigned char key, int mouseX, int mouseY)
{
	if (guiEnabled)
	{
		ImGuiIO& io = ImGui::GetIO();
		io.KeysDown[(int)key] = true;
		io.AddInputCharacter((int)key); // this is what makes keyboard input work in imgui
		// This is what makes the backspace button work
		int keyModifier = glutGetModifiers();
		switch (keyModifier)
		{
		case GLUT_ACTIVE_SHIFT:
			io.KeyShift = true;
			break;

		case GLUT_ACTIVE_CTRL:
			io.KeyCtrl = true;
			break;

		case GLUT_ACTIVE_ALT:
			io.KeyAlt = true;
			break;
		}
	}

	switch (key)
	{
	case 27: // the escape key
		break;
	case 'w':
	case 'W':
	case 'w' - 96:
		input.moveForward = true;
		break;
	case 's':
	case 'S':
	case 's' - 96:
		input.moveBackward = true;
		break;
	case 'd':
	case 'D':
	case 'd' - 96:
		input.moveRight = true;
		break;
	case 'a':
	case 'A':
	case 'a' - 96:
		input.moveLeft = true;
		break;
	case 'e':
	case 'E':
	case 'e' - 96:
		input.moveUp = true;
		break;
	case 'q':
	case 'Q':
	case 'q' - 96:
		input.moveDown = true;
		break;
	case 'l':
	case 'L':
	case 'l' - 96:
		input.rotateRight = true;
		break;
	case 'j':
	case 'J':
	case 'j' - 96:
		input.rotateLeft = true;
		break;
	case 'i':
	case 'I':
	case 'i' - 96:
		input.rotateUp = true;
		break;
	case 'k':
	case 'K':
	case 'k' - 96:
		input.rotateDown = true;
		break;
	case '1': {
		std::cout << "No Lighting" << std::endl;
		ambLight.sendBool(false, 0);
		speLight.sendBool(false, 0);
		rimLight.sendBool(false, 0);
		diffLight.sendBool(false, 0);
		diffRamp.sendBool(false, 0);
		specRamp.sendBool(false, 0);
		break;
	}
	case '2': {
		std::cout << "ambient lighting only" << std::endl;
		ambLight.sendBool(true, 0);
		speLight.sendBool(false, 0);
		rimLight.sendBool(false, 0);
		diffLight.sendBool(false, 0);
		diffRamp.sendBool(false, 0);
		specRamp.sendBool(false, 0);
		break;
	}
	case '3': {
		std::cout << "specular lighting only" << std::endl;
		ambLight.sendBool(false, 0);
		speLight.sendBool(true, 0);
		rimLight.sendBool(false, 0);
		diffLight.sendBool(false, 0);
		diffRamp.sendBool(false, 0);
		specRamp.sendBool(false, 0);
		break;
	}
	case '4': {
		std::cout << "specular + rim lighting" << std::endl;
		ambLight.sendBool(false, 0);
		speLight.sendBool(true, 0);
		rimLight.sendBool(true, 0);
		diffLight.sendBool(false, 0);
		diffRamp.sendBool(false, 0);
		specRamp.sendBool(false, 0);
		break;
	}
	case '5': {
		std::cout << "Ambient + specular + rim" << std::endl;
		ambLight.sendBool(true, 0);
		speLight.sendBool(true, 0);
		rimLight.sendBool(true, 0);
		diffLight.sendBool(false, 0);
		diffRamp.sendBool(false, 0);
		specRamp.sendBool(false, 0);
		break;
	}
	case '6': {
		if (toggle6)
		{
			std::cout << "TOGGLE diffuse warp/ramp" << std::endl;
			diffLight.sendBool(true, 0);
			diffRamp.sendBool(true, 0);
			toggle6 = false;
		}
		else
		{
			diffLight.sendBool(false, 0);
			diffRamp.sendBool(false, 0);
			toggle6 = true;
		}
		break;
	}
	case '7': {
		if (toggle7)
		{
			std::cout << "TOGGLE specular warp/ramp" << std::endl;
			speLight.sendBool(true, 0);
			specRamp.sendBool(true, 0);
			toggle7 = false;
		}
		else
		{
			speLight.sendBool(false, 0);
			specRamp.sendBool(false, 0);
			toggle7 = true;
		}
		break;
		}
	case '8': {
		if (toggle8)
		{
			std::cout << "TOGGLE Color Grading Warm" << std::endl;
			gradWarm.sendBool(true, 0);
			gradCool.sendBool(false, 0);
			gradCustom.sendBool(false, 0);
			toggle8 = false;
		}
		else
		{
			gradWarm.sendBool(false, 0);
			gradCool.sendBool(false, 0);
			gradCustom.sendBool(false, 0);
			toggle8 = true;
		}
		break;
		}
	case '9': {
		if (toggle9)
		{
			std::cout << "TOGGLE Color Grading Cool" << std::endl;
			gradWarm.sendBool(false, 0);
			gradCool.sendBool(true, 0);
			gradCustom.sendBool(false, 0);
			toggle9 = false;
		}
		else
		{
			gradWarm.sendBool(false, 0);
			gradCool.sendBool(false, 0);
			gradCustom.sendBool(false, 0);
			toggle9 = true;
		}
		break;
		}
	case '0': {
		if (toggle0)
		{
			std::cout << "TOGGLE Color Grading Custom Effect" << std::endl;
			gradWarm.sendBool(false, 0);
			gradCool.sendBool(false, 0);
			gradCustom.sendBool(true, 0);
			toggle0 = false;
		}
		else
		{
			gradWarm.sendBool(false, 0);
			gradCool.sendBool(false, 0);
			gradCustom.sendBool(false, 0);
			toggle0 = true;
		}
		break;
		}
	case 'z': {
		ambLight.sendBool(true, 0);
		speLight.sendBool(true, 0);
		rimLight.sendBool(false, 0);
		diffLight.sendBool(true, 0);
		diffRamp.sendBool(false, 0);
		specRamp.sendBool(false, 0);
		gradWarm.sendBool(false, 0);
		gradCool.sendBool(false, 0);
		gradCustom.sendBool(false, 0);
	}

	}
}

void Game::keyboardUp(unsigned char key, int mouseX, int mouseY)
{
	if (guiEnabled)
	{
		ImGuiIO& io = ImGui::GetIO();
		io.KeysDown[key] = false;

		int keyModifier = glutGetModifiers();
		io.KeyShift = false;
		io.KeyCtrl = false;
		io.KeyAlt = false;
		switch (keyModifier)
		{
		case GLUT_ACTIVE_SHIFT:
			io.KeyShift = true;
			break;

		case GLUT_ACTIVE_CTRL:
			io.KeyCtrl = true;
			break;

		case GLUT_ACTIVE_ALT:
			io.KeyAlt = true;
			break;
		}
	}

	switch(key)
	{
	case 32: // the space bar
		camera.cullingActive = !camera.cullingActive;
		break;
	case 27: // the escape key
		exit(1);
		break;
	case 'w':
	case 'W':
	case 'w' - 96:
		input.moveForward = false;
		break;
	case 's':
	case 'S':
	case 's' - 96:
		input.moveBackward = false;
		break;
	case 'd':
	case 'D':
	case 'd' - 96:
		input.moveRight = false;
		break;
	case 'a':
	case 'A':
	case 'a' - 96:
		input.moveLeft = false;
		break;
	case 'e':
	case 'E':
	case 'e' - 96:
		input.moveUp = false;
		break;
	case 'q':
	case 'Q':
	case 'q' - 96:
		input.moveDown = false;
		break;
	case 'l':
	case 'L':
	case 'l' - 96:
		input.rotateRight = false;
		break;
	case 'j':
	case 'J':
	case 'j' - 96:
		input.rotateLeft = false;
		break;
	case 'i':
	case 'I':
	case 'i' - 96:
		input.rotateUp = false;
		break;
	case 'k':
	case 'K':
	case 'k' - 96:
		input.rotateDown = false;
		break;
	}
}

void Game::keyboardSpecialDown(int key, int mouseX, int mouseY)
{
	switch (key)
	{
	case GLUT_KEY_F1:
		guiEnabled = !guiEnabled;
		if (!UI::isInit)
		{
			UI::InitImGUI();
		}
		break;
	case GLUT_KEY_F5:
		for (ShaderProgram* shader : ResourceManager::Shaders)
		{
			shader->reload();
		}
		break;
	case GLUT_KEY_CTRL_L:
		input.ctrlL = true;
		break;
	case GLUT_KEY_CTRL_R:
		input.ctrlR = true;
		break;
	case GLUT_KEY_SHIFT_L:
		input.shiftL = true;
		break;
	case GLUT_KEY_SHIFT_R:
		input.shiftR = true;
		break;
	case GLUT_KEY_ALT_L:
		input.altL = true;
		break;
	case GLUT_KEY_ALT_R:
		input.altR = true;
		break;
	case GLUT_KEY_UP:
		input.moveForward = true;
		break;
	case GLUT_KEY_DOWN:
		input.moveBackward = true;
		break;
	case GLUT_KEY_RIGHT:
		input.moveRight = true;
		break;
	case GLUT_KEY_LEFT:
		input.moveLeft = true;
		break;
	case GLUT_KEY_PAGE_UP:
		input.moveUp = true;
		break;
	case GLUT_KEY_PAGE_DOWN:
		input.moveDown = true;
		break;
	case GLUT_KEY_END:
		exit(1);
		break;
	}
}

void Game::keyboardSpecialUp(int key, int mouseX, int mouseY)
{
	switch (key)
	{
	case GLUT_KEY_CTRL_L:
		input.ctrlL = false;
		break;
	case GLUT_KEY_CTRL_R:
		input.ctrlR = false;
		break;
	case GLUT_KEY_SHIFT_L:
		input.shiftL = false;
		break;
	case GLUT_KEY_SHIFT_R:
		input.shiftR = false;
		break;
	case GLUT_KEY_ALT_L:
		input.altL = false;
		break;
	case GLUT_KEY_ALT_R:
		input.altR = false;
		break;
	case GLUT_KEY_UP:
		input.moveForward = false;
		break;
	case GLUT_KEY_DOWN:
		input.moveBackward = false;
		break;
	case GLUT_KEY_RIGHT:
		input.moveRight = false;
		break;
	case GLUT_KEY_LEFT:
		input.moveLeft = false;
		break;
	case GLUT_KEY_PAGE_UP:
		input.moveUp = false;
		break;
	case GLUT_KEY_PAGE_DOWN:
		input.moveDown = false;
		break;
	}
}

void Game::mouseClicked(int button, int state, int x, int y)
{
	if (guiEnabled)
	{
		ImGui::GetIO().MousePos = ImVec2((float)x, (float)y);
		ImGui::GetIO().MouseDown[0] = !state;
	}

	if(state == GLUT_DOWN) 
	{
		switch(button)
		{
		case GLUT_LEFT_BUTTON:

			break;
		case GLUT_RIGHT_BUTTON:
		
			break;
		case GLUT_MIDDLE_BUTTON:

			break;
		}
	}
	else
	{

	}
}

/*
 * mouseMoved(x,y)
 * - this occurs only when the mouse is pressed down
 *   and the mouse has moved.  you are given the x,y locations
 *   in window coordinates (from the top left corner) and thus 
 *   must be converted to screen coordinates using the screen to window pixels ratio
 *   and the y must be flipped to make the bottom left corner the origin.
 */
void Game::mouseMoved(int x, int y)
{
	if (guiEnabled)
	{
		ImGui::GetIO().MousePos = ImVec2((float)x, (float)y);

		if (!ImGui::GetIO().WantCaptureMouse)
		{

		}
	}
}

void Game::reshapeWindow(int w, int h)
{
	windowWidth = w;
	windowHeight = h;

	float aspect = static_cast<float>(windowWidth) / static_cast<float>(windowHeight);
	camera.perspective(90.0f, aspect, 0.05f, 1000.0f);
	glViewport(0, 0, w, h);
	frameBuffer.resize(w,h);
}