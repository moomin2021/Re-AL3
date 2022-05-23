#include "Matrix.h"
#include "Input.h"
#include "MathUtility.h"

// 拡縮
Matrix4 Matrix::SetScale(float x, float y, float z)
{
	// スケーリング行列を宣言
	Matrix4 matScale;

	// スケーリング倍率を行列に設定する
	matScale.m[0][0] = x;
	matScale.m[1][1] = y;
	matScale.m[2][2] = z;
	matScale.m[3][3] = 1.0f;

	return matScale;
}

// 回転
Matrix4 Matrix::SetRot(float x, float y, float z)
{
	// X, Y, Z軸回転行列を宣言
	Matrix4 matRotX, matRotY, matRotZ;

	Matrix4 matRot;

	// X軸回転行列の各要素を設定する
	matRotX.m[0][0] = 1.0f;
	matRotX.m[1][1] = cos(x);
	matRotX.m[1][2] = sin(x);
	matRotX.m[2][1] = -sin(x);
	matRotX.m[2][2] = cos(x);
	matRotX.m[3][3] = 1.0f;

	// Y軸回転行列の各要素を設定する
	matRotY.m[0][0] = cos(y);
	matRotY.m[0][2] = -sin(y);
	matRotY.m[1][1] = 1.0f;
	matRotY.m[2][0] = sin(y);
	matRotY.m[2][2] = cos(y);
	matRotY.m[3][3] = 1.0f;

	// Z軸回転行列の各要素を設定する
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

// 平行移動
Matrix4 Matrix::SetTrans(float x, float y, float z)
{
	// 平行移動行列を宣言
	Matrix4 matTrans = MathUtility::Matrix4Identity();

	// 移動量を行列に設定する
	matTrans.m[3][0] = x;
	matTrans.m[3][1] = y;
	matTrans.m[3][2] = z;

	return matTrans;
}