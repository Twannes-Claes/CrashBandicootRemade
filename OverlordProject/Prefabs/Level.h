#pragma once
#include "Crate.h"

class Level final : public GameObject
{
public:

	Level(GameScene* scene);
	virtual ~Level() override = default;
	Level(const Level& other) = delete;
	Level(Level&& other) noexcept = delete;
	Level& operator=(const Level& other) = delete;
	Level& operator=(Level&& other) noexcept = delete;

	void DrawImGUI() const {};

protected:

	void Initialize(const SceneContext&) override;
	void Update(const SceneContext&) override{};

private:

	FMOD::Sound* m_pMusic{ nullptr };
	FMOD::Channel* m_pChannel{ nullptr };

	void AddBorderWalls();

	void SpawnCrate(float x, float y, float z, Crate::CrateState state);

	GameScene* m_pGameScene{};

};


