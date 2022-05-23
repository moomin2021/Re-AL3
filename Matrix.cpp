#include "Matrix.h"
#include "Input.h"
#include "MathUtility.h"

// コンストラクタ
Matrix::Matrix()
{

}

// 拡縮
void Matrix::SetScale(WorldTransform worldTransform)
{
	// スケーリング行列を宣言
	matScale = MathUtility::Matrix4Identity();

	// スケーリング倍率を行列に設定する
	matScale.m[0][0] = worldTransform.scale_.x;
	matScale.m[1][1] = worldTransform.scale_.y;
	matScale.m[2][2] = worldTransform.scale_.z;
	matScale.m[3][3] = 1.0f;
}

// 回転
void Matrix::SetRot(WorldTransform worldTransform)
{
	// X, Y, Z軸回転行列を宣言
	Matrix4 matRotX, matRotY, matRotZ;

	matRot = MathUtility::Matrix4Identity();

	// X軸回転行列の各要素を設定する
	matRotX.m[0][0] = 1.0f;
	matRotX.m[1][1] = cos(worldTransform.rotation_.x);
	matRotX.m[1][2] = sin(worldTransform.rotation_.x);
	matRotX.m[2][1] = -sin(worldTransform.rotation_.x);
	matRotX.m[2][2] = cos(worldTransform.rotation_.x);
	matRotX.m[3][3] = 1.0f;

	// Y軸回転行列の各要素を設定する
	matRotY.m[0][0] = cos(worldTransform.rotation_.y);
	matRotY.m[0][2] = -sin(worldTransform.rotation_.y);
	matRotY.m[1][1] = 1.0f;
	matRotY.m[2][0] = sin(worldTransform.rotation_.y);
	matRotY.m[2][2] = cos(worldTransform.rotation_.y);
	matRotY.m[3][3] = 1.0f;

	// Z軸回転行列の各要素を設定する
	matRotZ.m[0][0] = cos(worldTransform.rotation_.z);
	matRotZ.m[0][1] = sin(worldTransform.rotation_.z);
	matRotZ.m[1][0] = -sin(worldTransform.rotation_.z);
	matRotZ.m[1][1] = cos(worldTransform.rotation_.z);
	matRotZ.m[2][2] = 1.0f;
	matRotZ.m[3][3] = 1.0f;

	matRotZ *= matRotX;
	matRotZ *= matRotY;
	matRot = matRotZ;
}

// 平行移動
void Matrix::SetTrans(WorldTransform worldTransform)
{
	// 平行移動行列を宣言
	matTrans = MathUtility::Matrix4Identity();

	// 移動量を行列に設定する
	matTrans.m[3][0] = worldTransform.translation_.x;
	matTrans.m[3][1] = worldTransform.translation_.y;
	matTrans.m[3][2] = worldTransform.translation_.z;
}

// 行列の計算
void Matrix::UpdateMatrix(WorldTransform & worldTransform)
{

	SetScale(worldTransform);
	SetRot(worldTransform);
	SetTrans(worldTransform);

	// 単位行列を代入
	worldTransform.matWorld_ = MathUtility::Matrix4Identity();

	// 掛け算
	worldTransform.matWorld_ *= matScale;
	worldTransform.matWorld_ *= matRot;
	worldTransform.matWorld_ *= matTrans;

	// 行列の転送
	worldTransform.TransferMatrix();
}

// 親と子の行列計算
void Matrix::UpdateParentMatrix(WorldTransform & w1, WorldTransform & w2)
{
	UpdateMatrix(w2);
	w2.matWorld_ *= w1.matWorld_;
	// 行列の転送
	w2.TransferMatrix();
}