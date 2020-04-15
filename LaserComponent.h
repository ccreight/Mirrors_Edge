#pragma once
#include "MeshComponent.h"
#include "SegmentCast.h"

class LaserComponent : public MeshComponent {

	public:
		LaserComponent(class Actor* owner);
		void Draw(class Shader* shader);
		void Update(float deltaTime);
		Matrix4 GetMatrix(LineSegment l);

	private:
		class Actor* mOwner;
		std::vector<LineSegment> mLine;

};