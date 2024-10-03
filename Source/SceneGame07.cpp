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

// ������
void SceneGame07::Initialize()
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
	
	// �Q�[�W�X�v���C�g
	gauge = new Sprite();

	earth = std::make_unique<Model>("Data/Model/Earth/earth.mdl");
	bomb_texture = std::make_unique<Texture>("Data/Effect/Texture/Explosion01_5x5.png");
	particle_bomb = std::make_unique<cParticleSystem>(bomb_texture->GetShaderResourceView().Get(), 5, 5, true, 3000);

	particle_texture = std::make_unique<Texture>("Data/Effect/Texture/particle256x256.png");
	particle_alpha = std::make_unique<cParticleSystem>(particle_texture->GetShaderResourceView().Get(), 4, 4, false, 10000);
	particle_add = std::make_unique<cParticleSystem>(particle_texture->GetShaderResourceView().Get(), 4, 4, false, 10000);
	particle_subtract = std::make_unique<cParticleSystem>(particle_texture->GetShaderResourceView().Get(), 4, 4, false, 10000);



}

// �I����
void SceneGame07::Finalize()
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
void SceneGame07::Update(float elapsedTime)
{
	// �J�����R���g���[���[�X�V����
	DirectX::XMFLOAT3 target = player->GetPosition();
	target.y += 0.5f;
	cameraController->SetTarget(target);
	cameraController->Update(elapsedTime);

	// �X�e�[�W�X�V����
	StageManager::Instance().Update(elapsedTime);
	waterSurface->Update(elapsedTime);
	//timer += elapsedTime;

	// �v���C���[�X�V����
	player->Update(elapsedTime);

	////  ��
	//sky->Update(elapsedTime);
	// �G�l�~�[�X�V����
	EnemyManager::Instance().Update(elapsedTime);

	// �G�t�F�N�g�X�V����
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

// �`�揈��

void SceneGame07::RenderShadow()
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
		DirectX::XMMATRIX Projection = DirectX::XMMatrixOrthographicLH(SHADOW_AREA.x, SHADOW_AREA.y, 0.1f, 100.0f);
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

void SceneGame07::Render()
{
	RenderShadow();
	//RenderReflect();
	Graphics& graphics = Graphics::Instance();
	ID3D11DeviceContext* dc = graphics.GetDeviceContext();
	ID3D11RenderTargetView* rtv = graphics.GetRenderTargetView();
	ID3D11DepthStencilView* dsv = graphics.GetDepthStencilView();

	// ��ʃN���A�������_�[�^�[�Q�b�g�ݒ�
	FLOAT color[] = { 0.2f, 0.2f, 0.2f, 1.0f };	// RGBA(0.0�`1.0)
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
		//�@�t�H���V�F�[�_
		Shader* shader = graphics.GetShader(static_cast<int>(ShaderType::PhongDrawShadowShader));
		shader->Begin(dc, rc);
		
		// �X�e�[�W�`��
		StageManager::Instance().Render(dc, shader);


		// �G�l�~�[�`��
		EnemyManager::Instance().Render(dc, shader);

		//// �v���C���[�`��
		//player->Render(dc, shader);

		shader->End(dc);
	}
	{
		//�@�g�D�[���V�F�[�_
		Shader* shader = graphics.GetShader(static_cast<int>(ShaderType::PhongDrawShadowShader));
		shader->Begin(dc, rc);

		// �v���C���[�`��
		player->Render(dc, shader);

		shader->End(dc);
	}


	{
		//�@����
		//Shader* shader = graphics.GetShader(static_cast<int>(ShaderType::WaterSurfaceShader));
		Shader* shader = graphics.GetShader();
		shader->Begin(dc, rc);

		// ���ʕ`��
		waterSurface->Render(dc, shader);

		shader->End(dc);
	}

	// ���j�p�[�e�B�N���A�j���[�V����
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


	// 3D�G�t�F�N�g�`��
	{
		EffectManager::Instance().Render(rc.view, rc.projection);
	}

	// �X�J�C�h�[��
	//{
	//	skybox->Render(dc, rc);
	//}

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
	//{
	//	ImGui::SetNextWindowPos(ImVec2(10, 200), ImGuiCond_FirstUseEver);
	//	ImGui::SetNextWindowSize(ImVec2(300, 300), ImGuiCond_FirstUseEver);
	//	bool show_another_window = true;
	//	if (ImGui::Begin(u8"���t���N�V�����}�b�v", nullptr, ImGuiWindowFlags_None))
	//	{
	//		const int w = static_cast<float>(REFLECT_SIZE.x) / 2;
	//		const int h = static_cast<float>(REFLECT_SIZE.y) / 2;
	//		//ImGui::Text(u8"�X�N���[���摜");
	//		ImGui::Image(reflectMap->GetShaderResourceView(), { w, h });

	//	}
	//	ImGui::End();
	//}

}

// �G�l�~�[HP�Q�[�W�`��
void SceneGame07::RenderEnemyGauge(
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
