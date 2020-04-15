#include "LaserComponent.h"
#include "Actor.h"
#include "MeshComponent.h"
#include "Shader.h"
#include "Mesh.h"
#include "Actor.h"
#include "Game.h"
#include "Renderer.h"
#include "Texture.h"
#include "VertexArray.h"
#include <iostream>
#include "Block.h"

LaserComponent::LaserComponent(Actor* owner) : MeshComponent(owner, true) {

	mOwner = owner;
	SetMesh(mOwner->GetGame()->GetRenderer()->GetMesh("Assets/Laser.gpmesh"));

}

void LaserComponent::Draw(class Shader* shader) {

	if (mMesh)
	{
		for (LineSegment l : mLine) {

			// Set the world transform
			shader->SetMatrixUniform("uWorldTransform",
				GetMatrix(l));
			// Set the active texture
			Texture* t = mMesh->GetTexture(mTextureIndex);
			if (t)
			{
				t->SetActive();
			}
			// Set the mesh's vertex array as active
			VertexArray* va = mMesh->GetVertexArray();
			va->SetActive();
			// Draw
			glDrawElements(GL_TRIANGLES, va->GetNumIndices(), GL_UNSIGNED_INT, nullptr);

		}
	}

}

Matrix4 LaserComponent::GetMatrix(LineSegment l) {

	Matrix4 m;

	float arr1[4][4] = { {l.Length(), 0, 0, 0},
						{0, 1, 0, 0},
						{0, 0, 1, 0},
						{0, 0, 0, 1} };

	Matrix4 scaleMatrix = Matrix4(arr1);

	Vector3 midpoint = ((l.mStart + l.mEnd) * 0.5f);
	Matrix4 translateMatrix = Matrix4::CreateTranslation(midpoint);

	/*float rot = mOwner->GetRotation();

	float arr3[4][4] = { {cos(rot), sin(rot), 0, 0},
						{-1 * sin(rot), cos(rot), 0, 0},
						{0, 0, 1, 0},
						{0, 0, 0, 1} };

	Matrix4 rotateMatrix = Matrix4(arr3);*/

	Vector3 oldV = Vector3::UnitX;
	Vector3 newV = Vector3::Normalize(l.mEnd - l.mStart);
	Vector3 axis = Vector3::Zero;
	float angle = 0.0f;
	Quaternion q;

	if (newV.x != oldV.x || newV.y != oldV.y || newV.z != oldV.z) {
		axis = Vector3::Normalize(Vector3::Cross(oldV, newV));
		angle = acos(Vector3::Dot(oldV, newV));
		
		if (Vector3::Dot(oldV, newV) != -1) {
			q = Quaternion(axis, angle);
		}
		else {
			q = Quaternion(Vector3::UnitZ, Math::Pi);
		}
		
	}
	else {
		q = Quaternion::Identity;
	}

	
	Matrix4 rotateMatrix = Matrix4::CreateFromQuaternion(q);

	m = scaleMatrix * rotateMatrix * translateMatrix;
	return m;

}

void LaserComponent::Update(float deltaTime) {

	if (!mLine.empty()) {
		LineSegment l = mLine[0];
		mLine.clear();
		mLine.push_back(l);
		mLine[0].mStart = mOwner->GetWorldPosition();
		mLine[0].mEnd = mOwner->GetWorldPosition() + 500.0f * mOwner->GetWorldForward();
	}
	else {
		mLine.push_back(LineSegment(mOwner->GetPosition(), mOwner->GetPosition() + 500.0f * mOwner->GetQuaternionForward()));
	}
	

	CastInfo c;
	bool intersects = SegmentCast(mOwner->GetGame()->mPlayer, mLine[0], c);

	if (intersects) {
		Mix_Chunk* sound = mOwner->GetGame()->GetSound("Assets/Sounds/Laser.wav");
		Mix_PlayChannel(-1, sound, 0);
		mLine[0].mEnd = c.mPoint;
		mOwner->GetGame()->mPlayer->GetComponent<PlayerMove>()->Respawn();
	}
	else {

		intersects = SegmentCast(mOwner->GetGame()->mBlocks, mLine[0], c, mOwner->GetParent());

		if (intersects) {

			int index = 0;
			mLine[index].mEnd = c.mPoint;

			while (((Block*)(c.mActor))->IsMirror() && intersects) {

				mLine[index].mEnd = c.mPoint;
				mLine.push_back(LineSegment(c.mPoint, c.mPoint + 1000 * Vector3::Reflect(Vector3::Normalize(mLine[index].mEnd - mLine[index].mStart), c.mNormal)));
				index++;
				intersects = SegmentCast(mOwner->GetGame()->mBlocks, mLine[index], c, c.mActor);

				if (intersects) {
					mLine[index].mEnd = c.mPoint;
				}

			}

		}

	}

}