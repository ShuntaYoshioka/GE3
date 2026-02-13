#pragma once
#include <engine/math/Vector4.h>
#include <engine/math/Vector3.h>
#include <engine/math/Vector2.h>
#include <engine/math/Matrix4x4.h>

#include <cstdint>
#include <d3d12.h>
#include <wrl.h>

class SpriteCommon;
class DirectXCommon;

class Sprite
{
public:
	void Initialize(SpriteCommon* spriteCommon, DirectXCommon* dxCommon, D3D12_GPU_DESCRIPTOR_HANDLE srvHandle);

	void Update();

	void Draw();


	struct Transform
	{
		Vector3 scale;
		Vector3 rotate;
		Vector3 translate;
	};

	Transform transform =
	{
		{1.0f,1.0f,1.0f},
		{0.0f,0.0f,0.0f},
		{0.0f,0.0f,0.0f}
	};
private:
	SpriteCommon* spriteCommon = nullptr;

	DirectXCommon* dxCommon_ = nullptr;

	struct VertexData {
		Vector4 position;
		Vector2 texcoord;
	};


	struct Material
	{
		Vector4 color;
		int32_t enableLighting;
		float padding[3];
		Matrix4x4 uvTransform;
	};

	struct TransformationMatrix
	{
		Matrix4x4 WVP;

		Matrix4x4 World;
	};

	//バッファリソース
	Microsoft::WRL::ComPtr<ID3D12Resource> vertexResource;
	Microsoft::WRL::ComPtr<ID3D12Resource> indexResource;
	Microsoft::WRL::ComPtr<ID3D12Resource> materialResource;
	Microsoft::WRL::ComPtr<ID3D12Resource> transformationMatrixResource;
	//バッファリソース内のデータを指すポインタ
	VertexData* vertexData = nullptr;
	uint32_t* indexData = nullptr;
	Material* materialData = nullptr;
	TransformationMatrix* transformationMatrixData = nullptr;
	//バッファリソースの使い道を補足するバッファビュー
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView{};
	D3D12_INDEX_BUFFER_VIEW indexBufferView{};

	D3D12_GPU_DESCRIPTOR_HANDLE textureSrvHandleGPU{};
};

