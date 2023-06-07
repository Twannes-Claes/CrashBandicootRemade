#pragma once
class SpinParticle : public GameObject
{
public:
	explicit SpinParticle(const XMFLOAT3& pos) :m_Pos(pos) {}
	virtual ~SpinParticle() override = default;
	SpinParticle(const SpinParticle& other) = delete;
	SpinParticle(SpinParticle&& other) noexcept = delete;
	SpinParticle& operator=(const SpinParticle& other) = delete;
	SpinParticle& operator=(SpinParticle&& other) noexcept = delete;

protected:

	void Initialize(const SceneContext&) override;

private:

	const XMFLOAT3 m_Pos{};
};

