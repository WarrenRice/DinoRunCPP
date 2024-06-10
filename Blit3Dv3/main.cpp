/*
	Asteroids ship movement example
*/
//memory leak detection
#define CRTDBG_MAP_ALLOC
#ifdef _DEBUG
	#ifndef DBG_NEW
		#define DBG_NEW new ( _NORMAL_BLOCK , __FILE__ , __LINE__ )
		#define new DBG_NEW
	#endif
#endif  // _DEBUG

#include <stdlib.h>
#include <crtdbg.h>

#include "Blit3D.h"

#include "Unit.h"
#include <random>
#include <fstream>
#include <iostream>
#include <algorithm>

void drawScore(int s);

Blit3D *blit3D = NULL;

//GLOBAL DATA
extern std::mt19937 rng;

double elapsedTime = 0;
float timeSlice = 1.f / 120.f;

Sprite* backgroundSprite1 = NULL; //a pointer to a background sprite
Sprite* backgroundSprite2 = NULL; //a pointer to a background sprite

Unit* player = NULL;
Sprite* badGroundSprite = NULL;
Sprite* badAirSprite = NULL;

std::uniform_real_distribution<float> reloadBad(1.f, 2.5f);

std::vector<Bad> badList;

bool jump = false;
bool drop = false;
double timerG = 0;			//reload timer for badGround
double timerA = 7.14;			//reload timer for badAir
float speedMain = 500.f;	//scenespeed

float bg1_x = 1920.f / 2;
float bg2_x = 1920.f / 2 + 1920.f;

glm::vec2 gravity = glm::vec2(0, -10);

enum class GameState { PLAYING, PAUSE, GAMEOVER, START};

GameState gameState = GameState::START; //set starting game state

Sprite* titleGraphic = NULL;	//Title image

//Fonts
int score = 0;					//set score = 0

//import font
AngelcodeFont* RobotCrush100 = NULL;

void setUp() {
	player->position = glm::vec2(128.f, 128.f);
	player->alive = true;

	score = 0;
	timerG = 0;
	timerA = 7.14;

	bg1_x = 1920.f / 2;
	bg2_x = 1920.f / 2 + 1920.f;

	badList.clear();

}

void Init()
{
	//turn cursor off
	blit3D->ShowCursor(false);

	//load our background image: the arguments are upper-left corner x, y, width to copy, height to copy, and file name.
	backgroundSprite1 = blit3D->MakeSprite(0, 0, 1920, 1080, "Media\\bg1.png");
	backgroundSprite2 = blit3D->MakeSprite(0, 0, 1920, 1080, "Media\\bg1.png");

	//create a ship
	player = new Unit; //instantiation

	//load a sprite off of a spritesheet
	for (int i = 0; i < 4; ++i)
		player->spriteList.push_back(blit3D->MakeSprite(i * 64, 0, 64, 64, "Media\\Units.png"));

	//player->sprite = blit3D->MakeSprite(0, 0, 64, 64, "Media\\Unit.png");

	//set up player's unit postion
	player->position = glm::vec2(128.f, 128.f);
	
	badGroundSprite = blit3D->MakeSprite(0, 0, 64, 64, "Media\\badGround.png");
	badAirSprite = blit3D->MakeSprite(0, 0, 64, 64, "Media\\badAir.png");

	//load Font
	RobotCrush100 = blit3D->MakeAngelcodeFontFromBinary32("Media\\RobotCrush100.bin");

	//set the clear colour
	//glClearColor(1.0f, 0.0f, 1.0f, 0.0f);	//clear colour: r,g,b,a 	
	glClearColor(1.0f, 1.0f, 1.0f, 0.0f);	//clear colour: r,g,b,a 	
}

void DeInit(void)
{
	if(player != NULL) delete player;
	//any sprites/fonts still allocated are freed automatically by the Blit3D object when we destroy it
}

void Update(double seconds)
{


	switch (gameState) {
		//update by a full timeslice when it's time
	case GameState::PLAYING:
		//only update time to a maximun amount - prevents big jumps in 
		//the simulation if the computer "hiccups"
		if (seconds < 0.15)
			elapsedTime += seconds;
		else elapsedTime += 0.15;


		while (elapsedTime >= timeSlice) {

			//move background
			bg1_x = bg1_x - timeSlice * speedMain;
			bg2_x = bg2_x - timeSlice * speedMain;

			//reposition background
			if (bg1_x < (1920.f / 2 - 1920.f)) {
				bg1_x = 1920.f / 2 + 1920.f;
			}
			if (bg2_x < (1920.f / 2 - 1920.f)) {
				bg2_x = 1920.f / 2 + 1920.f;
			}
			
			
			elapsedTime -= timeSlice;

			player->Update(timeSlice);

			player->velocity.y += gravity.y; //apply gravity to player velocity

			timerG -= elapsedTime;	//count down to generate bads
			if (timerG < 0) {
				badList.push_back(BadStuffGenerator(BadType::G, speedMain));

				timerG = reloadBad(rng) - 0.875;
			}

			timerA -= elapsedTime;	//count down to generate bads
			if (timerA < 0) {
				badList.push_back(BadStuffGenerator(BadType::A, speedMain));

				timerA = reloadBad(rng) + 0.314;
			}

			//update bad
			for (auto& B : badList) B.Update(timeSlice);

			if (jump) {	//if jump is pressed
				if (player->jumpTimes > 0) {
					player->Jump();	
				}
				jump = false;
				drop = false;
			}

			if (drop) {	//if drop is pressed
				if (player->position.y > 160) {
					player->Drop();
				}
				jump = false;
				drop = false;
			}

			//check collide
			for (auto B : badList) {	
				if (Collide(player, B)) {
					gameState = GameState::GAMEOVER;
					break;
				}
			}

			//check bad is out
			for (int bIndex = badList.size() - 1; bIndex >= 0; --bIndex) {
				if (badList[bIndex].position.x < 0) {
					
					badList.erase(badList.begin() + bIndex);
					
				}
			}
			score += 1;

		}
		break; //end PLAYING state

		case GameState::START:

			setUp();	//reset 

			gameState = GameState::PAUSE;
			break;

		case GameState::PAUSE:
		case GameState::GAMEOVER:
		default:
			break;
	}
}

void Draw(void)
{
	// wipe the drawing surface clear
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//draw stuff here

	switch (gameState) {

	case GameState::PAUSE: {
		backgroundSprite1->Blit(bg1_x, 1080.f / 2);
		backgroundSprite2->Blit(bg2_x, 1080.f / 2);

		player->Draw();

		for (auto& B : badList) B.Draw();

		//draw the score
		drawScore(score);

		std::string titleString = "A PERSON WHO CAN JUMP";
		float widthTitle = RobotCrush100->WidthText(titleString);
		RobotCrush100->BlitText((1920.f / 2) - (widthTitle / 2), 615.f, titleString);

		titleString = "TO AVOID BAD STUFFS GAME";
		widthTitle = RobotCrush100->WidthText(titleString);
		RobotCrush100->BlitText((1920.f / 2) - (widthTitle / 2), 465.f, titleString);

	}
	break;

	case GameState::GAMEOVER: {
		backgroundSprite1->Blit(bg1_x, 1080.f / 2);
		backgroundSprite2->Blit(bg2_x, 1080.f / 2);

		player->Draw();

		for (auto& B : badList) B.Draw();

		//draw the score
		drawScore(score);

		std::string titleString = "GAME OVER";
		float widthTitle = RobotCrush100->WidthText(titleString);
		RobotCrush100->BlitText((1920.f / 2) - (widthTitle / 2), 1080.f/2, titleString);

	}
	break;

	case GameState::START: 

	case GameState::PLAYING:
		//draw the background in the middle of the screen
		//the arguments to Blit(0 are the x, y pixel coords to draw the center of the sprite at, 
		//starting as 0,0 in the bottom-left corner.
		backgroundSprite1->Blit(bg1_x, 1080.f / 2);
		backgroundSprite2->Blit(bg2_x, 1080.f / 2);

		player->Draw();

		for (auto& B : badList) B.Draw();

		//draw the score
		drawScore(score);

		break;
	}
}

//the key codes/actions/mods for DoInput are from GLFW: check its documentation for their values
void DoInput(int key, int scancode, int action, int mods)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		blit3D->Quit(); //start the shutdown sequence

	switch (gameState) {

	case GameState::GAMEOVER:
		if (action == GLFW_RELEASE)
			gameState = GameState::START;
		break;
		

	case GameState::PAUSE:
		if (action == GLFW_RELEASE)
			gameState = GameState::PLAYING;
		break;

	case GameState::PLAYING:
		if (key == GLFW_KEY_SPACE && action == GLFW_PRESS)
			jump = true;

		if (key == GLFW_KEY_S && action == GLFW_PRESS)
			drop = true;

		if (key == GLFW_KEY_P && action == GLFW_RELEASE)
			gameState = GameState::PAUSE;
		break;

	case GameState::START:
	default:
		break;
	}//end gameState switch
}

int main(int argc, char *argv[])
{
	//memory leak detection
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

	//set X to the memory allocation number in order to force a break on the allocation:
	//useful for debugging memory leaks, as long as your memory allocations are deterministic.
	//_crtBreakAlloc = X;

	blit3D = new Blit3D(Blit3DWindowModel::BORDERLESSFULLSCREEN_1080P, 640, 400);

	//set our callback funcs
	blit3D->SetInit(Init);
	blit3D->SetDeInit(DeInit);
	blit3D->SetUpdate(Update);
	blit3D->SetDraw(Draw);
	blit3D->SetDoInput(DoInput);

	//Run() blocks until the window is closed
	blit3D->Run(Blit3DThreadModel::SINGLETHREADED);
	if (blit3D) delete blit3D;
}


void drawScore(int s) {
	s = s / 100;
	std::string scoreString = std::to_string(s);
	float widthTitle = RobotCrush100->WidthText(scoreString);
	RobotCrush100->BlitText((1920.f / 2) - (widthTitle / 2), 1080 - 20, scoreString);
}