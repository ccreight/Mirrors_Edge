#include "Player.h"
#include "Game.h"
#include "Renderer.h"

Player::Player(class Game* game, Actor* parent) : Actor(game, parent) {

	mGame = game;

	/*mCollision = new CollisionComponent(this);
	mCollision->SetSize(WIDTH, HEIGHT, DEPTH);*/

	mMove = new PlayerMove(this);
	mCamera = new CameraComponent(this);

	mCollision = new CollisionComponent(this);
	mCollision->SetSize(WIDTH, HEIGHT, DEPTH);

	mHUD = new HUD(this);

}