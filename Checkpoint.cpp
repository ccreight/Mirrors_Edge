#include "Checkpoint.h"
#include "Game.h"
#include "Renderer.h"
#include "Mesh.h"

Checkpoint::Checkpoint(Game* game, Actor* parent) : Actor(game, parent) {

	mGame = game;
	mParent = parent;

	mMesh = new MeshComponent(this);
	mMesh->SetMesh(mGame->GetRenderer()->GetMesh("Assets/Checkpoint.gpmesh"));

	mCollision = new CollisionComponent(this);
	mCollision->SetSize(25, 25, 25);

	mGame->mCheckpoints.push_back(this);

	if (mGame->mCheckpoints.empty()) {
		mMesh->SetTextureIndex(0);
	}
	else {
		mMesh->SetTextureIndex(1);
	}

}

void Checkpoint::OnUpdate(float deltaTime) {

	if ((size_t)mGame->mCurrentCheckpoint < mGame->mCheckpoints.size() && mGame->mCheckpoints[mGame->mCurrentCheckpoint] == this && mGame->mPlayer->GetComponent<CollisionComponent>()->Intersect(mCollision)) {

		Mix_Chunk* sound = mGame->GetSound("Assets/Sounds/Checkpoint.wav");
		Mix_PlayChannel(-1, sound, 0);

		if (!mLevelString.empty()) {
			mGame->SetNextLevel(mLevelString);
		}

		GetGame()->mPlayer->GetComponent<HUD>()->SetCheckText(mTextString);

		this->SetState(ActorState::Destroy);
		mGame->mCurrentCheckpoint++;

		mGame->mPlayer->setRespawnPos(mGame->mCheckpoints[mGame->mCurrentCheckpoint - 1]->GetPosition());

		if ((size_t)mGame->mCurrentCheckpoint < mGame->mCheckpoints.size()) {
			mGame->mCheckpoints[mGame->mCurrentCheckpoint]->GetComponent<MeshComponent>()->SetTextureIndex(0);
		}

	}

}
