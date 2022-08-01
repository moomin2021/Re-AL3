#include "GameScene.h"
#include "TextureManager.h"
#include <cassert>
#include "AxisIndicator.h"
#include "PrimitiveDrawer.h"
#include <random>

float Abs(float value) {
	if (value < 0) return value *= -1;
	return value;
}

bool RayCirCollision(Vector3 ray, Vector3 cir) {
	Vector3 cross = ray.normalize().cross(cir);
	float len = ray.length();
	float dot = ray.normalize().dot(cir);
	if (0 <= dot && dot <= len) return true;
	return 0;
}

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

	// --敵-- //
	enemy_.translation_ = { 0.0f, 0.0f, 10.0f };
	enemy_.Initialize();

	// --Ray-- //
	ray_[0].translation_ = { 0.0f, 0.0f, -20.0f };
	ray_[0].Initialize();

	ray_[1].parent_ = &ray_[0];
	ray_[1].translation_ = { 0.0f, 0.0f, 15.0f };
	ray_[1].scale_ = { 0.1f, 0.1f, 15.0f };
	ray_[1].Initialize();

	// --当たり判定可視化用-- //
	objectCol.translation_ = { 0.0f, -10.0f, 0.0f };
	objectCol.scale_ = { 10.0f, 0.1f, 10.0f };
	objectCol.Initialize();

	// --カメラの視点座標の設定-- //
	viewProjection_.eye = { 20.0f, 10.0f, -20.0f };

	// --カメラの注視点を設定-- //
	viewProjection_.target = { 0.0f, 0.0f, 0.0f };

	// --カメラの垂直方向視野角の設定-- //
	viewProjection_.fovAngleY = MathUtility::Degree2Radian(45.0f);

	// --ビュープロジェクションの初期化-- //
	viewProjection_.Initialize();
}

void GameScene::Update()
{
	// --敵の更新処理-- //
	enemy_.UpdateMatrix();

	Vector3 ray = { 0.0f, 0.0f, 30.0f };

	// --Rayの始点から敵(円)の中心点までのベクトル
	Vector3 vecAP = enemy_.translation_ - ray_[0].translation_;

	// --Rayの終点から敵(円)の中心点までのベクトル
	Vector3 vecBP = enemy_.translation_ - (ray_[0].translation_ + ray);

	// --敵(円)の中心から線分への最短距離PX
	Vector3 crossPX = ray.cross(vecAP) / ray.length();

	// --Rayの始点から最短距離PXの長さ
	float dotAX = ray.dot(vecAP) / ray.length();

	// --最短距離PXの長さ
	float len = Abs(crossPX.length());

	// --PXが最短ではなかった場合
	if (dotAX < 0) {
		len = vecAP.length();
	}
	else if (dotAX > ray.length()) {
		len = vecBP.length();
	}

	col = len < 1.0f;

	// --Ray移動処理-- //
	{
		// --Rayの移動量
		Vector3 move{};

		// --Rayの移動速度
		float speed = 0.2f;

		if (input_->PushKey(DIK_LSHIFT)) speed = 0.1f;

		move.x += (input_->PushKey(DIK_D) - input_->PushKey(DIK_A)) * speed;
		move.y += (input_->PushKey(DIK_W) - input_->PushKey(DIK_S)) * speed;
		move.z += (input_->PushKey(DIK_UP) - input_->PushKey(DIK_DOWN)) * speed;

		ray_[0].translation_+= move;
	}

	// --行列再計算-- //
	ray_[0].UpdateMatrix();
	ray_[1].UpdateMatrix();

	objectCol.UpdateMatrix();

	// --行列の再計算-- //
	viewProjection_.UpdateMatrix();

	// --デバッグ用表示-- //
	//debugText_->SetPos(50, 50);
	//debugText_->Printf("vecAP:(%f, %f, %f)", vecAP.x, vecAP.y, vecAP.z);

	//debugText_->SetPos(50, 70);
	//debugText_->Printf("vecBP:(%f, %f, %f)", vecBP.x, vecBP.y, vecBP.z);

	//debugText_->SetPos(50, 90);
	//debugText_->Printf("dotAX:%f", dotAX);

	//debugText_->SetPos(50, 110);
	//debugText_->Printf("crossPX:(%f, %f, %f)", crossPX.x, crossPX.y, crossPX.z);

	//debugText_->SetPos(50, 130);
	//debugText_->Printf("len:%f", len);

	debugText_->SetPos(780, 250);
	debugText_->Printf("Hit:%d", col);
}

void GameScene::Draw()
{

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

	// --3Dモデル追加-- //
	model_->Draw(enemy_, viewProjection_, textureHandle_);
	//model_->Draw(ray_[0], viewProjection_);
	model_->Draw(ray_[1], viewProjection_);

	if (col) model_->Draw(objectCol, viewProjection_, textureHandle_);

	// --デバックカメラ
	//model_->Draw(worldTransform_, debugCamera_->GetViewProjection(), textureHandle_);

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
