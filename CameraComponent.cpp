#include "CameraComponent.h"
#include "Actor.h"
#include "Game.h"
#include "Renderer.h"
#include <iostream>

CameraComponent::CameraComponent(Actor* owner) : Component(owner){
	mOwner = owner;
	mCollSide = CollSide::None;
}

void CameraComponent::SetTilt(bool b, CollSide c) {

	mIsTilted = b;
	mCollSide = c;

}

void CameraComponent::Update(float deltaTime) {

	Matrix4 completeRotation;

	mPitchAngle += mPitchSpeed * deltaTime;

	if (mPitchAngle < -1 * Math::PiOver2 / 2.0f) {
		mPitchAngle = -1 * Math::PiOver2 / 2.0f;
	}
	else if (mPitchAngle > Math::PiOver2 / 2.0f) {
		mPitchAngle = Math::PiOver2 / 2.0f;
	}

	Matrix4 matPitch = Matrix4::CreateRotationY(mPitchAngle);
	Matrix4 matYaw = Matrix4::CreateRotationZ(mOwner->GetRotation());
	completeRotation = matPitch * matYaw;

	mTempPitch = mPitchAngle;
	mRollAngle = 0.0f;

	Vector3 transformed = Vector3(1,0,0); 
	transformed = Vector3::Transform(transformed, completeRotation);

	Matrix4 viewMatrix = Matrix4::CreateLookAt(mOwner->GetPosition(), mOwner->GetPosition() + transformed, Vector3::UnitZ);
	mOwner->GetGame()->GetRenderer()->SetViewMatrix(viewMatrix);

}