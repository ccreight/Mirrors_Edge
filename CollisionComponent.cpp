#include "CollisionComponent.h"
#include "Actor.h"

CollisionComponent::CollisionComponent(class Actor* owner)
:Component(owner)
,mWidth(0.0f)
,mHeight(0.0f)
,mDepth(0.0f)
{
	
}

CollisionComponent::~CollisionComponent()
{
	
}

bool CollisionComponent::Intersect(const CollisionComponent* other)
{
	Vector3 aMin = GetMin();
	Vector3 aMax = GetMax();
	Vector3 bMin = other->GetMin();
	Vector3 bMax = other->GetMax();

	bool no = aMax.x < bMin.x ||
		aMax.y < bMin.y ||
		aMax.z < bMin.z ||
		bMax.x < aMin.x ||
		bMax.y < aMin.y ||
		bMax.z < aMin.z;

	return !no;
}

Vector3 CollisionComponent::GetMin() const
{
	Vector3 v = mOwner->GetPosition();
	v.x -= mDepth * mOwner->GetScale() / 2.0f;
	v.y -= mWidth * mOwner->GetScale() / 2.0f;
	v.z -= mHeight * mOwner->GetScale() / 2.0f;
	return v;
}

Vector3 CollisionComponent::GetMax() const
{
	Vector3 v = mOwner->GetPosition();
	v.x += mDepth * mOwner->GetScale() / 2.0f;
	v.y += mWidth * mOwner->GetScale() / 2.0f;
	v.z += mHeight * mOwner->GetScale() / 2.0f;
	return v;
}

const Vector3& CollisionComponent::GetCenter() const
{
	return mOwner->GetPosition();
}

CollSide CollisionComponent::GetMinOverlap(
	const CollisionComponent* other, Vector3& offset)
{
	
	if (mOwner == nullptr) {
		offset = Vector3::Zero;
		return CollSide::None;
	}

	// Returns no collision if "this" doesn't intersect with "other"
	if (!this->Intersect(other)) {
		offset = Vector3::Zero;
		return CollSide::None;
	}

	float overlapLeft, overlapRight, overlapTop, overlapBottom, overlapForward, overlapBack;
	float thisMinX, thisMinY, thisMaxX, thisMaxY, thisMinZ, thisMaxZ;
	float otherMinX, otherMinY, otherMaxX, otherMaxY, otherMinZ, otherMaxZ;

	// Sets coordinates of "this"
	thisMinX = this->GetMin().x;
	thisMinY = this->GetMin().y;
	thisMaxX = this->GetMax().x;
	thisMaxY = this->GetMax().y;
	thisMaxZ = this->GetMax().z;
	thisMinZ = this->GetMin().z;

	// Sets coordinates of "other"
	otherMinX = other->GetMin().x;
	otherMinY = other->GetMin().y;
	otherMaxX = other->GetMax().x;
	otherMaxY = other->GetMax().y;
	otherMinZ = other->GetMin().z;
	otherMaxZ = other->GetMax().z;

	// Calculates overlap
	overlapLeft = abs(thisMaxX - otherMinX);
	overlapRight = abs(thisMinX - otherMaxX);
	overlapTop = abs(thisMaxY - otherMinY);
	overlapBottom = abs(thisMinY - otherMaxY);
	overlapForward = abs(thisMaxZ - otherMinZ);
	overlapBack = abs(thisMinZ - otherMaxZ);

	// Finds minimum overlap
	float minValue = fmin(overlapLeft, fmin(overlapRight, fmin(overlapTop, fmin(overlapBottom, fmin(overlapForward, overlapBack)))));

	// Returns where overlap is + sets offset value
	if (minValue == overlapLeft) {
		offset.x = -1 * overlapLeft;
		return CollSide::SideY2;
	}
	else if (minValue == overlapRight) {
		offset.x = overlapRight;
		return CollSide::SideY1;
	}
	else if (minValue == overlapTop) {
		offset.y = -1 * overlapTop;
		return CollSide::SideX2;
	}
	else if (minValue == overlapBottom) {
		offset.y = overlapBottom;
		return CollSide::SideX1;
	}
	else if (minValue == overlapForward) {
		offset.z = overlapForward;
		return CollSide::Top;
	}
	else {
		offset.z = overlapBack;
		return CollSide::Bottom;
	}

}
