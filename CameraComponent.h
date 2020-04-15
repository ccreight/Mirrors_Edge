#pragma once
#include "Component.h"
#include "Math.h"
#include "CollisionComponent.h"

class CameraComponent : public Component {

	public:
		CameraComponent(class Actor* owner);
		void Update(float deltaTime);
		float GetPitchSpeed() { return mPitchSpeed; }
		void SetPitchSpeed(float s) { mPitchSpeed = s; }
		void SetTilt(bool b, CollSide c);

	private:
		class Actor* mOwner;
		const float H_DIST = 60.0f;
		const float V_DIST = 70.0f;
		const float TARGET_DIST = 75.0f;
		float mPitchAngle = 0.0f;
		float mPitchSpeed = 0.0f;

		bool mIsTilted = false;
		CollSide mCollSide;
		float mRollAngle = 0.0f;
		float mTempPitch = 0.0f;

};