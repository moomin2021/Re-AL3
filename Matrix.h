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
	// �R���X�g���N�^
	Matrix();

	// �g�k
	void SetScale(WorldTransform worldTransform);

	// ��]
	void SetRot(WorldTransform worldTransform);

	// ���s�ړ�
	void SetTrans(WorldTransform worldTransform);

	// �s��̌v�Z
	void UpdateMatrix(WorldTransform & worldTransform);

	// �e�Ǝq�̍s��v�Z
	void UpdateParentMatrix(WorldTransform & w1, WorldTransform & w2);
};

