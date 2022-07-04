#include "GameScene.h"
#include "TextureManager.h"
#include <cassert>
#include "AxisIndicator.h"
#include "PrimitiveDrawer.h"
#include <random>

GameScene::GameScene() {}

GameScene::~GameScene()
{
	delete sprite_;
	delete model_;
	delete debugCamera_;
}

void GameScene::Initialize()
{

	dxCommon_ = DirectXCommon::GetInstance();
	input_ = Input::GetInstance();
	audio_ = Audio::GetInstance();
	debugText_ = DebugText::GetInstance();

	// 乱数シード生成器
	std::random_device seed_gen;

	// メルセンヌ・ツイスターの乱数エンジン
	std::mt19937_64 engine(seed_gen());

	// 乱数範囲の指定
	std::uniform_real_distribution<float> dist(-10, 10);

	std::uniform_real_distribution<float> rotDist(0.0f, MathUtility::PI * 2.0f);

	// ファイル名を指定してテクスチャを読み込む
	textureHandle_ = TextureManager::Load("mario.jpg");

	// 3Dモデルの生成
	model_ = Model::Create();

	// スプライト生成
	sprite_ = Sprite::Create(textureHandle_, { WinApp::kWindowWidth / 2.0f - 64, WinApp::kWindowHeight / 2.0f - 64 });

	// デバックカメラの生成
	debugCamera_ = new DebugCamera(WinApp::kWindowWidth, WinApp::kWindowHeight);

	// 軸方向表示の表示を有効にする
	AxisIndicator::GetInstance()->SetVisible(true);

	// 軸方向表示が参照するビュープロジェクションを指定する（アドレス無し）
	AxisIndicator::GetInstance()->SetTargetViewProjection(&viewProjection_);

	// ライン描画が参照するビュープロジェクションを指定する（アドレス無し）
	PrimitiveDrawer::GetInstance()->SetViewProjection(&viewProjection_);

	/// <summary>
	/// 各オブジェクトの位置を設定して人型にする
	/// </summary>
	
	// --大元の位置設定-- //
	worldTransforms_[PartId::kRoot].translation_ = { 0.0f, 0.0f, 0.0f };

	// --頭の位置設定-- //
	worldTransforms_[PartId::kHead].translation_ = { 0.0f, 3.0f, 0.0f };
	worldTransforms_[PartId::kHead].parent_ = &worldTransforms_[PartId::kRoot];

	// --胸の位置と親子構造設定-- //
	worldTransforms_[PartId::kChest].translation_ = { 0.0f, 0.0f, 0.0f };
	worldTransforms_[PartId::kChest].parent_ = &worldTransforms_[PartId::kRoot];

	// --左腕の位置と親子構造設定-- //
	worldTransforms_[PartId::kArmL].translation_ = {-3.0f, 0.0f, 0.0f};
	worldTransforms_[PartId::kArmL].parent_ = &worldTransforms_[PartId::kChest];

	// --右腕の位置と親子関係設定-- //
	worldTransforms_[PartId::kArmR].translation_ = { 3.0f, 0.0f, 0.0f };
	worldTransforms_[PartId::kArmR].parent_ = &worldTransforms_[PartId::kChest];

	// --尻の位置と親子構造設定-- //
	worldTransforms_[PartId::kHip].translation_ = { 0.0f, -3.0f, 0.0f };
	worldTransforms_[PartId::kHip].parent_ = &worldTransforms_[PartId::kRoot];

	// --左足の位置と親子構造設定-- //
	worldTransforms_[PartId::kLegL].translation_ = { -3.0f, -3.0f, 0.0f };
	worldTransforms_[PartId::kLegL].parent_ = &worldTransforms_[PartId::kHip];

	// --左足の位置と親子構造設定-- //
	worldTransforms_[PartId::kLegR].translation_ = { 3.0f, -3.0f, 0.0f };
	worldTransforms_[PartId::kLegR].parent_ = &worldTransforms_[PartId::kHip];

	// --オブジェクト全体の初期化-- //
	for (size_t i = 0; i < _countof(worldTransforms_); i++) {
		worldTransforms_[i].Initialize();
	}

	// --カメラの視点座標の設定-- //
	viewProjection_.eye = { 0.0f, 3.0f, -30.0f };

	// --カメラの注視点を設定-- //
	viewProjection_.target = { 0.0f, 3.0f, 0.0f };

	// --カメラの垂直方向視野角の設定-- //
	viewProjection_.fovAngleY = MathUtility::Degree2Radian(45.0f);

	// --ビュープロジェクションの初期化-- //
	viewProjection_.Initialize();
}

void GameScene::Update()
{

	// --オブジェクト全体の横移動処理-- //
	{
		// --移動量
		float moveSpeed = 0.8f;

		// --キーボードのADが押されたら横移動させる
		worldTransforms_[PartId::kRoot].translation_.x +=
			(input_->PushKey(DIK_D) - input_->PushKey(DIK_A)) * moveSpeed;
	}

	// --オブジェクト全体の回転処理-- //
	{
		// --回転量
		float rotaSpeed = 2.0f;

		// --キーボードの横アローキーが押されたら回転する
		worldTransforms_[PartId::kRoot].rotation_.y +=
			MathUtility::Degree2Radian((input_->PushKey(DIK_LEFT) - input_->PushKey(DIK_RIGHT)) * rotaSpeed);
	}

	// --腕足の回転処理-- //
	{
		// --腕と足の回転スピード-- //
		float rotaSpeed = 10.0f;

		// --左腕の回転処理
		worldTransforms_[PartId::kArmL].rotation_.x -= MathUtility::Degree2Radian(rotaSpeed);
		
		// --右腕の回転処理
		worldTransforms_[PartId::kArmR].rotation_.x += MathUtility::Degree2Radian(rotaSpeed);
	
		// --左足の回転処理
		worldTransforms_[PartId::kLegL].rotation_.x += MathUtility::Degree2Radian(rotaSpeed);

		// --右足の回転処理
		worldTransforms_[PartId::kLegR].rotation_.x -= MathUtility::Degree2Radian(rotaSpeed);
	}

	// --各オブジェクトの再計算-- //
	for (size_t i = 0; i < _countof(worldTransforms_); i++) {
		worldTransforms_[i].UpdateMatrix();
	}

	// --行列の再計算-- //
	viewProjection_.UpdateMatrix();

	// --デバックカメラの更新-- //
	debugCamera_->Update();

	// --デバッグ用表示-- //
	debugText_->SetPos(50, 50);
	debugText_->Printf("eye:(%f, %f, %f)", viewProjection_.eye.x, viewProjection_.eye.y, viewProjection_.eye.z);

	debugText_->SetPos(50, 70);
	debugText_->Printf("target:(%f, %f, %f)", viewProjection_.target.x, viewProjection_.target.y, viewProjection_.target.z);

	debugText_->SetPos(50, 90);
	debugText_->Printf("up:(%f, %f, %f)", viewProjection_.up.x, viewProjection_.up.y, viewProjection_.up.z);

	debugText_->SetPos(50, 110);
	debugText_->Printf("fovAngleY(Degree):%f", MathUtility::Radian2Degree(viewProjection_.fovAngleY));

	debugText_->SetPos(50, 130);
	debugText_->Printf("nearZ:%f", viewProjection_.nearZ);
}

void GameScene::Draw()
{

	// コマンドリストの取得
	ID3D12GraphicsCommandList * commandList = dxCommon_->GetCommandList();

#pragma region 背景スプライト描画
	// 背景スプライト描画前処理
	Sprite::PreDraw(commandList);

	/// <summary>
	/// ここに背景スプライトの描画処理を追加できる
	/// </summary>

	// スプライト描画後処理
	Sprite::PostDraw();
	// 深度バッファクリア
	dxCommon_->ClearDepthBuffer();
#pragma endregion

#pragma region 3Dオブジェクト描画
	// 3Dオブジェクト描画前処理
	Model::PreDraw(commandList);

	/// <summary>
	/// ここに3Dオブジェクトの描画処理を追加できる
	/// </summary>

	// --3Dモデル追加-- //

	for (size_t i = 0; i < _countof(worldTransforms_); i++) {
		model_->Draw(worldTransforms_[i], viewProjection_, textureHandle_);
	}

	// ライン描画が参照するビュープロジェクションを指定する（アドレス無し）

	//// Z軸(奥行き)方向のライン描画(複数)
	//for (size_t i = 0; i < 21; i++)
	//{
	//	PrimitiveDrawer::GetInstance()->DrawLine3d(Vector3(10.0f - i, 0.0f, 10.0f), Vector3(10.0f - i, 0.0f, -10.0f), Vector4(0.0f, 0.0f, 1.0f, 1.0f));
	//}

	//// X軸(横)方向のライン描画(複数)
	//for (size_t i = 0; i < 21; i++)
	//{
	//	PrimitiveDrawer::GetInstance()->DrawLine3d(Vector3(10.0f, 0.0f, 10.0f - i), Vector3(-10.0f, 0.0f, 10.0f - i), Vector4(1.0f, 0.0f, 0.0f, 1.0f));
	//}

	// 3Dオブジェクト描画後処理
	Model::PostDraw();
#pragma endregion

#pragma region 前景スプライト描画
	// 前景スプライト描画前処理
	Sprite::PreDraw(commandList);

	/// <summary>
	/// ここに前景スプライトの描画処理を追加できる
	/// </summary>

	// デバッグテキストの描画
	debugText_->DrawAll(commandList);
	//
	// スプライト描画後処理
	Sprite::PostDraw();

#pragma endregion
}
