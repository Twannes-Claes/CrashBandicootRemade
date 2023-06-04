#pragma once

class PostPixel : public PostProcessingMaterial
{
public:
	PostPixel();
	~PostPixel() override = default;
	PostPixel(const PostPixel& other) = delete;
	PostPixel(PostPixel&& other) noexcept = delete;
	PostPixel& operator=(const PostPixel& other) = delete;
	PostPixel& operator=(PostPixel&& other) noexcept = delete;

	void SetPixelResolution(float x, float y) const;

protected:
	void Initialize(const GameContext& /*gameContext*/) override {}
};

