#pragma once
#include <engine/base/DirectXCommon.h>
class SpriteCommon
{
	public:
		void Initialize(DirectXCommon* dxCommon);

		void CreateRootSignature();

		void CreateGraphicsPipeline();
		//共通描画設定
		void SetCommonPipelineState();

		DirectXCommon* GetDirectXCommon() { return dxCommon_; }

private:

	DirectXCommon* dxCommon_;

	//RootSignature
	//バイナリを元に生成
	Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature = nullptr;
	D3D12_ROOT_SIGNATURE_DESC descriptionRootSignature{};

	Microsoft::WRL::ComPtr<IDxcBlob> vertexShaderBlob;
	Microsoft::WRL::ComPtr<IDxcBlob> pixelShaderBlob;

	D3D12_INPUT_LAYOUT_DESC inputLayoutDesc{};
	// InputLayout
	D3D12_INPUT_ELEMENT_DESC inputElementDescs[2] = {};
	// BlendStateの
	D3D12_BLEND_DESC blendDesc{};
	//RasiterzerStateの設定
	D3D12_RASTERIZER_DESC rasterizerDesc{};

	// PSOを生成する

	Microsoft::WRL::ComPtr<ID3D12PipelineState> graphicsPipelineState = nullptr;
	D3D12_GRAPHICS_PIPELINE_STATE_DESC graphicsPipelineStateDesc{};

};

