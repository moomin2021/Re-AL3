#include "GameScene.h"
#include "TextureManager.h"
#include <cassert>
#include "AxisIndicator.h"
#include "PrimitiveDrawer.h"
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
	matrix = new Matrix;

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

	//for (WorldTransform & worldTransform : worldTransforms_)
	//{
	//	// ワールドトランスフォームの初期化
	//	worldTransform.Initialize();

	//	// X, Y, Z方向のスケーリングを設定
	//	worldTransform.matWorld_ *= Matrix::SetScale(1.0f, 1.0f, 1.0f);

	//	// X, Y, Z軸周りの回転角を乱数で設定
	//	worldTransform.matWorld_ *= Matrix::SetRot(rotDist(engine), rotDist(engine), rotDist(engine));

	//	// X, Y, Z軸周りの平行移動（座標）を乱数で設定
	//	worldTransform.matWorld_ *= Matrix::SetTrans(dist(engine), dist(engine), dist(engine));

	//	// 行列の転送
	//	worldTransform.TransferMatrix();
	//}

	// キャラクターの大元を初期化
	worldTransforms_[PartId::kRoot].Initialize();

	// 脊椎の初期化と設定
	worldTransforms_[PartId::kSpine].translation_ = { 0, 0, 0 };// --------------> 脊椎のローカル座標を設定
	worldTransforms_[PartId::kSpine].parent_ = &worldTransforms_[PartId::kRoot];// -> 脊椎の親を大元に設定
	worldTransforms_[PartId::kSpine].Initialize();// -----------------------------> 脊椎を初期化

	// -上半身の初期化と設定- //

	// 胸
	worldTransforms_[PartId::kChest].translation_ = { 0, 0, 0 };// -------------------> 胸のローカル座標を設定
	worldTransforms_[PartId::kChest].parent_ = &worldTransforms_[PartId::kSpine];// --> 胸の親を脊椎に設定
	worldTransforms_[PartId::kChest].Initialize();// -------------------------------> 胸を初期化

	// 頭
	worldTransforms_[PartId::kHead].translation_ = { 0, 3, 0 };// ------------------> 頭のローカル座標を設定
	worldTransforms_[PartId::kHead].parent_ = &worldTransforms_[PartId::kChest];// -> 頭の親を胸に設定
	worldTransforms_[PartId::kHead].Initialize();// ------------------------------> 頭の初期化

	// 左腕
	worldTransforms_[PartId::kArmL].translation_ = { -3, 0, 0 };// -----------------> 左腕のローカル座標を設定
	worldTransforms_[PartId::kArmL].parent_ = &worldTransforms_[PartId::kChest];// -> 左腕の親を胸に設定
	worldTransforms_[PartId::kArmL].Initialize();// ------------------------------> 左腕の初期化

	// 右腕
	worldTransforms_[PartId::kArmR].translation_ = { 3, 0, 0 };// -----------------> 右腕のローカル座標を設定
	worldTransforms_[PartId::kArmR].parent_ = &worldTransforms_[PartId::kChest];// -> 右腕の親を胸に設定
	worldTransforms_[PartId::kArmR].Initialize();// -----------------------------> 右腕の初期化

	// -下半身の初期化- //

	// 尻
	worldTransforms_[PartId::kHip].translation_ = { 0, -3, 0 };// -----------------> 尻のローカル座標を設定
	worldTransforms_[PartId::kHip].parent_ = &worldTransforms_[PartId::kSpine];// -> 尻の親を脊椎に設定
	worldTransforms_[PartId::kHip].Initialize();// ------------------------------> 尻の初期化

	// 左足
	worldTransforms_[PartId::kLegL].translation_ = { -3, -3, 0 };// --------------> 左足のローカル座標を設定
	worldTransforms_[PartId::kLegL].parent_ = &worldTransforms_[PartId::kHip];// -> 左足の親を尻に設定
	worldTransforms_[PartId::kLegL].Initialize();// ----------------------------> 左足の初期化

	// 右足
	worldTransforms_[PartId::kLegR].translation_ = { 3, -3, 0 };// ---------------> 右足のローカル座標を設定
	worldTransforms_[PartId::kLegR].parent_ = &worldTransforms_[PartId::kHip];// -> 右足の親を尻に設定
	worldTransforms_[PartId::kLegR].Initialize();// ----------------------------> 右足の初期化

	// ビュープロジェクションの初期化
	viewProjection_.Initialize();
}

void GameScene::Update()
{

	// デバックカメラの更新
	debugCamera_->Update();

	// キャラクターの移動処理
	{
		// キャラクターの移動ベクトル
		Vector3 move = { 0, 0, 0 };

		// 左キーを押すと値を減算
		if (input_->PushKey(DIK_LEFT))
		{
			move.x -= 0.1f;
		}

		// 右キーを押すと値を加算
		else if (input_->PushKey(DIK_RIGHT))
		{
			move.x += 0.1f;
		}

		worldTransforms_[PartId::kRoot].translation_ += move;
		matrix->UpdateMatrix(worldTransforms_[PartId::kRoot]);

	}

	// 上半身回転処理
	{
		// 押した方向で移動ベクトルを変更
		if (input_->PushKey(DIK_U))
		{
			worldTransforms_[PartId::kChest].rotation_.y -= 0.1f;
		}
		else if (input_->PushKey(DIK_I))
		{
			worldTransforms_[PartId::kChest].rotation_.y += 0.1f;
		}
	}

	// 下半身回転処理
	{
		// 押した方向で移動ベクトルを変更
		if (input_->PushKey(DIK_J))
		{
			worldTransforms_[PartId::kHip].rotation_.y -= 0.1f;
		}
		else if (input_->PushKey(DIK_K))
		{
			worldTransforms_[PartId::kHip].rotation_.y += 0.1f;
		}
	}

	// 子の計算
	for (size_t i = 1;i <= PartId::kLegR;i++)
	{
		matrix->UpdateMatrix(worldTransforms_[i]);
		worldTransforms_[i].matWorld_ *= worldTransforms_[i].parent_->matWorld_;
		worldTransforms_[i].TransferMatrix();
	}

	// 行列の再計算
	viewProjection_.UpdateMatrix();

	// デバッグ用表示
	debugText_->SetPos(50, 50);
	debugText_->Printf("translation:(%f, %f, %f)", worldTransforms_[0].matWorld_.m[3][0], worldTransforms_[0].matWorld_.m[3][1], worldTransforms_[0].matWorld_.m[3][2]);

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

	//for (WorldTransform & worldTransform : worldTransforms_)
	//{
	//	model_->Draw(worldTransform, viewProjection_, textureHandle_);
	//}

	for (size_t i = 2; i <= PartId::kLegR; i++)
	{
		model_->Draw(worldTransforms_[i], viewProjection_, textureHandle_);
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
