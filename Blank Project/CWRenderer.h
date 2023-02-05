#pragma once

#include "../nclgl/OGLRenderer.h"
#include "../nclgl/SceneNode.h"
#include "../nclgl/Frustum.h"

class Camera;
class Shader;
class HeightMap;
class SceneNode;

class Renderer : public OGLRenderer {
public:
	Renderer(Window& parent);
	~Renderer(void);
	void RenderScene() override;
	void UpdateScene(float dt, bool pause, bool LockCam);

protected:
	void DrawSkybox();	// Skybox I think needs to be seperate from deffered rendering?

	void FillBuffers(); // G-Buffer Fill Render Pass
	void DrawLights(); // Lighting Render Pass
	void CombineBuffers(); // Combination Render Pass

	void GenerateScreenTexture(GLuint& into, bool depth = false); // Make new textures

	void DrawWater();

	void UpdateSunAndMoon();
	void UpdateEnemies(Vector3 heightmapSize);

	Vector3 GetSolarPosition();
	Vector3 GetLunarPosition();

	vector<Shader*> shaders;

	//Shader* skyboxShader;
	//Shader* sceneShader; // Shader to fill our GBuffers
	//Shader* pointlightShader; // Shader to calculate lighting for pointlights
	//Shader* dirlightShader; // Shader to calculate lighting for directional lights
	//Shader* combineShader; // Shader to stick it all together
	//Shader* lightShader;
	//Shader* reflectShader;

	GLuint bufferFBO; // FBO for the G-Buffer pass
	GLuint bufferColourTex; // Albedo goes here
	GLuint bufferNormalTex; // Normals go here
	GLuint bufferDepthTex; // Depth goes here

	GLuint lightFBO; // GBO for the lighting pass
	GLuint lightDiffuseTex; // Store the diffuse lighting here
	GLuint lightSpecularTex; // Store the specular lighting here

	HeightMap* heightMap; // Base terrain

	Mesh* quad; // Quad for skybox drawing, and final draw
	Mesh* quadPatch;
	Mesh* sphere; // Volume of light to draw

	Mesh* enemyMesh; // Enemy
	MeshAnimation* enemyAni;
	MeshMaterial* enemyMat;

	float timeOfDay; // Time of day, ranges from 0-3600
	Light* sunLight; // Light for the sun
	Light* moonLight; // Light for the moon
	Light* pointLights; // Array of lights
	Camera* camera;

	vector<GLuint> textures;
	GLuint earthGrass;
	GLuint earthCliff;
	GLuint earthBump;

	float waterRotate;
	float waterCycle;

	void BuildNodeLists(SceneNode* from);
	void SortNodeLists();
	void ClearNodeLists();
	void DrawNodes();
	void DrawNode(SceneNode* n);

	SceneNode* root;
	vector<SceneNode*> enemyList;

	Frustum frameFrustum;

	vector<SceneNode*> transparentNodeList;
	vector<SceneNode*> nodeList;
};