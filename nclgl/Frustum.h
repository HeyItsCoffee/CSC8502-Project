#pragma once

#include "Plane.h"
class SceneNode; // These let the compiler now that these keywords
class Matrix4; // are classes, without including the whole header

class Frustum {
public:
	Frustum(void) {};
	~Frustum(void) {};

	void FromMatrix(const Matrix4& mvp);
	bool InsideFrustum(SceneNode& n);

protected:
	Plane planes[6];
};