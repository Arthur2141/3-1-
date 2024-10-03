#include "Graphics/Graphics.h"
#include "SceneLoading.h"
#include "SceneManager.h"
#include "SceneGame07.h"

#include "Camera.h"
#include "EnemyManager.h"
#include "EnemySlime.h"
#include "EffectManager.h"
#include "Input/Input.h"
#include "StageManager.h"
#include "StageMain.h"
#include "StageMoveFloor.h"

// 初期化
void SceneGame07::Initialize()
{
	// ステージ初期化
	StageManager& stageManager = StageManager::Instance();
	StageMain* stageMain = new StageMain();
	stageManager.Register(stageMain);

	StageMoveFloor* stageMoveFloor = new StageMoveFloor();
	stageMoveFloor->SetStartPoint(DirectX::XMFLOAT3(0, 1, 3));
	stageMoveFloor->SetGoalPoint(DirectX::XMFLOAT3(10, 2, 3));
	stageMoveFloor->SetTorque(DirectX::XMFLOAT3(0, 1.0f, 0));
	stageManager.Register(stageMoveFloor);

	// プレイヤー
	player = new Player();

	// カメラ初期設定
	Graphics& graphics = Graphics::Instance();
	Camera& camera = Camera::Instance();
	camera.SetLookAt(
		DirectX::XMFLOAT3(0, 0, -10),
		DirectX::XMFLOAT3(0, 0, 0),
		DirectX::XMFLOAT3(0, 1, 0)
	);
	camera.SetPerspectiveFov(
		DirectX::XMConvertToRadians(45),
		graphics.GetScreenWidth() / graphics.GetScreenHeight(),
		0.1f,
		1000.0f
	);

	// カメラコントローラー初期化
	cameraController = new CameraController();

	// エネミー初期化
	EnemyManager& enemyManager = EnemyManager::Instance();
	for (int i = 0; i < 5; ++i)
	{
		EnemySlime* slime = new EnemySlime();
		slime->SetPosition(DirectX::XMFLOAT3(i * 2.0f, 0, 5));
		slime->SetTerritory(slime->GetPosition(), 10.0f);
		enemyManager.Register(slime);
	}
	// フレームバッファ
	shadowMap = std::make_unique<RenderTarget>(SHADOW_SIZE.x, SHADOW_SIZE.y, 6,DXGI_FORMAT_R32G32_FLOAT);
	//reflectMap = std::make_unique<FrameBuffer>
	//	(REFLECT_SIZE.x, REFLECT_SIZE.y, DXGI_FORMAT_R8G8B8A8_UNORM);

	//sky = std::make_unique<Sky>();
	//sky->SetScale({10,10,10});
	//skybox = std::make_unique<SkyBoxSprite>("Data/Model/SkyBox/sunflowers.jpg");
	//skybox = std::make_unique<SkyBox>("Data/SkyBox/earth.jpg");

	//skybox = std::make_unique<SkyBox>("Data/SkyBox/field20220706.jpg");
	//skybox = std::make_unique<SkyBox>("Data/SkyBox/sky.png");
	//skybox = std::make_unique<SkyBox>("Data/SkyBox/sunflowers.jpg");
	//skybox = std::make_unique<SkyBox>("Data/SkyBox/sunSet20220706.jpg");
	//skybox = std::make_unique<SkyBox>("Data/SkyBox/torii20220706.jpg");
	//skybox = std::make_unique<SkyBox>("Data/SkyBox/earth.jpg");
	//skybox = std::make_unique<SkyBox>("Data/SkyBox/town20220706.jpg");
	//skybox = std::make_unique<SkyBox>("Data/SkyBox/beautiful_sky.jpg");


	waterSurface = std::make_unique<WaterSurface>();
	
	// ゲージスプライト
	gauge = new Sprite();

	earth = std::make_unique<Model>("Data/Model/Earth/earth.mdl");
	bomb_texture = std::make_unique<Texture>("Data/Effect/Texture/Explosion01_5x5.png");
	particle_bomb = std::make_unique<cParticleSystem>(bomb_texture->GetShaderResourceView().Get(), 5, 5, true, 3000);

	particle_texture = std::make_unique<Texture>("Data/Effect/Texture/particle256x256.png");
	particle_alpha = std::make_unique<cParticleSystem>(particle_texture->GetShaderResourceView().Get(), 4, 4, false, 10000);
	particle_add = std::make_unique<cParticleSystem>(particle_texture->GetShaderResourceView().Get(), 4, 4, false, 10000);
	particle_subtract = std::make_unique<cParticleSystem>(particle_texture->GetShaderResourceView().Get(), 4, 4, false, 10000);



}

// 終了化
void SceneGame07::Finalize()
{
	// ゲージスプライト終了化
	if (gauge != nullptr)
	{
		delete gauge;
		gauge = nullptr;
	}

	// エネミー終了化
	EnemyManager::Instance().Clear();

	// カメラコントローラー終了化
	if (cameraController != nullptr)
	{
		delete cameraController;
		cameraController = nullptr;
	}

	// プレイヤー終了化
	if (player != nullptr)
	{
		delete player;
		player = nullptr;
	}

	// ステージ終了化
	StageManager::Instance().Clear();
}

// 更新処理
void SceneGame07::Update(float elapsedTime)
{
	// カメラコントローラー更新処理
	DirectX::XMFLOAT3 target = player->GetPosition();
	target.y += 0.5f;
	cameraController->SetTarget(target);
	cameraController->Update(elapsedTime);

	// ステージ更新処理
	StageManager::Instance().Update(elapsedTime);
	waterSurface->Update(elapsedTime);
	//timer += elapsedTime;

	// プレイヤー更新処理
	player->Update(elapsedTime);

	////  空
	//sky->Update(elapsedTime);
	// エネミー更新処理
	EnemyManager::Instance().Update(elapsedTime);

	// エフェクト更新処理
	EffectManager::Instance().Update(elapsedTime);


	if (::GetKeyState('E') < 0)
	//if (::GetAsyncKeyState('E') & 1)
	{
		float angleY = player->GetAngle().y;
		DirectX::XMFLOAT3 dir;
		dir.x = sinf(angleY)*6.0f;
		dir.y = -2.0f;
		dir.z = cosf(angleY)*6.0f;
		DirectX::XMFLOAT3 pos = player->GetPosition();
		pos.y += 1.7f;

		for (int i = 0; i < 1; i++) {
			pos.x += dir.x*i*0.06f;
			pos.y += dir.y*i*0.06f;
			pos.z += dir.z*i*0.06f;
			particle_bomb->Set(
				0, 60.0f, true,
				DirectX::XMFLOAT3(
					pos.x + (rand() % 200 - 100) * 0.0015f,
					pos.y + (rand() % 200 - 100) * 0.0015f,
					pos.z + (rand() % 200 - 100) * 0.0015f),

				//dir,
				DirectX::XMFLOAT3(
					dir.x + (rand() % 200 - 100) * 0.001f,
					dir.y + (rand() % 200 - 100) * 0.001f,
					dir.z + (rand() % 200 - 100) * 0.001f),
				
				DirectX::XMFLOAT3(0, 0.5f, 0),
				DirectX::XMFLOAT2(2.0f, 2.0f)
			);
		}
	}



	if (::GetAsyncKeyState('Q') & 1)
	{
		particle_bomb->Set(
			0, 60.0f,true,
			DirectX::XMFLOAT3((rand() % 30 - 15) * 0.1f, rand() % 30 * 0.1f + 1, (rand() % 30 - 15) * 0.1f + 3),
			DirectX::XMFLOAT3(
				(rand() % 200 - 100) * 0.004f,
				(rand() % 100) * 0.01f,
				0),
			DirectX::XMFLOAT3(0, 0.5f, 0),
			DirectX::XMFLOAT2(4.0f, 4.0f)
		);
	}

	//particle_alpha->Snow(DirectX::XMFLOAT3(0, 5, 0), 20);

	//particle_add->Spark(DirectX::XMFLOAT3(2, 0, 0), 15);

	//particle_add->Fire(DirectX::XMFLOAT3(-2, 0, 0), 1);
	//particle_subtract->Smoke(DirectX::XMFLOAT3(-2, 0, 0), 5);



	particle_bomb->Update(elapsedTime);
	particle_alpha->Update(elapsedTime);
	particle_subtract->Update(elapsedTime);
	particle_add->Update(elapsedTime);



}

// 描画処理

void SceneGame07::RenderShadow()
{
	Graphics& graphics = Graphics::Instance();
	ID3D11DeviceContext* dc = graphics.GetDeviceContext();
	ID3D11RenderTargetView* rtv[1] = { shadowMap->GetRenderTargetView() };
	ID3D11DepthStencilView* dsv = shadowMap->GetDepthStencilView();

	// 画面クリア＆レンダーターゲット設定
	FLOAT color[] = { 0.0f, 0.0f, 0.5f, 1.0f };	// RGBA(0.0～1.0)
	dc->ClearRenderTargetView(rtv[0], color);
	dc->ClearDepthStencilView(dsv, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	dc->OMSetRenderTargets(1, rtv, dsv);

	// 描画処理
	RenderContext rc{};
	rc.lightDirection = lightDirection;	// ライト方向（下方向）
	DirectX::XMFLOAT3 viewPosition = player->GetPosition();
	viewPosition.x -= lightDirection.x * 30.0f;
	viewPosition.y -= lightDirection.y * 30.0f;
	viewPosition.z -= lightDirection.z * 30.0f;

	{// カメラパラメータ設定

		DirectX::XMVECTOR Eye = DirectX::XMLoadFloat3(&viewPosition);
		DirectX::XMVECTOR Focus = DirectX::XMLoadFloat3(&player->GetPosition());
		DirectX::XMVECTOR Up = DirectX::XMVectorSet(0, 1, 0, 0);
		DirectX::XMMATRIX View = DirectX::XMMatrixLookAtLH(Eye, Focus, Up);
		DirectX::XMFLOAT4X4 view;
		DirectX::XMStoreFloat4x4(&view, View);
		// 平行投影
		DirectX::XMMATRIX Projection = DirectX::XMMatrixOrthographicLH(SHADOW_AREA.x, SHADOW_AREA.y, 0.1f, 100.0f);
		DirectX::XMFLOAT4X4 projection;
		DirectX::XMStoreFloat4x4(&projection, Projection);
		rc.view = view;
		rc.projection = projection;
		rc.viewPosition.x = viewPosition.x;
		rc.viewPosition.y = viewPosition.y;
		rc.viewPosition.z = viewPosition.z;
		rc.viewPosition.w = 1;
		// 影用行列
		DirectX::XMStoreFloat4x4(&shadowViewProjection, View * Projection);
	}


	{// 3Dモデル描画
		D3D11_VIEWPORT viewport{};
		viewport.TopLeftX = 0.0f;
		viewport.TopLeftY = 0.0f;
		viewport.Width = static_cast<FLOAT>(SHADOW_SIZE.x);
		viewport.Height = static_cast<FLOAT>(SHADOW_SIZE.y);
		viewport.MinDepth = 0.0f;
		viewport.MaxDepth = 1.0f;
		dc->RSSetViewports(1, &viewport);

		//シャドーマップシェーダ
		Shader* shader = graphics.GetShader(static_cast<int>(ShaderType::ShadowMapShader));
		shader->Begin(dc, rc);

		// ステージ描画
		StageManager::Instance().Render(dc, shader);
		// プレイヤー描画
		player->Render(dc, shader);
		// エネミー描画
		EnemyManager::Instance().Render(dc, shader);

		shader->End(dc);
	}
	// ミップマップ作成
	dc->GenerateMips(shadowMap->GetShaderResourceView());

}

void SceneGame07::Render()
{
	RenderShadow();
	//RenderReflect();
	Graphics& graphics = Graphics::Instance();
	ID3D11DeviceContext* dc = graphics.GetDeviceContext();
	ID3D11RenderTargetView* rtv = graphics.GetRenderTargetView();
	ID3D11DepthStencilView* dsv = graphics.GetDepthStencilView();

	// 画面クリア＆レンダーターゲット設定
	FLOAT color[] = { 0.2f, 0.2f, 0.2f, 1.0f };	// RGBA(0.0～1.0)
	dc->ClearRenderTargetView(rtv, color);
	dc->ClearDepthStencilView(dsv, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	dc->OMSetRenderTargets(1, &rtv, dsv);

	// 描画処理
	RenderContext rc;
	rc.lightDirection = lightDirection;	// ライト方向（下方向）

	// カメラパラメータ設定
	Camera& camera = Camera::Instance();
	rc.view = camera.GetView();
	camera.SetPerspectiveFov(
		DirectX::XMConvertToRadians(45),
		graphics.GetScreenWidth() / graphics.GetScreenHeight(),
		0.1f,
		1000.0f
	);

	rc.projection = camera.GetProjection();

	rc.viewPosition.x = camera.GetEye().x;
	rc.viewPosition.y = camera.GetEye().y;
	rc.viewPosition.z = camera.GetEye().z;
	rc.viewPosition.w = 1.0f;
	rc.shaderResourceView = shadowMap->GetShaderResourceView();
	rc.shadowViewProjection = shadowViewProjection;


	D3D11_VIEWPORT viewport{};
	viewport.TopLeftX = 0.0f;
	viewport.TopLeftY = 0.0f;
	viewport.Width = graphics.GetScreenWidth();
	viewport.Height = graphics.GetScreenHeight();
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;
	dc->RSSetViewports(1, &viewport);


	// 3Dモデル描画
	{
		//　フォンシェーダ
		Shader* shader = graphics.GetShader(static_cast<int>(ShaderType::PhongDrawShadowShader));
		shader->Begin(dc, rc);
		
		// ステージ描画
		StageManager::Instance().Render(dc, shader);


		// エネミー描画
		EnemyManager::Instance().Render(dc, shader);

		//// プレイヤー描画
		//player->Render(dc, shader);

		shader->End(dc);
	}
	{
		//　トゥーンシェーダ
		Shader* shader = graphics.GetShader(static_cast<int>(ShaderType::PhongDrawShadowShader));
		shader->Begin(dc, rc);

		// プレイヤー描画
		player->Render(dc, shader);

		shader->End(dc);
	}


	{
		//　水面
		//Shader* shader = graphics.GetShader(static_cast<int>(ShaderType::WaterSurfaceShader));
		Shader* shader = graphics.GetShader();
		shader->Begin(dc, rc);

		// 水面描画
		waterSurface->Render(dc, shader);

		shader->End(dc);
	}

	// 爆破パーティクルアニメーション
	{
		particle_bomb->Render(dc, camera.GetView(), camera.GetProjection(),
			static_cast<int>(BlendStateId::Alpha));

		particle_alpha->Render(dc, camera.GetView(), camera.GetProjection(),
			static_cast<int>(BlendStateId::Alpha));
		particle_add->Render(dc, camera.GetView(), camera.GetProjection(),
			static_cast<int>(BlendStateId::Add));
		particle_subtract->Render(dc, camera.GetView(), camera.GetProjection(),
			static_cast<int>(BlendStateId::Subtract));



	}


	// 3Dエフェクト描画
	{
		EffectManager::Instance().Render(rc.view, rc.projection);
	}

	// スカイドーム
	//{
	//	skybox->Render(dc, rc);
	//}

	// 3Dデバッグ描画
	//{
	//	// プレイヤーデバッグ描画
	//	player->DrawDebugPrimitive();

	//	// エネミーデバッグ描画
	//	EnemyManager::Instance().DrawDebugPrimitive();

	//	// ラインレンダラ描画実行
	//	graphics.GetLineRenderer()->Render(dc, rc.view, rc.projection);

	//	// デバッグレンダラ描画実行
	//	graphics.GetDebugRenderer()->Render(dc, rc.view, rc.projection);
	//}

	// 2Dスプライト描画
	{
		RenderEnemyGauge(dc, rc.view, rc.projection);
	}


	// 2DデバッグGUI描画
	//{
	//	ImGui::SetNextWindowPos(ImVec2(10, 10), ImGuiCond_FirstUseEver);
	//	ImGui::SetNextWindowSize(ImVec2(300, 300), ImGuiCond_FirstUseEver);

	//	if (ImGui::Begin("Debug Menu", nullptr, ImGuiWindowFlags_None))
	//	{
	//		int id = 0;
	//		// プレイヤーデバッグ描画
	//		ImGui::PushID(id++);
	//		{
	//			player->DrawDebugGUI();
	//		}
	//		ImGui::PopID();

	//		// カメラデバッグ描画
	//		ImGui::PushID(id++);
	//		{
	//			camera.DrawDebugGUI();
	//		}
	//		ImGui::PopID();

	//		// カメラコントローラーデバッグ描画
	//		ImGui::PushID(id++);
	//		{
	//			cameraController->DrawDebugGUI();
	//		}
	//		ImGui::PopID();

	//		// エネミーマネージャーデバッグ描画
	//		ImGui::PushID(id++);
	//		{
	//			EnemyManager::Instance().DrawDebugGUI();
	//		}
	//		ImGui::PopID();
	//	}
	//	ImGui::End();
	//}

	{
		ImGui::SetNextWindowPos(ImVec2(10, 200), ImGuiCond_FirstUseEver);
		ImGui::SetNextWindowSize(ImVec2(300, 300), ImGuiCond_FirstUseEver);
		bool show_another_window = true;
		if (ImGui::Begin(u8"シャドーマップ", nullptr, ImGuiWindowFlags_None))
		{
			const int w = static_cast<float>(SHADOW_SIZE.x) / 2;
			const int h = static_cast<float>(SHADOW_SIZE.y) / 2;
			//ImGui::Text(u8"スクリーン画像");
			ImGui::Image(shadowMap->GetShaderResourceView(), { w, h });

		}
		ImGui::End();
	}
	//{
	//	ImGui::SetNextWindowPos(ImVec2(10, 200), ImGuiCond_FirstUseEver);
	//	ImGui::SetNextWindowSize(ImVec2(300, 300), ImGuiCond_FirstUseEver);
	//	bool show_another_window = true;
	//	if (ImGui::Begin(u8"リフレクションマップ", nullptr, ImGuiWindowFlags_None))
	//	{
	//		const int w = static_cast<float>(REFLECT_SIZE.x) / 2;
	//		const int h = static_cast<float>(REFLECT_SIZE.y) / 2;
	//		//ImGui::Text(u8"スクリーン画像");
	//		ImGui::Image(reflectMap->GetShaderResourceView(), { w, h });

	//	}
	//	ImGui::End();
	//}

}

// エネミーHPゲージ描画
void SceneGame07::RenderEnemyGauge(
	ID3D11DeviceContext* dc,
	const DirectX::XMFLOAT4X4& view,
	const DirectX::XMFLOAT4X4& projection)
{
	// ビューポート
	D3D11_VIEWPORT viewport;
	UINT numViewports = 1;
	dc->RSGetViewports(&numViewports, &viewport);

	// 変換行列
	DirectX::XMMATRIX View = DirectX::XMLoadFloat4x4(&view);
	DirectX::XMMATRIX Projection = DirectX::XMLoadFloat4x4(&projection);
	DirectX::XMMATRIX World = DirectX::XMMatrixIdentity();

	// 全ての敵の頭上にHPゲージを表示
	EnemyManager& enemyManager = EnemyManager::Instance();
	int enemyCount = enemyManager.GetEnemyCount();

	for (int i = 0; i < enemyCount; ++i)
	{
		Enemy* enemy = enemyManager.GetEnemy(i);

		// エネミーの頭上のワールド座標
		DirectX::XMFLOAT3 worldPosition = enemy->GetPosition();
		worldPosition.y += enemy->GetHeight();

		DirectX::XMVECTOR WorldPosition = DirectX::XMLoadFloat3(&worldPosition);

		// ワールド座標からスクリーン座標へ変換
		DirectX::XMVECTOR ScreenPosition = DirectX::XMVector3Project(
			WorldPosition,
			viewport.TopLeftX,
			viewport.TopLeftY,
			viewport.Width,
			viewport.Height,
			viewport.MinDepth,
			viewport.MaxDepth,
			Projection,
			View,
			World
		);

		// スクリーン座標
		DirectX::XMFLOAT2 screenPosition;
		DirectX::XMStoreFloat2(&screenPosition, ScreenPosition);

		// ゲージの長さ
		const float gaugeWidth = 30.0f;
		const float gaugeHeight = 5.0f;

		float healthRate = enemy->GetHealth() / static_cast<float>(enemy->GetMaxHealth());

		//　ゲージ描画
		gauge->Render(dc,
			screenPosition.x - gaugeWidth * 0.5f,
			screenPosition.y - gaugeHeight,
			gaugeWidth * healthRate,
			gaugeHeight,
			0, 0,
			static_cast<float>(gauge->GetTextureWidth()),
			static_cast<float>(gauge->GetTextureHeight()),
			0.0f,
			1.0f, 0.0f, 0.0f, 1.0f
		);
	}

	// エネミー配置処理
	Mouse& mouse = Input::Instance().GetMouse();
	if (mouse.GetButtonDown() & Mouse::BTN_LEFT)
	{
		// マウスカーソル座標を取得
		DirectX::XMFLOAT3 screenPosition;
		screenPosition.x = static_cast<float>(mouse.GetPositionX());
		screenPosition.y = static_cast<float>(mouse.GetPositionY());

		DirectX::XMVECTOR ScreenPosition, WorldPosition;

		// レイの始点を算出
		screenPosition.z = 0.0f;
		ScreenPosition = DirectX::XMLoadFloat3(&screenPosition);
		WorldPosition = DirectX::XMVector3Unproject(
			ScreenPosition,
			viewport.TopLeftX,
			viewport.TopLeftY,
			viewport.Width,
			viewport.Height,
			viewport.MinDepth,
			viewport.MaxDepth,
			Projection,
			View,
			World
		);
		DirectX::XMFLOAT3 rayStart;
		DirectX::XMStoreFloat3(&rayStart, WorldPosition);

		// レイの終点を算出
		screenPosition.z = 1.0f;
		ScreenPosition = DirectX::XMLoadFloat3(&screenPosition);
		WorldPosition = DirectX::XMVector3Unproject(
			ScreenPosition,
			viewport.TopLeftX,
			viewport.TopLeftY,
			viewport.Width,
			viewport.Height,
			viewport.MinDepth,
			viewport.MaxDepth,
			Projection,
			View,
			World
		);
		DirectX::XMFLOAT3 rayEnd;
		DirectX::XMStoreFloat3(&rayEnd, WorldPosition);

		// レイキャスト
		HitResult hit;
		if (StageManager::Instance().RayCast(rayStart, rayEnd, hit))
		{
			// 敵を配置
			EnemySlime* slime = new EnemySlime();
			slime->SetPosition(hit.position);
			slime->SetTerritory(hit.position, 10.0f);
			EnemyManager::Instance().Register(slime);
		}
	}


}
