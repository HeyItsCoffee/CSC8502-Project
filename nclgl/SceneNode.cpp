#include "SceneNode.h"

SceneNode::SceneNode(Mesh* mesh, Shader* shader, Vector4 colour) {
	this->mesh = mesh;
	this->colour = colour;
	this->shader = shader;
	parent = NULL;
	modelScale = Vector3(1, 1, 1);
	boundingRadius = 1.0f;
	distanceFromCamera = 0.0f;
	texture = 0;
	animated = false;
}

SceneNode::~SceneNode(void) {
	for (unsigned int i = 0; i < children.size(); ++i) delete children[i];
}

void SceneNode::AddChild(SceneNode* s) {
	children.push_back(s);
	s->parent = this;
}

void SceneNode::AddTexture(GLuint texture, std::string texName) {
	textures.push_back(texture);
	texNames.push_back(texName);
}

// Binds all textures needed to draw the SceneNode
void SceneNode::Prepare(GLint program) {
	for (int i = 0; i < textures.size(); i++) {
		glUniform1i(glGetUniformLocation(program, (texNames[i]).c_str()), i);
		glActiveTexture(GL_TEXTURE0+i);
		if(texNames[i]._Equal("cubeTex"))
			glBindTexture(GL_TEXTURE_CUBE_MAP, textures[i]);
		else glBindTexture(GL_TEXTURE_2D, textures[i]);
	}
}

void SceneNode::MakeAnimated(MeshAnimation* anim, MeshMaterial* mat) {
	meshAnim = anim;
	meshMat = mat;
	currentFrame = 0;
	frameTime = 0.0f;
	for (int i = 0; i < mesh->GetSubMeshCount(); ++i) {
		const MeshMaterialEntry* matEntry = meshMat->GetMaterialForLayer(i);

		const string* filename = nullptr;
		matEntry->GetEntry("Diffuse", &filename);
		string path = TEXTUREDIR + *filename;
		GLuint texID = SOIL_load_OGL_texture(path.c_str(), SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y);
		matTextures.emplace_back(texID);
	}
	animated = true;
}

void SceneNode::Draw(const OGLRenderer& r) {
	if (mesh) { 
		if (animated) {
			glUniformMatrix4fv(glGetUniformLocation(shader->GetProgram(), "joints"), frameMatrices.size(), false, (float*)frameMatrices.data());
			glUniform1i(glGetUniformLocation(shader->GetProgram(), "diffuseTex"), 0);

			for (int i = 0; i < mesh->GetSubMeshCount(); ++i) {
				glActiveTexture(GL_TEXTURE0);
				glBindTexture(GL_TEXTURE_2D, matTextures[i]);
				mesh->DrawSubMesh(i);
			}
		}
		else mesh->Draw();
	}
}

void SceneNode::Update(float dt) {
	if (parent) worldTransform = parent->worldTransform * transform; // This node has a parent
	else worldTransform = transform; // Root node, world transform is local transform
	if (animated) {
		frameTime -= dt * 0.05;
		while (frameTime < 0.0f) {
			currentFrame = (currentFrame + 1) % meshAnim->GetFrameCount();
			frameTime += 1.0f / meshAnim->GetFrameRate();
		}
		while (frameTime > 1.0f / meshAnim->GetFrameRate()) {
			currentFrame = (currentFrame - 1) % meshAnim->GetFrameCount();
			frameTime -= 1.0f / meshAnim->GetFrameRate();
		}

		frameMatrices.clear();

		const Matrix4* invBindPose = mesh->GetInverseBindPose();
		const Matrix4* frameData = meshAnim->GetJointData(currentFrame);

		for (unsigned int i = 0; i < mesh->GetJointCount(); ++i)
			frameMatrices.emplace_back(frameData[i] * invBindPose[i]);
	}
	for (vector<SceneNode*>::iterator i = children.begin(); i != children.end(); ++i) (*i)->Update(dt);
}