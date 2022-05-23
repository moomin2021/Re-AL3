#pragma once
#include "Matrix4.h"
#include "WorldTransform.h"

class Matrix
{
private:
	Matrix4 matScale;
	Matrix4 matRot;
	Matrix4 matTrans;

public:
	// コンストラクタ
	Matrix();

	// 拡縮
	void SetScale(WorldTransform worldTransform);

	// 回転
	void SetRot(WorldTransform worldTransform);

	// 平行移動
	void SetTrans(WorldTransform worldTransform);

	// 行列の計算
	void UpdateMatrix(WorldTransform & worldTransform);

	// 親と子の行列計算
	void UpdateParentMatrix(WorldTransform & w1, WorldTransform & w2);
};

