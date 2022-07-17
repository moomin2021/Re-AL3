#include "Player.h"
#include <cassert>

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

		// --移動ベクトルを変更する処理
		move.x = input_->PushKey(DIK_D) - input_->PushKey(DIK_A);
		move.y = input_->PushKey(DIK_W) - input_->PushKey(DIK_S);

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
}

/// --END-- ///
#pragma endregion