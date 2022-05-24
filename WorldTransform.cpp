#include "WorldTransform.h"
#include <vector>
#include "MathUtility.h"

// 拡縮
void WorldTransform::SetScale()
{
	matScale = MathUtility::Matrix4Identity();

	// スケーリング倍率を行列に設定する
	matScale.m[0][0] = scale_.x;
	matScale.m[1][1] = scale_.y;
	matScale.m[2][2] = scale_.z;
	matScale.m[3][3] = 1.0f;
}

void WorldTransform::SetRot()
{
	// X, Y, Z軸回転行列を宣言
	Matrix4 matRotX, matRotY, matRotZ;

	// 単位行列を代入
	matRot = MathUtility::Matrix4Identity();

	// X軸回転行列の各要素を設定する
	matRotX.m[0][0] = 1.0f;
	matRotX.m[1][1] = cos(rotation_.x);
	matRotX.m[1][2] = sin(rotation_.x);
	matRotX.m[2][1] = -sin(rotation_.x);
	matRotX.m[2][2] = cos(rotation_.x);
	matRotX.m[3][3] = 1.0f;

	// Y軸回転行列の各要素を設定する
	matRotY.m[0][0] = cos(rotation_.y);
	matRotY.m[0][2] = -sin(rotation_.y);
	matRotY.m[1][1] = 1.0f;
	matRotY.m[2][0] = sin(rotation_.y);
	matRotY.m[2][2] = cos(rotation_.y);
	matRotY.m[3][3] = 1.0f;

	// Z軸回転行列の各要素を設定する
	matRotZ.m[0][0] = cos(rotation_.z);
	matRotZ.m[0][1] = sin(rotation_.z);
	matRotZ.m[1][0] = -sin(rotation_.z);
	matRotZ.m[1][1] = cos(rotation_.z);
	matRotZ.m[2][2] = 1.0f;
	matRotZ.m[3][3] = 1.0f;

	matRotZ *= matRotX;
	matRotZ *= matRotY;
	matRot = matRotZ;
}

void WorldTransform::SetTrans()
{
	// 単位行列代入
	matTrans = MathUtility::Matrix4Identity();

	// 移動量を行列に設定する
	matTrans.m[3][0] = translation_.x;
	matTrans.m[3][1] = translation_.y;
	matTrans.m[3][2] = translation_.z;
}

void WorldTransform::UpdateMatrix()
{
	SetScale();
	SetRot();
	SetTrans();

	// 単位行列を代入
	matWorld_ = MathUtility::Matrix4Identity();

	// 掛け算
	matWorld_ *= matScale;
	matWorld_ *= matRot;
	matWorld_ *= matTrans;

	if (parent_ != nullptr)
	{
		matWorld_ *= parent_->matWorld_;
	}

	// 行列の転送
	TransferMatrix();
}