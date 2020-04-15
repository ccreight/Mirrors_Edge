#include "MoveComponent.h"
#include <iostream>

MoveComponent::MoveComponent(class Actor* owner)
:Component(owner, 50)
,mAngularSpeed(0.0f)
,mForwardSpeed(0.0f)
{
}

void MoveComponent::Update(float deltaTime)
{
	Vector3 forward = mOwner->GetForward();
	Vector3 right = mOwner->GetRight();

	mOwner->SetPosition(mOwner->GetPosition() + Vector3(forward.x * mForwardSpeed * deltaTime, forward.y * mForwardSpeed * deltaTime, 0.0f));
	mOwner->SetPosition(mOwner->GetPosition() + Vector3(right.x * mStrafeSpeed * deltaTime, right.y * mStrafeSpeed * deltaTime, 0.0f));

	// NOT HANDLING ANGULAR SPEED

}
