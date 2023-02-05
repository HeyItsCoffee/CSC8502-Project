#include "HeightMap.h"
#include <iostream>
using namespace std::chrono;

HeightMap::HeightMap(const std::string& name, Vector3 scale) {
	int iHeight, iChans;
	unsigned char* data = SOIL_load_image(name.c_str(), &iWidth, &iHeight, &iChans, 1);

	if (!data) {
		std::cout << "Heightmap can't load file!\n";
		return;
	}

	// Will interpolate between heightmap values
	// For heightmap vertices	a c		will use vertices	a b c                Where the y of b, d, f, h, and e are average of (a and c), (a and g), (c and i), (g and i), (b, d, f and h) respectively
	//							g i							d e f
	//														g h i


	/*		A C E				A b C d E
	*		K M O				f g h i j
	*		U W U				K l M n O
	*							p q r s t
	*							U v W x Y
	*/			
	iWidth = iWidth * 2 - 1;
	iHeight = iHeight * 2 - 1;

	numVertices = iWidth * iHeight;
	numIndices = (iWidth - 1) * (iHeight - 1) * 6;
	vertices = new Vector3[numVertices];
	colours = new Vector4[numVertices];
	textureCoords = new Vector2[numVertices];
	indices = new GLuint[numIndices];

	Vector3 vertexScale = scale;
	Vector2 textureScale = Vector2(1.0f / (scale.x * 2.0f * acos(0.0)), 1.0f / (scale.z * 2.0f * acos(0.0)));
	float heights[256*4] = {};
	/*for (int z = 0; z < iHeight; z += 1) {
		int test = 0;
		for (int x = 0; x < iWidth; x += 1) {
			int offset = (z * iWidth) + x;
			vertices[offset] = Vector3(0, 0, 0);
			vertices[offset] = Vector3(x, data[offset], z);
			vertices[offset] = vertices[offset] * vertexScale;
			heights[data[offset]]++;
			textureCoords[offset] = Vector2(x, z) * textureScale;
		}
	}*/
	
	int cycles = 0;

	uint64_t time, now, last;

	time = duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();

	// Base Values
	for (int z = 0; z < iHeight; z += 2) 
		for (int x = 0; x < iWidth; x += 2) {
			cycles++;
			int offset = (z * iWidth) + x;
			int dataOffset = ((z / 2) * ((iWidth+1)/2)) + (x / 2);
			vertices[offset] = Vector3(0.0f, 0.0f, 0.0f);
			float val = data[dataOffset];
			vertices[offset] = Vector3(x, val, z);
			vertices[offset] = vertices[offset] * vertexScale;
			heights[(int)(val*4.0f)]++;
			textureCoords[offset] = Vector2(x, z) * textureScale;
		}

#ifdef DEBUG
	now = duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
	std::cout << "BASE CYCLES: " << cycles << " TIME SINCE STARTED: " << now - time << std::endl;
	last = duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
#endif
	
	// Interpolate values for b and h
	for(int z = 0; z < iHeight; z += 2)
		for (int x = 1; x < iWidth - 1; x += 2) {
			cycles++;
			int offset = (z * iWidth) + x;
			float left = vertices[offset - 1].y;
			float right = vertices[offset + 1].y;
			float newVal = (left + right) / 2.0f;
			vertices[offset] = Vector3(x * vertexScale.x, newVal, z * vertexScale.z);
			heights[(int)(newVal*4.0f)]++;
			textureCoords[offset] = Vector2(x, z) * textureScale;
		}

#ifdef DEBUG
	now = duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
	std::cout << "AFTER ADDING X: " << cycles << " TIME SINCE STARTED: " << now - time << " TIME SINCE LAST UPDATE: " << now - last << std::endl;
	last = duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
#endif

	// Interpolate values for d and f
	for (int z = 1; z < iHeight - 1; z += 2)
		for (int x = 0; x < iWidth; x += 2) {
			cycles++;
			int offset = (z * iWidth) + x;
			float up = vertices[offset - iWidth].y;
			float down = vertices[offset + iWidth].y;
			float newVal = (up + down) / 2.0f;
			vertices[offset] = Vector3(x * vertexScale.x, newVal, z * vertexScale.z);
			heights[(int)(newVal * 4.0f)]++;
			textureCoords[offset] = Vector2(x, z) * textureScale;
		}

#ifdef DEBUG
	now = std::time(nullptr);
	std::cout << "AFTER ADDING Z: " << cycles << " TIME SINCE STARTED: " << now - time << " TIME SINCE LAST UPDATE: " << now - last << std::endl;
	last = std::time(nullptr);
#endif

	// Interpolate values for e
	for (int z = 1; z < iHeight - 1; z += 2)
		for (int x = 1; x < iWidth - 1; x += 2) {
			cycles++;
			int offset = (z * iWidth) + x;
			float left = vertices[offset - 1].y;
			float right = vertices[offset + 1].y;
			float up = vertices[offset - iWidth].y;
			float down = vertices[offset + iWidth].y;
			float newVal = (vertices[offset - iWidth].y + vertices[offset - 1].y + vertices[offset + 1].y + vertices[offset + iWidth].y) / 4.0f;
			vertices[offset] = Vector3(x * vertexScale.x, newVal, z * vertexScale.z);
			heights[(int)(newVal * 4.0f)]++;
			textureCoords[offset] = Vector2(x, z) * textureScale;
		}
	
#ifdef DEBUG
	now = duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
	std::cout << "AFTER ADDING MIDDLE: " << cycles << " TIME SINCE STARTED: " << now - time << " TIME SINCE LAST UPDATE: " << now - last << std::endl;
	last = duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
#endif
		
	SOIL_free_image_data(data);

#ifdef DEBUG
	for (int i = 0; i < 1024; i++) std::cout << "HEIGHT " << i/4.0f << ": " << heights[i] << std::endl;
	last = duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
#endif

	int i = 0;
	for(int z = 0; z < iHeight-1; ++z)
		for (int x = 0; x < iWidth - 1; ++x) {
			int a = (z * iWidth) + x;
			int b = (z * iWidth) + x + 1;
			int c = ((z + 1) * iWidth) + x + 1;
			int d = ((z + 1) * iWidth) + x;

			indices[i++] = a;
			indices[i++] = c;
			indices[i++] = b;

			indices[i++] = c;
			indices[i++] = a;
			indices[i++] = d;
		}

#ifdef DEBUG
	now = duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
	std::cout << "FINISH INDICES" << " TIME SINCE STARTED: " << now - time << " TIME SINCE LAST UPDATE: " << now - last << std::endl;
	last = duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
#endif

	GenerateNormals();

#ifdef DEBUG
	now = duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
	std::cout << "FINISH NORMALS" << " TIME SINCE STARTED: " << now - time << " TIME SINCE LAST UPDATE: " << now - last << std::endl;
	last = duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
#endif

	GenerateTangents();

#ifdef DEBUG
	now = duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
	std::cout << "FINISH TANGENTS" << " TIME SINCE STARTED: " << now - time << " TIME SINCE LAST UPDATE: " << now - last << std::endl;
	last = duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
#endif

	BufferData();
#ifdef DEBUG
	now = duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
	std::cout << "FINISH BUFFER" << " TIME SINCE STARTED: " << now - time << " TIME SINCE LAST UPDATE: " << now - last << std::endl;
	last = duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
#endif
	
	heightmapSize.x = vertexScale.x * (iWidth - 1);
	heightmapSize.y = vertexScale.y * 255.0f; // Each height is a byte
	heightmapSize.z = vertexScale.z * (iHeight - 1);
}

float HeightMap::GetHeightAtLocation(int x, int z) {
	x /= 2;
	z /= 2;
	int offset = (z * iWidth) + x;
	return vertices[offset].y;
}