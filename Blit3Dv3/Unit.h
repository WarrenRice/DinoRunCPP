#pragma once
#include<Blit3D.h>

class Unit {
public:
	std::vector<Sprite*> spriteList;
	int frameNumber = 0;
	float frameTimer = 0;

	//Sprite* sprite = NULL;

	glm::vec2 velocity, position;	

	//float ymix = 64.f;
	//float ymin = 0.f;

	bool jump = false;
	bool alive = true;
	int jumpTimes = 0;
	
	void Draw();
	void Update(float seconds);
	void Jump();
	void Drop();
};


class Bad {
public:
	Sprite* sprite = NULL;

	glm::vec2 velocity, position;

	//float ymix = 64.f;
	//float ymin = 0.f;

	void Draw();
	void Update(float seconds);
};

enum class BadType {G, A};

Bad BadStuffGenerator(BadType type, float sceneSpeed);
void InitializeRNG();
BadType RandomBadStuff();

float FindDistanceX(glm::vec2 pos1, glm::vec2 pos2);
float FindDistanceY(glm::vec2 pos1, glm::vec2 pos2);

bool Collide(Unit* u, Bad& b);
