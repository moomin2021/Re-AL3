#pragma once
#include "Model.h"
#include "WorldTransform.h"
#include "ViewProjection.h"

class PlayerBullet {
private:
	// --���[���h�ϊ��f�[�^-- //
	WorldTransform worldTransform_;

	// --���f���̃|�C���^-- //
	Model* model_;

	// --�e�N�X�`���n���h��-- //
	uint32_t textureHandle_ = 0u;

public:
	~PlayerBullet();

	// --����������-- //
	void Initialize(Model* model, const Vector3& position);

	// --�X�V����-- //
	void Update();

	// --�`�揈��-- //
	void Draw(const ViewProjection& viewProjection);
};