#pragma once
#include "Actor.h"
#include "MeshComponent.h"
#include "SecurityCamera.h"

class SecurityCone : public Actor {

public:
	SecurityCone(class Game* game, Actor* parent);
	bool Collides() {
		return mColliding;
	}
	SecurityState mState;

private:
	class Game* mGame;
	Actor* mParent;
	MeshComponent* mMesh;

	void OnUpdate(float deltaTime);

	float mTimeSinceYellow = 0.0f;
	float mTimeSinceRed = 0.0f;
	bool mColliding;

};