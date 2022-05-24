#include "WorldTransform.h"
#include <vector>
#include "MathUtility.h"

// �g�k
void WorldTransform::SetScale()
{
	matScale = MathUtility::Matrix4Identity();

	// �X�P�[�����O�{�����s��ɐݒ肷��
	matScale.m[0][0] = scale_.x;
	matScale.m[1][1] = scale_.y;
	matScale.m[2][2] = scale_.z;
	matScale.m[3][3] = 1.0f;
}

void WorldTransform::SetRot()
{
	// X, Y, Z����]�s���錾
	Matrix4 matRotX, matRotY, matRotZ;

	// �P�ʍs�����
	matRot = MathUtility::Matrix4Identity();

	// X����]�s��̊e�v�f��ݒ肷��
	matRotX.m[0][0] = 1.0f;
	matRotX.m[1][1] = cos(rotation_.x);
	matRotX.m[1][2] = sin(rotation_.x);
	matRotX.m[2][1] = -sin(rotation_.x);
	matRotX.m[2][2] = cos(rotation_.x);
	matRotX.m[3][3] = 1.0f;

	// Y����]�s��̊e�v�f��ݒ肷��
	matRotY.m[0][0] = cos(rotation_.y);
	matRotY.m[0][2] = -sin(rotation_.y);
	matRotY.m[1][1] = 1.0f;
	matRotY.m[2][0] = sin(rotation_.y);
	matRotY.m[2][2] = cos(rotation_.y);
	matRotY.m[3][3] = 1.0f;

	// Z����]�s��̊e�v�f��ݒ肷��
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
	// �P�ʍs����
	matTrans = MathUtility::Matrix4Identity();

	// �ړ��ʂ��s��ɐݒ肷��
	matTrans.m[3][0] = translation_.x;
	matTrans.m[3][1] = translation_.y;
	matTrans.m[3][2] = translation_.z;
}

void WorldTransform::UpdateMatrix()
{
	SetScale();
	SetRot();
	SetTrans();

	// �P�ʍs�����
	matWorld_ = MathUtility::Matrix4Identity();

	// �|���Z
	matWorld_ *= matScale;
	matWorld_ *= matRot;
	matWorld_ *= matTrans;

	if (parent_ != nullptr)
	{
		matWorld_ *= parent_->matWorld_;
	}

	// �s��̓]��
	TransferMatrix();
}