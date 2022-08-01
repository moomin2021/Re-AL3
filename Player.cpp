#include "Player.h"
#include <cassert>

// --�L�����N�^�[�̐��񏈗�-- //
void Player::Rotate() {
	// --���񑬓x-- //
	float speed = 0.1f;

	// --�����������ŉ�]������-- //
	worldTransform_.rotation_.y += (input_->PushKey(DIK_U) - input_->PushKey(DIK_I)) * speed;

	// --�s��X�V
	worldTransform_.UpdateMatrix();
}

// --�U��-- //
void Player::Attack() {
	// --�X�y�[�X�L�[����������-- //
	if (input_->TriggerKey(DIK_SPACE)) {
		// --�e�𐶐����A������
		std::unique_ptr<PlayerBullet> newBullet = std::make_unique<PlayerBullet>();
		newBullet->Initialize(model_, worldTransform_.translation_);

		// --�e��o�^����
		bullets_.push_back(std::move(newBullet));
	}
}

/// --����������-- ///
#pragma region

void Player::Initialize(Model* model, uint32_t textureHandle) {

	// --NULL�|�C���^�`�F�b�N-- //
	assert(model);

	// --�󂯎�����f�[�^�������o�ϐ��ɋL�^-- //
	model_ = model;
	textureHandle_ = textureHandle;

	// --�V���O���g���C���X�^���X���擾����-- //
	input_ = Input::GetInstance();
	debugText_ = DebugText::GetInstance();

	// --���[���h�ϊ�������-- //
	worldTransform_.Initialize();
}

/// --END-- ///
#pragma endregion

/// --�X�V����-- ///
#pragma region

void Player::Update() {

	// --�L�����N�^�[�[�ړ�����
	{
		// --�L�����N�^�[�ړ��x�N�g��
		Vector3 move = { 0, 0, 0 };

		// --�L�����N�^�[�̈ړ����x-- //
		float speed = 0.1f;

		// --�ړ��x�N�g����ύX���鏈��
		move.x = (input_->PushKey(DIK_D) - input_->PushKey(DIK_A)) * speed;
		move.y = (input_->PushKey(DIK_W) - input_->PushKey(DIK_S)) * speed;

		// --���W�ړ��i�x�N�g���̉��Z�j
		worldTransform_.translation_ += move;

		// --�ړ����E���W
		const float kMoveLimitX = 35;
		const float kMoveLimitY = 20;

		// --�L�����N�^�[����ʊO�ɂ����Ȃ��悤�ɐ���
		worldTransform_.translation_.x = MathUtility::Clamp(worldTransform_.translation_.x, kMoveLimitX, -kMoveLimitX);
		worldTransform_.translation_.y = MathUtility::Clamp(worldTransform_.translation_.y, kMoveLimitY, -kMoveLimitY);


		// --�s��X�V
		worldTransform_.UpdateMatrix();
	}

	// --���񏈗�-- //
	Rotate();

	// --�L�����N�^�[�U������-- //
	Attack();

	// --�e�X�V-- //
	for (std::unique_ptr<PlayerBullet>& bullet : bullets_) bullet->Update();

	// --�f�o�b�N�e�L�X�g-- //
	{
		// --�L�����N�^�[�̍��W����ʕ\�����鏈��
		debugText_->SetPos(50, 70);
		debugText_->Printf("pos:%f, %f, %f",
			worldTransform_.translation_.x, worldTransform_.translation_.y, worldTransform_.translation_.z);
	}

}

/// --END-- ///
#pragma endregion

/// --�`�揈��-- ///
#pragma region

void Player::Draw(ViewProjection viewProjection) {
	// --3D���f����`��-- //
	model_->Draw(worldTransform_, viewProjection, textureHandle_);

	// --�e�̕`��-- //
	for (std::unique_ptr<PlayerBullet>& bullet : bullets_) bullet->Draw(viewProjection);
}

/// --END-- ///
#pragma endregion