#pragma once
#include "MoveComponent.h"
#include "CollisionComponent.h"

enum class MoveState {

	OnGround,
	Jump,
	Falling,
	WallClimb,
	WallRun

};

class PlayerMove : public MoveComponent {

	public:
		PlayerMove(class Player* owner);
		~PlayerMove();

		void Update(float deltaTime);
		void ProcessInput(const Uint8* keyState);

		void ChangeState(MoveState m) {
			mCurrentState = m;
		}

		CollSide FixCollision(CollisionComponent* self, CollisionComponent* block);

		void PhysicsUpdate(float deltaTime);
		void AddForce(const Vector3& force);

		void Respawn();

	private:
		class Player* mOwner;
		const float SPEED = 700.0f;
		const float XY_SPEED = 400.0f;
		const float BREAK_FACTOR = 0.9f;
		MoveState mCurrentState;
		/*float mZSpeed = 0.0f;
		const float GRAVITY = -980.0f;
		const float JUMP_SPEED = 500.0f;*/
		bool mSpacePressedLast = false;
		Vector3 mVelocity;
		Vector3 mAcceleration;
		Vector3 mPendingForces;
		float mMass = 1.0f;
		Vector3 mGravity;
		Vector3 mJumpForce;
		Vector3 mWallClimbForce;
		float mWallClimbTimer = 0.0f;
		Vector3 mWallRunForce;
		float mWallRunTimer = 0.0f;
		int mRunningSFX;

	protected:
		void UpdateOnGround(float deltaTime);
		void UpdateJump(float deltaTime);
		void UpdateFalling(float deltaTime);
		void UpdateWallClimb(float deltaTime);
		void UpdateWallRun(float deltaTime);
		void FixXYVelocity();
		bool CanWallClimb(CollSide c);
		bool CanWallRun(CollSide c);

};