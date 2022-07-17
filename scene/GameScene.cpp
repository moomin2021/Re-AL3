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
	delete player_;
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

	// --カメラの視点座標の設定-- //
	viewProjection_.eye = { 0.0f, 0.0f, -50.0f };

	// --カメラの注視点を設定-- //
	viewProjection_.target = { 0.0f, 0.0f, 0.0f };

	// --カメラの垂直方向視野角の設定-- //
	viewProjection_.fovAngleY = MathUtility::Degree2Radian(45.0f);

	// --ビュープロジェクションの初期化-- //
	viewProjection_.Initialize();

	// --プレイヤーの生成-- //
	player_ = new Player();

	// --プレイヤーの初期化-- //
	player_->Initialize(model_, textureHandle_);
}

void GameScene::Update()
{

	// --デバックカメラ有効化-- //
#ifdef _DEBUG
	if (input_->TriggerKey(DIK_B)) {
		isDebugCameraActive_ = true;
	}
#endif

	// --プレイヤーの更新処理-- //
	player_->Update();

	// --カメラ処理-- //
	if (isDebugCameraActive_) {
		
		// --デバックカメラの更新
		debugCamera_->Update();

		// --デバックカメラビュー行列
		viewProjection_.matView = debugCamera_->GetViewProjection().matView;

		// --デバックカメラプロジェクション行列
		viewProjection_.matProjection = debugCamera_->GetViewProjection().matProjection;

		// --ビュープロジェクション行列の再計算と転送
		viewProjection_.UpdateMatrix();
		viewProjection_.TransferMatrix();
	}
	else {
		// --行列の再計算-- //
		viewProjection_.UpdateMatrix();
	}


	// --デバッグ用表示-- //
	debugText_->SetPos(50, 50);
	debugText_->Printf("eye:(%f, %f, %f)", viewProjection_.eye.x, viewProjection_.eye.y, viewProjection_.eye.z);

	debugText_->SetPos(50, 50);
	debugText_->Printf("DebugCameraActive:%d", isDebugCameraActive_);
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

	// --プレイヤーの描画処理-- //
	player_->Draw(viewProjection_);

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
