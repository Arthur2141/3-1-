#include "Graphics/Graphics.h"
#include "SceneLoading.h"
#include "SceneManager.h"
#include "Audio/Audio.h"
#include "SceneGame00.h"
#include "Decoration.h"
#include "Graphics/DebugRenderer.h"
#include "SceneGame01.h"
#include "SceneEnd.h"
#include "Camera.h"
#include "EnemyManager.h"
#include "EnemySlime.h"
#include "EffectManager.h"
#include "ProjectileSauce.h"
#include "ProjectileManager.h"
#include "Input/Input.h"
#include "StageManager.h"
#include "StageMain.h"
#include "StageMoveFloor.h"
//#include "StageWaterSurface.h"

// 初期化
void SceneGame00::Initialize()
{
	// ステージ初期化
	StageManager& stageManager = StageManager::Instance();
	StageMain* stageMain = new StageMain();
	stageManager.Register(stageMain);

	
	
	/*StageMoveFloor* stageMoveFloor = new StageMoveFloor();
	stageMoveFloor->SetStartPoint(DirectX::XMFLOAT3(-20, 1, 0));
	stageMoveFloor->SetGoalPoint(DirectX::XMFLOAT3(-10, 2, 3));
	stageMoveFloor->SetTorque(DirectX::XMFLOAT3(0, 1.0f, 0));
	stageManager.Register(stageMoveFloor);*/

	// プレイヤー
	player = new Player();
	flypan = std::make_unique<Flypan>();
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
	
	flypan->SetPosition({-12,-2,0});
	flypan->SetScale({ 0.1f,0.1f,0.1f });

	player->SetPosition({ -20,5,0 });
	// ゲージスプライト

	Decoration* isu=new Decoration("Data/Model/Chair/chair.mdl");
	isu->SetPosition({ -37.5,-2.9f,-1.2f});
	isu->SetScale({ 0.1f,0.1f,0.1f });
	decors.emplace_back(isu);

	Decoration* table = new Decoration("Data/Model/Table/table.mdl");
	table->SetPosition({ -38.6f,-2.9f,0.8f });
	table->SetScale({ 0.075f,0.075f,0.075f });
	decors.emplace_back(table);

	Decoration* closet = new Decoration("Data/Model/Closet/closet.mdl");
	closet->SetPosition({ -38.6f,-2.9f,12.0f });
	closet->SetScale({ 0.1f,0.1f,0.1f });
	decors.emplace_back(closet);
	
	
	Decoration* sofa = new Decoration("Data/Model/Sofa/sofa.mdl");
	sofa->SetPosition({ -30.6f, -2.9f,-12.3 });
	sofa->SetScale({ 0.1f,0.1f,0.1f });
	decors.emplace_back(sofa);

	gauge = new Sprite();
}

// 終了化
void SceneGame00::Finalize()
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
	for (Decoration* decor : decors)
	{
		delete decor;
	}
	decors.clear();
	projectileManager.Clear();
	flypan.reset();
}

// 更新処理
void SceneGame00::Update(float elapsedTime)
{
	// カメラコントローラー更新処理
	DirectX::XMFLOAT3 target = player->GetPosition();
	target.y += 0.5f;
	cameraController->SetTarget(target);
	cameraController->Update(elapsedTime);

	// ステージ更新処理
	StageManager::Instance().Update(elapsedTime);
	//waterSurface->Update(elapsedTime);
	//timer += elapsedTime;

	// プレイヤー更新処理
	player->Update(elapsedTime);
	flypan->Update(elapsedTime);
	projectileManager.Update(elapsedTime);
	int decorCount = static_cast<int>(decors.size());
	for (int i = 0; i < decorCount; i++)
	{
		decors.at(i)->Update(elapsedTime);
		//decors->at(i).Update(elapsedTime);
	}

	if (flypan->Goal(player))
	{
		
		
		{
			SceneManager::Instance().ChangeScene(new SceneLoading(new SceneEnd(projectileManager.getOffsets())));
		}
	}
	////  空
	//sky->Update(elapsedTime);
	// エネミー更新処理
	EnemyManager::Instance().Update(elapsedTime);
	UpdateEffect(elapsedTime);
	// エフェクト更新処理
	EffectManager::Instance().Update(elapsedTime);
	

}

void SceneGame00::UpdateEffect(float elapsedTime)
{
	Graphics& graphics = Graphics::Instance();
	ID3D11DeviceContext* dc = graphics.GetDeviceContext();
	D3D11_VIEWPORT viewport;
	UINT numViewports = 1;
	dc->RSGetViewports(&numViewports, &viewport);
	DirectX::XMMATRIX View = DirectX::XMLoadFloat4x4(&Camera::Instance().GetView());
	DirectX::XMMATRIX Projection = DirectX::XMLoadFloat4x4(&Camera::Instance().GetProjection());
	DirectX::XMMATRIX World = DirectX::XMMatrixIdentity();
	int projectilecount = projectileManager.GetProjectileCount();
	for (int i = 0; i < projectilecount; i++)
	{
		Projectile* proj = projectileManager.GetProjectile(i);
		if (proj->getStat())continue;

		DirectX::XMFLOAT3 s, e;
		HitResult result;
		s = proj->GetPosition();
		//s.y += 0.5f;
		DirectX::XMStoreFloat3(&e, DirectX::XMVectorAdd(DirectX::XMLoadFloat3(&proj->GetPosition()), DirectX::XMLoadFloat3(&proj->GetDirection())));
		//e.y = proj->GetPosition().y + 0.5f;

		DirectX::XMFLOAT3 out = { 0,0,0 };

		if (Collision::IntersectRayVsModel(s, e, player->getModel(), result))
		{

			DirectX::XMFLOAT3 offV;

			DirectX::XMStoreFloat3(&offV, DirectX::XMVectorNegate(DirectX::XMVectorSubtract(DirectX::XMLoadFloat3(&player->GetPosition()), DirectX::XMLoadFloat3(&result.position))));
			offV.y *= -1;
			proj->setOffset(offV);
			projectileManager.setOffsets(offV);
			/*proj->setTarget(&player->GetPosition());
			proj->stuckMode();*/
			proj->getEFfect()->Play(result.position);
			proj->Destroy();
		}

	}

	Mouse& mouse = Input::Instance().GetMouse();
	if (mouse.GetButtonDown() & Mouse::BTN_LEFT)
	{
		// マウスカーソル座標を取得
		DirectX::XMFLOAT3 screenPosition;
		screenPosition.x = static_cast<float>(mouse.GetOldPositionX());
		screenPosition.y = static_cast<float>(mouse.GetOldPositionY());
		screenPosition.z = 0.0f;

		DirectX::XMVECTOR screenPos = DirectX::XMLoadFloat3(&screenPosition);

		DirectX::XMVECTOR WorldPosition = DirectX::XMVector3Unproject(
			screenPos,
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

		DirectX::XMFLOAT3 start;
		DirectX::XMStoreFloat3(&start, WorldPosition);

		screenPosition.z = 1.0f;

		screenPos = DirectX::XMLoadFloat3(&screenPosition);

		WorldPosition = DirectX::XMVector3Unproject(
			screenPos,
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

		DirectX::XMFLOAT3 end;
		DirectX::XMStoreFloat3(&end, WorldPosition);

		DirectX::XMVECTOR playerPosVec = DirectX::XMLoadFloat3(&Camera::Instance().GetEye());

		DirectX::XMVECTOR endPosVec = DirectX::XMLoadFloat3(&end);
		particledata particleData;
		HitResult hit;
		

		DirectX::XMStoreFloat3(&particleData.dir, DirectX::XMVector3Normalize(DirectX::XMVectorSubtract(endPosVec, playerPosVec)));

		DirectX::XMVECTOR particlePosVec;
		particlePosVec = playerPosVec;

		DirectX::XMFLOAT3 dir;
		DirectX::XMStoreFloat3(&dir, DirectX::XMVector3Normalize(DirectX::XMVectorSubtract(endPosVec, playerPosVec)));
		ProjectileSauce* projectile = new ProjectileSauce(&projectileManager);
		projectile->Launch(dir, Camera::Instance().GetEye());
	}
}

// 描画処理

//void SceneGame00::RenderShadow()
//{
//	Graphics& graphics = Graphics::Instance();
//	ID3D11DeviceContext* dc = graphics.GetDeviceContext();
//	ID3D11RenderTargetView* rtv[1] = { shadowMap->GetRenderTargetView() };
//	ID3D11DepthStencilView* dsv = shadowMap->GetDepthStencilView();
//
//	// 画面クリア＆レンダーターゲット設定
//	FLOAT color[] = { 0.0f, 0.0f, 0.5f, 1.0f };	// RGBA(0.0～1.0)
//	dc->ClearRenderTargetView(rtv[0], color);
//	dc->ClearDepthStencilView(dsv, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
//	dc->OMSetRenderTargets(1, rtv, dsv);
//
//	// 描画処理
//	RenderContext rc{};
//	rc.lightDirection = lightDirection;	// ライト方向（下方向）
//	DirectX::XMFLOAT3 viewPosition = player->GetPosition();
//	viewPosition.x -= lightDirection.x * 30.0f;
//	viewPosition.y -= lightDirection.y * 30.0f;
//	viewPosition.z -= lightDirection.z * 30.0f;
//
//	{// カメラパラメータ設定
//
//		DirectX::XMVECTOR Eye = DirectX::XMLoadFloat3(&viewPosition);
//		DirectX::XMVECTOR Focus = DirectX::XMLoadFloat3(&player->GetPosition());
//		DirectX::XMVECTOR Up = DirectX::XMVectorSet(0, 1, 0, 0);
//		DirectX::XMMATRIX View = DirectX::XMMatrixLookAtLH(Eye, Focus, Up);
//		DirectX::XMFLOAT4X4 view;
//		DirectX::XMStoreFloat4x4(&view, View);
//		// 平行投影
//		DirectX::XMMATRIX Projection = DirectX::XMMatrixOrthographicLH(32, 32, 0.1f, 100.0f);
//		DirectX::XMFLOAT4X4 projection;
//		DirectX::XMStoreFloat4x4(&projection, Projection);
//		rc.view = view;
//		rc.projection = projection;
//		rc.viewPosition.x = viewPosition.x;
//		rc.viewPosition.y = viewPosition.y;
//		rc.viewPosition.z = viewPosition.z;
//		rc.viewPosition.w = 1;
//		// 影用行列
//		DirectX::XMStoreFloat4x4(&shadowViewProjection, View * Projection);
//	}
//
//
//	{// 3Dモデル描画
//		D3D11_VIEWPORT viewport{};
//		viewport.TopLeftX = 0.0f;
//		viewport.TopLeftY = 0.0f;
//		viewport.Width = static_cast<FLOAT>(SHADOW_SIZE.x);
//		viewport.Height = static_cast<FLOAT>(SHADOW_SIZE.y);
//		viewport.MinDepth = 0.0f;
//		viewport.MaxDepth = 1.0f;
//		dc->RSSetViewports(1, &viewport);
//
//		//シャドーマップシェーダ
//		Shader* shader = graphics.GetShader(static_cast<int>(ShaderType::ShadowMapShader));
//		shader->Begin(dc, rc);
//
//		// ステージ描画
//		StageManager::Instance().Render(dc, shader);
//		// プレイヤー描画
//		player->Render(dc, shader);
//		// エネミー描画
//		EnemyManager::Instance().Render(dc, shader);
//
//		shader->End(dc);
//	}
//	// ミップマップ作成
//	dc->GenerateMips(shadowMap->GetShaderResourceView());
//
//}
//void SceneGame00::RenderReflect()
//{
//	Graphics& graphics = Graphics::Instance();
//	ID3D11DeviceContext* dc = graphics.GetDeviceContext();
//	ID3D11RenderTargetView* rtv[1] = { reflectMap->GetRenderTargetView() };
//	ID3D11DepthStencilView* dsv = reflectMap->GetDepthStencilView();
//
//	// 画面クリア＆レンダーターゲット設定
//	FLOAT color[] = { 0.0f, 0.0f, 0.0f, 1.0f };	// RGBA(0.0～1.0)
//	dc->ClearRenderTargetView(rtv[0], color);
//
//	dc->ClearDepthStencilView(dsv, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
//	dc->OMSetRenderTargets(1, rtv, dsv);
//
//	// 描画処理
//	RenderContext rc{};
//	rc.lightDirection = lightDirection;	// ライト方向（下方向）
//
//	// カメラパラメータ設定
//	Camera& camera = Camera::Instance();
//	DirectX::XMFLOAT3 cameraPos = camera.GetEye();
//	const float water_surface_hight = -0.5f;
//	cameraPos.y = water_surface_hight * 2.0 - cameraPos.y;
//	// 視点、注視点、上方向からビュー行列を作成
//	DirectX::XMVECTOR Eye = DirectX::XMLoadFloat3(&cameraPos);
//	DirectX::XMFLOAT3 focus = player->GetPosition();
//	focus.y += 0.5f;
//	DirectX::XMVECTOR Focus = DirectX::XMLoadFloat3(&focus);
//	//DirectX::XMVECTOR Focus = DirectX::XMLoadFloat3(&player->GetPosition());
//	DirectX::XMVECTOR Up = DirectX::XMVectorSet(0, 1, 0, 0);
//	DirectX::XMMATRIX View = DirectX::XMMatrixLookAtLH(Eye, Focus, Up);
//	DirectX::XMFLOAT4X4 view;
//	DirectX::XMStoreFloat4x4(&view, View);
//
//	rc.view = view;
//	camera.SetPerspectiveFov(
//		DirectX::XMConvertToRadians(90),
//		static_cast<float>(REFLECT_SIZE.x) / static_cast<float>(REFLECT_SIZE.y),
//		0.1f,
//		1000.0f
//	);
//	rc.projection = camera.GetProjection();
//	rc.viewPosition.x = cameraPos.x;
//	rc.viewPosition.y = cameraPos.y;
//	rc.viewPosition.z = cameraPos.z;
//	rc.viewPosition.w = 0;
//	DirectX::XMMATRIX Projection = DirectX::XMLoadFloat4x4(&camera.GetProjection());
//
//	//	Reflect_Matrix
//	DirectX::XMStoreFloat4x4(&reflectViewProjection, View * Projection);//追加
//	rc.water_surface_height = -0.5f;
//
//	D3D11_VIEWPORT viewport{};
//	viewport.TopLeftX = 0.0f;
//	viewport.TopLeftY = 0.0f;
//	viewport.Width = static_cast<float>(REFLECT_SIZE.x);
//	viewport.Height = static_cast<float>(REFLECT_SIZE.y);
//	viewport.MinDepth = 0.0f;
//	viewport.MaxDepth = 1.0f;
//	dc->RSSetViewports(1, &viewport);
//
//	{
//		//Lambertシェーダ
//		Shader* shader = graphics.GetShader(static_cast<int>(ShaderType::ReflectMapShader));
//		shader->Begin(dc, rc);
//		// プレイヤー描画
//		player->Render(dc, shader);
//		// エネミー描画
//		EnemyManager::Instance().Render(dc, shader);
//		// ステージ描画
//		StageManager::Instance().Render(dc, shader);
//
//		shader->End(dc);
//	}
//
//
//}

void SceneGame00::Render()
{
	//RenderShadow();
	//RenderReflect();
	Graphics& graphics = Graphics::Instance();
	ID3D11DeviceContext* dc = graphics.GetDeviceContext();
	ID3D11RenderTargetView* rtv = graphics.GetRenderTargetView();
	ID3D11DepthStencilView* dsv = graphics.GetDepthStencilView();
	DebugRenderer* debugRend=graphics.GetDebugRenderer();
	// 画面クリア＆レンダーターゲット設定
	FLOAT color[] = { 0.5f, 0.5f, 0.5f, 1.0f };	// RGBA(0.0～1.0)
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
	//rc.shaderResourceView = shadowMap->GetShaderResourceView();
	//rc.shadowViewProjection = shadowViewProjection;

	//rc.timer = timer;
	//rc.envShaderResourceView = skybox->GetShaderResouceView();
	//rc.reflectShaderResourceView = reflectMap->GetShaderResourceView();
	//rc.reflectViewProjection = reflectViewProjection;

	//rc.ripplesPosition = player->GetPosition();
	//rc.ripplesSize = 0.2f;

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
		//　トゥーンシェーダ
		Shader* shader = graphics.GetShader();
		shader->Begin(dc, rc);
		
		// ステージ描画
		StageManager::Instance().Render(dc, shader);


		// エネミー描画
		EnemyManager::Instance().Render(dc, shader);

		// プレイヤー描画
		player->Render(dc, shader);
		flypan->Render(dc, shader);
		projectileManager.Render(dc, shader);

		int decorCount = static_cast<int>(decors.size());
		for (int i = 0; i < decorCount; i++)
		{
			//Decoration* dec = decors.at(i);

			
			
			decors.at(i)->Render(dc,shader);
		}
		shader->End(dc);
	}

	{


		Shader* shader = graphics.GetShader();
		shader->Begin(dc, rc);

	
		RenderEnemyGauge(dc, rc.view, rc.projection);
		
		shader->End(dc);
	}




	// 3Dエフェクト描画
	{
		EffectManager::Instance().Render(rc.view, rc.projection);
	}


	{
		
	}
	/*ImGui::SetNextWindowPos(ImVec2(10, 10), ImGuiCond_FirstUseEver);
		ImGui::SetNextWindowSize(ImVec2(300, 300), ImGuiCond_FirstUseEver);
		if (ImGui::Begin("Debug Menu", nullptr, ImGuiWindowFlags_None))
		{
			player->DrawDebugGUI();
	
		}
		ImGui::SetNextWindowPos(ImVec2(10, 200), ImGuiCond_FirstUseEver);
			ImGui::SetNextWindowSize(ImVec2(300, 300), ImGuiCond_FirstUseEver);
		
			ImGui::End();*/


}

// エネミーHPゲージ描画
void SceneGame00::RenderEnemyGauge(
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

	

}
