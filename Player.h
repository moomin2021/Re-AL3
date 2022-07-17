#pragma once
#include "Model.h"
#include "WorldTransform.h"
#include "Input.h"
#include "DebugText.h"
#include "ViewProjection.h"

class Player {
private:
	// --���[���h�ϊ��f�[�^-- //
	WorldTransform worldTransform_;

	// --���f��-- //
	Model* model_ = nullptr;

	// --�e�N�X�`���n���h��-- //
	uint32_t textureHandle_ = 0u;

	// --�}�E�X����-- //
	Input* input_ = nullptr;

	// --�f�o�b�N�e�L�X�g-- //
	DebugText* debugText_ = nullptr;

public:
	// --����������-- //
	void Initialize(Model * model, uint32_t textureHandle);

	// --�X�V����-- //
	void Update();

	// --�`�揈��-- //
	void Draw(ViewProjection viewProjection);
};