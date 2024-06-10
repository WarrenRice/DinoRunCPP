#include "Unit.h"
#include <random>

std::mt19937 rng;

void Unit::Draw() {
	//sprite->Blit(position.x, position.y);
	spriteList[frameNumber]->Blit(position.x, position.y);
}

void Unit::Update(float seconds){

	position += velocity * seconds;

	if (position.y <= 128) {	//stop y at ground = 128
		position.y = 128;
		jumpTimes = 3;			//reload jump
	}

	frameTimer += seconds;

	if (frameTimer >= 1.f / 20.f) {		//slow animation
		
		if (position.y > 160) {
			frameNumber = 0;
		}
		else {
			frameNumber++;
			if (frameNumber > 1) {
				frameNumber = 0;
			}

			frameTimer -= 1.f / 20.f;
		}
	}
}

void Unit::Jump() {

	velocity.y = 600;
	jumpTimes = jumpTimes - 1;

}

void Unit::Drop() {

	velocity.y = -1000;
	jumpTimes = 0;

}

void Bad::Draw() {
	sprite->Blit(position.x, position.y);
}

void Bad::Update(float seconds) {
	position += velocity * seconds;
}

extern Sprite* badGroundSprite;
extern Sprite* badAirSprite;

std::uniform_real_distribution<float> posDistY(160.f, 640.f);


Bad BadStuffGenerator(BadType type, float sceneSpeed) {
	Bad B;

	B.velocity.y = 0;
	B.position.x = 2000.f;		//start in the back

	switch(type) {
	case BadType::G:
		B.sprite = badGroundSprite;
		B.velocity.x = -sceneSpeed;
		B.position.y = 128.f;
		break;
	case BadType::A:
		B.sprite = badAirSprite;
		B.velocity.x = -sceneSpeed * 1.2;
		B.position.y = posDistY(rng);
		break;
	}

	return B;
}

void InitializeRNG() {
	std::random_device rd;
	rng.seed(rd());
}

std::uniform_int_distribution<int> BanRngDist(0, 1);

BadType RandomBadStuff() {
	int type = BanRngDist(rng);

	switch (type) {
	case 1:
		return BadType::A;
		break;
	default:
		return BadType::G;
		break;
	}
}

float FindDistanceX(glm::vec2 pos1, glm::vec2 pos2) {
	return (pos1.x - pos2.x) * (pos1.x - pos2.x);
}

float FindDistanceY(glm::vec2 pos1, glm::vec2 pos2) {
	return (pos1.y - pos2.y) * (pos1.y - pos2.y);
}


bool Collide(Unit* u, Bad& b) {			//V.0: need to update when the unit and bad sprite change in V.0.2
	if ( FindDistanceX(u->position, b.position) < 2500.f) {  //sprite diamension is 64px
		if (FindDistanceY(u->position, b.position) < 2500.f) {
			return true;
		}
		return false;
	}
	return false;
}
