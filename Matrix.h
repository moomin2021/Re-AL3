#pragma once
#include "Matrix4.h"

class Matrix
{
public:
	// Šgk
	static Matrix4 SetScale(float x, float y, float z);

	// ‰ñ“]
	static Matrix4 SetRot(float x, float y, float z);

	// •½sˆÚ“®
	static Matrix4 SetTrans(float x, float y, float z);
};

