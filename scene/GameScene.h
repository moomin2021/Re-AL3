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
	enum PartId
	{
		kRoot,// --> 大元
		kHead,// --> 頭
		kChest,// -> 胸
		kArmL,// --> 左腕
		kArmR,// --> 右腕
		kHandL,// -> 左手
		kHandR,// -> 右手
		kHip,// ---> 尻
		kLegL1,// -> 左足1
		kLegL2,// -> 左足2
		kLegR1,// -> 右足1
		kLegR2// --> 右足2
	};

	bool angleChange = false;

	float angle = 0.0f;

	// カメラ上方向の角度
	float viewAngle = 0.0f;
	
	// テクスチャハンドル
	uint32_t textureHandle_;

	// スプライト
	Sprite * sprite_;

	// 3Dモデル
	Model * model_ = nullptr;

	// ワールドトランスフォーム
	WorldTransform worldTransforms_[12];

	// ビュープロジェクション
	ViewProjection viewProjection_;

	// デバッグのカメラ
	DebugCamera * debugCamera_ = nullptr;
};
