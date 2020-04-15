#include "Actor.h"
#include "Game.h"
#include "Component.h"
#include <algorithm>

Actor::Actor(Game* game, Actor* parent)
	:mGame(game)
	,mState(ActorState::Active)
	,mPosition(Vector3::Zero)
	,mScale(1.0f)
	,mRotation(0.0f)
{

	mParent = parent;

	if (parent == nullptr) {
		game->AddActor(this);
	}
	else {
		mParent->AddChild(this);
	}
	
}

Actor::~Actor()
{

	while (!mChildren.empty()) {
		delete mChildren.back();
	}

	if (mParent == nullptr) {
		mGame->RemoveActor(this);
	}
	else {
		mParent->RemoveChild(this);
	}

	std::vector<Component*> temp = mComponents;

	for (unsigned int i = 0; i < temp.size(); i++) {
		delete temp[i];
	}

	mComponents.clear();

}

Vector3 Actor::GetForward() {

	// Calculates the forward position
	Vector3 position = Vector3(cos(mRotation), sin(mRotation), 0.0f);
	return position;

}

Vector3 Actor::GetRight() {

	Vector3 position = Vector3(cos(mRotation + Math::PiOver2), sin(mRotation + Math::PiOver2), 0.0f);
	return position;

}

void Actor::Update(float deltaTime)
{

	CalcWorldTransform();

	// Updates active actors
	if (mState == ActorState::Active) {
		for (unsigned int x = 0; x < mComponents.size(); x++) {
			mComponents[x]->Update(deltaTime);
		}
		OnUpdate(deltaTime);
	}

	CalcWorldTransform();

	for (auto c : mChildren) {
		c->Update(deltaTime);
	}

}

void Actor::OnUpdate(float deltaTime)
{
}

void Actor::CalcWorldTransform() {

	float arr1[4][4] = { {mScale, 0, 0, 0},
						{0, mScale, 0, 0},
						{0, 0, mScale, 0},
						{0, 0, 0, 1} };

	Matrix4 scaleMatrix = Matrix4(arr1);

	float arr2[4][4] = { {1, 0, 0, 0},
						{0, 1, 0, 0},
						{0, 0, 1, 0},
						{mPosition.x, mPosition.y, mPosition.z, 1} };

	Matrix4 translateMatrix = Matrix4(arr2);

	float arr3[4][4] = { {cos(mRotation), sin(mRotation), 0, 0},
						{-1 * sin(mRotation), cos(mRotation), 0, 0},
						{0, 0, 1, 0},
						{0, 0, 0, 1} };

	Matrix4 rotateMatrix = Matrix4(arr3);

	Matrix4 quatMatrix = Matrix4::CreateFromQuaternion(mQuat);

	mWorldTransform = scaleMatrix * rotateMatrix * quatMatrix * translateMatrix;

	if (mParent != nullptr) {

		if (mInheritScale) {
			mWorldTransform *= mParent->GetWorldTransform();
		}
		else {
			mWorldTransform *= mParent->CalcWorldRotTrans();
		}

	}

}

Matrix4 Actor::CalcWorldRotTrans() {

	float arr1[4][4] = { {1, 0, 0, 0},
						{0, 1, 0, 0},
						{0, 0, 1, 0},
						{0, 0, 0, 1} };

	Matrix4 scaleMatrix = Matrix4(arr1);

	float arr2[4][4] = { {1, 0, 0, 0},
						{0, 1, 0, 0},
						{0, 0, 1, 0},
						{mPosition.x, mPosition.y, mPosition.z, 1} };

	Matrix4 translateMatrix = Matrix4(arr2);

	float arr3[4][4] = { {cos(mRotation), sin(mRotation), 0, 0},
						{-1 * sin(mRotation), cos(mRotation), 0, 0},
						{0, 0, 1, 0},
						{0, 0, 0, 1} };

	Matrix4 rotateMatrix = Matrix4(arr3);

	Matrix4 quatMatrix = Matrix4::CreateFromQuaternion(mQuat);

	Matrix4 worldTrans = scaleMatrix * rotateMatrix * quatMatrix * translateMatrix;

	if (mParent != nullptr) {

		worldTrans *= mParent->CalcWorldRotTrans();

	}

	return worldTrans;

}

void Actor::ProcessInput(const Uint8* keyState)
{
	// Processes input for active actors
	if (mState == ActorState::Active) {
		for (unsigned int x = 0; x < mComponents.size(); x++) {
			mComponents[x]->ProcessInput(keyState);
		}
		OnProcessInput(keyState);
	}

}

void Actor::OnProcessInput(const Uint8* keyState)
{
}

Vector3 Actor::GetWorldPosition() {

	return mWorldTransform.GetTranslation();

}

Vector3 Actor::GetWorldForward() {

	return mWorldTransform.GetXAxis();

}

void Actor::RemoveChild(Actor* a) {

	auto iter = std::find(mChildren.begin(), mChildren.end(), a);
	if (iter != mChildren.end())
	{
		// Swap to end of vector and pop off (avoid erase copies)
		auto iter2 = mChildren.end() - 1;
		std::iter_swap(iter, iter2);
		mChildren.pop_back();
	}

}

void Actor::AddComponent(Component* c)
{
	mComponents.emplace_back(c);
	std::sort(mComponents.begin(), mComponents.end(), [](Component* a, Component* b) {
		return a->GetUpdateOrder() < b->GetUpdateOrder();
	});
}

Vector3 Actor::GetQuaternionForward() {

	Vector3 v = Vector3::Transform(Vector3::UnitX, mQuat);
	v = Vector3::Normalize(v);
	return v;

}