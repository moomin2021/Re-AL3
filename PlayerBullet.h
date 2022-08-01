#pragma once
#include "Model.h"
#include "WorldTransform.h"
#include "ViewProjection.h"

class PlayerBullet {
private:
	// --ワールド変換データ-- //
	WorldTransform worldTransform_;

	// --モデルのポインタ-- //
	Model* model_;

	// --テクスチャハンドル-- //
	uint32_t textureHandle_ = 0u;

public:
	~PlayerBullet();

	// --初期化処理-- //
	void Initialize(Model* model, const Vector3& position);

	// --更新処理-- //
	void Update();

	// --描画処理-- //
	void Draw(const ViewProjection& viewProjection);
};