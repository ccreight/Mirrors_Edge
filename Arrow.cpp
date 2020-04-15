#include "Arrow.h"
#include "Game.h"
#include "Mesh.h"
#include "Renderer.h"

Arrow::Arrow(Game* game, Actor* parent) : Actor(game, parent){

	mGame = game;
	mParent = parent;

	mMesh = new MeshComponent(this);
	mMesh->SetMesh(mGame->GetRenderer()->GetMesh("Assets/Arrow.gpmesh"));
	
	SetScale(.15f);

}

void Arrow::OnUpdate(float deltaTime) {

	if (mGame->mCurrentCheckpoint >= mGame->mCheckpoints.size() || mGame->mCheckpoints[mGame->mCurrentCheckpoint] == nullptr) {
		mQuat = Quaternion::Identity;
	}
	else {
		Vector3 dist = mGame->mCheckpoints[mGame->mCurrentCheckpoint]->GetPosition() - mGame->mPlayer->GetPosition();
		dist = Vector3::Normalize(dist);

		float angle = acos(Vector3::Dot(Vector3(1, 0, 0), dist));
		Vector3 axis = Vector3::Normalize(Vector3::Cross(Vector3(1, 0, 0), dist));


		if (Vector3::Dot(Vector3(1, 0, 0), dist) == -1 || Vector3::Dot(Vector3(1, 0, 0), dist) == 1) {
			mQuat = Quaternion::Identity;
		}
		else {
			mQuat = Quaternion(axis, angle);
		}
	}

	Vector3 pos = mGame->GetRenderer()->Unproject(Vector3(0.0f, 250.0f, 0.1f));
	SetPosition(pos);

}