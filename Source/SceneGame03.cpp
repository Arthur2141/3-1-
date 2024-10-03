#include "Graphics/Graphics.h"
#include "SceneLoading.h"
#include "SceneManager.h"
#include "SceneGame01.h"

#include "SceneGame03.h"
#include "Camera.h"
#include "EnemyManager.h"
#include "EnemySlime.h"
#include "EffectManager.h"
#include "Input/Input.h"
#include "StageManager.h"
#include "StageMain.h"
#include "StageMoveFloor.h"
//#include "StageWaterSurface.h"

// ������
void SceneGame03::Initialize()
{
	// �X�e�[�W������
	StageManager& stageManager = StageManager::Instance();
	StageMain* stageMain = new StageMain();
	stageManager.Register(stageMain);

	StageMoveFloor* stageMoveFloor = new StageMoveFloor();
	stageMoveFloor->SetStartPoint(DirectX::XMFLOAT3(0, 1, 3));
	stageMoveFloor->SetGoalPoint(DirectX::XMFLOAT3(10, 2, 3));
	stageMoveFloor->SetTorque(DirectX::XMFLOAT3(0, 1.0f, 0));
	stageManager.Register(stageMoveFloor);

	// �v���C���[
	player = new Player();

	// �J���������ݒ�
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

	// �J�����R���g���[���[������
	cameraController = new CameraController();

	// �G�l�~�[������
	EnemyManager& enemyManager = EnemyManager::Instance();
	for (int i = 0; i < 5; ++i)
	{
		EnemySlime* slime = new EnemySlime();
		slime->SetPosition(DirectX::XMFLOAT3(i * 2.0f, 0, 5));
		slime->SetTerritory(slime->GetPosition(), 10.0f);
		enemyManager.Register(slime);
	}
	// �t���[���o�b�t�@
	shadowMap = std::make_unique<RenderTarget>(SHADOW_SIZE.x, SHADOW_SIZE.y, 6,DXGI_FORMAT_R32G32_FLOAT);
	reflectMap = std::make_unique<RenderTarget>
		(REFLECT_SIZE.x, REFLECT_SIZE.y, DXGI_FORMAT_R8G8B8A8_UNORM);

	sky = std::make_unique<Sky>();
	sky->SetScale({10,10,10});
	//skybox = std::make_unique<SkyBoxSprite>("Data/Model/SkyBox/sunflowers.jpg");
	//skybox = std::make_unique<SkyBox>("Data/SkyBox/earth.jpg");

	//skybox = std::make_unique<SkyBox>("Data/SkyBox/field20220706.jpg");
	//skybox = std::make_unique<SkyBox>("Data/SkyBox/sky.png");
	//skybox = std::make_unique<SkyBox>("Data/SkyBox/sunflowers.jpg");
	//skybox = std::make_unique<SkyBox>("Data/SkyBox/sunSet20220706.jpg");
	skybox = std::make_unique<SkyBox>("Data/SkyBox/torii20220706.jpg");
	//skybox = std::make_unique<SkyBox>("Data/SkyBox/earth.jpg");
	//skybox = std::make_unique<SkyBox>("Data/SkyBox/town20220706.jpg");
	//skybox = std::make_unique<SkyBox>("Data/SkyBox/beautiful_sky.jpg");


	waterSurface = std::make_unique<WaterSurface>();
	
	// �Q�[�W�X�v���C�g
	gauge = new Sprite();
}

// �I����
void SceneGame03::Finalize()
{
	// �Q�[�W�X�v���C�g�I����
	if (gauge != nullptr)
	{
		delete gauge;
		gauge = nullptr;
	}

	// �G�l�~�[�I����
	EnemyManager::Instance().Clear();

	// �J�����R���g���[���[�I����
	if (cameraController != nullptr)
	{
		delete cameraController;
		cameraController = nullptr;
	}

	// �v���C���[�I����
	if (player != nullptr)
	{
		delete player;
		player = nullptr;
	}

	// �X�e�[�W�I����
	StageManager::Instance().Clear();
}

// �X�V����
void SceneGame03::Update(float elapsedTime)
{
	// �J�����R���g���[���[�X�V����
	DirectX::XMFLOAT3 target = player->GetPosition();
	target.y += 0.5f;
	cameraController->SetTarget(target);
	cameraController->Update(elapsedTime);

	// �X�e�[�W�X�V����
	StageManager::Instance().Update(elapsedTime);
	waterSurface->Update(elapsedTime);
	timer += elapsedTime;

	// �v���C���[�X�V����
	player->Update(elapsedTime);

	//  ��
	sky->Update(elapsedTime);
	// �G�l�~�[�X�V����
	EnemyManager::Instance().Update(elapsedTime);

	// �G�t�F�N�g�X�V����
	EffectManager::Instance().Update(elapsedTime);


}

// �`�揈��

void SceneGame03::RenderShadow()
{
	Graphics& graphics = Graphics::Instance();
	ID3D11DeviceContext* dc = graphics.GetDeviceContext();
	ID3D11RenderTargetView* rtv[1] = { shadowMap->GetRenderTargetView() };
	ID3D11DepthStencilView* dsv = shadowMap->GetDepthStencilView();

	// ��ʃN���A�������_�[�^�[�Q�b�g�ݒ�
	FLOAT color[] = { 0.0f, 0.0f, 0.5f, 1.0f };	// RGBA(0.0�`1.0)
	dc->ClearRenderTargetView(rtv[0], color);
	dc->ClearDepthStencilView(dsv, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	dc->OMSetRenderTargets(1, rtv, dsv);

	// �`�揈��
	RenderContext rc{};
	rc.lightDirection = lightDirection;	// ���C�g�����i�������j
	DirectX::XMFLOAT3 viewPosition = player->GetPosition();
	viewPosition.x -= lightDirection.x * 30.0f;
	viewPosition.y -= lightDirection.y * 30.0f;
	viewPosition.z -= lightDirection.z * 30.0f;

	{// �J�����p�����[�^�ݒ�

		DirectX::XMVECTOR Eye = DirectX::XMLoadFloat3(&viewPosition);
		DirectX::XMVECTOR Focus = DirectX::XMLoadFloat3(&player->GetPosition());
		DirectX::XMVECTOR Up = DirectX::XMVectorSet(0, 1, 0, 0);
		DirectX::XMMATRIX View = DirectX::XMMatrixLookAtLH(Eye, Focus, Up);
		DirectX::XMFLOAT4X4 view;
		DirectX::XMStoreFloat4x4(&view, View);
		// ���s���e
		DirectX::XMMATRIX Projection = DirectX::XMMatrixOrthographicLH(32, 32, 0.1f, 100.0f);
		DirectX::XMFLOAT4X4 projection;
		DirectX::XMStoreFloat4x4(&projection, Projection);
		rc.view = view;
		rc.projection = projection;
		rc.viewPosition.x = viewPosition.x;
		rc.viewPosition.y = viewPosition.y;
		rc.viewPosition.z = viewPosition.z;
		rc.viewPosition.w = 1;
		// �e�p�s��
		DirectX::XMStoreFloat4x4(&shadowViewProjection, View * Projection);
	}


	{// 3D���f���`��
		D3D11_VIEWPORT viewport{};
		viewport.TopLeftX = 0.0f;
		viewport.TopLeftY = 0.0f;
		viewport.Width = static_cast<FLOAT>(SHADOW_SIZE.x);
		viewport.Height = static_cast<FLOAT>(SHADOW_SIZE.y);
		viewport.MinDepth = 0.0f;
		viewport.MaxDepth = 1.0f;
		dc->RSSetViewports(1, &viewport);

		//�V���h�[�}�b�v�V�F�[�_
		Shader* shader = graphics.GetShader(static_cast<int>(ShaderType::ShadowMapShader));
		shader->Begin(dc, rc);

		// �X�e�[�W�`��
		StageManager::Instance().Render(dc, shader);
		// �v���C���[�`��
		player->Render(dc, shader);
		// �G�l�~�[�`��
		EnemyManager::Instance().Render(dc, shader);

		shader->End(dc);
	}
	// �~�b�v�}�b�v�쐬
	dc->GenerateMips(shadowMap->GetShaderResourceView());

}
void SceneGame03::RenderReflect()
{
	Graphics& graphics = Graphics::Instance();
	ID3D11DeviceContext* dc = graphics.GetDeviceContext();
	ID3D11RenderTargetView* rtv[1] = { reflectMap->GetRenderTargetView() };
	ID3D11DepthStencilView* dsv = reflectMap->GetDepthStencilView();

	// ��ʃN���A�������_�[�^�[�Q�b�g�ݒ�
	FLOAT color[] = { 0.0f, 0.0f, 0.0f, 1.0f };	// RGBA(0.0�`1.0)
	dc->ClearRenderTargetView(rtv[0], color);

	dc->ClearDepthStencilView(dsv, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	dc->OMSetRenderTargets(1, rtv, dsv);

	// �`�揈��
	RenderContext rc{};
	rc.lightDirection = lightDirection;	// ���C�g�����i�������j

	// �J�����p�����[�^�ݒ�
	Camera& camera = Camera::Instance();
	DirectX::XMFLOAT3 cameraPos = camera.GetEye();
	const float water_surface_hight = -0.5f;
	cameraPos.y = water_surface_hight * 2.0 - cameraPos.y;
	// ���_�A�����_�A���������r���[�s����쐬
	DirectX::XMVECTOR Eye = DirectX::XMLoadFloat3(&cameraPos);
	DirectX::XMFLOAT3 focus = player->GetPosition();
	focus.y += 0.5f;
	DirectX::XMVECTOR Focus = DirectX::XMLoadFloat3(&focus);
	//DirectX::XMVECTOR Focus = DirectX::XMLoadFloat3(&player->GetPosition());
	DirectX::XMVECTOR Up = DirectX::XMVectorSet(0, 1, 0, 0);
	DirectX::XMMATRIX View = DirectX::XMMatrixLookAtLH(Eye, Focus, Up);
	DirectX::XMFLOAT4X4 view;
	DirectX::XMStoreFloat4x4(&view, View);

	rc.view = view;
	camera.SetPerspectiveFov(
		DirectX::XMConvertToRadians(90),
		static_cast<float>(REFLECT_SIZE.x) / static_cast<float>(REFLECT_SIZE.y),
		0.1f,
		1000.0f
	);
	rc.projection = camera.GetProjection();
	rc.viewPosition.x = cameraPos.x;
	rc.viewPosition.y = cameraPos.y;
	rc.viewPosition.z = cameraPos.z;
	rc.viewPosition.w = 0;
	DirectX::XMMATRIX Projection = DirectX::XMLoadFloat4x4(&camera.GetProjection());

	//	Reflect_Matrix
	DirectX::XMStoreFloat4x4(&reflectViewProjection, View * Projection);//�ǉ�
	rc.water_surface_height = -0.5f;

	D3D11_VIEWPORT viewport{};
	viewport.TopLeftX = 0.0f;
	viewport.TopLeftY = 0.0f;
	viewport.Width = static_cast<float>(REFLECT_SIZE.x);
	viewport.Height = static_cast<float>(REFLECT_SIZE.y);
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;
	dc->RSSetViewports(1, &viewport);

	{
		//Lambert�V�F�[�_
		Shader* shader = graphics.GetShader(static_cast<int>(ShaderType::ReflectMapShader));
		shader->Begin(dc, rc);
		// �v���C���[�`��
		player->Render(dc, shader);
		// �G�l�~�[�`��
		EnemyManager::Instance().Render(dc, shader);
		// �X�e�[�W�`��
		StageManager::Instance().Render(dc, shader);

		shader->End(dc);
	}


}

void SceneGame03::Render()
{
	RenderShadow();
	RenderReflect();
	Graphics& graphics = Graphics::Instance();
	ID3D11DeviceContext* dc = graphics.GetDeviceContext();
	ID3D11RenderTargetView* rtv = graphics.GetRenderTargetView();
	ID3D11DepthStencilView* dsv = graphics.GetDepthStencilView();

	// ��ʃN���A�������_�[�^�[�Q�b�g�ݒ�
	FLOAT color[] = { 0.0f, 0.0f, 0.5f, 1.0f };	// RGBA(0.0�`1.0)
	dc->ClearRenderTargetView(rtv, color);
	dc->ClearDepthStencilView(dsv, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	dc->OMSetRenderTargets(1, &rtv, dsv);

	// �`�揈��
	RenderContext rc;
	rc.lightDirection = lightDirection;	// ���C�g�����i�������j

	// �J�����p�����[�^�ݒ�
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

	// 
	rc.timer = timer;
	rc.envShaderResourceView = skybox->GetShaderResouceView();
	rc.reflectShaderResourceView = reflectMap->GetShaderResourceView();
	rc.reflectViewProjection = reflectViewProjection;

	//�ǉ�
	rc.ripplesPosition = player->GetPosition();
	rc.ripplesSize = 0.2f;

	D3D11_VIEWPORT viewport{};
	viewport.TopLeftX = 0.0f;
	viewport.TopLeftY = 0.0f;
	viewport.Width = graphics.GetScreenWidth();
	viewport.Height = graphics.GetScreenHeight();
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;
	dc->RSSetViewports(1, &viewport);


	// 3D���f���`��
	{
		//�@�g�D�[���V�F�[�_
		Shader* shader = graphics.GetShader(static_cast<int>(ShaderType::ToonShader));
		shader->Begin(dc, rc);
		
		// �X�e�[�W�`��
		StageManager::Instance().Render(dc, shader);


		// �G�l�~�[�`��
		EnemyManager::Instance().Render(dc, shader);

		// �v���C���[�`��
		player->Render(dc, shader);

		shader->End(dc);
	}

	{
		////�@���ʔg��
		//Shader* shader = graphics.GetShader(static_cast<int>(ShaderType::WaterSurfaceRipplesShader));
		//�@����
		Shader* shader = graphics.GetShader(static_cast<int>(ShaderType::WaterSurfaceShader));
		shader->Begin(dc, rc);

		// ���ʕ`��
		waterSurface->Render(dc, shader);

		shader->End(dc);
	}




	// 3D�G�t�F�N�g�`��
	{
		EffectManager::Instance().Render(rc.view, rc.projection);
	}

	// �X�J�C�h�[��
	{
		skybox->Render(dc, rc);
	}

	// 3D�f�o�b�O�`��
	//{
	//	// �v���C���[�f�o�b�O�`��
	//	player->DrawDebugPrimitive();

	//	// �G�l�~�[�f�o�b�O�`��
	//	EnemyManager::Instance().DrawDebugPrimitive();

	//	// ���C�������_���`����s
	//	graphics.GetLineRenderer()->Render(dc, rc.view, rc.projection);

	//	// �f�o�b�O�����_���`����s
	//	graphics.GetDebugRenderer()->Render(dc, rc.view, rc.projection);
	//}

	// 2D�X�v���C�g�`��
	{
		RenderEnemyGauge(dc, rc.view, rc.projection);
	}

	// 2D�f�o�b�OGUI�`��
	//{
	//	ImGui::SetNextWindowPos(ImVec2(10, 10), ImGuiCond_FirstUseEver);
	//	ImGui::SetNextWindowSize(ImVec2(300, 300), ImGuiCond_FirstUseEver);

	//	if (ImGui::Begin("Debug Menu", nullptr, ImGuiWindowFlags_None))
	//	{
	//		int id = 0;
	//		// �v���C���[�f�o�b�O�`��
	//		ImGui::PushID(id++);
	//		{
	//			player->DrawDebugGUI();
	//		}
	//		ImGui::PopID();

	//		// �J�����f�o�b�O�`��
	//		ImGui::PushID(id++);
	//		{
	//			camera.DrawDebugGUI();
	//		}
	//		ImGui::PopID();

	//		// �J�����R���g���[���[�f�o�b�O�`��
	//		ImGui::PushID(id++);
	//		{
	//			cameraController->DrawDebugGUI();
	//		}
	//		ImGui::PopID();

	//		// �G�l�~�[�}�l�[�W���[�f�o�b�O�`��
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
		if (ImGui::Begin(u8"�V���h�[�}�b�v", nullptr, ImGuiWindowFlags_None))
		{
			const int w = static_cast<float>(SHADOW_SIZE.x) / 2;
			const int h = static_cast<float>(SHADOW_SIZE.y) / 2;
			//ImGui::Text(u8"�X�N���[���摜");
			ImGui::Image(shadowMap->GetShaderResourceView(), { w, h });

		}
		ImGui::End();
	}
	{
		ImGui::SetNextWindowPos(ImVec2(10, 200), ImGuiCond_FirstUseEver);
		ImGui::SetNextWindowSize(ImVec2(300, 300), ImGuiCond_FirstUseEver);
		bool show_another_window = true;
		if (ImGui::Begin(u8"���t���N�V�����}�b�v", nullptr, ImGuiWindowFlags_None))
		{
			const int w = static_cast<float>(REFLECT_SIZE.x) / 2;
			const int h = static_cast<float>(REFLECT_SIZE.y) / 2;
			//ImGui::Text(u8"�X�N���[���摜");
			ImGui::Image(reflectMap->GetShaderResourceView(), { w, h });

		}
		ImGui::End();
	}

}

// �G�l�~�[HP�Q�[�W�`��
void SceneGame03::RenderEnemyGauge(
	ID3D11DeviceContext* dc,
	const DirectX::XMFLOAT4X4& view,
	const DirectX::XMFLOAT4X4& projection)
{
	// �r���[�|�[�g
	D3D11_VIEWPORT viewport;
	UINT numViewports = 1;
	dc->RSGetViewports(&numViewports, &viewport);

	// �ϊ��s��
	DirectX::XMMATRIX View = DirectX::XMLoadFloat4x4(&view);
	DirectX::XMMATRIX Projection = DirectX::XMLoadFloat4x4(&projection);
	DirectX::XMMATRIX World = DirectX::XMMatrixIdentity();

	// �S�Ă̓G�̓����HP�Q�[�W��\��
	EnemyManager& enemyManager = EnemyManager::Instance();
	int enemyCount = enemyManager.GetEnemyCount();

	for (int i = 0; i < enemyCount; ++i)
	{
		Enemy* enemy = enemyManager.GetEnemy(i);

		// �G�l�~�[�̓���̃��[���h���W
		DirectX::XMFLOAT3 worldPosition = enemy->GetPosition();
		worldPosition.y += enemy->GetHeight();

		DirectX::XMVECTOR WorldPosition = DirectX::XMLoadFloat3(&worldPosition);

		// ���[���h���W����X�N���[�����W�֕ϊ�
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

		// �X�N���[�����W
		DirectX::XMFLOAT2 screenPosition;
		DirectX::XMStoreFloat2(&screenPosition, ScreenPosition);

		// �Q�[�W�̒���
		const float gaugeWidth = 30.0f;
		const float gaugeHeight = 5.0f;

		float healthRate = enemy->GetHealth() / static_cast<float>(enemy->GetMaxHealth());

		//�@�Q�[�W�`��
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

	// �G�l�~�[�z�u����
	Mouse& mouse = Input::Instance().GetMouse();
	if (mouse.GetButtonDown() & Mouse::BTN_LEFT)
	{
		// �}�E�X�J�[�\�����W���擾
		DirectX::XMFLOAT3 screenPosition;
		screenPosition.x = static_cast<float>(mouse.GetPositionX());
		screenPosition.y = static_cast<float>(mouse.GetPositionY());

		DirectX::XMVECTOR ScreenPosition, WorldPosition;

		// ���C�̎n�_���Z�o
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

		// ���C�̏I�_���Z�o
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

		// ���C�L���X�g
		HitResult hit;
		if (StageManager::Instance().RayCast(rayStart, rayEnd, hit))
		{
			// �G��z�u
			EnemySlime* slime = new EnemySlime();
			slime->SetPosition(hit.position);
			slime->SetTerritory(hit.position, 10.0f);
			EnemyManager::Instance().Register(slime);
		}
	}


}
