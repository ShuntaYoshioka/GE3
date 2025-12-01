#pragma once
#include <d3d12.h>
#include <dxgi1_6.h>
#include <wrl.h>
#include "WinApp.h"

class DirectXCommon
{
public:
	void Initialize(WinApp* winApp);
	void deviceInitialize();
	void SwapChain();
	void Descriptor();
	void CreateDescriptorHeaps();
	void PreDraw();
	void PostDraw();

private:
	//DirectX12デバイス
	Microsoft::WRL::ComPtr<ID3D12Device> device;
	//DXGIファクトリー
	Microsoft::WRL::ComPtr<IDXGIFactory7> dxgiFactory;

	WinApp* winApp = nullptr;
};

