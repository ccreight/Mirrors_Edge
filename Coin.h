#pragma once
#include "MeshComponent.h"
#include "CollisionComponent.h"
#include "Actor.h"

class Coin : public Actor {
	
public:
	Coin(class Game* game, Actor* actor);

private:
	class Game* mGame;
	Actor* mParent;
	MeshComponent* mMesh;
	CollisionComponent* mCollision;
	void OnUpdate(float deltaTime);

};