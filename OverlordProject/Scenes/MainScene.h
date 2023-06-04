#pragma once
class PostBlur;
class Player;
class PostPixel;

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

	PostPixel* m_pPostPixel;

	PostBlur* m_pPostBlur;

	float m_PixelRes[2]{256, 224};
};

