#pragma once
#include <d3d12.h>
#include <dxgi1_6.h>
#include <wrl.h>
#include <array> 
#include "WinApp.h"
#include <string>
#include <dxcapi.h>
#include "StringUtility.h"
#include <chrono>
#include "externals/DirectXTex/DirectXTex.h"

class DirectXCommon
{
public:

	HRESULT hr;

	void Initialize(WinApp* winApp);
	
	void DeviceInitialize();
	void CommandInitialize();
	void CreateSwapChain();
	///深度バッファ
	void CreateDescriptorHeaps();
	//RTV
	void RenderTargetViewInitialize();
	//DSV
	void DepthStencilViewInitialize();
	//fance
	void FenceInitialize();
	//ビューポート
	void ViewportInitialize();
	void ScissorRectInitialize();//未実装
	void CreateDXC();
	void ImGuiInitialize();

	
	void PreDraw();
	void PostDraw();

	void InitializeFixFPS();

	void UpdateFixFPS();


	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> CreateDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE heapType, UINT numDescriptors, bool shaderVisible);

	D3D12_CPU_DESCRIPTOR_HANDLE GetSRVCPUDescriptorHandle(uint32_t index);

	D3D12_GPU_DESCRIPTOR_HANDLE GetSRVGPUDescriptorHandle(uint32_t index);

	Microsoft::WRL::ComPtr<ID3D12Resource>CreateDepthStencilTextureResource(int32_t width, int32_t height);

	Microsoft::WRL::ComPtr<IDxcBlob>  CompileShader(
		//CompilerするShaderファイルへのパス
		const std::wstring& filePath,
		//Compilerに使用するProfile
		const wchar_t* profile
	);

	Microsoft::WRL::ComPtr <ID3D12Resource> CreateBufferResource(size_t sizeInBytes);

	Microsoft::WRL::ComPtr <ID3D12Resource>  CreateTextureResource(const DirectX::TexMetadata& metadata);

	static DirectX::ScratchImage LoadTexture(const std::string& filePath);

   Microsoft::WRL::ComPtr<ID3D12Resource>UploadTextureData(Microsoft::WRL::ComPtr<ID3D12Resource> texture, const DirectX::ScratchImage& mipImages);


	ID3D12Device* GetDevice()
	{
		return device.Get();
	}

	ID3D12GraphicsCommandList* GetCommandList()
	{
		return commandList.Get();
	}

private:

	//DirectX12デバイス
	Microsoft::WRL::ComPtr<ID3D12Device> device;

	Microsoft::WRL::ComPtr<ID3D12CommandQueue> commandQueue;
	Microsoft::WRL::ComPtr<ID3D12CommandAllocator> commandAllocator;
	Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> commandList;
	Microsoft::WRL::ComPtr<IDXGISwapChain4> swapChain;


	//DXGIファクトリー
	Microsoft::WRL::ComPtr<IDXGIFactory7> dxgiFactory;

	DXGI_SWAP_CHAIN_DESC1 swapChainDesc{};

	static D3D12_CPU_DESCRIPTOR_HANDLE GetCPUDescriptorHandle(Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> descriptorHeap, uint32_t descriptorSize, uint32_t index);

	static D3D12_GPU_DESCRIPTOR_HANDLE GetGPUDescriptorHandle(Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> descriptorHeap, uint32_t descriptorSize, uint32_t index);

	D3D12_RESOURCE_BARRIER barrier{};

	std::array<Microsoft::WRL::ComPtr<ID3D12Resource>,2> swapChainResources;

	D3D12_CPU_DESCRIPTOR_HANDLE rtvHandles[2];

	//RTVの設定
	D3D12_RENDER_TARGET_VIEW_DESC rtvDesc{};

	//RTVハンドル
	D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle;

	D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle;

	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> rtvDescriptorHeap;
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> srvDescriptorHeap;
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> dsvDescriptorHeap;
	uint32_t descriptorSizeSRV;
	uint32_t descriptorSizeRTV;
	uint32_t descriptorSizeDSV;

	//深度バッファ
	Microsoft::WRL::ComPtr<ID3D12Resource> depthStencilResource;

	//フェンス
	Microsoft::WRL::ComPtr<ID3D12Fence> fence;
	HANDLE fenceEvent;
	UINT64 fenceValue = 0;


	//ビューポート
	D3D12_VIEWPORT viewport{ };

	IDxcUtils* dxcUtils;
	IDxcCompiler3* dxcCompiler;
	IDxcIncludeHandler* includeHandler;

	WinApp* winApp = nullptr;

	std::chrono::steady_clock::time_point reference_;
};

