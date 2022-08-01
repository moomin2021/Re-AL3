#include "Player.h"
#include <cassert>

// --キャラクターの旋回処理-- //
void Player::Rotate() {
	// --旋回速度-- //
	float speed = 0.1f;

	// --押した方向で回転させる-- //
	worldTransform_.rotation_.y += (input_->PushKey(DIK_U) - input_->PushKey(DIK_I)) * speed;

	// --行列更新
	worldTransform_.UpdateMatrix();
}

// --攻撃-- //
void Player::Attack() {
	// --スペースキーを押したら-- //
	if (input_->TriggerKey(DIK_SPACE)) {
		// --弾を生成し、初期化
		std::unique_ptr<PlayerBullet> newBullet = std::make_unique<PlayerBullet>();
		newBullet->Initialize(model_, worldTransform_.translation_);

		// --弾を登録する
		bullets_.push_back(std::move(newBullet));
	}
}

/// --初期化処理-- ///
#pragma region

void Player::Initialize(Model* model, uint32_t textureHandle) {

	// --NULLポインタチェック-- //
	assert(model);

	// --受け取ったデータをメンバ変数に記録-- //
	model_ = model;
	textureHandle_ = textureHandle;

	// --シングルトンインスタンスを取得する-- //
	input_ = Input::GetInstance();
	debugText_ = DebugText::GetInstance();

	// --ワールド変換初期化-- //
	worldTransform_.Initialize();
}

/// --END-- ///
#pragma endregion

/// --更新処理-- ///
#pragma region

void Player::Update() {

	// --キャラクターー移動処理
	{
		// --キャラクター移動ベクトル
		Vector3 move = { 0, 0, 0 };

		// --キャラクターの移動速度-- //
		float speed = 0.1f;

		// --移動ベクトルを変更する処理
		move.x = (input_->PushKey(DIK_D) - input_->PushKey(DIK_A)) * speed;
		move.y = (input_->PushKey(DIK_W) - input_->PushKey(DIK_S)) * speed;

		// --座標移動（ベクトルの加算）
		worldTransform_.translation_ += move;

		// --移動限界座標
		const float kMoveLimitX = 35;
		const float kMoveLimitY = 20;

		// --キャラクターが画面外にいかないように制限
		worldTransform_.translation_.x = MathUtility::Clamp(worldTransform_.translation_.x, kMoveLimitX, -kMoveLimitX);
		worldTransform_.translation_.y = MathUtility::Clamp(worldTransform_.translation_.y, kMoveLimitY, -kMoveLimitY);


		// --行列更新
		worldTransform_.UpdateMatrix();
	}

	// --旋回処理-- //
	Rotate();

	// --キャラクター攻撃処理-- //
	Attack();

	// --弾更新-- //
	for (std::unique_ptr<PlayerBullet>& bullet : bullets_) bullet->Update();

	// --デバックテキスト-- //
	{
		// --キャラクターの座標を画面表示する処理
		debugText_->SetPos(50, 70);
		debugText_->Printf("pos:%f, %f, %f",
			worldTransform_.translation_.x, worldTransform_.translation_.y, worldTransform_.translation_.z);
	}

}

/// --END-- ///
#pragma endregion

/// --描画処理-- ///
#pragma region

void Player::Draw(ViewProjection viewProjection) {
	// --3Dモデルを描画-- //
	model_->Draw(worldTransform_, viewProjection, textureHandle_);

	// --弾の描画-- //
	for (std::unique_ptr<PlayerBullet>& bullet : bullets_) bullet->Draw(viewProjection);
}

/// --END-- ///
#pragma endregion