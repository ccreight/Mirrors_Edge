#include "SecurityCone.h"
#include "Game.h"
#include "Renderer.h"
#include "Mesh.h"
#include <iostream>
#include "SecurityCamera.h"

SecurityCone::SecurityCone(Game* game, Actor* parent) : Actor(game, parent) {

	mGame = game;
	mParent = parent;

	mMesh = new MeshComponent(this, true);
	mMesh->SetMesh(mGame->GetRenderer()->GetMesh("Assets/Cone.gpmesh"));

	SetPosition(Vector3(30, 0, 0));
	SetScale(3.0f);

	mState = SecurityState::white;
	mColliding = false;

}

void SecurityCone::OnUpdate(float deltaTime) {

	mColliding = false;

	float theta = acos(Vector3::Dot(mGame->mPlayer->GetPosition() - GetWorldPosition(), GetWorldForward()) / (mGame->mPlayer->GetPosition() - GetWorldPosition()).Length());
	float dist = Vector3::Dot(mGame->mPlayer->GetPosition() - GetWorldPosition(), GetWorldForward());

	if (theta <= Math::PiOver2 / 3.0f && dist <= 300.0f) {
		mColliding = true;
	}

	if (mState == SecurityState::white && mColliding) {
		mMesh->SetTextureIndex((int)SecurityState::yellow);
		mState = SecurityState::yellow;
		mTimeSinceYellow = 0.0f;
		mTimeSinceRed = 0.0f;
	}
	else if (mState == SecurityState::yellow) {
		mTimeSinceYellow += deltaTime;
		mTimeSinceRed = 0.0f;
	}

	if (mTimeSinceYellow > 2.0f) {
		mMesh->SetTextureIndex((int)SecurityState::red);
		mState = SecurityState::red;
		mTimeSinceRed += deltaTime;
	}

	if (mTimeSinceRed >= 0.5f) {
		mGame->mPlayer->GetComponent<PlayerMove>()->Respawn();
	}

	if (!mColliding) {
		mMesh->SetTextureIndex((int)SecurityState::white);
		mState = SecurityState::white;
		mTimeSinceYellow = 0.0f;
		mTimeSinceRed = 0.0f;
	}

}