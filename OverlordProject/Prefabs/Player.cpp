#include "stdafx.h"
#include "Player.h"

#include "Materials/DiffuseMaterial.h"
#include "Materials/DiffuseMaterial_Skinned.h"
#include "Materials/Shadow/DiffuseMaterial_Shadow_Skinned.h"
#include "Prefabs/Character.h"

void Player::Initialize(const SceneContext& sceneContext)
{
	InputManager::ForceMouseToCenter(m_LockCursor);

	sceneContext.pLights->SetDirectionalLight({ -95.6139526f,66.1346436f,-41.1850471f }, { 0.740129888f, -0.597205281f, 0.309117377f });

	const auto pDefaultMaterial = PxGetPhysics().createMaterial(0.5f, 0.5f, 0.5f);

	//Character
	CharacterDesc characterDesc{ pDefaultMaterial };

	characterDesc.actionId_MoveForward = CharacterMoveForward;
	characterDesc.actionId_MoveBackward = CharacterMoveBackward;
	characterDesc.actionId_MoveLeft = CharacterMoveLeft;
	characterDesc.actionId_MoveRight = CharacterMoveRight;
	characterDesc.actionId_Jump = CharacterJump;

	characterDesc.rotationSpeed = 5.f;
	characterDesc.controllerRotationSpeed = 40.f;

	characterDesc.minPitch = -5.f;
	characterDesc.maxPitch = 15.f;

	characterDesc.hasStaticPitch = false;
	characterDesc.lookAtPitch = 20.f;

	characterDesc.followMouse = true;

	//Input
	auto inputAction = InputAction(CharacterMoveLeft, InputState::down, 'A');
	sceneContext.pInput->AddInputAction(inputAction);

	inputAction = InputAction(CharacterMoveRight, InputState::down, 'D');
	sceneContext.pInput->AddInputAction(inputAction);

	inputAction = InputAction(CharacterMoveForward, InputState::down, 'W');
	sceneContext.pInput->AddInputAction(inputAction);

	inputAction = InputAction(CharacterMoveBackward, InputState::down, 'S');
	sceneContext.pInput->AddInputAction(inputAction);

	inputAction = InputAction(CharacterJump, InputState::pressed, VK_SPACE, -1, XINPUT_GAMEPAD_A);
	sceneContext.pInput->AddInputAction(inputAction);

	inputAction = InputAction(CharacterAttackStart, InputState::pressed, -1, 2 , XINPUT_GAMEPAD_RIGHT_SHOULDER);
	sceneContext.pInput->AddInputAction(inputAction);

	inputAction = InputAction(CharacterAttackEnd, InputState::released, -1, 2, XINPUT_GAMEPAD_RIGHT_SHOULDER);
	sceneContext.pInput->AddInputAction(inputAction);

	//Character
	const auto pMatSkin = MaterialManager::Get()->CreateMaterial<DiffuseMaterial_Shadow_Skinned>();
	pMatSkin->SetDiffuseTexture(L"Textures/CrashSkin.dds");

	const auto pMatBack = MaterialManager::Get()->CreateMaterial<DiffuseMaterial_Shadow_Skinned>();
	pMatBack->SetDiffuseTexture(L"Textures/CrashBack.dds");

	const auto pMatShoes = MaterialManager::Get()->CreateMaterial<DiffuseMaterial_Shadow_Skinned>();
	pMatShoes->SetDiffuseTexture(L"Textures/CrashShoes.dds");

	m_pCharacter = AddChild(new Character(characterDesc));
	m_pCharacter->GetTransform()->Translate(0.f, 5.f, 0.f);

	m_pCharacter->GetCamera()->GetTransform()->Translate(0.f, 1.5f, -4.f);

	m_pPlayer = m_pCharacter->AddChild(new GameObject);
	const auto pModel = m_pPlayer->AddComponent(new ModelComponent(L"Meshes/Crash_Animations.ovm"));

	pModel->SetMaterial(pMatSkin, 0);
	pModel->SetMaterial(pMatShoes,0);
	pModel->SetMaterial(pMatBack, 2);

	m_pPlayer->GetTransform()->Scale(0.015f, 0.015f, 0.015f);
	m_pPlayer->GetTransform()->Rotate(0.f, 180.f, 0.f);
	m_pPlayer->GetTransform()->Translate(0.f, -1.3f, 0.f);

	m_pAnimator = pModel->GetAnimator();

	if(m_pAnimator)
	{
		m_pAnimator->SetAnimationSpeed(200);
		m_pAnimator->SetAnimation(Idle);
		m_pAnimator->Play();
	} 
}

void Player::Update(const SceneContext& sceneContext)
{
	if (InputManager::IsKeyboardKey(InputState::pressed, VK_LCONTROL))
	{
		m_LockCursor = !m_LockCursor;

		InputManager::ForceMouseToCenter(m_LockCursor);
	}

	const auto velocity = m_pCharacter->GetVelocity();
	const auto input = m_pCharacter->GetInputAxis();

	const float rotationRad = std::atan2(-input.x, -input.y);

	m_pPlayer->GetTransform()->Rotate(0, rotationRad, 0, false);

	if (!m_pAnimator) return;

	bool idling{};

	if (abs(velocity.x) > 0 || abs(velocity.z) > 0) idling = false;
	if (velocity.x == 0 && velocity.z == 0) idling = true;

	switch (m_CurrentState)
	{
		case Idle:
		{
			if (idling == false) SetState(Run);

			ChecKAttack(sceneContext);
			CheckJump(sceneContext);

			break;
		}
		case Jump:
		{
			if (m_pCharacter->OnGround() && m_pCharacter->GetVelocity().y <= 0) SetState(Idle);

			ChecKAttack(sceneContext);

			break;
		}
		case Run:
		{
			if (idling) SetState(Idle);

			ChecKAttack(sceneContext);
			CheckJump(sceneContext);
			break;
		}
		case Attack:
		{
			if (sceneContext.pInput->IsActionTriggered(CharacterAttackEnd))
			{
				SetState(Idle);
			}

			CheckJump(sceneContext);
			break;
		}
		default:
			break;
	}
}

void Player::SetState(const PlayerState& state) const
{
	m_pAnimator->SetAnimation(state);
	m_CurrentState = state;
}

void Player::CheckJump(const SceneContext& sceneContext)
{
	if(sceneContext.pInput->IsActionTriggered(CharacterJump))
	{
		SetState(Jump);
	}
}

void Player::ChecKAttack(const SceneContext& sceneContext)
{
	if (m_CurrentState != Attack && sceneContext.pInput->IsActionTriggered(CharacterAttackStart))
	{
		SetState(Attack);
	}
}

void Player::DrawImGUI() const
{
	m_pCharacter->DrawImGui();
}
