#pragma once

class Level final : public GameObject
{
public:

	Level(GameScene* scene);
	virtual ~Level() override = default;
	Level(const Level& other) = delete;
	Level(Level&& other) noexcept = delete;
	Level& operator=(const Level& other) = delete;
	Level& operator=(Level&& other) noexcept = delete;

	void DrawImGUI(){};

protected:

	void Initialize(const SceneContext&) override;
	void Update(const SceneContext&) override{};

private:

	void SpawnCrate(float x, float y, float z, bool hasWumpa);

	GameScene* m_pGameScene{};

};

