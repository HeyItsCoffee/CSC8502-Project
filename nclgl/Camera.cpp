#include "Camera.h"
#include "Window.h"
#include <algorithm>
void Camera::UpdateCamera(float dt) {
	pitch -= (Window::GetMouse()->GetRelativePosition().y);
	yaw -= (Window::GetMouse()->GetRelativePosition().x);

	pitch = std::min(pitch, 90.0f);
	pitch = std::max(pitch, -90.0f);

	fixYaw();

	Matrix4 rotation = Matrix4::Rotation(yaw, Vector3(0, 1, 0));

	Vector3 forward = rotation * Vector3(0, 0, -1);
	Vector3 right = rotation * Vector3(1, 0, 0);

	float speed = 30.0f * dt;

#ifdef DEBUG
	std::cout << "PITCH: " << pitch << " YAW: " << yaw << std::endl;
#endif

	if (Window::GetKeyboard()->KeyDown(KEYBOARD_W)) position += forward * speed;
	if (Window::GetKeyboard()->KeyDown(KEYBOARD_A)) position -= right * speed;
	if (Window::GetKeyboard()->KeyDown(KEYBOARD_S)) position -= forward * speed;
	if (Window::GetKeyboard()->KeyDown(KEYBOARD_D)) position += right * speed;

	if (Window::GetKeyboard()->KeyDown(KEYBOARD_SHIFT)) position.y += speed;
	if (Window::GetKeyboard()->KeyDown(KEYBOARD_SPACE)) position.y -= speed;
}

void Camera::LockUpdateCamera(float timeOfDay, Vector3 worldDimensions) {
	float progress = timeOfDay / 3600 * 4;				// 0 --> 1 --> 2 --> 3 --> 4
	float altProgress;									// 0 --> 1 --> 0 --> -1 --> 0
	if (progress <= 1) altProgress = progress;
	else if (progress <= 3) altProgress = 1 - (progress - 1);
	else altProgress = progress - 4;

	pitch = -15 - (15 * altProgress);					// -15 --> -30 --> -15 --> 0 --> -15
	yaw = -90 + progress * 90;
	fixYaw();
														//	270 --> 0 --> 90 --> 180 --> 270
	float xPoint = ((progress <= 1 || progress >= 3 ? -cos(PI * altProgress / 2) : cos(PI * altProgress / 2)) + 1) / 2;
														// 0 --> 0.5 --> 1 --> 0.5 --> 0
	float yPoint = 0.875 + (0.25 * altProgress);			// 0.5 --> 0.75 --> 0.5 --> 0.25 --> 0.5
	float zPoint = (sin(PI * altProgress / 2) + 1) / 2;	// 0.5 --> 1 --> 0.5 --> 0 --> 0.5
	position.x = worldDimensions.x * xPoint;
	position.y = worldDimensions.y * yPoint;
	position.z = worldDimensions.z * zPoint;
}

void Camera::fixYaw() {
	if (yaw < 0) yaw += 360.0f;
	if (yaw > 360.0f) yaw -= 360.0f;
}

Matrix4 Camera::BuildViewMatrix() {
	return Matrix4::Rotation(-pitch, Vector3(1, 0, 0)) *
		Matrix4::Rotation(-yaw, Vector3(0, 1, 0)) *
		Matrix4::Translation(-position);
}