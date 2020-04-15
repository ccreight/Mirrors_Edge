#include "Coin.h"
#include "Game.h"
#include "Renderer.h"
#include "Mesh.h"

Coin::Coin(Game* game, Actor* actor) : Actor(game, actor){

	mGame = game;
	mParent = actor;

	mMesh = new MeshComponent(this);
	mMesh->SetMesh(mGame->GetRenderer()->GetMesh("Assets/Coin.gpmesh"));

	mCollision = new CollisionComponent(this);
	mCollision->SetSize(100, 100, 100);

}

void Coin::OnUpdate(float deltaTime) {

	SetRotation(GetRotation() + Math::Pi * deltaTime);

	if (mGame->mPlayer->GetComponent<CollisionComponent>()->Intersect(mCollision)) {

		Mix_Chunk* sound = mGame->GetSound("Assets/Sounds/Coin.wav");
		Mix_PlayChannel(-1, sound, 0);

		GetGame()->mPlayer->GetComponent<HUD>()->CoinPickedUp();

		SetState(ActorState::Destroy);

	}

}