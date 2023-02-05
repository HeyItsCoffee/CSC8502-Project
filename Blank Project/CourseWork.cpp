#include "../NCLGL/window.h"
#include "CWRenderer.h"

int main()	{
	Window w("Not ELDEN RING!", 1280, 720, false);

	if(!w.HasInitialised()) {
		return -1;
	}
	
	Renderer renderer(w);
	if(!renderer.HasInitialised()) {
		return -1;
	}

	bool pause = false;
	bool forward = true;
	bool lockCam = true;
	float speedMult = 30.0f;

	w.LockMouseToWindow(true);
	w.ShowOSPointer(false);

	while(w.UpdateWindow()  && !Window::GetKeyboard()->KeyDown(KEYBOARD_ESCAPE)){
		renderer.UpdateScene(forward? w.GetTimer()->GetTimeDeltaSeconds() : -w.GetTimer()->GetTimeDeltaSeconds(), pause, lockCam);
		renderer.RenderScene();
		renderer.SwapBuffers();
		if (Window::GetKeyboard()->KeyDown(KEYBOARD_F5)) {
			Shader::ReloadAllShaders();
		}

		if (Window::GetKeyboard()->KeyDown(KEYBOARD_P)) {
			pause = true;
		}
		if (Window::GetKeyboard()->KeyDown(KEYBOARD_O)) {
			pause = false;
		}
		if (Window::GetKeyboard()->KeyDown(KEYBOARD_K)) {
			forward = false;
		}
		if (Window::GetKeyboard()->KeyDown(KEYBOARD_L)) {
			forward = true;
		}
		if (Window::GetKeyboard()->KeyDown(KEYBOARD_Q)) {
			lockCam = false;
		}
	}
	return 0;
}