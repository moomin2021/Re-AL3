#include "TextureManager.h"
#include "PlayerBullet.h"
#include <cassert>


PlayerBullet::~PlayerBullet() {

}

// --初期化処理-- //
void PlayerBullet::Initialize(Model* model, const Vector3& position) {
	// --NULLポインタチェック-- //
	assert(model);

	// --モデル読み込み-- //
	model_ = model;

	// --テクスチャ読み込み-- //
	textureHandle_ = TextureManager::Load("mario.jpg");

	// --ワールドトランスフォームの初期化-- //
	worldTransform_.Initialize();

	// --引数で受け取った初期座標をセット-- //
	worldTransform_.translation_ = position;
}

// --更新処理-- //
void PlayerBullet::Update() {
	// --ワールドトランスフォームの更新-- //
	worldTransform_.UpdateMatrix();
}

// --描画処理-- //
void PlayerBullet::Draw(const ViewProjection& viewProjection) {
	// --モデルの描画-- //
	model_->Draw(worldTransform_, viewProjection, textureHandle_);
}