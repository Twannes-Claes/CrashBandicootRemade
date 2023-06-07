#pragma once
class WumpaHUD : public GameObject
{
public:
	WumpaHUD() = default;
	virtual ~WumpaHUD() override = default;
	WumpaHUD(const WumpaHUD& other) = delete;
	WumpaHUD(WumpaHUD&& other) noexcept = delete;
	WumpaHUD& operator=(const WumpaHUD& other) = delete;
	WumpaHUD& operator=(WumpaHUD&& other) noexcept = delete;

	static void AddWumpa()
	{
		++m_AmountWumpas;
		m_Text = std::to_wstring(m_AmountWumpas) + L"/" + std::to_wstring(m_TotalWUmpas);

		if (m_TotalWUmpas == m_AmountWumpas)
		{
			m_Speed *= 2.5f;
			m_MaxAngle = 75.f;
		}
	}

	static void AddTotalWumpas()
	{
		++m_TotalWUmpas;
	}

protected:

	void Initialize(const SceneContext&) override;

	void Update(const SceneContext&) override;

	void PostInitialize(const SceneContext&) override;

	void PostDraw(const SceneContext&) override;

private:

	TransformComponent* m_pSpriteTransform{};

	inline static int m_AmountWumpas{};
	inline static int m_TotalWUmpas{};

	inline static float m_Speed{ 60.f };

	inline static float m_MaxAngle{35.f};

	float m_Rotation{};

	SpriteFont* m_pFont{};
	SpriteFont* m_pFontOutline{};

	inline static std::wstring m_Text{};

	XMFLOAT2 m_Pos{};
};

