#pragma once
#include "Actor.h"
#include "CollisionComponent.h"
#include "PlayerMove.h"
#include "CameraComponent.h"
#include "HUD.h"

class Player : public Actor {

	public:
		Player(class Game* game, Actor* parent);

		Vector3 getRespawnPos() {
			return mRespawnPos;
		}

		void setRespawnPos(Vector3 v) {
			mRespawnPos = v;
		}

	private:
		const float WIDTH = 50.0f, HEIGHT = 175.0f, DEPTH = 50.0f;
		class Game* mGame;
		PlayerMove* mMove;
		CameraComponent* mCamera;
		CollisionComponent* mCollision;
		Vector3 mRespawnPos;
		HUD* mHUD;

};