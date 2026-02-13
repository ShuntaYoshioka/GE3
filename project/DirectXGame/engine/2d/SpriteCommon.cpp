#include "SpriteCommon.h"

void SpriteCommon::Initialize(DirectXCommon* dxCommon)
{
	dxCommon_ = dxCommon;
	// RootSignatureの作成
	CreateRootSignature();
	// グラフィックスパイプラインの作成
	CreateGraphicsPipeline();
}

void SpriteCommon::CreateRootSignature()
{

	descriptionRootSignature.Flags =
		D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
	D3D12_DESCRIPTOR_RANGE descriptorRange[1] = {};
	descriptorRange[0].BaseShaderRegister = 0;
	descriptorRange[0].NumDescriptors = 1;
	descriptorRange[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	descriptorRange[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	D3D12_ROOT_PARAMETER rootParmeters[3]{};
	rootParmeters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	rootParmeters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
	rootParmeters[0].Descriptor.ShaderRegister = 0;

	rootParmeters[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV; // CBVを使う
	rootParmeters[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX; //VertexShaderで使う 
	rootParmeters[1].Descriptor.ShaderRegister = 0; //レジスタ番号0とインド 

	rootParmeters[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE; //DescriptorTableを使う
	rootParmeters[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL; // PixelShaderで使う
	rootParmeters[2].DescriptorTable.pDescriptorRanges = descriptorRange; // Tableの中身の配列を指定
	rootParmeters[2].DescriptorTable.NumDescriptorRanges = _countof(descriptorRange); // Tableで利用する数

	descriptionRootSignature.pParameters = rootParmeters;
	descriptionRootSignature.NumParameters = 3;

	D3D12_STATIC_SAMPLER_DESC staticSamplers[1] = {};
	staticSamplers[0].Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR; // バイリニアフィルタ
	staticSamplers[0].AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	staticSamplers[0].AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	staticSamplers[0].AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	staticSamplers[0].ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER; //比較しない
	staticSamplers[0].MaxLOD = D3D12_FLOAT32_MAX; // ありったけのmipmapを使う
	staticSamplers[0].ShaderRegister = 0; // レジスタ番号を使う
	staticSamplers[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL; // Pixel Shaderで使う
	
	descriptionRootSignature.pStaticSamplers = staticSamplers;
	descriptionRootSignature.NumStaticSamplers = _countof(staticSamplers);



	HRESULT hr;
	// シリアライズしてバイナリにする
	Microsoft::WRL::ComPtr<ID3D10Blob> signatureBlob = nullptr;
	Microsoft::WRL::ComPtr<ID3D10Blob> errorBlob = nullptr;
	hr = D3D12SerializeRootSignature(
		&descriptionRootSignature,
		D3D_ROOT_SIGNATURE_VERSION_1,
		&signatureBlob, &errorBlob);

	if (FAILED(hr)) {
		//Log(reinterpret_cast<char*>(errorBlob->GetBufferPointer()));
		assert(false);
	}


	//バイナリを元に生成
	hr = dxCommon_->GetDevice()->CreateRootSignature(0,
		signatureBlob->GetBufferPointer(), signatureBlob->GetBufferSize(),
		IID_PPV_ARGS(&rootSignature));
	assert(SUCCEEDED(hr));

	// InputLayout
	inputElementDescs[0].SemanticName = "POSITION";
	inputElementDescs[0].SemanticIndex = 0;
	inputElementDescs[0].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	inputElementDescs[0].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;

	inputElementDescs[1].SemanticName = "TEXCOORD";
	inputElementDescs[1].SemanticIndex = 0;
	inputElementDescs[1].Format = DXGI_FORMAT_R32G32_FLOAT;
	inputElementDescs[1].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;

	inputLayoutDesc.pInputElementDescs = inputElementDescs;
	inputLayoutDesc.NumElements = _countof(inputElementDescs);

	// 全ての色要素を書き込む
	//すべての色要素を書き込む
	blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

	//裏面(時計回り)を表示しない
	rasterizerDesc.CullMode = D3D12_CULL_MODE_BACK;
	//三角形の中を塗りつぶす
	rasterizerDesc.FillMode = D3D12_FILL_MODE_SOLID;

	// Shaderをコンパイルする
	 vertexShaderBlob = dxCommon_->CompileShader(L"resources/shader/Object3D.VS.hlsl", L"vs_6_0");
	assert(vertexShaderBlob != nullptr);

	 pixelShaderBlob = dxCommon_->CompileShader(L"resources/shader/Object3D.PS.hlsl", L"ps_6_0");
	assert(pixelShaderBlob != nullptr);

}

void SpriteCommon::CreateGraphicsPipeline()
{

	// PSOを生成する
	graphicsPipelineStateDesc.pRootSignature = rootSignature.Get(); // RootSignatrue
	graphicsPipelineStateDesc.InputLayout = inputLayoutDesc;  // InputLayout
	graphicsPipelineStateDesc.VS = { vertexShaderBlob->GetBufferPointer(),
	vertexShaderBlob->GetBufferSize() }; // VertexShader
	graphicsPipelineStateDesc.PS = { pixelShaderBlob->GetBufferPointer(),
	pixelShaderBlob->GetBufferSize() }; // PixelShader 
	graphicsPipelineStateDesc.BlendState = blendDesc;// BlensState
	graphicsPipelineStateDesc.RasterizerState = rasterizerDesc;// RasterizerState
	//書き込むRTVの情報
	graphicsPipelineStateDesc.NumRenderTargets = 1;
	graphicsPipelineStateDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	// 利用するトポロジ(形状)のタイプ
	graphicsPipelineStateDesc.PrimitiveTopologyType =
		D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	//どのように画面に色を打ち込むかの設定
	graphicsPipelineStateDesc.SampleDesc.Count = 1;
	graphicsPipelineStateDesc.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;

	////DepthStencilStateの設定
	D3D12_DEPTH_STENCIL_DESC depthStencilDesc{};
	////機能を有効化
	depthStencilDesc.DepthEnable = true;
	////書き込み
	depthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
	depthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;

	// DepthStencilの設定
	graphicsPipelineStateDesc.DepthStencilState = depthStencilDesc;
	graphicsPipelineStateDesc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;

	//実際に生成
	HRESULT hr = dxCommon_->GetDevice()->CreateGraphicsPipelineState
	(
		&graphicsPipelineStateDesc,
		IID_PPV_ARGS(&graphicsPipelineState)
	);
	assert(SUCCEEDED(hr));

}

void SpriteCommon::SetCommonPipelineState()
{
	dxCommon_->GetCommandList()->SetGraphicsRootSignature(rootSignature.Get());
	dxCommon_->GetCommandList()->SetPipelineState(graphicsPipelineState.Get()); 
	dxCommon_->GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	//	dxCommon->GetCommandList()->IASetVertexBuffers(0, 1, &vertexBufferView);
}
