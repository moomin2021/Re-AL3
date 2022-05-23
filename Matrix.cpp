#include "Matrix.h"
#include "Input.h"
#include "MathUtility.h"

// �g�k
Matrix4 Matrix::SetScale(float x, float y, float z)
{
	// �X�P�[�����O�s���錾
	Matrix4 matScale;

	// �X�P�[�����O�{�����s��ɐݒ肷��
	matScale.m[0][0] = x;
	matScale.m[1][1] = y;
	matScale.m[2][2] = z;
	matScale.m[3][3] = 1.0f;

	return matScale;
}

// ��]
Matrix4 Matrix::SetRot(float x, float y, float z)
{
	// X, Y, Z����]�s���錾
	Matrix4 matRotX, matRotY, matRotZ;

	Matrix4 matRot;

	// X����]�s��̊e�v�f��ݒ肷��
	matRotX.m[0][0] = 1.0f;
	matRotX.m[1][1] = cos(x);
	matRotX.m[1][2] = sin(x);
	matRotX.m[2][1] = -sin(x);
	matRotX.m[2][2] = cos(x);
	matRotX.m[3][3] = 1.0f;

	// Y����]�s��̊e�v�f��ݒ肷��
	matRotY.m[0][0] = cos(y);
	matRotY.m[0][2] = -sin(y);
	matRotY.m[1][1] = 1.0f;
	matRotY.m[2][0] = sin(y);
	matRotY.m[2][2] = cos(y);
	matRotY.m[3][3] = 1.0f;

	// Z����]�s��̊e�v�f��ݒ肷��
	matRotZ.m[0][0] = cos(z);
	matRotZ.m[0][1] = sin(z);
	matRotZ.m[1][0] = -sin(z);
	matRotZ.m[1][1] = cos(z);
	matRotZ.m[2][2] = 1.0f;
	matRotZ.m[3][3] = 1.0f;

	matRotZ *= matRotX;
	matRotZ *= matRotY;
	matRot = matRotZ;

	return matRot;
}

// ���s�ړ�
Matrix4 Matrix::SetTrans(float x, float y, float z)
{
	// ���s�ړ��s���錾
	Matrix4 matTrans = MathUtility::Matrix4Identity();

	// �ړ��ʂ��s��ɐݒ肷��
	matTrans.m[3][0] = x;
	matTrans.m[3][1] = y;
	matTrans.m[3][2] = z;

	return matTrans;
}