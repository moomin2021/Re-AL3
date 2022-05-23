#include "Matrix.h"
#include "Input.h"
#include "MathUtility.h"

// �R���X�g���N�^
Matrix::Matrix()
{

}

// �g�k
void Matrix::SetScale(WorldTransform worldTransform)
{
	// �X�P�[�����O�s���錾
	matScale = MathUtility::Matrix4Identity();

	// �X�P�[�����O�{�����s��ɐݒ肷��
	matScale.m[0][0] = worldTransform.scale_.x;
	matScale.m[1][1] = worldTransform.scale_.y;
	matScale.m[2][2] = worldTransform.scale_.z;
	matScale.m[3][3] = 1.0f;
}

// ��]
void Matrix::SetRot(WorldTransform worldTransform)
{
	// X, Y, Z����]�s���錾
	Matrix4 matRotX, matRotY, matRotZ;

	matRot = MathUtility::Matrix4Identity();

	// X����]�s��̊e�v�f��ݒ肷��
	matRotX.m[0][0] = 1.0f;
	matRotX.m[1][1] = cos(worldTransform.rotation_.x);
	matRotX.m[1][2] = sin(worldTransform.rotation_.x);
	matRotX.m[2][1] = -sin(worldTransform.rotation_.x);
	matRotX.m[2][2] = cos(worldTransform.rotation_.x);
	matRotX.m[3][3] = 1.0f;

	// Y����]�s��̊e�v�f��ݒ肷��
	matRotY.m[0][0] = cos(worldTransform.rotation_.y);
	matRotY.m[0][2] = -sin(worldTransform.rotation_.y);
	matRotY.m[1][1] = 1.0f;
	matRotY.m[2][0] = sin(worldTransform.rotation_.y);
	matRotY.m[2][2] = cos(worldTransform.rotation_.y);
	matRotY.m[3][3] = 1.0f;

	// Z����]�s��̊e�v�f��ݒ肷��
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

// ���s�ړ�
void Matrix::SetTrans(WorldTransform worldTransform)
{
	// ���s�ړ��s���錾
	matTrans = MathUtility::Matrix4Identity();

	// �ړ��ʂ��s��ɐݒ肷��
	matTrans.m[3][0] = worldTransform.translation_.x;
	matTrans.m[3][1] = worldTransform.translation_.y;
	matTrans.m[3][2] = worldTransform.translation_.z;
}

// �s��̌v�Z
void Matrix::UpdateMatrix(WorldTransform & worldTransform)
{

	SetScale(worldTransform);
	SetRot(worldTransform);
	SetTrans(worldTransform);

	// �P�ʍs�����
	worldTransform.matWorld_ = MathUtility::Matrix4Identity();

	// �|���Z
	worldTransform.matWorld_ *= matScale;
	worldTransform.matWorld_ *= matRot;
	worldTransform.matWorld_ *= matTrans;

	// �s��̓]��
	worldTransform.TransferMatrix();
}

// �e�Ǝq�̍s��v�Z
void Matrix::UpdateParentMatrix(WorldTransform & w1, WorldTransform & w2)
{
	UpdateMatrix(w2);
	w2.matWorld_ *= w1.matWorld_;
	// �s��̓]��
	w2.TransferMatrix();
}