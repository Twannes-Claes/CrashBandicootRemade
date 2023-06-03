#pragma once
class Crate final : public GameObject
{
public:

	Crate(float x, float y, float z, bool hasWumpa);
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

	PxVec3 m_Position{};

	inline constexpr static float m_Scale{ 0.75f };

	const bool m_HasWumpa{};

	bool m_PlayerInside{};

};

