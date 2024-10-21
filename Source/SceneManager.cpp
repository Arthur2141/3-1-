#include "Graphics/Graphics.h"

#include "SceneManager.h"
#include "SceneLoading.h"
#include "Scenesousa.h"
#include "SceneGame00.h"
#include "SceneGame01.h"
#include "SceneGame02.h"
#include "SceneGame03.h"
#include "SceneGame04.h"
#include "SceneGame05.h"
#include "SceneGame06.h"
#include "SceneGame07.h"


// �X�V����
void SceneManager::Update(float elapsedTime)
{
	if (nextScene != nullptr)
	{
		// �Â��V�[�����I������
		Clear();

		// �V�����V�[����ݒ�
		currentScene = nextScene;
		nextScene = nullptr;

		// �V�[������������
		if (!currentScene->IsReady())
		{
			currentScene->Initialize();
		}
	}

	if (currentScene != nullptr)
	{
		currentScene->Update(elapsedTime);
	}
}

// �`�揈��
void SceneManager::Render()
{
	imGuiSceneChanger();

	if (currentScene != nullptr)
	{
		currentScene->Render();
	}
}

// �V�[���N���A
void SceneManager::Clear()
{
	if (currentScene != nullptr)
	{
		currentScene->Finalize();
		delete currentScene;
		currentScene = nullptr;
	}
}

// �V�[���؂�ւ�
void SceneManager::ChangeScene(Scene* scene)
{
	nextScene = scene;
}


void SceneManager::imGuiSceneChanger()
{
	/*ImGui::Begin(u8"�V�[���ύX");
	if (ImGui::Button(u8"�����V�[��"))
	{
		ChangeScene(new SceneLoading(new Scenesousa));

	}
	if (ImGui::Button(u8"�������"))
	{
		ChangeScene(new SceneLoading(new SceneGame00));
	}
	if (ImGui::Button(u8"SceneGame01"))
	{
		ChangeScene(new SceneLoading(new SceneGame01));
	}

	if (ImGui::Button(u8"SceneGame02"))
	{
		ChangeScene(new SceneLoading(new SceneGame02));
	}
	if (ImGui::Button(u8"SceneGame03"))
	{
		ChangeScene(new SceneLoading(new SceneGame03));
	}
	if (ImGui::Button(u8"SceneGame04"))
	{
		ChangeScene(new SceneLoading(new SceneGame04));
	}
	if (ImGui::Button(u8"SceneGame05"))
	{
		ChangeScene(new SceneLoading(new SceneGame05));
	}
	if (ImGui::Button(u8"SceneGame06"))
	{
		ChangeScene(new SceneLoading(new SceneGame06));
	}

	if (ImGui::Button(u8"SceneGame07"))
	{
		ChangeScene(new SceneLoading(new SceneGame07));
	}


	ImGui::End();*/
}
