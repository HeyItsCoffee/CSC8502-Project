#pragma once
#include <string>
#include "Mesh.h"

class HeightMap : public Mesh {
public:
	HeightMap(const std::string& name, Vector3 scale = Vector3(16.0f, 1.0f, 16.0f));
	~HeightMap(void) {};

	Vector3 GetHeightmapSize() const { return heightmapSize; }

	float GetHeightAtLocation(int x, int z);

protected:
	Vector3 heightmapSize;
	int iWidth;
};