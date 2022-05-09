#include "GameScene.h"
#include "TextureManager.h"
#include <cassert>
#include "AxisIndicator.h"
#include "PrimitiveDrawer.h"

double PI = 3.141592f;

float RadianChange(float n)
{
	return n * (PI / 180.0f);
}

void SubIdentityMatrix(Matrix4 * m)
{
	for (size_t i = 0; i < 4; i++)
	{
		m->m[i][i];
	}
}

GameScene::GameScene() {}

GameScene::~GameScene() {
	delete model_;
	delete debugCamera_;
}

void GameScene::Initialize() {

	dxCommon_ = DirectXCommon::GetInstance();
	input_ = Input::GetInstance();
	audio_ = Audio::GetInstance();
	debugText_ = DebugText::GetInstance();

	// ファイル名を指定してテクスチャを読み込む
	textureHandle_ = TextureManager::Load("mario.jpg");

	// 3Dモデルの生成
	model_ = Model::Create();

	// ワールドトランスフォームの初期化
	worldTransform_.Initialize();

	// ビュープロジェクション
	viewProjection_.Initialize();

	// デバックカメラの生成
	debugCamera_ = new DebugCamera(WinApp::kWindowWidth, WinApp::kWindowHeight);

	// 軸方向表示の表示を有効にする
	AxisIndicator::GetInstance()->SetVisible(true);

	// 軸方向表示が参照するビュープロジェクションを指定する（アドレス無し）
	AxisIndicator::GetInstance()->SetTargetViewProjection(&debugCamera_->GetViewProjection());

	// ライン描画が参照するビュープロジェクションを指定する（アドレス無し）
	PrimitiveDrawer::GetInstance()->SetViewProjection(&debugCamera_->GetViewProjection());

	//// スケール

	//// X, Y, Z方向のスケーリングを設定
	//worldTransform_.scale_ = { 1.0f, 5.0f, 1.0f };

	//// スケーリング行列を宣言
	//Matrix4 matScale;

	//// スケーリング倍率を行列に設定する
	//matScale.m[0][0] = worldTransform_.scale_.x;
	//matScale.m[1][1] = worldTransform_.scale_.y;
	//matScale.m[2][2] = worldTransform_.scale_.z;
	//matScale.m[3][3] = 1.0f;

	//// 単位行列を代入
	//SubIdentityMatrix(&worldTransform_.matWorld_);

	//// 掛け算
	//worldTransform_.matWorld_ *= matScale;

	// 回転

	// X, Y, Z軸周りの回転角を設定
	worldTransform_.rotation_ = { 0.0f, 0.0f, RadianChange(45.0f)};

	// Z軸回転行列を宣言
	Matrix4 matRotZ;

	// Z軸回転行列の各要素を設定する
	matRotZ.m[0][0] = cos(worldTransform_.rotation_.z);
	matRotZ.m[0][1] = sin(worldTransform_.rotation_.z);
	matRotZ.m[1][0] = -sin(worldTransform_.rotation_.z);
	matRotZ.m[1][1] = cos(worldTransform_.rotation_.z);
	matRotZ.m[2][2] = 1.0f;
	matRotZ.m[3][3] = 1.0f;

	// 単位行列を代入
	SubIdentityMatrix(&worldTransform_.matWorld_);

	// 掛け算
	worldTransform_.matWorld_ *= matRotZ;

	// 行列の転送
	worldTransform_.TransferMatrix();
}

void GameScene::Update() {

	// デバックカメラの更新
	debugCamera_->Update();

	debugText_->SetPos(50, 50);
	debugText_->Printf("[0]:(%f, %f, %f, %f)", worldTransform_.matWorld_.m[0][0], worldTransform_.matWorld_.m[0][1], worldTransform_.matWorld_.m[0][2], worldTransform_.matWorld_.m[0][3]);

	debugText_->SetPos(50, 70);
	debugText_->Printf("[1]:(%f, %f, %f, %f)", worldTransform_.matWorld_.m[1][0], worldTransform_.matWorld_.m[1][1], worldTransform_.matWorld_.m[1][2], worldTransform_.matWorld_.m[1][3]);

	debugText_->SetPos(50, 90);
	debugText_->Printf("[2]:(%f, %f, %f, %f)", worldTransform_.matWorld_.m[2][0], worldTransform_.matWorld_.m[2][1], worldTransform_.matWorld_.m[2][2], worldTransform_.matWorld_.m[2][3]);

	debugText_->SetPos(50, 110);
	debugText_->Printf("[3]:(%f, %f, %f, %f)", worldTransform_.matWorld_.m[3][0], worldTransform_.matWorld_.m[3][1], worldTransform_.matWorld_.m[3][2], worldTransform_.matWorld_.m[3][3]);
}

void GameScene::Draw() {

	// コマンドリストの取得
	ID3D12GraphicsCommandList* commandList = dxCommon_->GetCommandList();

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
	
	// 3Dモデル追加
	model_->Draw(worldTransform_, viewProjection_, textureHandle_);

	model_->Draw(worldTransform_, debugCamera_->GetViewProjection(), textureHandle_);
	
	// ライン描画が参照するビュープロジェクションを指定する（アドレス無し）

	// Z軸(奥行き)方向のライン描画(複数)
	for (size_t i = 0; i < 21; i++)
	{
		PrimitiveDrawer::GetInstance()->DrawLine3d(Vector3(10.0f - i, 0.0f, 10.0f), Vector3(10.0f - i, 0.0f, -10.0f), Vector4(0.0f, 0.0f, 1.0f, 1.0f));
	}

	// X軸(横)方向のライン描画(複数)
	for (size_t i = 0; i < 21; i++)
	{
		PrimitiveDrawer::GetInstance()->DrawLine3d(Vector3(10.0f, 0.0f, 10.0f - i), Vector3(-10.0f, 0.0f, 10.0f - i), Vector4(1.0f, 0.0f, 0.0f, 1.0f));
	}

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
