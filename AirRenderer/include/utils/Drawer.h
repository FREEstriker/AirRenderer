#pragma once
#include <glm/ext/vector_int2.hpp>
#include "Color.h"
#include "include/utils/Buffer.h"
#include "include/core_object/Configuration.h"
using namespace glm;

class Drawer
{
public:
	static void DrawLine(ivec2 startPosition, ivec2 endPosition, Color color, Buffer<Color>& buffer);
	static void DrawTriangle(ivec2 positionA, ivec2 positionB, ivec2 positionC, Color color, Buffer<Color>& buffer);
	static void DrawTriangle_CheckBoundry(ivec2 positionA, ivec2 positionB, ivec2 positionC, Color color, Buffer<Color>& buffer, int width, int height);
private:
	static bool CheckInBoundry(ivec2 position, int width, int height);
};

