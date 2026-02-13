#pragma once
#include <externals/DirectXTex/DirectXTex.h>
#include <string>
#include <d3d12.h>
#include <wrl.h>  
#include <d3d12.h> 

using Microsoft::WRL::ComPtr;

class DirectXCommon;

class TextureManager
{
public:

	void Initialize(DirectXCommon* dxCommon);
	//シングルトンインスタンスの取得
	static TextureManager* GetInstance();
	//終了
	void Finalize();

	void LoadTexture(const std::string& filePath);

	uint32_t GetTextureIndexByFilePath(const std::string& filePath);

	//テクスチャ番号からGPUハンドルを取得
	D3D12_GPU_DESCRIPTOR_HANDLE GetSrvHandleGPU(uint32_t textureIndex);


private:

	DirectXCommon* dxCommon_ = nullptr;

	static uint32_t kSRVIndexTop;

	// テクスチャ1枚分のデータ
	struct TextureData
	{
		std::string filePath;
		DirectX::TexMetadata metadata;

		ComPtr<ID3D12Resource> resource;
		ComPtr<ID3D12Resource> intermediateResource;
		D3D12_CPU_DESCRIPTOR_HANDLE srvHandleCPU{};
		D3D12_GPU_DESCRIPTOR_HANDLE srvHandleGPU{};
	};

	std::vector<TextureData> textureDatas;

	static TextureManager* instance;

	TextureManager() = default;
	~TextureManager() = default;
	TextureManager(TextureManager&) = delete;
	TextureManager& operator=(TextureManager&) = delete;

};

