#pragma once
#include <d3d12.h>
#include <dxgi1_6.h>
#include <wrl.h>
#include <array> 
#include "WinApp.h"

class DirectXCommon
{
public:

	HRESULT hr;

	void Initialize(WinApp* winApp);
	
	void deviceInitialize();
	void CommandInitialize();
	void CreateSwapChain();
	void Descriptor();
	void CreateDescriptorHeaps();
	void RenderTargetViewInitialize();
	void DepthStencilViewInitialize();
	void FenceInitialize();
	void ViewportInitialize();
	void ScissorRectInitialize();
	void CreateDXC();
	void ImGuiInitialize();

	
	void PreDraw();
	void PostDraw();

	D3D12_CPU_DESCRIPTOR_HANDLE GetSRVCPUDescriptorHandle(uint32_t index);

	D3D12_GPU_DESCRIPTOR_HANDLE GetSRVGPUDescriptorHandle(uint32_t index);

private:
	//DirectX12デバイス
	Microsoft::WRL::ComPtr<ID3D12Device> device;
	//DXGIファクトリー
	Microsoft::WRL::ComPtr<IDXGIFactory7> dxgiFactory;

	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> CreateDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE heapType, UINT numDescriptors, bool shaderVisible);

	static D3D12_CPU_DESCRIPTOR_HANDLE GetCPUDescriptorHandle(const Microsoft::WRL::ComPtr<ID3D12DescriptorHeap>& descriptorHeap,uint32_t descriptorSize, uint32_t index);

	static D3D12_GPU_DESCRIPTOR_HANDLE GetGPUDescriptorHandle(const Microsoft::WRL::ComPtr<ID3D12DescriptorHeap>& descriptorHeap, uint32_t descriptorSize, uint32_t index);

	std::array<Microsoft::WRL::ComPtr<ID3D12Resource>,2> swapChainResources;

	WinApp* winApp = nullptr;
};

