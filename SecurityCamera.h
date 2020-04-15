#pragma once
#include "Actor.h"
#include "MeshComponent.h"

enum class SecurityState {
	white,
	yellow,
	red
};

class SecurityCamera : public Actor {

	public:
		SecurityCamera(class Game* game, Actor* parent);
		~SecurityCamera();

		void SetStartQuat(Quaternion q) {
			mStart = q;
			mQuat = q;
		}

		void SetEndQuat(Quaternion q) {
			mEnd = q;
		}

		void SetInterpTime(float f) {
			mInterpTime = f;
		}

		void SetPauseTime(float f) {
			mPauseTime = f;
		}

		void OnUpdate(float deltaTime);

	private:
		class Game* mGame;
		Actor* mParent;
		MeshComponent* mMesh;
		class SecurityCone* mSC;

		Quaternion mStart;
		Quaternion mEnd;
		float mInterpTime;
		float mPauseTime;

		bool mStartToEnd = true;
		float mTimeSinceMove = 0.0f;
		bool mIsPaused = false;
		float mTimeSincePause = 0.0f;
		SecurityState mLastState;

		int mWhiteYellowChannel = 0;
		int mTurningChannel = 0;

		bool mFirstStartEnd = true;
		bool mFirstEndStart = false;
		bool mFirstPauseFrame = true;

};