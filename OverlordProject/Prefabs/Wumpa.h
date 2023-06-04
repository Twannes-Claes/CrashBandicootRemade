#pragma once
class Wumpa : public GameObject
{
public:
	explicit Wumpa(const XMFLOAT3& pos);
	virtual ~Wumpa() override = default;
	Wumpa(const Wumpa& other) = delete;
	Wumpa(Wumpa&& other) noexcept = delete;
	Wumpa& operator=(const Wumpa& other) = delete;
	Wumpa& operator=(Wumpa&& other) noexcept = delete;

protected:

	void Initialize(const SceneContext&) override;
	void Update(const SceneContext&) override;

private:

	const XMFLOAT3 m_SpawnPos{};

	const PxMaterial* m_pMat{};

	GameObject* m_pObject{};

	RigidBodyComponent* m_pRigid{};

	bool m_CanDelete{};

	//-1 is cant add, 1 is can add, 0 is already added
	int m_CanAddCollider{-1};

	float m_CollisionTimer{0.5f};

	void OnTriggerEvent(GameObject* pTriggerObject, GameObject* pOtherObject, PxTriggerAction action);
};

