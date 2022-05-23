#pragma once
#include "Matrix4.h"

class Matrix
{
public:
	// �g�k
	static Matrix4 SetScale(float x, float y, float z);

	// ��]
	static Matrix4 SetRot(float x, float y, float z);

	// ���s�ړ�
	static Matrix4 SetTrans(float x, float y, float z);
};

