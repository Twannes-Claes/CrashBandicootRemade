#pragma once
class Character;

class Player final : public GameObject
{
public:

	Player() = default;
	virtual ~Player() override = default;
	Player(const Player& other) = delete;
	Player(Player&& other) noexcept = delete;
	Player& operator=(const Player& other) = delete;
	Player& operator=(Player&& other) noexcept = delete;

	void DrawImGUI() const;

	static bool IsPlayerAttacking()
	{
		if (m_CurrentState == Attack) return true;
		return false;
	}

protected:

	void Initialize(const SceneContext&) override;
	void Update(const SceneContext&) override;

private:
	enum InputIds
	{
		CharacterMoveLeft,
		CharacterMoveRight,
		CharacterMoveForward,
		CharacterMoveBackward,
		CharacterJump,
		CharacterAttackStart,
		CharacterAttackEnd
	};

	enum PlayerState
	{
		Idle,
		Jump,
		Run,
		Attack
	};

	void SetState(const PlayerState& state);
	void CheckJump(const SceneContext& sceneContext);

	void ChecKAttack(const SceneContext& sceneContext);

	inline static PlayerState m_CurrentState{ Idle };

	Character* m_pCharacter{};

	GameObject* m_pPlayer{};

	ModelAnimator* m_pAnimator{};

	bool m_LockCursor{ true };
};

