#pragma once
//Resharper Disable All

	class PostPixel : public PostProcessingMaterial
	{
	public:
		PostPixel();
		~PostPixel() override = default;
		PostPixel(const PostPixel& other) = delete;
		PostPixel(PostPixel&& other) noexcept = delete;
		PostPixel& operator=(const PostPixel& other) = delete;
		PostPixel& operator=(PostPixel&& other) noexcept = delete;
		
	protected:
		void Initialize(const GameContext& /*gameContext*/) override {}
	};

