#include "Matrix4.h"

// �R���X�g���N�^
Matrix4::Matrix4() {}

// �������w�肵�Ă̐���
Matrix4::Matrix4(
	float m00, float m01, float m02, float m03,
	float m10, float m11, float m12, float m13,
	float m20, float m21, float m22, float m23,
	float m30, float m31, float m32, float m33)
{
	m[0][0] = m00, m[0][1] = m01, m[0][2] = m02, m[0][3] = m03;
	m[1][0] = m10, m[1][1] = m11, m[1][2] = m12, m[1][3] = m13;
	m[2][0] = m20, m[2][1] = m21, m[2][2] = m22, m[2][3] = m23;
	m[3][0] = m30, m[3][1] = m31, m[3][2] = m32, m[3][3] = m33;
}

Matrix4 Matrix4::operator+() const
{
	return *this;
}

Matrix4 Matrix4::operator-() const
{
	return -(*this);
}

// ������Z�q�I�[�o�[���[�h
Matrix4& Matrix4::operator*=(const Matrix4 & m2)
{
	m[0][0] *= m2.m[0][0], m[0][1] *= m2.m[0][1], m[0][2] *= m2.m[0][2], m[0][3] *= m2.m[0][3];
	m[1][0] *= m2.m[1][0], m[1][1] *= m2.m[1][1], m[1][2] *= m2.m[1][2], m[1][3] *= m2.m[1][3];
	m[2][0] *= m2.m[2][0], m[2][1] *= m2.m[2][1], m[2][2] *= m2.m[2][2], m[2][3] *= m2.m[2][3];
	m[3][0] *= m2.m[3][0], m[3][1] *= m2.m[3][1], m[3][2] *= m2.m[3][2], m[3][3] *= m2.m[3][3];
	return *this;
}

Matrix4 operator*(const Matrix4 & m1, const Matrix4 & m2)
{
	return m1 * m2;
}