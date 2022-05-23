#include "GameScene.h"
#include "TextureManager.h"
#include <cassert>
#include "AxisIndicator.h"
#include "PrimitiveDrawer.h"
#include "Matrix.h"
#include <random>

float PI = 3.141592f;

float RadianChange(float n)
{
	return n * (PI / 180.0f);
}

float AngleChange(float n)
{
	return 180.0f / PI * n;
}

float Minimum(float n, float max)
{
	if (n > max) return max;
	return n;
}

float Maximum(float n, float min)
{
	if (n < min) return min;
	return n;
}

GameScene::GameScene() {}

GameScene::~GameScene()
{
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

	std::uniform_real_distribution<float> rotDist(0.0f, PI * 2.0f);

	// ファイル名を指定してテクスチャを読み込む
	textureHandle_ = TextureManager::Load("mario.jpg");

	// 3Dモデルの生成
	model_ = Model::Create();

	// デバックカメラの生成
	debugCamera_ = new DebugCamera(WinApp::kWindowWidth, WinApp::kWindowHeight);

	// 軸方向表示の表示を有効にする
	AxisIndicator::GetInstance()->SetVisible(true);

	// 軸方向表示が参照するビュープロジェクションを指定する（アドレス無し）
	AxisIndicator::GetInstance()->SetTargetViewProjection(&viewProjection_);

	// ライン描画が参照するビュープロジェクションを指定する（アドレス無し）
	PrimitiveDrawer::GetInstance()->SetViewProjection(&viewProjection_);

	for (WorldTransform & worldTransform : worldTransforms_)
	{
		// ワールドトランスフォームの初期化
		worldTransform.Initialize();

		// X, Y, Z方向のスケーリングを設定
		worldTransform.matWorld_ *= Matrix::SetScale(1.0f, 1.0f, 1.0f);

		// X, Y, Z軸周りの回転角を乱数で設定
		worldTransform.matWorld_ *= Matrix::SetRot(rotDist(engine), rotDist(engine), rotDist(engine));

		// X, Y, Z軸周りの平行移動（座標）を乱数で設定
		worldTransform.matWorld_ *= Matrix::SetTrans(dist(engine), dist(engine), dist(engine));

		// 行列の転送
		worldTransform.TransferMatrix();
	}

	// カメラ垂直方向視野角を設定
	viewProjection_.fovAngleY = RadianChange(10.0f);

	// アスペクト比を設定
	//viewProjection_.aspectRatio = 1.0f;

	// ニアクリップ距離を設定
	viewProjection_.nearZ = 52.0f;

	// ファークリップ距離を設定
	viewProjection_.farZ = 53.0f;

	// ビュープロジェクションの初期化
	viewProjection_.Initialize();
}

void GameScene::Update()
{

	// デバックカメラの更新
	debugCamera_->Update();


	//// 視点移動処理
	//{
	//	// 視点の移動ベクトル
	//	Vector3 move = { 0, 0, 0 };

	//	// 視点の移動速さ
	//	const float kEyeSpeed = 0.2f;

	//	// 押した方向で有働ベクトルを変更
	//	if (input_->PushKey(DIK_W))
	//	{
	//		move.z += kEyeSpeed;
	//	}
	//	else if (input_->PushKey(DIK_S))
	//	{
	//		move.z -= kEyeSpeed;
	//	}

	//	// 視点移動（ベクトルの加算）
	//	viewProjection_.eye += move;
	//}

	//// 注視点移動処理
	//{
	//	// 注視点移動ベクトル
	//	Vector3 move = { 0, 0, 0 };

	//	// 注視点の移動速さ
	//	const float kTargetSpeed = 0.2f;

	//	// 押した方向で移動ベクトルを変更
	//	if (input_->PushKey(DIK_LEFT))
	//	{
	//		move.x -= kTargetSpeed;
	//	}
	//	else if (input_->PushKey(DIK_RIGHT))
	//	{
	//		move.x += kTargetSpeed;
	//	}

	//	// 注視点移動（ベクトルの加算）
	//	viewProjection_.target += move;
	//}

	//// 上方向回転処理
	//{
	//	// 上方向の回転速さ[ラジアン/frame]
	//	const float kUpRotSpeed = 0.05f;

	//	// 押した方向で移動ベクトルを変更
	//	if (input_->PushKey(DIK_SPACE))
	//	{
	//		viewAngle += kUpRotSpeed;
	//		// 2πを超えたら0に戻す
			//viewAngle = fmodf(viewAngle, PI * 2.0f);
	//	}

	//	// 上方向ベクトルを計算（半径1の円周上の座標）
	//	viewProjection_.up = { cosf(viewAngle), sinf(viewAngle), 0.0f };
	//}

	//// Fov変更処理
	//{
	//	// 上キーで視野角が広がる
	//	if (input_->PushKey(DIK_UP))
	//	{
	//		viewProjection_.fovAngleY += 0.1f;
	//		// πを上回らないように
	//		viewProjection_.fovAngleY = Minimum(viewProjection_.fovAngleY, PI);
	//	}

	//	// 下キーで視野角が狭まる
	//	if (input_->PushKey(DIK_DOWN))
	//	{
	//		viewProjection_.fovAngleY -= 0.1f;
	//		// 0.01fを下回らないように
	//		viewProjection_.fovAngleY = Maximum(viewProjection_.fovAngleY, 0.01f);
	//	}
	//}

	// クリップ距離変更処理
	{
		// 上下キーでニアクリップ距離を増減
		if (input_->PushKey(DIK_UP))
		{
			viewProjection_.nearZ += 0.1f;
		}
		else if (input_->PushKey(DIK_DOWN))
		{
			viewProjection_.nearZ -= 0.1f;
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
	debugText_->Printf("fovAngleY(Degree):%f", AngleChange(viewProjection_.fovAngleY));

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
	//model_->Draw(worldTransform_, viewProjection_, textureHandle_);

	for (WorldTransform & worldTransform : worldTransforms_)
	{
		model_->Draw(worldTransform, viewProjection_, textureHandle_);
	}

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
