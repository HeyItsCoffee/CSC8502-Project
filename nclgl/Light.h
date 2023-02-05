#pragma once

#include "Vector3.h"
#include "Vector4.h"

enum LightType {
	Point,
	Directional,
	Spot
};

class Light {
public:
	Light() {} // Default constructor for later
	Light(const Vector3& position, const Vector4& colour, float radius) {
		this->type = Point;
		this->position = position;
		this->colour = colour;
		this->radius = radius;
	}
	Light(const Vector3& position, const Vector4& colour, const Vector3& target) {
		this->type = Directional;
		this->position = position;
		this->target = target;
		this->colour = colour;
	}

	~Light(void) {};

	int GetType() const { 
		return type; }

	Vector3 GetPosition() const { return position; }
	void SetPosition(const Vector3& val) { position = val; }

	float GetRadius() const { return radius; }
	void SetRadius(const float val) { radius = val; }

	Vector4 GetColour() const { return colour; }
	void SetColour(const Vector4& val) { colour = val; }

	float GetAlpha() const { return colour.w; }
	void SetAlpha(const float val) { colour.w = val; }

	Vector3 GetDirection() const {
		if (type == Directional) {
			return position - target;
		}
	else return Vector3(0, 0, 0); }

protected:

	LightType type;
	Vector3 position;
	Vector3 target;
	float radius;
	Vector4 colour;
};