#pragma once
#include <windows.h>
#include <GL/glew.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#include <GL/freeglut.h>

#include "Timer.h"
#include "Transform.h"
#include "Camera.h"
#include "Mesh.h"
#include "ShaderProgram.h"
#include "Texture.h"
#include "IO.h"
#include "GameObject.h"
#include "UniformBuffer.h"
#include "Light.h"
#include "FrameBuffer.h"

#define WINDOW_SCREEN_WIDTH		640
#define WINDOW_SCREEN_HEIGHT	432
#define WINDOW_WIDTH			1000
#define WINDOW_HEIGHT			500
#define FRAMES_PER_SECOND		60

const int FRAME_DELAY_SPRITE = 1000 / FRAMES_PER_SECOND;

struct RGB
	
{
	
	float r;
	
	float g;
	
	float b;
	
};

class Game
{
public:
	Game();
	~Game();

	void initializeGame();
	void update();
	void draw();
	void GUI();

	/* input callback functions */
	void keyboardDown(unsigned char key, int mouseX, int mouseY);
	void keyboardUp(unsigned char key, int mouseX, int mouseY);
	void keyboardSpecialDown(int key, int mouseX, int mouseY);
	void keyboardSpecialUp(int key, int mouseX, int mouseY);
	void mouseClicked(int button, int state, int x, int y);
	void mouseMoved(int x, int y);
	void reshapeWindow(int w, int h);

	/* Data Members */
	Timer *updateTimer	= nullptr;
	float TotalGameTime = 0.0f;
	int windowWidth = WINDOW_WIDTH;
	int windowHeight = WINDOW_HEIGHT;

private:
	// Scene Objects.
	Camera camera;

	FrameBuffer frameBuffer;

	Mesh meshSphere;
	Mesh meshSkybox;
	Mesh meshLight;

	GameObject goSun;
	GameObject goEarth;
	GameObject goMoon;
	GameObject goSaturn;
	GameObject goSkybox;

	// OpenGL Handles
	ShaderProgram shaderBasic;
	ShaderProgram shaderTexture;
	ShaderProgram shaderSky;
	ShaderProgram shaderTextureAlphaDiscard;
	ShaderProgram shaderTextureJupiter;

	ShaderProgram shaderPassthrough;
	ShaderProgram shaderGrayscale;

	UniformBuffer uniformBufferCamera;
	UniformBuffer uniformBufferTime;
	UniformBuffer uniformBufferLightScene;
	UniformBuffer uniformBufferLight;
	Light light;
	UniformBuffer uniformBufferToon;

	UniformBuffer	ambLight  ;
	UniformBuffer	speLight  ;
	UniformBuffer	rimLight  ;
	UniformBuffer	diffLight ;
	UniformBuffer	diffRamp  ;
	UniformBuffer	specRamp  ;
	UniformBuffer	gradWarm  ;
	UniformBuffer	gradCool  ;
	UniformBuffer	gradCustom;

	std::vector<Texture*> textureToonRamp;

	bool guiEnabled = false;

	struct KeyInput
	{
		bool moveLeft = false;
		bool moveRight = false;
		bool moveUp = false;
		bool moveDown = false;
		bool moveForward = false;
		bool moveBackward = false;

		bool rotateUp = false;
		bool rotateDown = false;
		bool rotateLeft = false;
		bool rotateRight = false;


		bool ctrlL = false;
		bool ctrlR = false;
		bool shiftL = false;
		bool shiftR = false;
		bool altL = false;
		bool altR = false;
	} input;


	bool toggle6 = true;
	bool toggle7 = true;
	bool toggle8 = true;
	bool toggle9 = true;
	bool toggle0 = true;


	std::vector<RGB> LUT;
	std::string LUTpath;
	GLuint colorWarm;
	GLuint colorCool;
	GLuint colorCustom;
};
