#include "Block.h"
#include "Game.h"
#include "Renderer.h"
#include "Mesh.h"

Block::Block(class Game* game, Actor* parent) : Actor(game, parent){

	mGame = game;
	SetScale(SCALE);

	mMesh = new MeshComponent(this);
	mMesh->SetMesh(mGame->GetRenderer()->GetMesh("Assets/Cube.gpmesh"));

	mCollision = new CollisionComponent(this);
	mCollision->SetSize(1, 1, 1);

	mGame->AddBlock(this);

}

Block::~Block() {

	mGame->RemoveBlock(this);

}

void Block::OnUpdate(float deltaTime) {

	if (mCanRotate) {
		SetRotation(GetRotation() + (float)(Math::PiOver2 / 2.0f * deltaTime));
	}

}