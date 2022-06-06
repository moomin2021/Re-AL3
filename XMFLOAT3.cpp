#include "XMFLOAT3.h"

XMFLOAT3::XMFLOAT3() : x(0.0f), y(0.0f), z(0.0f)
{

}

XMFLOAT3::XMFLOAT3(float x, float y, float z)
{
	this->x = x, this->y = y, this->z = z;
}