#pragma once
#include "Actor.h"
#include "MeshComponent.h"
#include "LaserComponent.h"

class LaserMine : public Actor {

	public:
		LaserMine(class Game* game, Actor* parent);

	private:
		class Game* mGame;
		MeshComponent* mMesh;
		LaserComponent* mLaser;

};