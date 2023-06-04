#pragma once


class PostBlur final: public PostProcessingMaterial
{
public:
	PostBlur();
	~PostBlur() override = default;
	PostBlur(const PostBlur& other) = delete;
	PostBlur(PostBlur&& other) noexcept = delete;
	PostBlur& operator=(const PostBlur& other) = delete;
	PostBlur& operator=(PostBlur&& other) noexcept = delete;

	void SetOffset(float ) const;
	void SetSamples(int n) const;

protected:
	void Initialize(const GameContext& /*gameContext*/) override {}
};

