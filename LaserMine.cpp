#include "LaserMine.h"
#include "Game.h"
#include "Renderer.h"
#include "Mesh.h"

LaserMine::LaserMine(Game* game, Actor* parent) : Actor(game, parent) {

	mGame = game;

	mMesh = new MeshComponent(this);
	mMesh->SetMesh(mGame->GetRenderer()->GetMesh("Assets/LaserMine.gpmesh"));

	mLaser = new LaserComponent(this);

}