#include "stdafx.h"
#include "Character.h"

Character::Character(const CharacterDesc& characterDesc) :
	m_CharacterDesc{ characterDesc },
	m_MoveAcceleration(characterDesc.maxMoveSpeed / characterDesc.moveAccelerationTime),
	m_FallAcceleration(characterDesc.maxFallSpeed / characterDesc.fallAccelerationTime)
{}

void Character::Initialize(const SceneContext& /*sceneContext*/)
{
	//Controller
	m_pControllerComponent = AddComponent(new ControllerComponent(m_CharacterDesc.controller));

	m_pControllerComponent->SetCollisionGroup(CollisionGroup::Group9);

	//Camera
	const auto pCamera = AddChild(new FixedCamera());
	m_pCameraComponent = pCamera->GetComponent<CameraComponent>();

	m_pCameraComponent->SetActive(true); //Uncomment to make this camera the active camera

	pCamera->GetTransform()->Translate(0.f, m_CharacterDesc.controller.height * .5f, 0.f);

	pCamera->GetTransform()->Rotate(m_CharacterDesc.lookAtPitch, 0.f, 0.f);
}

void Character::Update(const SceneContext& sceneContext)
{
	if (m_pCameraComponent->IsActive())
	{
		constexpr float epsilon{ 0.01f }; //Constant that can be used to compare if a float is near zero
		const float elapsedSeconds = sceneContext.pGameTime->GetElapsed();

		//***************
		//HANDLE INPUT

		//## Input Gathering (move)
		XMFLOAT2 move{};
		//move.y should contain a 1 (Forward) or -1 (Backward) based on the active input (check corresponding actionId in m_CharacterDesc)

		if(sceneContext.pInput->IsActionTriggered(m_CharacterDesc.actionId_MoveForward))
		{
			move.y = 1.f;
		}
		if(sceneContext.pInput->IsActionTriggered(m_CharacterDesc.actionId_MoveBackward))
		{
			move.y = -1.f;
		}

		//Optional: if move.y is near zero (abs(move.y) < epsilon), you could use the ThumbStickPosition.y for movement
		if(abs(move.y) < epsilon)
		{
			move.y = InputManager::GetThumbstickPosition().y;
		}

		//move.x should contain a 1 (Right) or -1 (Left) based on the active input (check corresponding actionId in m_CharacterDesc)

		if (sceneContext.pInput->IsActionTriggered(m_CharacterDesc.actionId_MoveRight))
		{
			move.x = 1.f;
		}
		if (sceneContext.pInput->IsActionTriggered(m_CharacterDesc.actionId_MoveLeft))
		{
			move.x = -1.f;
		}

		//Optional: if move.x is near zero (abs(move.x) < epsilon), you could use the Left ThumbStickPosition.x for movement
		if (abs(move.x) < epsilon)
		{
			move.x = InputManager::GetThumbstickPosition().x;
		}

		m_InputAxis = move;

		//## Input Gathering (look)
		XMFLOAT2 look{ 0.f, 0.f };

		//Only if the Left Mouse Button is Down >
			// Store the MouseMovement in the local 'look' variable (cast is required)

		if (InputManager::IsMouseButton(InputState::down, 1) || m_CharacterDesc.followMouse)
		{
			const auto& mouseMovement = InputManager::GetMouseMovement();
			look.x = static_cast<float>(mouseMovement.x);
			look.y = static_cast<float>(mouseMovement.y);
		}

		//Optional: in case look.x AND look.y are near zero, you could use the Right ThumbStickPosition for look

		if(abs(look.x) < epsilon && abs(look.y) < epsilon)
		{
			const XMFLOAT2 thumbPos = InputManager::GetThumbstickPosition(false);

			XMVECTOR thumbstickVector = XMLoadFloat2(&thumbPos);

			thumbstickVector = XMVectorSetX(thumbstickVector, -1.0f * XMVectorGetX(thumbstickVector));

			thumbstickVector = XMVectorScale(thumbstickVector, -(m_CharacterDesc.controllerRotationSpeed));

			XMStoreFloat2(&look, thumbstickVector);
		}

		if(InputManager::IsMouseCentered())
		{
			look = XMFLOAT2(0, 0);
		}

		//************************
		//GATHERING TRANSFORM INFO


		//Retrieve the TransformComponent
		const auto pTransform = GetTransform();
		//Retrieve the forward & right vector (as XMVECTOR) from the TransformComponent

		const XMVECTOR forward = XMLoadFloat3(&pTransform->GetForward());
		const XMVECTOR right = XMLoadFloat3(&pTransform->GetRight());

		//***************
		//CAMERA ROTATION

		//Adjust the TotalYaw (m_TotalYaw) & TotalPitch (m_TotalPitch) based on the local 'look' variable
		//Make sure this calculated on a framerate independent way and uses CharacterDesc::rotationSpeed.

		const float angle = elapsedSeconds * m_CharacterDesc.rotationSpeed;

		m_TotalYaw += look.x * angle;

		//Rotate this character based on the TotalPitch (X) and TotalYaw (Y)
		pTransform->Rotate(0.f, m_TotalYaw, 0.f);

		//clamp pitch

		if(m_CharacterDesc.hasStaticPitch == false)
		{
			m_TotalPitch += look.y * angle;
			m_TotalPitch = std::clamp(m_TotalPitch, m_CharacterDesc.minPitch, m_CharacterDesc.maxPitch);

			//rotate camera not character
			sceneContext.pCamera->GetTransform()->Rotate(m_TotalPitch, 0.f, 0.f);

			//std::cout << m_TotalPitch << "\n";
		}
		

		//********
		//MOVEMENT

		//## Horizontal Velocity (Forward/Backward/Right/Left)
		//Calculate the current move acceleration for this frame (m_MoveAcceleration * ElapsedTime)

		const float currMoveAccel = m_MoveAcceleration * elapsedSeconds;

		//If the character is moving (= input is pressed)
		if (abs(move.x) > epsilon || abs(move.y) > epsilon)
		{
			//Calculate & Store the current direction (m_CurrentDirection) >> based on the forward/right vectors and the pressed input

			const XMVECTOR dir{ forward * move.y + right * move.x };
			XMStoreFloat3(&m_CurrentDirection, dir);

			//Increase the current MoveSpeed with the current Acceleration (m_MoveSpeed)

			m_MoveSpeed += currMoveAccel;

			//Make sure the current MoveSpeed stays below the maximum MoveSpeed (CharacterDesc::maxMoveSpeed)
		}
		//Else (character is not moving, or stopped moving)
		else
		{
			//Decrease the current MoveSpeed with the current Acceleration (m_MoveSpeed)

			m_MoveSpeed -= currMoveAccel;

			//Make sure the current MoveSpeed doesn't get smaller than zero
		}

		m_MoveSpeed = std::clamp(m_MoveSpeed, 0.f, m_CharacterDesc.maxMoveSpeed);

		//Now we can calculate the Horizontal Velocity which should be stored in m_TotalVelocity.xz
		//Calculate the horizontal velocity (m_CurrentDirection * MoveSpeed)
		//Set the x/z component of m_TotalVelocity (horizontal_velocity x/z)
		//It's important that you don't overwrite the y component of m_TotalVelocity (contains the vertical velocity)

		m_TotalVelocity.x = m_CurrentDirection.x * m_MoveSpeed;
		m_TotalVelocity.z = m_CurrentDirection.z * m_MoveSpeed;

		//## Vertical Movement (Jump/Fall)
		//If the Controller Component is NOT grounded (= freefall)

		//try on the raycast but dont know how to filtert out the player collision

		if ((m_pControllerComponent->GetCollisionFlags() & PxControllerCollisionFlag::eCOLLISION_DOWN) == false)
		{
			//Decrease the y component of m_TotalVelocity with a fraction (ElapsedTime) of the Fall Acceleration (m_FallAcceleration)
			m_TotalVelocity.y -= elapsedSeconds * m_FallAcceleration;
			//Make sure that the minimum speed stays above -CharacterDesc::maxFallSpeed (negative!)

			if (m_TotalVelocity.y < -m_CharacterDesc.maxFallSpeed)
			{
				m_TotalVelocity.y = -m_CharacterDesc.maxFallSpeed;
			}

			m_Grounded = false;
		}
		//Else If the jump action is triggered
		else if (sceneContext.pInput->IsActionTriggered(m_CharacterDesc.actionId_Jump))
		{
			//Set m_TotalVelocity.y equal to CharacterDesc::JumpSpeed
			m_TotalVelocity.y = m_CharacterDesc.JumpSpeed;
			m_Grounded = true;
		}
		//Else (=Character is grounded, no input pressed)
		else
		{
			//m_TotalVelocity.y is zero
			m_TotalVelocity.y -= 0.01f;
			m_Grounded = true;
		}

		//************
		//DISPLACEMENT

		//The displacement required to move the Character Controller (ControllerComponent::Move) can be calculated using our TotalVelocity (m/s)
		//Calculate the displacement (m) for the current frame and move the ControllerComponent

		const XMFLOAT3 displacement
		{
			m_TotalVelocity.x * elapsedSeconds,
			m_TotalVelocity.y * elapsedSeconds,
			m_TotalVelocity.z * elapsedSeconds
		};
		m_pControllerComponent->Move(displacement);

		//The above is a simple implementation of Movement Dynamics, adjust the code to further improve the movement logic and behaviour.
		//Also, it can be usefull to use a seperate RayCast to check if the character is grounded (more responsive)
	}
}

void Character::DrawImGui()
{
	if (ImGui::CollapsingHeader("Character"))
	{
		ImGui::Text(std::format("Move Speed: {:0.1f} m/s", m_MoveSpeed).c_str());
		ImGui::Text(std::format("Fall Speed: {:0.1f} m/s", m_TotalVelocity.y).c_str());

		ImGui::Text(std::format("Move Acceleration: {:0.1f} m/s2", m_MoveAcceleration).c_str());
		ImGui::Text(std::format("Fall Acceleration: {:0.1f} m/s2", m_FallAcceleration).c_str());

		const float jumpMaxTime = m_CharacterDesc.JumpSpeed / m_FallAcceleration;
		const float jumpMaxHeight = (m_CharacterDesc.JumpSpeed * jumpMaxTime) - (0.5f * (m_FallAcceleration * powf(jumpMaxTime, 2)));
		ImGui::Text(std::format("Jump Height: {:0.1f} m", jumpMaxHeight).c_str());

		ImGui::Dummy({ 0.f,5.f });
		if (ImGui::DragFloat("Max Move Speed (m/s)", &m_CharacterDesc.maxMoveSpeed, 0.1f, 0.f, 0.f, "%.1f") ||
			ImGui::DragFloat("Move Acceleration Time (s)", &m_CharacterDesc.moveAccelerationTime, 0.1f, 0.f, 0.f, "%.1f"))
		{
			m_MoveAcceleration = m_CharacterDesc.maxMoveSpeed / m_CharacterDesc.moveAccelerationTime;
		}

		ImGui::Dummy({ 0.f,5.f });
		if (ImGui::DragFloat("Max Fall Speed (m/s)", &m_CharacterDesc.maxFallSpeed, 0.1f, 0.f, 0.f, "%.1f") ||
			ImGui::DragFloat("Fall Acceleration Time (s)", &m_CharacterDesc.fallAccelerationTime, 0.1f, 0.f, 0.f, "%.1f"))
		{
			m_FallAcceleration = m_CharacterDesc.maxFallSpeed / m_CharacterDesc.fallAccelerationTime;
		}

		ImGui::Dummy({ 0.f,5.f });
		ImGui::DragFloat("Jump Speed", &m_CharacterDesc.JumpSpeed, 0.1f, 0.f, 0.f, "%.1f");
		ImGui::DragFloat("Rotation Speed (deg/s)", &m_CharacterDesc.rotationSpeed, 0.1f, 0.f, 0.f, "%.1f");

		bool isActive = m_pCameraComponent->IsActive();
		if(ImGui::Checkbox("Character Camera", &isActive))
		{
			m_pCameraComponent->SetActive(isActive);
		}
	}
}