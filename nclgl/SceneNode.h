#pragma once
#include "Matrix4.h"
#include "Vector3.h"
#include "Vector4.h"
#include "Mesh.h"
#include <vector>
#include <string>
#include "MeshAnimation.h"
#include "MeshMaterial.h"

class SceneNode {
public:
	SceneNode(Mesh* m = NULL, Shader* shader = 0, Vector4 colour = Vector4(1, 1, 1, 1));
	~SceneNode(void);

	void SetTransform(const Matrix4& matrix) { transform = matrix; }
	const Matrix4& GetTransform() const { return transform; }
	Matrix4 GetWorldTransform() const { return worldTransform; }

	Vector4 GetColour() const { return colour; }
	void SetColour(Vector4 c) { colour = c; }

	Vector3 GetModelScale() const { return modelScale; }
	void SetModelScale(Vector3 s) { modelScale = s; }

	Mesh* GetMesh() const { return mesh; }
	void SetMesh(Mesh* m) { mesh = m; }

	float GetBoundingRadius() const { return boundingRadius; }
	void SetBoundingRadius(float f) { boundingRadius = f; }

	float GetCameraDistance() const { return distanceFromCamera; }
	void SetCameraDistance(float f) { distanceFromCamera = f; }

	GLuint GetTexture() const { return texture; }
	void SetTexture(GLuint tex) { texture = tex; }

	Shader* GetShader() const { return shader; }
	void SetShader(Shader* shade) { shader = shade;	}

	static bool CompareByCameraDistance(SceneNode* a, SceneNode* b) { return (a->distanceFromCamera < b->distanceFromCamera) ? true : false; }

	void AddChild(SceneNode* s);
	void AddTexture(GLuint texture, std::string texName);
	void MakeAnimated(MeshAnimation* anim, MeshMaterial* mat);

	virtual void Update(float dt);
	virtual void Prepare(GLint program);
	virtual void Draw(const OGLRenderer& r);

	std::vector<SceneNode*>::const_iterator GetChildIteratorStart() { return children.begin(); }
	std::vector<SceneNode*>::const_iterator GetChildIteratorEnd() { return children.end(); }

protected:
	SceneNode* parent;
	std::vector<SceneNode*> children;

	bool animated;
	int currentFrame;
	float frameTime;

	Mesh* mesh;
	MeshAnimation* meshAnim;
	MeshMaterial* meshMat;
	vector<GLuint> matTextures;
	vector<Matrix4> frameMatrices;

	Matrix4 worldTransform;
	Matrix4 transform;

	Vector3 modelScale;

	Vector4 colour;

	float distanceFromCamera;

	float boundingRadius;

	std::vector<GLuint> textures;
	std::vector<std::string> texNames;
	GLuint texture;

	Shader* shader;
};