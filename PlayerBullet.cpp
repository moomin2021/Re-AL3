#include "TextureManager.h"
#include "PlayerBullet.h"
#include <cassert>


PlayerBullet::~PlayerBullet() {

}

// --����������-- //
void PlayerBullet::Initialize(Model* model, const Vector3& position) {
	// --NULL�|�C���^�`�F�b�N-- //
	assert(model);

	// --���f���ǂݍ���-- //
	model_ = model;

	// --�e�N�X�`���ǂݍ���-- //
	textureHandle_ = TextureManager::Load("mario.jpg");

	// --���[���h�g�����X�t�H�[���̏�����-- //
	worldTransform_.Initialize();

	// --�����Ŏ󂯎�����������W���Z�b�g-- //
	worldTransform_.translation_ = position;
}

// --�X�V����-- //
void PlayerBullet::Update() {
	// --���[���h�g�����X�t�H�[���̍X�V-- //
	worldTransform_.UpdateMatrix();
}

// --�`�揈��-- //
void PlayerBullet::Draw(const ViewProjection& viewProjection) {
	// --���f���̕`��-- //
	model_->Draw(worldTransform_, viewProjection, textureHandle_);
}