#include "GameScene.h"
#include "TextureManager.h"
#include <cassert>
#include "AxisIndicator.h"
#include "PrimitiveDrawer.h"
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

	// オブジェクト間の間隔
	const int objectSpace = 5;

	// オブジェクトを配置する半径
	const int radius = 20;

	// オブジェクト初期化
	for (size_t i = 0; i < _countof(worldTransforms_); i++)
	{
		// オブジェクトの位置を指定
		worldTransforms_[i].translation_ = { dist(engine), dist(engine), dist(engine) };

		// オブジェクトの回転を指定
		worldTransforms_[i].rotation_ = { rotDist(engine), rotDist(engine), rotDist(engine) };

		// オブジェクト初期化
		worldTransforms_[i].Initialize();
	}

	// 垂直方向視野角を設定
	viewProjection_.fovAngleY = MathUtility::Degree2Radian(90.0f);

	// ビュープロジェクションの初期化
	viewProjection_.Initialize();
}

void GameScene::Update()
{

	// デバックカメラの更新
	debugCamera_->Update();

	for (size_t i = 0; i < _countof(worldTransforms_); i++)
	{
		// 行列の再計算
		worldTransforms_[i].UpdateMatrix();
	}

	// 注視点の移動
	{
		// 視点の移動ベクトル
		Vector3 move = { 0, 0, 0 };

		// 視点の移動速度
		const float targetSpeed = 0.2f;

		// 視点の横移動計算
		move.x = ((input_->PushKey(DIK_RIGHT)) - (input_->PushKey(DIK_LEFT))) * targetSpeed;

		// 視点の縦移動計算
		move.y = ((input_->PushKey(DIK_UP)) - (input_->PushKey(DIK_DOWN))) * targetSpeed;

		// ベクトルの加算
		viewProjection_.target += move;

		// 視点を制限
		viewProjection_.target.x = MathUtility::Clamp(viewProjection_.target.x, 10.0f, -10.0f);
		viewProjection_.target.y = MathUtility::Clamp(viewProjection_.target.y, 10.0f, -10.0f);
	}

	// スコープモード
	{
		// スペースを押した場合
		if (input_->TriggerKey(DIK_SPACE))
		{
			// スコープモードが真だったらスコープモードを偽にする
			if (isScopeMode) isScopeMode = false, fovAngle = 90.0f, viewProjection_.fovAngleY = MathUtility::Degree2Radian(fovAngle);

			// スコープモードが偽だったらスコープモードを真にする
			else isScopeMode = true, fovAngle = 30.0f, viewProjection_.fovAngleY = MathUtility::Degree2Radian(fovAngle);
		}

		// 視野角が変わるスピード
		float fovSpeed = 2.0f;

		// スコープモードが真だった場合
		if (isScopeMode)
		{
			// Wキーが押されたら
			if (input_->PushKey(DIK_W))
			{
				scopeMagnification = 8;
			}

			// Sキーが押されたら
			else if (input_->PushKey(DIK_S))
			{
				scopeMagnification = 4;
			}

			if (scopeMagnification == 4)
			{
				// 垂直方向視野角を増加させる
				fovAngle += fovSpeed;

				// 一定の値からは値が減らないようにする
				fovAngle = MathUtility::Clamp(fovAngle, 30.0f, 15.0f);

				// 垂直方向視野角の値を設定する
				viewProjection_.fovAngleY = MathUtility::Degree2Radian(fovAngle);
			}

			if (scopeMagnification == 8)
			{
				// 垂直方向視野角を増加させる
				fovAngle -= fovSpeed;

				// 一定の値からは値が減らないようにする
				fovAngle = MathUtility::Clamp(fovAngle, 30.0f, 15.0f);

				// 垂直方向視野角の値を設定する
				viewProjection_.fovAngleY = MathUtility::Degree2Radian(fovAngle);
			}
		}
	}

	// 行列の再計算
	viewProjection_.UpdateMatrix();

	// デバッグ用表示
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
