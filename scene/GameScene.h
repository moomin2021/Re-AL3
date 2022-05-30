#pragma once

#include "Audio.h"
#include "DirectXCommon.h"
#include "DebugText.h"
#include "Input.h"
#include "Model.h"
#include "SafeDelete.h"
#include "Sprite.h"
#include "ViewProjection.h"
#include "WorldTransform.h"
#include "DebugCamera.h"

/// <summary>
/// ゲームシーン
/// </summary>
class GameScene {

  public: // メンバ関数
	/// <summary>
	/// コンストクラタ
	/// </summary>
	GameScene();

	/// <summary>
	/// デストラクタ
	/// </summary>
	~GameScene();

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();

	/// <summary>
	/// 毎フレーム処理
	/// </summary>
	void Update();

	/// <summary>
	/// 描画
	/// </summary>
	void Draw();

  private: // メンバ変数
	DirectXCommon* dxCommon_ = nullptr;
	Input* input_ = nullptr;
	Audio* audio_ = nullptr;
	DebugText* debugText_ = nullptr;

	/// <summary>
	/// ゲームシーン用
	/// </summary>
	/// 
	
public:
	
	// カメラ上方向の角度
	float viewAngle = 0.0f;
	
	// テクスチャハンドル
	uint32_t textureHandle_[3]{};

	// スプライト
	Sprite * sprite_[2]{};

	// 3Dモデル
	Model * model_ = nullptr;

	bool isScopeMode = false;

	float fovAngle = 90.0f;

	int scopeMagnification = 4.0f;

	// ワールドトランスフォーム
	WorldTransform worldTransforms_[100];

	// ビュープロジェクション
	ViewProjection viewProjection_;

	// デバッグのカメラ
	DebugCamera * debugCamera_ = nullptr;
};
