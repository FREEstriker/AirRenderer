#include "include/utils/Drawer.h"
#include <algorithm>


void Drawer::DrawLine(ivec2 startPosition, ivec2 endPosition, Color color, Buffer<Color>& buffer)
{
	bool isSteep = false;
	ivec2 difference = endPosition - startPosition;
	if (abs(difference.y) > abs(difference.x))
	{
		isSteep = true;

		int t = startPosition.x;
		startPosition.x = startPosition.y;
		startPosition.y = t;

		t = endPosition.x;
		endPosition.x = endPosition.y;
		endPosition.y = t;
	}

	if (endPosition.x < startPosition.x)
	{
		ivec2 t = startPosition;
		startPosition = endPosition;
		endPosition = t;
	}

	difference = endPosition - startPosition;
	int deltaAbs2X = abs(difference.x) * 2;
	int deltaAbs2Y = abs(difference.y) * 2;
	int d = 0;
	int dy = difference.y > 0 ? 1 : -1;
	if (isSteep)
	{
		for (int x = startPosition.x, y = startPosition.y; x <= endPosition.x; x++)
		{
			buffer.SetData(color, y, x);

			d += deltaAbs2Y;
			if (d > difference.x)
			{
				y += dy;
				d -= deltaAbs2X;
			}
		}
	}
	else
	{
		for (int x = startPosition.x, y = startPosition.y; x <= endPosition.x; x++)
		{
			buffer.SetData(color, x, y);

			d += deltaAbs2Y;
			if (d > difference.x)
			{
				y += dy;
				d -= deltaAbs2X;
			}
		}
	}
}
void Drawer::DrawTriangle(ivec2 positionA, ivec2 positionB, ivec2 positionC, Color color, Buffer<Color>& buffer)
{
	if (positionA.y > positionC.y)
	{
		ivec2 t = positionA;
		positionA = positionC;
		positionC = t;
	}
	if (positionB.y > positionC.y)
	{
		ivec2 t = positionB;
		positionB = positionC;
		positionC = t;
	}
	if (positionA.y > positionB.y)
	{
		ivec2 t = positionA;
		positionA = positionB;
		positionB = t;
	}

	int acHeight = positionC.y - positionA.y;
	int abHeight = positionB.y - positionA.y;
	int bcHeight = positionC.y - positionB.y;
	ivec2 ac = positionC - positionA;
	ivec2 ab = positionB - positionA;
	ivec2 bc = positionC - positionB;
	for (int y = positionA.y; y < positionB.y; y++)
	{	
		int m = positionA.x + ac.x * (double(y - positionA.y) / double(acHeight));
		int n = positionA.x + ab.x * (double(y - positionA.y) / double(abHeight));
		int minX = std::min(m, n);
		int maxX = std::max(m, n);
		for (int x = minX; x <= maxX; x++)
		{
			buffer.SetData(color, x, y);
		}
	}
	if (abHeight == 0)
	{
		int minX = std::min(positionA.x, positionB.x);
		int maxX = std::max(positionA.x, positionB.x);
		for (int x = minX; x <= maxX; x++)
		{
			buffer.SetData(color, x, positionB.y);
		}
	}
	else if (bcHeight == 0)
	{
		int minX = std::min(positionC.x, positionB.x);
		int maxX = std::max(positionC.x, positionB.x);
		for (int x = minX; x <= maxX; x++)
		{
			buffer.SetData(color, x, positionB.y);
		}
	}
	else
	{
		int m = positionC.x + ac.x * (double(positionB.y - positionC.y) / double(acHeight));
		int n = positionB.x;
		int minX = std::min(m, n);
		int maxX = std::max(m, n);
		for (int x = minX; x <= maxX; x++)
		{
			buffer.SetData(color, x, positionB.y);
		}
	}
	for (int y = positionC.y; y > positionB.y; y--)
	{	
		int m = positionC.x + ac.x * (double(y - positionC.y) / double(acHeight));
		int n = positionC.x + bc.x * (double(y - positionC.y) / double(bcHeight));
		int minX = std::min(m, n);
		int maxX = std::max(m, n);
		for (int x = minX; x <= maxX; x++)
		{
			buffer.SetData(color, x, y);
		}
	}
}

void Drawer::DrawTriangle_CheckBoundry(ivec2 positionA, ivec2 positionB, ivec2 positionC, Color color, Buffer<Color>& buffer, int width, int height)
{
	if (positionA.y > positionC.y)
	{
		ivec2 t = positionA;
		positionA = positionC;
		positionC = t;
	}
	if (positionB.y > positionC.y)
	{
		ivec2 t = positionB;
		positionB = positionC;
		positionC = t;
	}
	if (positionA.y > positionB.y)
	{
		ivec2 t = positionA;
		positionA = positionB;
		positionB = t;
	}

	int acHeight = positionC.y - positionA.y;
	int abHeight = positionB.y - positionA.y;
	int bcHeight = positionC.y - positionB.y;
	ivec2 ac = positionC - positionA;
	ivec2 ab = positionB - positionA;
	ivec2 bc = positionC - positionB;
	for (int y = positionA.y; y < positionB.y; y++)
	{
		int m = positionA.x + ac.x * (double(y - positionA.y) / double(acHeight));
		int n = positionA.x + ab.x * (double(y - positionA.y) / double(abHeight));
		int minX = std::min(m, n);
		int maxX = std::max(m, n);
		for (int x = minX; x <= maxX; x++)
		{
			if(CheckInBoundry(ivec2(x, y), width, height)) buffer.SetData(color, x, y);
		}
	}
	if (abHeight == 0)
	{
		int minX = std::min(positionA.x, positionB.x);
		int maxX = std::max(positionA.x, positionB.x);
		for (int x = minX; x <= maxX; x++)
		{
			if (CheckInBoundry(ivec2(x, positionB.y), width, height)) buffer.SetData(color, x, positionB.y);
		}
	}
	else if (bcHeight == 0)
	{
		int minX = std::min(positionC.x, positionB.x);
		int maxX = std::max(positionC.x, positionB.x);
		for (int x = minX; x <= maxX; x++)
		{
			if (CheckInBoundry(ivec2(x, positionB.y), width, height)) buffer.SetData(color, x, positionB.y);
		}
	}
	else
	{
		int m = positionC.x + ac.x * (double(positionB.y - positionC.y) / double(acHeight));
		int n = positionB.x;
		int minX = std::min(m, n);
		int maxX = std::max(m, n);
		for (int x = minX; x <= maxX; x++)
		{
			if (CheckInBoundry(ivec2(x, positionB.y), width, height)) buffer.SetData(color, x, positionB.y);
		}
	}
	for (int y = positionC.y; y > positionB.y; y--)
	{
		int m = positionC.x + ac.x * (double(y - positionC.y) / double(acHeight));
		int n = positionC.x + bc.x * (double(y - positionC.y) / double(bcHeight));
		int minX = std::min(m, n);
		int maxX = std::max(m, n);
		for (int x = minX; x <= maxX; x++)
		{
			if (CheckInBoundry(ivec2(x, y), width, height)) buffer.SetData(color, x, y);
		}
	}
}

bool Drawer::CheckInBoundry(ivec2 position, int width, int height)
{
	if (0 <= position.x && position.x < width
		&& 0 <= position.y && position.y < height)
	{
		return true;
	}
	else
	{
		return false;
	}
}
