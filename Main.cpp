#include <iostream>
#include "Circle.h"
#include "Square.h"
#include "Rectangle.h"
#include "ShaderClass.h"
#include "TextureClass.h"
#include <random>

// // GLEW - OpenGL Extension Wrangler - http://glew.sourceforge.net/
// // NOTE: include before SDL.h
#ifndef GLEW_H
#define GLEW_H
//#include <GL/glew.h>

// SDL - Simple DirectMedia Layer - https://www.libsdl.org/
#include "SDL.h"
#include "SDL_image.h"
#include "SDL_mixer.h"
#include "SDL_ttf.h"

// // - OpenGL Mathematics - https://glm.g-truc.net/
#define GLM_FORCE_RADIANS // force glm to use radians
// // NOTE: must do before including GLM headers
// // NOTE: GLSL uses radians, so will do the same, for consistency
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// https://opengameart.org/content/puzzle-game-art - game art for bricks and paddle
// https://opengameart.org/content/heart-1616 - game art for the Heart life indicator 
// https://opengameart.org/content/numbers-blocks-set-01 - game art for the numbers

int main(int argc, char *argv[]) {
	// SDL initialise
	if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
		SDL_LogCritical(SDL_LOG_CATEGORY_APPLICATION, "SDL_Init Error: %s\n",
			SDL_GetError());
		return 1;
	}

	SDL_Log("SDL initialised OK!\n");

	// Window Creation - modified for OpenGL
	SDL_Window *win = nullptr;

	SDL_DisplayMode dm;
	SDL_GetCurrentDisplayMode(0, &dm);
	int width = dm.w;
	int height = dm.h;

	win = SDL_CreateWindow("Ross Mills - 14589844 | Module - Graphics - GCP2012M", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width / 2, height / 2, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
	if (win == nullptr) 
	{
		SDL_LogCritical(SDL_LOG_CATEGORY_APPLICATION,
			"SDL_CreateWindow init error: %s\n", SDL_GetError());
		return 1;
	}

	// Create an OpenGL context associated with the window.
	SDL_GLContext glcontext = SDL_GL_CreateContext(win);

	std::cout << "OpenGL version is" << glGetString(GL_VERSION) << std::endl;

	//create objects
	int objectCount = 5;
	Square squares[5][5];
	Square background;
	Square paddle;
	Square lives[4];
	Square scoreDigit1;
	Square scoreDigit2;

	Square borderTop;
	Square borderLeft;
	Square borderRight;
	Square borderBottom;

	Circle ball(0.01);

	//create textures
	Texture texArray[10];
	Texture numberTextures[10];

	texArray[0].load("..//src//rectRed.png");
	texArray[0].setBuffers();
	texArray[1].load("..//src//rectYellow.png");
	texArray[1].setBuffers();
	texArray[2].load("..//src//rectGreen.png");
	texArray[2].setBuffers();
	texArray[3].load("..//src//rectBlue.png");
	texArray[3].setBuffers();
	texArray[4].load("..//src//rectPurple.png");
	texArray[4].setBuffers();
	texArray[5].load("..//src//sky.png");
	texArray[5].setBuffers();
	texArray[6].load("..//src//ball.png");
	texArray[6].setBuffers();
	texArray[7].load("..//src//paddle.png");
	texArray[7].setBuffers();
	texArray[8].load("..//src//heart.png");
	texArray[8].setBuffers();
	texArray[9].load("..//src//brick.png");
	texArray[9].setBuffers();

	numberTextures[0].load("..//src//numbers/0.png");
	numberTextures[0].setBuffers();
	numberTextures[1].load("..//src//numbers/1.png");
	numberTextures[1].setBuffers();
	numberTextures[2].load("..//src//numbers/2.png");
	numberTextures[2].setBuffers();
	numberTextures[3].load("..//src//numbers/3.png");
	numberTextures[3].setBuffers();
	numberTextures[4].load("..//src//numbers/4.png");
	numberTextures[4].setBuffers();
	numberTextures[5].load("..//src//numbers/5.png");
	numberTextures[5].setBuffers();
	numberTextures[6].load("..//src//numbers/6.png");
	numberTextures[6].setBuffers();
	numberTextures[7].load("..//src//numbers/7.png");
	numberTextures[7].setBuffers();
	numberTextures[8].load("..//src//numbers/8.png");
	numberTextures[8].setBuffers();
	numberTextures[9].load("..//src//numbers/9.png");
	numberTextures[9].setBuffers();

	//create shaders
	Shader vSh("..//src//shader_projection.vert");
	Shader fSh("..//src//shader_projection.frag");

	//OpenGL specific values
	//****************************
	// OpenGL calls.
	GLenum err = glewInit();

	//compile the shader code
	//1 for vertex, 2 for fragment
	vSh.getShader(1);
	fSh.getShader(2);

	//Create and link the Shader program
	GLuint shaderProgram;

	shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vSh.shaderID);
	glAttachShader(shaderProgram, fSh.shaderID);
	glLinkProgram(shaderProgram);

	//delete shaders
	glDeleteShader(vSh.shaderID);
	glDeleteShader(fSh.shaderID);

	//set uniform variables
	int modelLocation;
	int viewLocation;
	int projectionLocation;

	glUseProgram(shaderProgram);

	//OpenGL buffers
	//set buffers for shapes
	for (int i = 0; i < 5; i++)
	{
		for (int j = 0; j < 5; j++)
		{
			squares[i][j].setBuffers();
		}
	}

	for (int i = 0; i < 4; i++)
	{
		lives[i].setBuffers();

	}
	scoreDigit1.setBuffers();
	scoreDigit2.setBuffers();
	background.setBuffers();
	paddle.setBuffers();
	ball.setBuffers();

	borderTop.setBuffers();
	borderLeft.setBuffers();
	borderRight.setBuffers();
	borderBottom.setBuffers();

	//tranform matrices 
	glm::mat4 modelMatrix;
	glm::mat4 viewMatrix;
	glm::mat4 projectionMatrix;
	glm::mat4 mTranslate;
	glm::mat4 mRotate;
	glm::mat4 mScale;
	glm::mat4 backgroundScale;
	glm::mat4 backgroundTranslate;
	glm::mat4 paddleTranslate;
	glm::vec3 scaleFactor;
	glm::vec3 b_scaleFactor;

	glm::mat4 ballTranslate;
	glm::mat4 ballMoving;
	glm::mat4 ballScale;

	glm::mat4 collisionTranslate;
	glm::mat4 collisionRotate;

	glm::mat4 livesPosition;
	glm::mat4 livesTranslate;
	glm::mat4 livesScale;

	glm::mat4 scoreScale;
	glm::mat4 score1Translate;
	glm::mat4 score2Translate;

	glm::mat4 borderTopTranslate;
	glm::mat4 borderLeftTranslate;
	glm::mat4 borderRightTranslate;
	glm::mat4 borderBottomTranslate;
	glm::mat4 borderScale1;
	glm::mat4 borderScale2;

	//once only scale to square
	scaleFactor = { 1.0f, 0.4f, 1.0f };
	mScale = glm::scale(mScale, glm::vec3(scaleFactor));

	//once only scale to background
	b_scaleFactor = { 20.0f, 20.0f, 0.0f };
	backgroundScale = glm::scale(backgroundScale, glm::vec3(b_scaleFactor));
	backgroundTranslate = glm::translate(backgroundTranslate, glm::vec3(-2.15f, -2.15f, 0.0f));

	scoreScale = glm::scale(scoreScale, glm::vec3(0.5f, 0.5f, 0.5f));
	score1Translate = glm::translate(score1Translate, glm::vec3(0.85f, 0.1f, 0.0f));
	score2Translate = glm::translate(score2Translate, glm::vec3(0.9f, 0.1f, 0.0f));

	//set position of the lives
	livesPosition = glm::translate(livesPosition, glm::vec3(0.75f, 0.0f, 0.0f));
	livesScale = glm::scale(livesScale, glm::vec3(0.55f, 0.55f, 0.4f));

	//set initial position of paddle
	paddleTranslate = glm::translate(paddleTranslate, glm::vec3(0.3f, 0.9f, 0.0f));

	borderTopTranslate = glm::translate(borderTopTranslate, glm::vec3(-1.0f, -0.01f, 0.0f)); //translation for the top border
	borderLeftTranslate = glm::translate(borderLeftTranslate, glm::vec3(0.98f, -1.0f, 0.0f)); //translation for the left border
	borderRightTranslate = glm::translate(borderRightTranslate, glm::vec3(-0.01f, -1.0f, 0.0f)); //translation for the right border
	borderBottomTranslate = glm::translate(borderBottomTranslate, glm::vec3(-1.0f, 0.98f, 0.0f)); //translation for the bottom border

	borderScale1 = glm::scale(borderScale1, glm::vec3(10.0f, 0.1f, 1.0f)); //scaling for the top and bottom borders
	borderScale2 = glm::scale(borderScale2, glm::vec3(0.1f, 10.0f, 1.0f)); //scaling for the left and right borderd


	float paddlePoint[4]; //keeping track of paddle location, left, right, top, bottom
	paddlePoint[0] = 0.4f; //padddle left
	paddlePoint[1] = 0.5f; // paddle right
	paddlePoint[2] = 0.92f; // paddle top
	paddlePoint[3] = 1.0f; // paddle bottom

	//set initial position of ball
	float ballX = 0.0f; // ball X velocity
	float ballY = 0.0f; // ball Y velocity
	ballTranslate = glm::translate(ballTranslate, glm::vec3(0.45f, 0.92f, 0.0f));
	ballScale = glm::scale(ballScale, glm::vec3(1.0f, 1.5f, 0.0f));

	float ballPoint[2]; //keping track of ball location, X, Y
	ballPoint[0] = 0.45f; // ball X
	ballPoint[1] = 0.92f; // ball Y

	float squarePoints[100]; //points; left, right, top, bottom
#pragma region

	squarePoints[0] = 0.1f, squarePoints[1] = 0.2f, squarePoints[2] = 0.05f, squarePoints[3] = 0.1f; //test points left, right, top, bottom
	squarePoints[4] = 0.25f, squarePoints[5] = 0.35f, squarePoints[6] = 0.05f, squarePoints[7] = 0.1f;
	squarePoints[8] = 0.4f, squarePoints[9] = 0.5f, squarePoints[10] = 0.05f, squarePoints[11] = 0.1f;
	squarePoints[12] = 0.55f, squarePoints[13] = 0.65f, squarePoints[14] = 0.05f, squarePoints[15] = 0.1f;
	squarePoints[16] = 0.7f, squarePoints[17] = 0.8, squarePoints[18] = 0.05f, squarePoints[19] = 0.1f;

	squarePoints[20] = 0.1f, squarePoints[21] = 0.2f, squarePoints[22] = 0.15f, squarePoints[23] = 0.2f;
	squarePoints[24] = 0.25f, squarePoints[25] = 0.35f, squarePoints[26] = 0.15f, squarePoints[27] = 0.2f;
	squarePoints[28] = 0.4f, squarePoints[29] = 0.5f, squarePoints[30] = 0.15f, squarePoints[31] = 0.2f;
	squarePoints[32] = 0.55f, squarePoints[33] = 0.65f, squarePoints[34] = 0.15f, squarePoints[35] = 0.2f;
	squarePoints[36] = 0.7f, squarePoints[37] = 0.8f, squarePoints[38] = 0.15f, squarePoints[39] = 0.2f;

	squarePoints[40] = 0.1f, squarePoints[41] = 0.2f, squarePoints[42] = 0.25f, squarePoints[43] = 0.3f;
	squarePoints[44] = 0.25f, squarePoints[45] = 0.35f, squarePoints[46] = 0.25f, squarePoints[47] = 0.3f;
	squarePoints[48] = 0.4f, squarePoints[49] = 0.5f, squarePoints[50] = 0.25f, squarePoints[51] = 0.3f;
	squarePoints[52] = 0.55f, squarePoints[53] = 0.65f, squarePoints[54] = 0.25f, squarePoints[55] = 0.3f;
	squarePoints[56] = 0.7f, squarePoints[57] = 0.8f, squarePoints[58] = 0.25f, squarePoints[59] = 0.3f;

	squarePoints[60] = 0.1f, squarePoints[61] = 0.2f, squarePoints[62] = 0.35f, squarePoints[63] = 0.4f;
	squarePoints[64] = 0.25f, squarePoints[65] = 0.35f, squarePoints[66] = 0.35f, squarePoints[67] = 0.4f;
	squarePoints[68] = 0.4f, squarePoints[69] = 0.5f, squarePoints[70] = 0.35f, squarePoints[71] = 0.4f;
	squarePoints[72] = 0.55f, squarePoints[73] = 0.65f, squarePoints[74] = 0.35f, squarePoints[75] = 0.4f;
	squarePoints[76] = 0.7f, squarePoints[77] = 0.8f, squarePoints[78] = 0.35f, squarePoints[79] = 0.4f;

	squarePoints[80] = 0.1f, squarePoints[81] = 0.2f, squarePoints[82] = 0.45f, squarePoints[83] = 0.5f;
	squarePoints[84] = 0.25f, squarePoints[85] = 0.35f, squarePoints[86] = 0.45f, squarePoints[87] = 0.5f;
	squarePoints[88] = 0.4f, squarePoints[89] = 0.5f, squarePoints[90] = 0.45f, squarePoints[91] = 0.5f;
	squarePoints[92] = 0.55f, squarePoints[93] = 0.65f, squarePoints[94] = 0.45f, squarePoints[95] = 0.5f;
	squarePoints[96] = 0.7f, squarePoints[97] = 0.8f, squarePoints[98] = 0.45, squarePoints[99] = 0.5f;

#pragma endregion
	
	//set view matrix to identity
	//matrix equivalent of '1'
	viewMatrix = glm::mat4(1.0f);

	//orthographic projection
	//left, right, bottom, top, near clip plane, far clip plane
	//projectionMatrix = glm::ortho(-1.0f, 1.0f, -1.0f, 1.0f, -1.0f, 100.0f);
	projectionMatrix = glm::ortho(0.0f, 1.0f, 1.0f, 0.0f, -1.0f, 100.0f);

	SDL_Event event;
	bool windowOpen = true;

	bool ballmoving = false;
	bool ballmovingup = true;		//create bools for ball movement and direction
	bool ballmovingleft = true;

	int livesCount = 4;
	int scoreCount = 0;

	bool collided[25];
	for (int i = 0; i < 25; i++)
	{
		collided[i] = false; //set all values in colliion array to false
	}

// **************************************************************************

// **************************************************************************

	while (windowOpen)
	{
		//background colour
		glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);  //should be GL_COLOR_BUFFER_BIT

		// Draw the scene
		//need to 'use' the shaders before updating uniforms
		glUseProgram(shaderProgram);

		modelLocation = glGetUniformLocation(shaderProgram, "uModel");
		viewLocation = glGetUniformLocation(shaderProgram, "uView");
		projectionLocation = glGetUniformLocation(shaderProgram, "uProjection");

		//set background image
		glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(backgroundTranslate+backgroundScale));	
		glUniformMatrix4fv(viewLocation, 1, GL_FALSE, glm::value_ptr(viewMatrix));
		glUniformMatrix4fv(projectionLocation, 1, GL_FALSE, glm::value_ptr(projectionMatrix));
		glBindTexture(GL_TEXTURE_2D, texArray[5].texture);
		background.render();

		//set border top image
		glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(borderTopTranslate * borderScale1));
		glUniformMatrix4fv(viewLocation, 1, GL_FALSE, glm::value_ptr(viewMatrix));
		glUniformMatrix4fv(projectionLocation, 1, GL_FALSE, glm::value_ptr(projectionMatrix));
		glBindTexture(GL_TEXTURE_2D, texArray[0].texture);
		borderTop.render();

		//set border left image
		glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(borderLeftTranslate * borderScale2));
		glUniformMatrix4fv(viewLocation, 1, GL_FALSE, glm::value_ptr(viewMatrix));
		glUniformMatrix4fv(projectionLocation, 1, GL_FALSE, glm::value_ptr(projectionMatrix));
		glBindTexture(GL_TEXTURE_2D, texArray[0].texture);
		borderLeft.render();

		//set border right image
		glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(borderRightTranslate * borderScale2));
		glUniformMatrix4fv(viewLocation, 1, GL_FALSE, glm::value_ptr(viewMatrix));
		glUniformMatrix4fv(projectionLocation, 1, GL_FALSE, glm::value_ptr(projectionMatrix));
		glBindTexture(GL_TEXTURE_2D, texArray[0].texture);
		borderRight.render();

		//set border bottom image
		glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(borderBottomTranslate * borderScale1));
		glUniformMatrix4fv(viewLocation, 1, GL_FALSE, glm::value_ptr(viewMatrix));
		glUniformMatrix4fv(projectionLocation, 1, GL_FALSE, glm::value_ptr(projectionMatrix));
		glBindTexture(GL_TEXTURE_2D, texArray[0].texture);
		borderBottom.render();

		//set paddle image
		glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(paddleTranslate*mScale));
		glUniformMatrix4fv(viewLocation, 1, GL_FALSE, glm::value_ptr(viewMatrix));
		glUniformMatrix4fv(projectionLocation, 1, GL_FALSE, glm::value_ptr(projectionMatrix));
		glBindTexture(GL_TEXTURE_2D, texArray[7].texture);
		paddle.render();

		//set ball image
		glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(ballTranslate * ballMoving * ballScale));
		glUniformMatrix4fv(viewLocation, 1, GL_FALSE, glm::value_ptr(viewMatrix));
		glUniformMatrix4fv(projectionLocation, 1, GL_FALSE, glm::value_ptr(projectionMatrix));
		glBindTexture(GL_TEXTURE_2D, texArray[9].texture);
		ball.render();

		//set lives images
		for (int i = 0; i < livesCount; i++)
		{
			glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(livesPosition * livesTranslate * livesScale));
			glUniformMatrix4fv(viewLocation, 1, GL_FALSE, glm::value_ptr(viewMatrix));
			glUniformMatrix4fv(projectionLocation, 1, GL_FALSE, glm::value_ptr(projectionMatrix));
			
			livesTranslate = glm::translate(livesTranslate, glm::vec3(0.045f, 0.0f, 0.0f));

			glBindTexture(GL_TEXTURE_2D, texArray[8].texture);
			lives[i].render();
		}
		livesTranslate = glm::mat4(1.0f);

		if (scoreCount < 10)
		{
			//set score1 images if only one digit required
			glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(score1Translate * scoreScale));
			glUniformMatrix4fv(viewLocation, 1, GL_FALSE, glm::value_ptr(viewMatrix));
			glUniformMatrix4fv(projectionLocation, 1, GL_FALSE, glm::value_ptr(projectionMatrix));
			glBindTexture(GL_TEXTURE_2D, numberTextures[scoreCount].texture);
			scoreDigit1.render();
		}
		else if (scoreCount < 20)
		{
			//set score1 images to 1 (first digit of 10-19)
			glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(score1Translate * scoreScale));
			glUniformMatrix4fv(viewLocation, 1, GL_FALSE, glm::value_ptr(viewMatrix));
			glUniformMatrix4fv(projectionLocation, 1, GL_FALSE, glm::value_ptr(projectionMatrix));
			glBindTexture(GL_TEXTURE_2D, numberTextures[1].texture);
			scoreDigit1.render();

			//set score2 images
			glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(score2Translate * scoreScale));
			glUniformMatrix4fv(viewLocation, 1, GL_FALSE, glm::value_ptr(viewMatrix));
			glUniformMatrix4fv(projectionLocation, 1, GL_FALSE, glm::value_ptr(projectionMatrix));
			glBindTexture(GL_TEXTURE_2D, numberTextures[scoreCount - 10].texture);
			scoreDigit2.render();
		}
		else
		{
			//set score1 images to 2 (first digit of 20-29)
			glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(score1Translate * scoreScale));
			glUniformMatrix4fv(viewLocation, 1, GL_FALSE, glm::value_ptr(viewMatrix));
			glUniformMatrix4fv(projectionLocation, 1, GL_FALSE, glm::value_ptr(projectionMatrix));
			glBindTexture(GL_TEXTURE_2D, numberTextures[2].texture);
			scoreDigit1.render();

			//set score2 images
			glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(score2Translate * scoreScale));
			glUniformMatrix4fv(viewLocation, 1, GL_FALSE, glm::value_ptr(viewMatrix));
			glUniformMatrix4fv(projectionLocation, 1, GL_FALSE, glm::value_ptr(projectionMatrix));
			glBindTexture(GL_TEXTURE_2D, numberTextures[scoreCount - 20].texture);
			scoreDigit2.render();
		}

		//reset translation matrix
		mTranslate = glm::mat4(1.0f);

		//translate to the top left of the squares
		mTranslate = glm::translate(mTranslate, glm::vec3(0.0f, 0.0f, 0.0f));

		//position and draw the squares
		int a = 0; // collided counter
		int texIndex = 0;

		for (int x = 0; x < 5; x++)
		{
			for (int y = 0; y < 5; y++)
			{
				if (collided[a] == true)
				{
					modelMatrix = mTranslate * mRotate  * collisionTranslate * collisionRotate * mScale;
					//transform uniforms
					//set current value of transform matrix in shader
					glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(modelMatrix));
					glUniformMatrix4fv(viewLocation, 1, GL_FALSE, glm::value_ptr(viewMatrix));
					glUniformMatrix4fv(projectionLocation, 1, GL_FALSE, glm::value_ptr(projectionMatrix));

					//gap inbetween blocks on x axis
					mTranslate = glm::translate(mTranslate, glm::vec3(0.15f, 0.0f, 0.0f));

					collisionTranslate = glm::translate(collisionTranslate, glm::vec3(0.0f, 0.0f, 0.0f));
					collisionRotate = glm::rotate(collisionRotate, 1.0f, glm::vec3(0, 0, 1));
					collisionTranslate = glm::translate(collisionTranslate, glm::vec3(0.01f, 0.0f, 0.0f));

					squares[x][y].render();
				}
				else
				{
					modelMatrix = mTranslate * mRotate * mScale;
					//transform uniforms
					//set current value of transform matrix in shader
					glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(modelMatrix));
					glUniformMatrix4fv(viewLocation, 1, GL_FALSE, glm::value_ptr(viewMatrix));
					glUniformMatrix4fv(projectionLocation, 1, GL_FALSE, glm::value_ptr(projectionMatrix));

					//gap inbetween blocks on x axis
					mTranslate = glm::translate(mTranslate, glm::vec3(0.15f, 0.0f, 0.0f));

					//bind the texture we want to use
					glBindTexture(GL_TEXTURE_2D, texArray[texIndex].texture);
					//Draw the grid - call its render method
					squares[x][y].render();
				}
				a++; //incrimenting collided index
			}
			mTranslate = glm::mat4(1.0f);
			//set start of next line of blocks
			mTranslate = glm::translate(mTranslate, glm::vec3(0.0f, 0.0f + ((x + 1) / 10.0f), 0.0f));
			texIndex++; //incrimenting texture index
		}
		//reset the translation matrix using the 'identity' matrix
		mTranslate = glm::mat4(1.0f);

		SDL_GL_SwapWindow(win);

		//Process input
		if (SDL_PollEvent(&event))
		{
			if (event.type == SDL_QUIT)
			{
				windowOpen = false;
			}
			if (event.type == SDL_KEYDOWN)
			{
				switch (event.key.keysym.sym)
				{
				case SDLK_LEFT:
					if (paddlePoint[0] > 0) //wont allow paddle movement off the screeen
					{
						paddleTranslate = glm::translate(paddleTranslate, glm::vec3(-0.05f, 0.0f, 0.0f));
						paddlePoint[0] -= 0.05f;
						paddlePoint[1] -= 0.05f;
						if (ballmoving == false)
						{
							ballMoving = glm::translate(ballMoving, glm::vec3(-0.05f, 0.0f, 0.0f));
							ballPoint[0] -= 0.05f; //update ball point 
						}
						backgroundTranslate = glm::translate(backgroundTranslate, glm::vec3(-0.005, 0.0f, 0.0f));
					}	
					break;
				case SDLK_RIGHT:
					if (paddlePoint[1] < 1.0f)  //wont allow paddle movement off the screeen
					{
						paddleTranslate = glm::translate(paddleTranslate, glm::vec3(0.05f, 0.0f, 0.0f));
						paddlePoint[0] += 0.05f;
						paddlePoint[1] += 0.05f;
						if (ballmoving == false)
						{
							ballMoving = glm::translate(ballMoving, glm::vec3(0.05f, 0.0f, 0.0f));
							ballPoint[0] += 0.05f; //update ball point 
						}
						backgroundTranslate = glm::translate(backgroundTranslate, glm::vec3(0.005, 0.0f, 0.0f));
					}					
					break;
				case SDLK_f:
					SDL_SetWindowFullscreen(win, SDL_WINDOW_FULLSCREEN);
					break;
				case SDLK_q:
					viewMatrix = glm::translate(viewMatrix, glm::vec3(-0.01f, 0.0f, 0.0f));
					break;
				case SDLK_p:
					viewMatrix = glm::translate(viewMatrix, glm::vec3(0.01f, 0.0f, 0.0f));
					break;
				case SDLK_SPACE:
					if (ballmoving == false)
					{
						ballmoving = true; //start ball movement on space key press
					}
					break;
				case SDLK_ESCAPE:
					windowOpen = false;
				default:
					break;
				}
			}
		}

		if (ballmoving == true)
		{
			ballX = ballX;
			ballY = -0.01f;

			if (ballmovingleft == true)
			{
				ballPoint[0] = ballPoint[0] - 0.001f;
				ballX = -0.001f; //updating ball X 
			}
			else if (ballmovingleft == false)
			{
				ballPoint[0] = ballPoint[0] + 0.001f;
				ballX = 0.001f; //updating ball X 
			}

			if (ballmovingup == true)
			{
				ballPoint[1] -= 0.01f; //updating ball Y
				ballMoving = glm::translate(ballMoving, glm::vec3(ballX, ballY, 0.0f));
			}
			else if (ballmovingup == false)
			{
				ballPoint[1] += 0.01f; //updating ball Y
				ballMoving = glm::translate(ballMoving, glm::vec3(ballX, -ballY, 0.0f));
			}
		}
		
		for (int i = 0; i < 100; i += 4) 
		{
			if (ballPoint[1] <= squarePoints[i + 3] && ballPoint[1] >= squarePoints[i + 2] && ballPoint[0] >= squarePoints[i] && ballPoint[0] <= squarePoints[i + 1])
			{
				//AABB coliison for the ball and the blocks
				if (ballmovingup == true)
				{
					ballmovingup = false;
				}
				else if (ballmovingup == false)
				{
					ballmovingup = true;
				}

				collided[i / 4] = true;
				squarePoints[i] = NULL; //removing values from the squarePoints array once block has been hit
				squarePoints[i + 1] = NULL;
				squarePoints[i + 2] = NULL;
				squarePoints[i + 3] = NULL;

				scoreCount++; //incriment score counter when block has been hit
				SDL_Log("Block hit, score incrimented!");

				collisionTranslate = glm::mat4(1.0f);
				collisionRotate = glm::mat4(1.0f);
			}	
		}

		if (ballPoint[1] >= paddlePoint[2] && ballPoint[0] >= paddlePoint[0] && ballPoint[0] <= paddlePoint[1])
		{
			//AABB colliosn for the ball and the paddle
			ballmovingup = true; //reverse ball direction when colliding with paddle
		}

		if (ballPoint[1] < 0.0f)
		{
			ballmovingup = false;
			printf("%f |||", ballPoint[1]);
			//setting bounds for top of the window and reflecting the direction of motion 
		}
		else if (ballPoint[1] > 1.0f)
		{
			//ballmoving = false;
			ballmovingup = true;

			livesCount--; //decriment the lvies counter when ball hits bottom of window

			SDL_Log("Paddle Missed Ball, Life Lost!");
		}

		if (ballPoint[0] < 0.0f)
		{
			ballmovingleft = false;
			//setting bounds for left of the window and reflecting the direction of motion
		}
		else if (ballPoint[0] > 1.0f)
		{
			ballmovingleft = true;
			//setting bounds for right of the window and refelcting the direction of motion;
		}

		if (livesCount == 0 || scoreCount == 25)
		{
			windowOpen = false;
			SDL_Log("game Over");
			//end game once user is out of lives or all blocks have been hit
		}
	}
	// Clean up
	SDL_Log("Finished. Cleaning up and closing down\n");

	// Once finished with OpenGL functions, the SDL_GLContext can be deleted.
	SDL_GL_DeleteContext(glcontext);

	SDL_Quit();
	return 0;
}
#endif