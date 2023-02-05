#include "CWRenderer.h"
#include "../nclgl/Light.h"
#include "../nclgl/HeightMap.h"
#include "../nclgl/Shader.h"
#include "../nclgl/Camera.h"

#include <algorithm>

const int BALL_NUM = 100;

Renderer::Renderer(Window& parent) : OGLRenderer(parent) {
	// Quad mesh
	quad = Mesh::GenerateQuad();
	Mesh* quadPatch = Mesh::GenerateQuadIndices();
	// Sphere mesh
	sphere = Mesh::LoadFromMeshFile("Sphere.msh");
	// Enemy mesh
	enemyMesh = Mesh::LoadFromMeshFile("Role_T.msh");

	// Heightmap
	heightMap = new HeightMap(TEXTUREDIR"Rocky Land and Rivers/HMNormalisedBlurredSmallCurve.jpg", Vector3(2.0f, 1.0f, 2.0f));
	Vector3 heightmapSize = heightMap->GetHeightmapSize();
	// Camera
	camera = new Camera(15.0f, 0.0f, heightmapSize * Vector3(0.5f, 0.25f, 1.0f));

	// Sun and Moon
	timeOfDay = 1200.0f;
	sunLight = new Light(GetSolarPosition() * 5000.0f, Vector4(0.957f, 0.914f, 0.609f, 0.9f), heightmapSize * Vector3(0.5f, 0.5f, 0.5f));
	moonLight = new Light(GetSolarPosition() * 5000.0f, Vector4(0.668f, 0.754f, 0.887f, 0.5f), heightmapSize * Vector3(0.5f, 0.5f, 0.5f));

	// Textures
	textures.push_back(SOIL_load_OGL_texture(TEXTUREDIR"water.TGA", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS)); // Water texture [0]
	SetTextureRepeating(textures[0], true);
	textures.push_back(SOIL_load_OGL_texture(TEXTUREDIR"waterbump.png", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS)); // Water bumpmap [1]
	SetTextureRepeating(textures[1], true);
	textures.push_back(SOIL_load_OGL_cubemap(TEXTUREDIR"rusted_west.jpg", TEXTUREDIR"rusted_east.jpg", TEXTUREDIR"rusted_up.jpg",
		TEXTUREDIR"rusted_down.jpg", TEXTUREDIR"rusted_south.jpg", TEXTUREDIR"rusted_north.jpg", SOIL_LOAD_RGB, SOIL_CREATE_NEW_ID, 0)); // Skybox [2]
	textures.push_back(SOIL_load_OGL_texture(TEXTUREDIR"Magic Ball/Marble_Tiles_Diffuse.jpg", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS)); //[3]
	textures.push_back(SOIL_load_OGL_texture(TEXTUREDIR"Magic Ball/Marble_Tiles_Displacement.png", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS)); //[4]
	textures.push_back(SOIL_load_OGL_texture(TEXTUREDIR"Grass_01_0.png", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS)); // Grassy Heightmap texture [5]
	textures.push_back(SOIL_load_OGL_texture(TEXTUREDIR"rock_d01.jpg", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS)); // Cliffy Heightmap texture [6]
	textures.push_back(SOIL_load_OGL_texture(TEXTUREDIR"Rocky Land and Rivers/HMBump.jpg", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS)); // Bumpmap for the Heightmap, needs fixing [7]

	for (int i = 0; i < textures.size(); i++)
		if (!textures[i])
			return;
	SetTextureRepeating(textures[5], true);
	SetTextureRepeating(textures[6], true);
	SetTextureRepeating(textures[7], true);

	// Shaders
	shaders.push_back(new Shader("CWPointlightVertex.glsl", "CWPointlightFragment.glsl")); // Shader for point lights [0]
	shaders.push_back(new Shader("CWDirlightVertex.glsl", "CWDirlightFragment.glsl")); // Shader for direct lights [1]
	shaders.push_back(new Shader("CWCombineVertex.glsl", "CWCombineFragment.glsl")); // Shader for combining G-Buffer and lighting [2]
	shaders.push_back(new Shader("CWBumpVertex.glsl", "CWHeightMapFragment.glsl")); // Shader for heightmap [3]
	shaders.push_back(new Shader("CWSkyboxVertex.glsl", "CWSkyboxFragment.glsl")); // Shader for skybox [4]
	shaders.push_back(new Shader("CWBumpVertex.glsl", "CWBasicObjectFragment.glsl")); // Basic shader for objects that just use one texture and bumpmap [5]
	shaders.push_back(new Shader("CWBumpVertex.glsl", "CWMagicBallFragment.glsl")); // Almost the same but uses skybox style lighting [6]
	shaders.push_back(new Shader("CWReflectVertex.glsl", "CWReflectFragment.glsl")); // Shader for water [7]
	shaders.push_back(new Shader("CWSkinningVertex.glsl", "CWBasicObjectFragment.glsl")); // Shader for animated guy [8]
	shaders.push_back(new Shader("CWParticleVertex.glsl", "CWParticleFragment.glsl", "CWParticleGeom.glsl"));
	shaders.push_back(new Shader("CWParticleVertex.glsl", "CWParticleFragment.glsl", "", "CWParticleControl.glsl", "CWParticleEvaluation.glsl"));
	shaders.push_back(new Shader("CWParticleVertex.glsl", "CWParticleFragment.glsl", "CWParticleGeom.glsl", "CWParticleControl.glsl", "CWParticleEvaluation.glsl"));

	for(int i = 0; i < shaders.size(); i++)
		if (!shaders[i]->LoadSuccess())
			return;

	// SceneNode stuff
	root = new SceneNode();
	// Heightmap
	SceneNode* s = new SceneNode(heightMap, shaders[3]);
	s->SetBoundingRadius(std::max(std::max(heightmapSize.x, heightmapSize.y), heightmapSize.z) * 1.5f);
	s->AddTexture(textures[5], "grassTex");
	s->AddTexture(textures[6], "cliffTex");
	s->AddTexture(textures[7], "bumpTex");
	root->AddChild(s);
	// Magic Ball
	pointLights = new Light[1];
	s = new SceneNode(sphere, shaders[6]);
	s->AddTexture(textures[3], "diffuseTex");
	s->AddTexture(textures[4], "bumpTex");
	s->SetTransform(Matrix4::Translation(Vector3(heightmapSize.x * 0.5, 400, heightmapSize.z * 0.5)));
	pointLights[0] = Light(Vector3(heightmapSize.x * 0.5, 400, heightmapSize.z * 0.5),
		Vector4(0.5f + (float)(rand() / (float)RAND_MAX), 0.5f + (float)(rand() / (float)RAND_MAX), 0.5f + (float)(rand() / (float)RAND_MAX), 1.0f),
		750.0f);
	s->SetModelScale(Vector3(50, 50, 50));
	s->SetBoundingRadius(50);
	root->AddChild(s);
	// Flying balls
	SceneNode* ballBase = new SceneNode();
	ballBase->SetTransform(Matrix4::Translation(Vector3(heightmapSize.x * 0.5, 300, heightmapSize.z * 0.5)));
	root->AddChild(ballBase);
	for (int i = 0; i < BALL_NUM; i++) {
		int offsetLimit = 10;
		float xRandom = (float(rand()) / float((RAND_MAX)) * offsetLimit * 2) - offsetLimit;
		float zRandom = (float(rand()) / float((RAND_MAX)) * offsetLimit * 2) - offsetLimit;
		s = new SceneNode(sphere, shaders[5]);
		s->AddTexture(textures[3], "diffuseTex");
		s->AddTexture(textures[4], "bumpTex");
		s->SetTransform(Matrix4::Translation(Vector3(i * xRandom, 0, i * zRandom)));
		s->SetModelScale(Vector3(10, 10, 10));
		s->SetBoundingRadius(10);
		ballBase->AddChild(s);
	}
	// Fog [Not functional so not added in, but tried to use geometry and tesselation shaders for this]
	s = new SceneNode(quadPatch, shaders[5]);
	s->AddTexture(textures[3], "diffuseTex");
	s->AddTexture(textures[4], "bumpTex");
	s->SetTransform(Matrix4::Translation(Vector3(heightmapSize.x * 0.5f, 100.0f, heightmapSize.z * 0.5f)) * Matrix4::Rotation(-90, Vector3(1.0f, 0.0f, 0.0f)));
	s->SetModelScale(Vector3(heightmapSize.x*0.5f, heightmapSize.z*0.5f, 1.0f));
	s->SetBoundingRadius(std::max(heightmapSize.x, heightmapSize.z));
	//root->AddChild(s);
	// Enemies
	enemyAni = new MeshAnimation("Role_T.anm");
	enemyMat = new MeshMaterial("Role_T.mat");
	for (int i = 0; i < 5; i++) {
		enemyList.push_back(new SceneNode(enemyMesh, shaders[8]));
		enemyList[i]->MakeAnimated(enemyAni, enemyMat);
		enemyList[i]->SetModelScale(Vector3(25, 25, 25));
		enemyList[i]->SetBoundingRadius(50);
		root->AddChild(enemyList[i]);
	}
	// Water
	s = new SceneNode(quad, shaders[7]);
	s->AddTexture(textures[0], "diffuseTex");
	s->AddTexture(textures[1], "bumpTex");
	s->AddTexture(textures[2], "cubeTex");
	s->SetTransform(Matrix4::Translation(Vector3(heightmapSize.x * 0.5f, 25.1f, heightmapSize.z * 0.5f)) *
		Matrix4::Rotation(-90, Vector3(1.0f, 0.0f, 0.0f)));
	s->SetModelScale(Vector3(heightmapSize.x * 0.5f, heightmapSize.z * 0.5f, 1.0f));
	s->SetBoundingRadius(std::max(std::max(heightmapSize.x, heightmapSize.y), heightmapSize.z) * 1.5f);
	root->AddChild(s);
	


	// FBOs
	{
		glGenFramebuffers(1, &bufferFBO); // Make FBOs
		glGenFramebuffers(1, &lightFBO);

		GLenum buffers[2] = { // Each FBO has two colour attachments, in first to keep texture samples and normals, and second to keep diffuse and specular
			GL_COLOR_ATTACHMENT0,
			GL_COLOR_ATTACHMENT1
		};

		// Generate textures
		GenerateScreenTexture(bufferDepthTex, true);
		// Generate textures for FBOs to write/read to
		GenerateScreenTexture(bufferColourTex);
		GenerateScreenTexture(bufferNormalTex);
		GenerateScreenTexture(lightDiffuseTex);
		GenerateScreenTexture(lightSpecularTex);

		// Bind textures to FBOs
	
		// First render
		glBindFramebuffer(GL_FRAMEBUFFER, bufferFBO);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, bufferColourTex, 0);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, bufferNormalTex, 0);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, bufferDepthTex, 0);
		glDrawBuffers(2, buffers);

		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
			return;

		// Second render
		glBindFramebuffer(GL_FRAMEBUFFER, lightFBO);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, lightDiffuseTex, 0);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, lightSpecularTex, 0);
		glDrawBuffers(2, buffers);

		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
			return;

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	// Others

	projMatrix = Matrix4::Perspective(1.0f, 15000.0f, (float)width / (float)height, 45.0f);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glEnable(GL_CULL_FACE);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
	waterRotate = 0.0f;
	waterCycle = 0.0f;
	init = true;
}

Renderer::~Renderer(void) {
	delete quad;
	delete sphere;

	delete enemyMesh;
	delete enemyAni;
	delete enemyMat;
	
	delete heightMap;
	delete camera;
	
	delete sunLight;
	delete moonLight;

	delete[] pointLights;

	for each (auto shader in shaders)
	{
		delete shader;
	}

	delete root;

	glDeleteTextures(1, &bufferColourTex);
	glDeleteTextures(1, &bufferNormalTex);
	glDeleteTextures(1, &bufferDepthTex);
	glDeleteTextures(1, &lightDiffuseTex);
	glDeleteTextures(1, &lightSpecularTex);

	glDeleteFramebuffers(1, &bufferFBO);
	glDeleteFramebuffers(1, &lightFBO);
}

void Renderer::GenerateScreenTexture(GLuint& into, bool depth) {
	glGenTextures(1, &into);
	glBindTexture(GL_TEXTURE_2D, into);

	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	GLuint format = depth ? GL_DEPTH_COMPONENT24 : GL_RGBA8;
	GLuint type = depth ? GL_DEPTH_COMPONENT : GL_RGBA;

	glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, type, GL_UNSIGNED_BYTE, NULL);

	glBindTexture(GL_TEXTURE_2D, 0);
}

float moduloFloat(float val, int by) {
	while (val > by)
		val -= by;
	return val;
}

void Renderer::UpdateScene(float dt, bool pause, bool lockCam) {
	//firstShaderCall = true;
	if(!lockCam) camera->UpdateCamera(dt);
	if (pause) dt = 0;
	else {
		dt *= 30; // 30 = 120s/day
		timeOfDay = moduloFloat(timeOfDay + dt, 3600);
		UpdateSunAndMoon();
		Vector3 heightmapSize = heightMap->GetHeightmapSize();
		UpdateEnemies(heightmapSize);
		if (lockCam) camera->LockUpdateCamera(timeOfDay, heightmapSize);
	}
	viewMatrix = camera->BuildViewMatrix();
	waterRotate += dt * (2.0f * 0.05); // 2 degrees per second
	waterCycle += dt * (0.25f * 0.05); // 10 units per second

	frameFrustum.FromMatrix(projMatrix * viewMatrix);

	root->Update(dt);
}

void Renderer::UpdateSunAndMoon() {
	sunLight->SetPosition(GetSolarPosition() * 5000.0f);
	moonLight->SetPosition(GetLunarPosition() * 5000.0f);
	Vector3 sunDir = sunLight->GetDirection();
	sunDir.Normalise();
	sunLight->SetAlpha(std::max((sunDir.y + 0.15f) * 0.9f, 0.0f));
	Vector3 moonDir = moonLight->GetDirection();
	moonDir.Normalise();
	moonLight->SetAlpha(std::max((moonDir.y + 0.15f) * 0.5f, 0.0f));
//#define DayNightDEBUG
#ifdef DayNightDEBUG
	std::cout << timeOfDay << ": SOLAR DIRECTION {" << sunDir.x << ", " << sunDir.y << ", " << sunDir.z << "}" << std::endl;
	std::cout << timeOfDay << ": SOLAR COLOUR {" << sunLight->GetColour().x << ", " << sunLight->GetColour().y << ", " << sunLight->GetColour().z << ", " << sunLight->GetColour().w << "}" << std::endl;
	std::cout << timeOfDay << ": LUNAR DIRECTION {" << moonDir.x << ", " << moonDir.y << ", " << moonDir.z << "}" << std::endl;
	std::cout << timeOfDay << ": LUNAR COLOUR {" << moonLight->GetColour().x << ", " << moonLight->GetColour().y << ", " << moonLight->GetColour().z << ", " << moonLight->GetColour().w << "}" << std::endl;
#endif
}

void Renderer::UpdateEnemies(Vector3 heightmapSize) {
	float angle = sin(timeOfDay / 4 * PI / 90) * 180;
	for (int i = 0; i < enemyList.size(); i++) {
		float x = heightmapSize.x * (0.3f + 0.1f * i) - (angle - 90) * 2;
		float z = heightmapSize.z * (0.3f + 0.1f * i) - ((180 - angle) - 90) * 2;
		float y = heightMap->GetHeightAtLocation(x, z) + 2.5f;
		Matrix4 rotation = Matrix4::Rotation(angle, Vector3(0, 1, 0));
		enemyList[i]->SetTransform(Matrix4::Translation(Vector3(x, y, z)) * rotation);
	}
}

void Renderer::BuildNodeLists(SceneNode* from) {
	if (frameFrustum.InsideFrustum(*from)) {
		Vector3 dir = from->GetWorldTransform().GetPositionVector() - camera->GetPosition();

		from->SetCameraDistance(Vector3::Dot(dir, dir));

		if (from->GetColour().w < 1.0f)
			transparentNodeList.push_back(from);
		else nodeList.push_back(from);
	}
	else {
		int i = 1;
	}

	for (vector<SceneNode*>::const_iterator i = from->GetChildIteratorStart(); i != from->GetChildIteratorEnd(); ++i)
		BuildNodeLists((*i));
}

Vector3 Renderer::GetSolarPosition() {
	float solarTime = (timeOfDay - 1800);
	const float declination = 25.0f;
	const float latitude = 50.0f;
	const float partA = sin(declination) * sin(latitude);
	const float partB = cos(declination) * cos(latitude);
	float solarHeight = partA + (partB * cos(PI*solarTime / 1800)) + 0.2f;
	float xRatio = sin((PI*solarTime)/1800);
	Vector3 solarVec = Vector3((xRatio), solarHeight, (1 - abs(xRatio)));
#ifdef DayNightDEBUG
	std::cout << timeOfDay << ": SOLAR POSITION {" << solarVec.x << ", " << solarVec.y << ", " << solarVec.z << "}" << std::endl;
#endif
	return solarVec;
}

Vector3 Renderer::GetLunarPosition() {
	timeOfDay = moduloFloat(timeOfDay, 3600);
	const float declination = 25.0f;
	const float latitude = 50.0f;
	const float partA = sin(declination) * sin(latitude);
	const float partB = cos(declination) * cos(latitude);
	float lunarHeight = partA + (partB * cos(PI * timeOfDay / 1800)) - 0.25f;
	float xRatio = -sin((PI * timeOfDay) / 1800);
	Vector3 lunarVec = Vector3((xRatio), lunarHeight, (1 - abs(xRatio)));
#ifdef DayNightDEBUG
	std::cout << timeOfDay << ": LUNAR POSITION {" << lunarVec.x << ", " << lunarVec.y << ", " << lunarVec.z << "}" << std::endl;
#endif
	return lunarVec;
}

void Renderer::RenderScene() {
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

	BuildNodeLists(root);
	SortNodeLists();

	FillBuffers();
	DrawLights();
	CombineBuffers();

	ClearNodeLists();
}

void Renderer::FillBuffers() {
	glBindFramebuffer(GL_FRAMEBUFFER, bufferFBO);
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT); // Need to now clear the FBO

	DrawSkybox(); // Done seperately to turn off depth buffer for the skybox
	DrawNodes();

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Renderer::SortNodeLists() {
	std::sort(transparentNodeList.rbegin(), // note the r
		transparentNodeList.rend(), // note the r
		SceneNode::CompareByCameraDistance);

	std::sort(nodeList.begin(),
		nodeList.end(),
		SceneNode::CompareByCameraDistance);
}

void Renderer::DrawNodes() {
	for (const auto& i : nodeList)
		DrawNode(i);

	for (const auto& i : transparentNodeList)
		DrawNode(i);
}

void Renderer::ClearNodeLists() {
	transparentNodeList.clear();
	nodeList.clear();
}

void Renderer::DrawNode(SceneNode* n) {
	if (n->GetMesh()) {
		Shader* nodeShader = n->GetShader();
		BindShader(nodeShader);
		GLint program = nodeShader->GetProgram();
		n->Prepare(program);
		if (program == shaders[7]->GetProgram()) {
			glUniform3fv(glGetUniformLocation(program, "cameraPos"), 1, (float*)&camera->GetPosition());
			textureMatrix = Matrix4::Translation(Vector3(waterCycle, 0.0f, waterCycle))
				* Matrix4::Scale(Vector3(10.0f, 10.0f, 10.0f))
				* Matrix4::Rotation(waterRotate, Vector3(0.0f, 0.0f, 1.0f));
		}
		modelMatrix = n->GetWorldTransform() * Matrix4::Scale(n->GetModelScale());
		UpdateShaderMatrices();

		n->Draw(*this);
	}
}

void Renderer::DrawLights() {
	glBindFramebuffer(GL_FRAMEBUFFER, lightFBO);

	// Pointlights

	BindShader(shaders[0]);
	GLint program = shaders[0]->GetProgram();

	glClearColor(0, 0, 0, 1);
	glClear(GL_COLOR_BUFFER_BIT);
	glBlendFunc(GL_ONE, GL_ONE);
	glCullFace(GL_FRONT);
	glDepthFunc(GL_ALWAYS);
	glDepthMask(GL_FALSE);

	glUniform1i(glGetUniformLocation(program, "depthTex"), 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, bufferDepthTex);

	glUniform1i(glGetUniformLocation(program, "normTex"), 1);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, bufferNormalTex);

	glUniform3fv(glGetUniformLocation(program, "cameraPos"), 1, (float*)&camera->GetPosition());
	glUniform2f(glGetUniformLocation(program, "pixelSize"), 1.0f / width, 1.0f / height);

	Matrix4 invViewProj = (projMatrix * viewMatrix).Inverse();
	glUniformMatrix4fv(glGetUniformLocation(program, "inverseProjView"), 1, false, invViewProj.values);

	UpdateShaderMatrices();
	for (int i = 0; i < 1; ++i) {
		Light& l = pointLights[i];
		SetShaderLight(l);
		sphere->Draw();
	}

	// Directional lights

	BindShader(shaders[1]);
	program = shaders[1]->GetProgram();

	glUniform1i(glGetUniformLocation(program, "depthTex"), 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, bufferDepthTex);

	glUniform1i(glGetUniformLocation(program, "normTex"), 1);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, bufferNormalTex);

	glUniform3fv(glGetUniformLocation(program, "cameraPos"), 1, (float*)&camera->GetPosition());
	glUniform2f(glGetUniformLocation(program, "pixelSize"), 1.0f / width, 1.0f / height);

	invViewProj = (projMatrix * viewMatrix).Inverse();
	glUniformMatrix4fv(glGetUniformLocation(program, "inverseProjView"), 1, false, invViewProj.values);

	glDisable(GL_CULL_FACE);

	UpdateShaderMatrices();
	SetShaderLight(*sunLight);
	quad->Draw();
	SetShaderLight(*moonLight);
	quad->Draw();

	glEnable(GL_CULL_FACE);

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glCullFace(GL_BACK);
	glDepthFunc(GL_LEQUAL);
	glDepthMask(GL_TRUE);

	glClearColor(0.2f, 0.2f, 0.2f, 1);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Renderer::CombineBuffers() {
	BindShader(shaders[2]);
	GLint program = shaders[2]->GetProgram();
	modelMatrix.ToIdentity();
	viewMatrix.ToIdentity();
	Matrix4 temp = projMatrix;
	projMatrix.ToIdentity();
	UpdateShaderMatrices();

	glUniform1i(glGetUniformLocation(program, "diffuseTex"), 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, bufferColourTex);

	glUniform1i(glGetUniformLocation(program, "diffuseLight"), 1);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, lightDiffuseTex);

	glUniform1i(glGetUniformLocation(program, "specularLight"), 2);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, lightSpecularTex);

	glUniform2f(glGetUniformLocation(program, "lightStrength"), sunLight->GetAlpha(), moonLight->GetAlpha());

	quad->Draw();

	projMatrix = temp;
}

void Renderer::DrawSkybox() {
	glDepthMask(GL_FALSE); // Temporarily disable writing to depth buffer, oterwise heightmap and water would be discarded

	BindShader(shaders[4]);
	UpdateShaderMatrices();

	quad->Draw();

	glDepthMask(GL_TRUE);
}