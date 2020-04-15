#pragma once
#include "Actor.h"
#include "MeshComponent.h"
#include "CollisionComponent.h"
#include <string>
#include <iostream>

class Checkpoint : public Actor {

	public:
		Checkpoint(class Game* game, Actor* parent);
		void SetLevel(std::string level) {
			mLevelString = level;
		}

		void SetText(std::string text) {
			mTextString = text;
		}

	private:
		class Game* mGame;
		MeshComponent* mMesh;
		CollisionComponent* mCollision;
		Actor* mParent;
		void OnUpdate(float deltaTime);
		std::string mLevelString;
		std::string mTextString;
};