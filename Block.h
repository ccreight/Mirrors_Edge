#pragma once
#include "Actor.h"
#include "MeshComponent.h"
#include "CollisionComponent.h"

class Block : public Actor {

	public:
		Block(class Game* game, Actor* parent);
		~Block();

		void SetIsMirror(bool b) {
			mIsMirror = b;
		}

		bool IsMirror() {
			return mIsMirror;
		}

		bool CanRotate() {
			return mCanRotate;
		}

		void SetCanRotate(bool b) {
			mCanRotate = b;
		}

		void OnUpdate(float deltaTime);

	private:
		class Game* mGame = nullptr;
		const float SCALE = 64.0f;
		MeshComponent* mMesh = nullptr;
		CollisionComponent* mCollision = nullptr;
		bool mIsMirror = false;
		bool mCanRotate = false;

};