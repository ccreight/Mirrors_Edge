#include "PlayerMove.h"
#include <SDL/SDL.h>
#include "Game.h"
#include "Renderer.h"
#include "Actor.h"
#include "Player.h"
#include <iostream>

PlayerMove::PlayerMove(Player* owner) : MoveComponent(owner) {

	mOwner = owner;

	ChangeState(MoveState::Falling);
	mGravity = Vector3(0, 0, -980.0f);
	mJumpForce = Vector3(0, 0, 35000.0f);
	mWallClimbForce = Vector3(0, 0, 1800.0f);
	mWallRunForce = Vector3(0, 0, 1200.0f);

	mRunningSFX = Mix_PlayChannel(-1, mOwner->GetGame()->GetSound("Assets/Sounds/Running.wav"), -1);
	Mix_Pause(mRunningSFX);

}

PlayerMove::~PlayerMove() {
	Mix_HaltChannel(mRunningSFX);
}

void PlayerMove::Respawn() {
	mOwner->SetPosition(mOwner->getRespawnPos());
	mOwner->SetRotation(0);
	mVelocity = Vector3::Zero;
	mPendingForces = Vector3::Zero;
	mCurrentState = MoveState::Falling;
}

void PlayerMove::Update(float deltaTime) {

	if (mOwner->GetPosition().z < -750.0f) {
		Respawn();
	}

	if (mCurrentState == MoveState::Falling) {
		UpdateFalling(deltaTime);
	}
	else if (mCurrentState == MoveState::Jump) {
		UpdateJump(deltaTime);
	}
	else if (mCurrentState == MoveState::OnGround) {
		UpdateOnGround(deltaTime);
	}
	else if (mCurrentState == MoveState::WallClimb) {
		UpdateWallClimb(deltaTime);
	}
	else if (mCurrentState == MoveState::WallRun) {
		UpdateWallRun(deltaTime);
	}

	if ((mCurrentState == MoveState::OnGround && mVelocity.Length() > 50.0f) || mCurrentState == MoveState::WallClimb ||
		mCurrentState == MoveState::WallRun) {
		Mix_Resume(mRunningSFX);
	}
	else {
		Mix_Pause(mRunningSFX);
	}

}

void PlayerMove::ProcessInput(const Uint8* keyState) {

	int x, y;
	SDL_GetRelativeMouseState(&x, &y);

	float angSpeed = ((float)x) / 500.0f;
	angSpeed *= (float)M_PI * 10.0f;
	SetAngularSpeed(angSpeed);

	float pitchSpeed = ((float)y) / 500.0f;
	pitchSpeed *= (float)M_PI * 10.0f;
	mOwner->GetComponent<CameraComponent>()->SetPitchSpeed(pitchSpeed);

	if (keyState[SDL_SCANCODE_W] && !keyState[SDL_SCANCODE_S]) {
		AddForce(mOwner->GetForward() * SPEED);
	}
	else if(keyState[SDL_SCANCODE_S] && !keyState[SDL_SCANCODE_W]){
		AddForce(mOwner->GetForward() * SPEED * -1.0f);
	}

	if (keyState[SDL_SCANCODE_D] && !keyState[SDL_SCANCODE_A]) {
		AddForce(mOwner->GetRight() * SPEED);
	}
	else if (keyState[SDL_SCANCODE_A] && !keyState[SDL_SCANCODE_D]) {
		AddForce(mOwner->GetRight() * SPEED * -1.0f);
	}

	if (keyState[SDL_SCANCODE_SPACE] && !mSpacePressedLast && mCurrentState == MoveState::OnGround) {
		Mix_Chunk* sound = mOwner->GetGame()->GetSound("Assets/Sounds/Jump.wav");
		Mix_PlayChannel(-1, sound, 0);
		mSpacePressedLast = true;
		AddForce(mJumpForce);
		ChangeState(MoveState::Jump);
	}
	else if (!keyState[SDL_SCANCODE_SPACE]) {
		mSpacePressedLast = false;
	}

}

void PlayerMove::UpdateOnGround(float deltaTime) {
	
	PhysicsUpdate(deltaTime);

	CollisionComponent* c = mOwner->GetComponent<CollisionComponent>();
	bool isFalling = true;

	for (auto b : mOwner->GetGame()->mBlocks) {

		CollSide col = FixCollision(c, b->GetComponent<CollisionComponent>());

		if (col == CollSide::Bottom) {
			isFalling = false;
		}

		else if (col != CollSide::Top && col != CollSide::None) {
			if (CanWallClimb(col)) {
				ChangeState(MoveState::WallClimb);
				mWallClimbTimer = 0.0f;
				return;
			}
		}

	}

	if (isFalling) {
		ChangeState(MoveState::Falling);
	}

}

void PlayerMove::UpdateJump(float deltaTime) {

	AddForce(mGravity);
	PhysicsUpdate(deltaTime);

	CollisionComponent* c = mOwner->GetComponent<CollisionComponent>();

	for (auto b : mOwner->GetGame()->mBlocks) {

		CollSide col = FixCollision(c, b->GetComponent<CollisionComponent>());

		if (col == CollSide::Top) {
			mVelocity.z = 0.0f;
		}

		else if (col != CollSide::Top && col != CollSide::None) {
			if (CanWallClimb(col)) {
				ChangeState(MoveState::WallClimb);
				mWallClimbTimer = 0.0f;
				return;
			}
			else if (CanWallRun(col)) {
				ChangeState(MoveState::WallRun);
				mWallRunTimer = 0.0f;
				return;
			}
		}

	}

	if (mVelocity.z <= 0.0f) {
		ChangeState(MoveState::Falling);
	}

}

void PlayerMove::UpdateFalling(float deltaTime) {

	AddForce(mGravity);
	PhysicsUpdate(deltaTime);

	/*mVelocity.z += mGravity.z * deltaTime;
	mOwner->SetPosition(mOwner->GetPosition() + Vector3(0, 0, mVelocity.z * deltaTime));*/

	CollisionComponent* c = mOwner->GetComponent<CollisionComponent>();

	for (auto b : mOwner->GetGame()->mBlocks) {

		CollSide col = FixCollision(c, b->GetComponent<CollisionComponent>());

		if (col == CollSide::Bottom) {
			mVelocity.z = 0.0f;
			Mix_Chunk* sound = mOwner->GetGame()->GetSound("Assets/Sounds/Land.wav");
			Mix_PlayChannel(-1, sound, 0);
			ChangeState(MoveState::OnGround);
		}

	}

}

void PlayerMove::UpdateWallClimb(float deltaTime) {

	AddForce(mGravity);

	mWallClimbTimer += deltaTime;

	if (mWallClimbTimer < 0.4f) {
		AddForce(mWallClimbForce);
	}

	PhysicsUpdate(deltaTime);

	CollisionComponent* c = mOwner->GetComponent<CollisionComponent>();
	bool noCollSide = true;

	for (auto b : mOwner->GetGame()->mBlocks) {

		CollSide col = FixCollision(c, b->GetComponent<CollisionComponent>());

		if (col != CollSide::Bottom && col != CollSide::None && col != CollSide::Top) {
			noCollSide = false;
		}

	}

	if (noCollSide || mVelocity.z <= 0.0f) {
		mVelocity.z = 0.0f;
		ChangeState(MoveState::Falling);
	}

}

void PlayerMove::UpdateWallRun(float deltaTime) {

	AddForce(mGravity);

	mWallRunTimer += deltaTime;

	if (mWallRunTimer < 0.4f) {
		AddForce(mWallRunForce);
	}

	PhysicsUpdate(deltaTime);

	CollisionComponent* c = mOwner->GetComponent<CollisionComponent>();

	for (auto b : mOwner->GetGame()->mBlocks) {

		CollSide col = FixCollision(c, b->GetComponent<CollisionComponent>());

	}

	if (mVelocity.z <= 0.0f) {
		mVelocity.z = 0.0f;
		mOwner->GetComponent<CameraComponent>()->SetTilt(false, CollSide::None);
		ChangeState(MoveState::Falling);
	}

}

bool PlayerMove::CanWallClimb(CollSide c) {

	Vector3 forward;

	if (c == CollSide::SideX1) {
		forward = Vector3(0, 1, 0);
	}
	else if (c == CollSide::SideX2) {
		forward = Vector3(0, -1, 0);
	}
	else if (c == CollSide::SideY1) {
		forward = Vector3(-1, 0, 0);
	}
	else {
		forward = Vector3(1, 0, 0);
	}

	float dotProduct = Vector3::Dot(forward, mOwner->GetForward());
	float velocityDot = Vector3::Dot(forward, mVelocity);

	if (dotProduct > .8 && velocityDot > .8 && mVelocity.Length() > 350.0f) {
		return true;
	}

	return false;

}

bool PlayerMove::CanWallRun(CollSide c) {

	Vector3 forward;

	if (c == CollSide::SideX1) {
		forward = Vector3(0, 1, 0);
	}
	else if (c == CollSide::SideX2) {
		forward = Vector3(0, -1, 0);
	}
	else if (c == CollSide::SideY1) {
		forward = Vector3(-1, 0, 0);
	}
	else {
		forward = Vector3(1, 0, 0);
	}

	float dotProduct = Vector3::Dot(forward, mOwner->GetForward());
	float velocityDot = Vector3::Dot(mOwner->GetForward(), Vector3::Normalize(mVelocity));

	if (dotProduct < .4 && velocityDot > .6 && mVelocity.Length() > 350.0f) {
		mOwner->GetComponent<CameraComponent>()->SetTilt(true, c);
		return true;
	}

	return false;

}

CollSide PlayerMove::FixCollision(class CollisionComponent* self, class CollisionComponent* block) {

	Vector3 offset;
	CollSide c = self->GetMinOverlap(block, offset);

	if (c != CollSide::None) {
		mOwner->SetPosition(mOwner->GetPosition() + offset);
	}

	if (c == CollSide::SideX1) {
		AddForce(Vector3(0, SPEED, 0));
	}
	else if (c == CollSide::SideX2) {
		AddForce(Vector3(0, -1 * SPEED, 0));
	}
	else if (c == CollSide::SideY1) {
		AddForce(Vector3(SPEED, 0, 0));
	}
	else if (c == CollSide::SideY2) {
		AddForce(Vector3(-1 * SPEED, 0, 0));
	}

	return c;

}

void PlayerMove::PhysicsUpdate(float deltaTime) {

	mAcceleration = mPendingForces * (1.0f / mMass);
	mVelocity += mAcceleration * deltaTime;

	FixXYVelocity();

	mOwner->SetPosition(mOwner->GetPosition() + mVelocity * deltaTime);
	mOwner->SetRotation(mOwner->GetRotation() + GetAngularSpeed() * deltaTime);

	mPendingForces = Vector3::Zero;

}

void PlayerMove::AddForce(const Vector3& force) {

	mPendingForces += force;

}

void PlayerMove::FixXYVelocity() {

	Vector2 xyVelocity;
	xyVelocity.x = mVelocity.x;
	xyVelocity.y = mVelocity.y;

	if (xyVelocity.Length() > XY_SPEED) {
		xyVelocity.Normalize();
		xyVelocity *= XY_SPEED;
	}

	if (mCurrentState == MoveState::OnGround || mCurrentState == MoveState::WallClimb) {
		if (Math::NearZero(mAcceleration.x) || (mAcceleration.x < 0.0f && xyVelocity.x > 0.0f) || (mAcceleration.x > 0.0f && xyVelocity.x < 0.0f)) {
			xyVelocity.x *= BREAK_FACTOR;
		}
		if (Math::NearZero(mAcceleration.y) || (mAcceleration.y < 0.0f && xyVelocity.y > 0.0f) || (mAcceleration.y > 0.0f && xyVelocity.y < 0.0f)) {
			xyVelocity.y *= BREAK_FACTOR;
		}
	}

	mVelocity.x = xyVelocity.x;
	mVelocity.y = xyVelocity.y;

}