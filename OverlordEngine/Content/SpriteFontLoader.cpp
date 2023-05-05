#include "stdafx.h"
#include "SpriteFontLoader.h"

SpriteFont* SpriteFontLoader::LoadContent(const ContentLoadInfo& loadInfo)
{
	const auto pReader = new BinaryReader();
	pReader->Open(loadInfo.assetFullPath);

	if (!pReader->Exists())
	{
		Logger::LogError(L"Failed to read the assetFile!\nPath: \'{}\'", loadInfo.assetSubPath);
		return nullptr;
	}
	//See BMFont Documentation for Binary Layout

	const auto byte1 = pReader->Read<char>();
	const auto byte2 = pReader->Read<char>();
	const auto byte3 = pReader->Read<char>();

	//Parse the Identification bytes (B,M,F)
	//If Identification bytes doesn't match B|M|F,
	//Log Error (SpriteFontLoader::LoadContent > Not a valid .fnt font) &
	//return nullptr

	if (byte1 != 'B' && byte2 != 'M' && byte3 != 'F')
	{
		Logger::LogError(L"Not a valid .fnt font\n");
		return nullptr;
	}

	//Parse the version (version 3 required)
	//If version is < 3,
	//Log Error (SpriteFontLoader::LoadContent > Only .fnt version 3 is supported)
	//return nullptr

	const auto version = pReader->Read<char>();

	if (version < 3)
	{
		Logger::LogError(L"Only .fnt version 3 is supported\n");
		return nullptr;
	}

	//Valid .fnt file >> Start Parsing!
	//use this SpriteFontDesc to store all relevant information (used to initialize a SpriteFont object)
	SpriteFontDesc fontDesc{};

	//**********
	// BLOCK 0 *
	//**********
	//Retrieve the blockId and blockSize
	char blockId = pReader->Read<char>();
	int blockSize = pReader->Read<int>();
	//Retrieve the FontSize [fontDesc.fontSize]
	fontDesc.fontSize = pReader->Read<short>();
	//Move the binreader to the start of the FontName [BinaryReader::MoveBufferPosition(...) or you can set its position using BinaryReader::SetBufferPosition(...))
	pReader->MoveBufferPosition(12);
	//Retrieve the FontName [fontDesc.fontName]
	fontDesc.fontName = pReader->ReadNullString();
	//...

	//**********
	// BLOCK 1 *
	//**********
	//Retrieve the blockId and blockSize
	blockId = pReader->Read<char>();
	blockSize = pReader->Read<int>();
	//Retrieve Texture Width & Height [fontDesc.textureWidth/textureHeight]
	//first move to get the scales

	pReader->MoveBufferPosition(4);

	fontDesc.textureWidth = pReader->Read<unsigned short>();
	fontDesc.textureHeight = pReader->Read<unsigned short>();


	//Retrieve PageCount
	//> if pagecount > 1
	if(const unsigned short pageCount{ pReader->Read<unsigned short>() }; pageCount > 1)
	{
		//	> Log Error (Only one texture per font is allowed!)
		Logger::LogError(L"Only one texture per font is allowed!");
	}
	//Advance to Block2 (Move Reader)
	pReader->MoveBufferPosition(5);
	//...

	//**********
	// BLOCK 2 *
	//**********
	//Retrieve the blockId and blockSize
	blockId = pReader->Read<char>();
	blockSize = pReader->Read<int>();
	//Retrieve the PageName (BinaryReader::ReadNullString)
	const std::wstring pageName{ pReader->ReadNullString() };
	//Construct the full path to the page texture file
	//	>> page texture should be stored next to the .fnt file, pageName contains the name of the texture file
	//	>> full texture path = asset parent_path of .fnt file (see loadInfo.assetFullPath > get parent_path) + pageName (filesystem::path::append)
	//	>> Load the texture (ContentManager::Load<TextureData>) & Store [fontDesc.pTexture]
	fontDesc.pTexture = ContentManager::Load<TextureData>(loadInfo.assetFullPath.parent_path().append(pageName).wstring());

	//**********
	// BLOCK 3 *
	//**********
	//Retrieve the blockId and blockSize
	blockId = pReader->Read<char>();
	blockSize = pReader->Read<int>();
	//Retrieve Character Count (see documentation)
	const int charactercount = blockSize / 20;
	//Create loop for Character Count, and:
	for (int i{}; i < charactercount; ++i)
	{
		//> Retrieve CharacterId (store Local) and cast to a 'wchar_t'
		const wchar_t characterId = static_cast<wchar_t>(pReader->Read<unsigned int>());
		//> Create instance of FontMetric (struct)
		FontMetric metric{};
		//	> Set Character (CharacterId) [FontMetric::character]
		metric.character = characterId;
		//	> Retrieve Xposition (store Local)
		const unsigned short xPos = pReader->Read<unsigned short>();
		//	> Retrieve Yposition (store Local)
		const unsigned short yPos = pReader->Read<unsigned short>();
		//	> Retrieve & Set Width [FontMetric::width]
		metric.width = pReader->Read<unsigned short>();
		//	> Retrieve & Set Height [FontMetric::height]
		metric.height = pReader->Read<unsigned short>();
		//	> Retrieve & Set OffsetX [FontMetric::offsetX]
		metric.offsetX = pReader->Read<unsigned short>();
		//	> Retrieve & Set OffsetY [FontMetric::offsetY]
		metric.offsetY = pReader->Read<unsigned short>();
		//	> Retrieve & Set AdvanceX [FontMetric::advanceX]
		metric.advanceX = pReader->Read<unsigned short>();
		//	> Retrieve & Set Page [FontMetric::page]
		metric.page = pReader->Read<unsigned char>();
		//	> Retrieve Channel (BITFIELD!!!) 
		//		> See documentation for BitField meaning [FontMetrix::channel]

		switch (const unsigned char channel = pReader->Read<unsigned char>())
		{
		case 0x1:
			metric.channel = 2;
			break;
		case 0x2:
			metric.channel = 1;
			break;
		case 0x4:
			metric.channel = 0;
			break;
		case 0x8:
			metric.channel = 4;
			break;
		case 0xF:
		default:
			metric.channel = 0;
		}

		//	> Calculate Texture Coordinates using Xposition, Yposition, fontDesc.TextureWidth & fontDesc.TextureHeight [FontMetric::texCoord]

		metric.texCoord = XMFLOAT2
		{
			static_cast<float>(xPos) / fontDesc.textureWidth,
			static_cast<float>(yPos) / fontDesc.textureWidth
		};

		//> Insert new FontMetric to the metrics [font.metrics] map
		//	> key = (wchar_t) charId
		//	> value = new FontMetric
		fontDesc.metrics[metric.character] = metric;
		//(loop restarts till all metrics are parsed)
	}

	//Done!
	delete pReader;
	return new SpriteFont(fontDesc);
}

void SpriteFontLoader::Destroy(SpriteFont* objToDestroy)
{
	SafeDelete(objToDestroy);
}
