#pragma once
class Player;

class MainScene final : public GameScene
{
public:

	MainScene() :GameScene(L"Main scene") {}
	virtual ~MainScene() override = default;
	MainScene(const MainScene& other) = delete;
	MainScene(MainScene&& other) noexcept = delete;
	MainScene& operator=(const MainScene& other) = delete;
	MainScene& operator=(MainScene&& other) noexcept = delete;

protected:

	void Update() override;
	void Initialize() override;
	void OnGUI() override;

private:

	Player* m_pPlayer{};
};
