#include "SecurityCamera.h"
#include "Game.h"
#include "Renderer.h"
#include "Mesh.h"
#include "SecurityCone.h"
#include <iostream>

SecurityCamera::SecurityCamera(Game* game, Actor* parent) : Actor(game, parent){

	mGame = game;
	mParent = parent;

	mMesh = new MeshComponent(this);
	mMesh->SetMesh(mGame->GetRenderer()->GetMesh("Assets/Camera.gpmesh"));

	mSC = new SecurityCone(mGame, this);
	mPauseTime = 0.0f;
	mInterpTime = 0.0f;

	mLastState = SecurityState::white;

	mGame->mCameras.push_back(this);

}

SecurityCamera::~SecurityCamera() {

	if (mWhiteYellowChannel != -1) {
		Mix_HaltChannel(mWhiteYellowChannel);
	}

}

void SecurityCamera::OnUpdate(float deltaTime) {

	if (mIsPaused) {
		mTimeSincePause += deltaTime;
	}

	if (mIsPaused && mFirstPauseFrame) {
		mFirstPauseFrame = false;
		Mix_HaltChannel(mTurningChannel);
		mTurningChannel = -1;
		float channel = Mix_PlayChannel(Mix_GroupAvailable(1), GetGame()->GetSound("Assets/Sounds/CameraMotorStop.wav"), 0);

		float volume;
		float dist = (GetGame()->mPlayer->GetPosition() - GetPosition()).Length();

		if (dist > 1500.0f) {
			volume = 0;
		}
		else if (dist > 500.0f) {
			volume = 128.0f - (1000.0f / (1000.0f / 128.0f));
		}
		else {
			volume = 128.0f;
		}

		Mix_Volume(channel, volume);
	}

	if (mTimeSincePause >= mPauseTime || mLastState == SecurityState::yellow && mSC->mState == SecurityState::white || 
		mLastState == SecurityState::red && mSC->mState == SecurityState::white) {
		mIsPaused = false;
		mTimeSincePause = 0.0f;
	}

	if ((mLastState == SecurityState::white && mSC->mState == SecurityState::yellow) || (mLastState == SecurityState::yellow && mSC->mState == SecurityState::red)) {
		Mix_HaltChannel(mWhiteYellowChannel);
		mWhiteYellowChannel = Mix_PlayChannel(Mix_GroupAvailable(1), GetGame()->GetSound("Assets/Sounds/SecurityDetected.wav"), 0);
	}
	else if (mLastState != SecurityState::white && mSC->mState == SecurityState::white) {
		Mix_HaltChannel(mWhiteYellowChannel);
		mTurningChannel = Mix_PlayChannel(Mix_GroupAvailable(1), GetGame()->GetSound("Assets/Sounds/CameraMotor.wav"), 0);
	}

	if (mStartToEnd && !mIsPaused) {

		mTimeSinceMove += deltaTime;
		SetQuaternion(Quaternion::Slerp(mStart, mEnd, mTimeSinceMove / mInterpTime));

		if (mFirstStartEnd) {
			mTurningChannel = Mix_PlayChannel(Mix_GroupAvailable(1), GetGame()->GetSound("Assets/Sounds/CameraMotor.wav"), 0);
			mFirstStartEnd = false;
			mFirstEndStart = true;
		}

	}
	else if (!mIsPaused) {

		mTimeSinceMove += deltaTime;
		SetQuaternion(Quaternion::Slerp(mEnd, mStart, mTimeSinceMove / mInterpTime));

		if (mFirstEndStart) {
			mTurningChannel = Mix_PlayChannel(Mix_GroupAvailable(1), GetGame()->GetSound("Assets/Sounds/CameraMotor.wav"), 0);
			mFirstEndStart = false;
			mFirstStartEnd = true;
		}

	}

	// Update sound here
	if (!mIsPaused) {

		float volume;
		float dist = (GetGame()->mPlayer->GetPosition() - GetPosition()).Length();
		
		if (dist > 1500.0f) {
			volume = 0;
		}
		else if (dist > 500.0f) {
			volume = 128.0f - (dist / (1000.0f / 128.0f));
		}
		else {
			volume = 128.0f;
		}

		Mix_Volume(mTurningChannel, volume);
	}

	if (mTimeSinceMove >= mInterpTime && !mIsPaused) {

		mTimeSinceMove = 0.0f;
		mStartToEnd = !mStartToEnd;
		mIsPaused = true;
		mTimeSincePause = 0.0f;
		mFirstPauseFrame = true;

	}
	else if (mSC->mState != SecurityState::white && mLastState == SecurityState::white) {
		mIsPaused = true;
		mTimeSincePause = 0.0f;
		mFirstPauseFrame = true;
	}

	mLastState = mSC->mState;

}