#pragma once
class CrateParticle : public GameObject
{
public:
	explicit CrateParticle(const XMFLOAT3& pos):m_Pos(pos){}
	virtual ~CrateParticle() override = default;
	CrateParticle(const CrateParticle& other) = delete;
	CrateParticle(CrateParticle&& other) noexcept = delete;
	CrateParticle& operator=(const CrateParticle& other) = delete;
	CrateParticle& operator=(CrateParticle&& other) noexcept = delete;

protected:

	void Initialize(const SceneContext&) override;
	void Update(const SceneContext&) override;

private:

	ParticleEmitterComponent* m_pEmitter{};

	const float m_LifeTime{ 1.5f };
	float m_CurrentLifeTime{};

	const XMFLOAT3 m_Pos{};
};

