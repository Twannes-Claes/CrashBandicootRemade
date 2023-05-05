#pragma once
class Character;

class CharacterScene final : public GameScene
{
public:

	CharacterScene() :GameScene(L"Test scene") {}
	virtual ~CharacterScene() override = default;
	CharacterScene(const CharacterScene& other) = delete;
	CharacterScene(CharacterScene&& other) noexcept = delete;
	CharacterScene& operator=(const CharacterScene& other) = delete;
	CharacterScene& operator=(CharacterScene&& other) noexcept = delete;

protected:

	void Update() override;
	void Initialize() override;
	void OnGUI() override;

private:
	enum InputIds
	{
		CharacterMoveLeft,
		CharacterMoveRight,
		CharacterMoveForward,
		CharacterMoveBackward,
		CharacterJump
	};

	Character* m_pCharacter{};

	bool m_LockCursor{ true };
};

