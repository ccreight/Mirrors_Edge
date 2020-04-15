#pragma once
#include "Actor.h"
#include "MeshComponent.h"

class Arrow : public Actor {

public: 
	Arrow(class Game* game, Actor* parent);

private:
	class Game* mGame;
	MeshComponent* mMesh;
	Actor* mParent;
	void OnUpdate(float deltaTime);
	//Quaternion mQuat;

};