#pragma once
class Level;

class Crate final : public GameObject
{
public:

	Crate(const XMFLOAT3& pos, bool hasWumpa, Level* pLevel);
	virtual ~Crate() override = default;
	Crate(const Crate& other) = delete;
	Crate(Crate&& other) noexcept = delete;
	Crate& operator=(const Crate& other) = delete;
	Crate& operator=(Crate&& other) noexcept = delete;

	static float GetCrateSize() { return m_Scale * 2; }

protected:

	void Initialize(const SceneContext&) override;
	void Update(const SceneContext&) override;

private:

	void OnTriggerEvent(GameObject* pTriggerObject, GameObject* pOtherObject, PxTriggerAction action);

	void SpawnWumpa() const;

	const XMFLOAT3 m_Position{};

	inline constexpr static float m_Scale{ 0.75f };

	const bool m_HasWumpa{};

	bool m_PlayerInside{};

	Level* m_pLevel{};
};

