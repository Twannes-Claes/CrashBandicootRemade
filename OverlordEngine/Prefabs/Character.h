#pragma once
struct CharacterDesc
{
	CharacterDesc
	(
		PxMaterial* pMaterial,
		float radius = .5f,
		float height = 2.f
	)
	{
		controller.setToDefault();
		controller.radius = radius;
		controller.height = height;
		controller.material = pMaterial;
	}

	float maxMoveSpeed{ 15.f };
	float maxFallSpeed{ 15.f };

	float JumpSpeed{ 15.f };

	float moveAccelerationTime{ .3f };
	float fallAccelerationTime{ .3f };

	PxCapsuleControllerDesc controller{};

	float rotationSpeed{ 60.f };
	float controllerRotationSpeed{ 60.f };

	int actionId_MoveLeft{ -1 };
	int actionId_MoveRight{ -1 };
	int actionId_MoveForward{ -1 };
	int actionId_MoveBackward{ -1 };
	int actionId_Jump{ -1 };

	float minPitch{-90.f};
	float maxPitch{90.f};

	float lookAtPitch{-20.f};
	bool hasStaticPitch{false};

	bool followMouse{ false };
};

class Character : public GameObject
{
public:

	Character(const CharacterDesc& characterDesc);
	~Character() override = default;

	Character(const Character& other) = delete;
	Character(Character&& other) noexcept = delete;
	Character& operator=(const Character& other) = delete;
	Character& operator=(Character&& other) noexcept = delete;

	void DrawImGui();

	CameraComponent* GetCamera() const { return m_pCameraComponent; }

	XMFLOAT3 GetVelocity() const { return m_TotalVelocity; }
	XMFLOAT2 GetInputAxis() const { return m_InputAxis; }

	bool OnGround() const { return m_Grounded; }

protected:

	void Initialize(const SceneContext&) override;
	void Update(const SceneContext&) override;

private:



	CameraComponent* m_pCameraComponent{};
	ControllerComponent* m_pControllerComponent{};

	CharacterDesc m_CharacterDesc;

	float m_TotalPitch{}, m_TotalYaw{};				//Total camera Pitch(X) and Yaw(Y) rotation
	float m_MoveAcceleration{},						//Acceleration required to reach maxMoveVelocity after 1 second (maxMoveVelocity / moveAccelerationTime)
		m_FallAcceleration{},						//Acceleration required to reach maxFallVelocity after 1 second (maxFallVelocity / fallAccelerationTime)
		m_MoveSpeed{};								//MoveSpeed > Horizontal Velocity = MoveDirection * MoveVelocity (= TotalVelocity.xz)

	bool m_Grounded{};

	XMFLOAT3 m_TotalVelocity{};						//TotalVelocity with X/Z for Horizontal Movement AND Y for Vertical Movement (fall/jump)
	XMFLOAT3 m_CurrentDirection{};					//Current/Last Direction based on Camera forward/right (Stored for deacceleration)

	XMFLOAT2 m_InputAxis{};
};

