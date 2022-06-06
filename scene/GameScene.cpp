#include "GameScene.h"
#include "TextureManager.h"
#include <cassert>
#include "AxisIndicator.h"
#include "PrimitiveDrawer.h"
#include "XMFLOAT3.h"
#include <random>

GameScene::GameScene() {}

GameScene::~GameScene()
{
	delete model_;
	delete sprite_[0];
	delete sprite_[1];
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
	textureHandle_[0] = TextureManager::Load("mario.jpg");
	textureHandle_[1] = TextureManager::Load("reticle.png");
	textureHandle_[2] = TextureManager::Load("scope.png");

	// 3Dモデルの生成
	model_ = Model::Create();

	// スプライト生成
	sprite_[0] = Sprite::Create(textureHandle_[1], { WinApp::kWindowWidth / 2.0f - 64, WinApp::kWindowHeight / 2.0f - 64 });
	sprite_[1] = Sprite::Create(textureHandle_[2], { 0, 0 });

	// デバックカメラの生成
	debugCamera_ = new DebugCamera(WinApp::kWindowWidth, WinApp::kWindowHeight);

	// 軸方向表示の表示を有効にする
	AxisIndicator::GetInstance()->SetVisible(true);

	// 軸方向表示が参照するビュープロジェクションを指定する（アドレス無し）
	AxisIndicator::GetInstance()->SetTargetViewProjection(&viewProjection_);

	// ライン描画が参照するビュープロジェクションを指定する（アドレス無し）
	PrimitiveDrawer::GetInstance()->SetViewProjection(&viewProjection_);

	// 親の設定
	worldTransforms_[0].Initialize();

	// 子の設定

	// (1)
	worldTransforms_[1].translation_ = { -2.0f, 0.0f, -2.0f };
	worldTransforms_[1].parent_ = &worldTransforms_[0];
	worldTransforms_[1].Initialize();

	// (2)
	worldTransforms_[2].translation_ = { 0.0f, 0.0f, -2.0f };
	worldTransforms_[2].parent_ = &worldTransforms_[0];
	worldTransforms_[2].Initialize();

	// (3)
	worldTransforms_[3].translation_ = { 2.0f, 0.0f, -2.0f };
	worldTransforms_[3].parent_ = &worldTransforms_[0];
	worldTransforms_[3].Initialize();

	// マップ内に置くオブジェクトの設定
	worldTransforms_[4].translation_ = { 4.0f, 0.0f, 0.0f };
	worldTransforms_[4].Initialize();
	worldTransforms_[5].translation_ = { -4.0f, 0.0f, 0.0f };
	worldTransforms_[5].Initialize();

	// カメラ視点座標を設定
	viewProjection_.eye = { 0, 10, -15 };

	// ビュープロジェクションの初期化
	viewProjection_.Initialize();
}

void GameScene::Update()
{

	// デバックカメラの更新
	debugCamera_->Update();

	// 操作のモード切替
	if (input_->TriggerKey(DIK_Q))
	{
		if (controlMode == MODE1) controlMode = MODE2;
		else controlMode = MODE1;
	}

	// --バイオ歩き-- //
	if (controlMode == MODE1) {
		// 前方ベクトル
		XMFLOAT3 forwardVec = { 0, 0, 1 };

		XMFLOAT3 backwardVec = { 0, 0, -1 };

		// 計算結果
		XMFLOAT3 resultVec = { 0, 0, 0 };

		// 移動ベクトル
		XMFLOAT3 move = { 0, 0, 0 };

		// 右キー入力で右回転
		if (input_->PushKey(DIK_RIGHT))
		{
			worldTransforms_[0].rotation_.y += 0.1f;
		}

		// 左キー入力で左回転
		if (input_->PushKey(DIK_LEFT))
		{
			worldTransforms_[0].rotation_.y -= 0.1f;
		}

		resultVec.x = cosf(worldTransforms_[0].rotation_.y) * forwardVec.x +
			sinf(worldTransforms_[0].rotation_.y) * forwardVec.z;
		resultVec.z = -sinf(worldTransforms_[0].rotation_.y) * forwardVec.x +
			cosf(worldTransforms_[0].rotation_.y) * forwardVec.z;

		// 上キー入力で前方に移動
		if (input_->PushKey(DIK_UP))
		{
			move = { resultVec.x, 0, resultVec.z };
		}

		// 下キー入力で前方に移動
		if (input_->PushKey(DIK_DOWN))
		{
			move = { -resultVec.x, 0, -resultVec.z };
		}

		worldTransforms_[0].translation_.x += move.x * 0.2f;
		worldTransforms_[0].translation_.y += move.y * 0.2f;
		worldTransforms_[0].translation_.z += move.z * 0.2f;

		viewProjection_.eye.x = worldTransforms_[0].translation_.x - resultVec.x * 10.0f;
		viewProjection_.eye.y = 10.0f;
		viewProjection_.eye.z = worldTransforms_[0].translation_.z - resultVec.z * 10.0f;

		viewProjection_.target = worldTransforms_[0].translation_;
	}

	// カメラ視点の前後左右移動
	if (controlMode == MODE2) {
		// 正面ベクトル
		Vector3 forwardVec = {viewProjection_.target.x - viewProjection_.eye.x, 0.0f, viewProjection_.target.z - viewProjection_.eye.z};

		// 正規化
		forwardVec = forwardVec.normalize();

		// 右ベクトル
		Vector3 rightVec = Vector3(0, 1, 0).cross(forwardVec);

		// 正規化
		rightVec = rightVec.normalize();

		// 動く速度
		float moveSpeed = 0.5;

		// 結果加算する値
		Vector3 resultVec{};

		// 前後移動
		if (input_->PushKey(DIK_W))
		{
			resultVec += forwardVec * moveSpeed;
		}
		else if (input_->PushKey(DIK_S))
		{
			resultVec += -forwardVec * moveSpeed;
		}

		// 左右移動
		if (input_->PushKey(DIK_D))
		{
			resultVec += rightVec * moveSpeed;
		}
		else if (input_->PushKey(DIK_A))
		{
			resultVec += -rightVec * moveSpeed;
		}

		worldTransforms_[0].translation_ += resultVec;

		resultVec = { 0, 0, 0 };

		// カメラの前後移動
		if (input_->PushKey(DIK_UP))
		{
			resultVec += forwardVec * moveSpeed;
		}
		else if (input_->PushKey(DIK_DOWN))
		{
			resultVec += -forwardVec * moveSpeed;
		}

		// カメラの左右移動
		if (input_->PushKey(DIK_RIGHT))
		{
			resultVec += rightVec * moveSpeed;
		}
		else if (input_->PushKey(DIK_LEFT))
		{
			resultVec += -rightVec * moveSpeed;
		}

		viewProjection_.eye += resultVec;
		viewProjection_.target += resultVec;
	}

	for (size_t i = 0; i < _countof(worldTransforms_); i++)
	{
		// 行列の再計算
		worldTransforms_[i].UpdateMatrix();
	}

	// 行列の再計算
	viewProjection_.UpdateMatrix();

	// デバッグ用表示
	//debugText_->SetPos(50, 50);
	//debugText_->Printf("Qキーでモード切替");
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

	// 3Dモデル追加

	for (size_t i = 0; i < _countof(worldTransforms_); i++)
	{
		model_->Draw(worldTransforms_[i], viewProjection_, textureHandle_[0]);
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

	// スプライト描画
	if (isScopeMode) sprite_[0]->Draw(), sprite_[1]->Draw();

	// デバッグテキストの描画
	debugText_->DrawAll(commandList);
	//
	// スプライト描画後処理
	Sprite::PostDraw();

#pragma endregion
}
