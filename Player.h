#pragma once
#include "Model.h"
#include "WorldTransform.h"
#include "Input.h"
#include "DebugText.h"
#include "ViewProjection.h"
#include "PlayerBullet.h"

class Player {
private:
	// --ワールド変換データ-- //
	WorldTransform worldTransform_;

	// --モデル-- //
	Model* model_ = nullptr;

	// --テクスチャハンドル-- //
	uint32_t textureHandle_ = 0u;

	// --マウス入力-- //
	Input* input_ = nullptr;

	// --デバックテキスト-- //
	DebugText* debugText_ = nullptr;
	
	// --弾-- //
	PlayerBullet* bullet_ = nullptr;

private:
	// --キャラクターの旋回処理-- //
	void Rotate();

	// --攻撃-- //
	void Attack();

public:
	// --初期化処理-- //
	void Initialize(Model * model, uint32_t textureHandle);

	// --更新処理-- //
	void Update();

	// --描画処理-- //
	void Draw(ViewProjection viewProjection);
};