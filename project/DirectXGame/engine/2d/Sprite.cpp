#include "Sprite.h"
#include "SpriteCommon.h"
#include <engine/math/MyMath.h>
#include <TextureManager.h>


void Sprite::Initialize(SpriteCommon* spriteCommon, DirectXCommon* dxCommon, std::string textureFilePath)
{

	//引数をメンバ変数にセット
	this->spriteCommon = spriteCommon;

	dxCommon_ = dxCommon;
	//とりあえずテクスチャのハンドルを保存
	textureIndex = TextureManager::GetInstance()->GetTextureIndexByFilePath(textureFilePath);

	//頂点リソースを作る
	vertexResource = dxCommon_->CreateBufferResource(sizeof(VertexData) * 6);

	// Sprite用の頂点リソースを作る
	indexResource = dxCommon->CreateBufferResource(sizeof(uint32_t) * 6);

	//materialリソースを作る
	materialResource = dxCommon->CreateBufferResource(sizeof(Vector4));

	transformationMatrixResource = dxCommon->CreateBufferResource(sizeof(Matrix4x4));


	//頂点バッファビューを作成する
	vertexBufferView.BufferLocation = vertexResource->GetGPUVirtualAddress();// リソースの先頭のアドレスから使う
	vertexBufferView.SizeInBytes = UINT(sizeof(VertexData) *6);//使用するリソースのサイズは頂点のサイズ
	vertexBufferView.StrideInBytes = sizeof(VertexData); //  1頂点あたりのサイズ

	indexBufferView.BufferLocation = indexResource->GetGPUVirtualAddress();
	indexBufferView.SizeInBytes = sizeof(uint32_t) * 6;
	indexBufferView.Format = DXGI_FORMAT_R32_UINT;


	vertexResource->Map(0, nullptr, reinterpret_cast<void**>(&vertexData));

	// 書き込むためのアドレス取得
	indexResource->Map(0, nullptr, reinterpret_cast<void**>(&indexData));


	materialResource->Map(0, nullptr, reinterpret_cast<void**>(&materialData));
	materialData->color = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
	materialData->enableLighting = false;
	materialData->uvTransform = MyMath::MakeIdentity4x4();

	transformationMatrixResource->Map(0, nullptr, reinterpret_cast<void**>(&transformationMatrixData));

	transformationMatrixData->WVP = MyMath::MakeIdentity4x4();
	transformationMatrixData->World = MyMath::MakeIdentity4x4();


	//テクスチャ
	TextureManager::GetInstance()->LoadTexture(textureFilePath);
	textureIndex = TextureManager::GetInstance()->GetTextureIndexByFilePath(textureFilePath);

	Sprite::AdjustTextureSize();
}

void Sprite::Update() {

	// 座標
	transform.translate = { position.x,position.y,0.0f };
	// 回転
	transform.rotate = { 0.0f,0.0f,rotation };
	// サイズ
	transform.scale = { size.x,size.y,1.0f };

	// アンカーポイント
	float left = 0.0f - anchorPoint.x;
	float right = 1.0f - anchorPoint.x;
	float top = 0.0f - anchorPoint.y;
	float bottom = 1.0f - anchorPoint.y;

	// 左右反転
	if (isFlipX_)
	{
		left = -left;
		right = -right;
	}
	// 上下反転
	if (isFlipY_)
	{
		top = -top;
		bottom = -bottom;
	}

	const DirectX::TexMetadata& metadata = TextureManager::GetInstance()->GetMetadata(textureIndex);
	float tex_left = textureLeftTop.x / metadata.width;
	float tex_right = (textureLeftTop.x + textureSize.x) / metadata.width;
	float tex_top = textureLeftTop.y / metadata.height;
	float tex_bottom = (textureLeftTop.y + textureSize.y) / metadata.height;


	//頂点リソースにデータを叩き込む(4天分)
	vertexData[0].position = { left,bottom,0.0f,1.0f };
	vertexData[0].texcoord = { tex_left,tex_bottom };
	//vertexData[0].normal = { 0.0f,0.0f,-1.0f };
	vertexData[1].position = { left,top,0.0f,1.0f };
	vertexData[1].texcoord = { tex_left,tex_top };
	//vertexData[1].normal = { 0.0f,0.0f,-1.0f };
	vertexData[2].position = { right,bottom,0.0f,1.0f };
	vertexData[2].texcoord = { tex_right,tex_bottom};
	//vertexData[2].normal = { 0.0f,0.0f,-1.0f };
	vertexData[3].position = { right,top,0.0f,1.0f };
	vertexData[3].texcoord = { tex_right,tex_top };
	//vertexData[3].normal = { 0.0f,0.0f,-1.0f };

	//インデックスリソースにデータを叩き込む(6個分)
	indexData[0] = 0;

	indexData[1] = 1;

	indexData[2] = 2;

	indexData[3] = 1;

	indexData[4] = 3;

	indexData[5] = 2;


	Matrix4x4 worldMatrix = MyMath::MakeAffineMatrix(transform.scale, transform.rotate, transform.translate);

	Matrix4x4 viewMatrix = MyMath::MakeIdentity4x4();

	Matrix4x4 projectionMatrix = MyMath::MakeOrthographicMatrix(0.0f, 0.0f, float(WinApp::kClientWidth), float(WinApp::kClientHeight), 0.0f, 100.0f);


	Matrix4x4 worldViewProjectionMatrix = MyMath::Multiply(worldMatrix, MyMath::Multiply(viewMatrix, projectionMatrix));
	transformationMatrixData->WVP = MyMath::Multiply(worldMatrix, MyMath::Multiply(viewMatrix, projectionMatrix));
	transformationMatrixData->World = worldMatrix;

}

void Sprite::Draw(){

	// VBVを設定
	dxCommon_->GetCommandList()->IASetVertexBuffers(0, 1, &vertexBufferView);
	dxCommon_->GetCommandList()->IASetIndexBuffer(&indexBufferView);

	dxCommon_->GetCommandList()->SetGraphicsRootConstantBufferView(0, materialResource->GetGPUVirtualAddress());
	dxCommon_->GetCommandList()->SetGraphicsRootConstantBufferView(1, transformationMatrixResource->GetGPUVirtualAddress());
	dxCommon_->GetCommandList()->SetGraphicsRootDescriptorTable(2, TextureManager::GetInstance()->GetSrvHandleGPU(textureIndex));
	//描画
	dxCommon_->GetCommandList()->DrawIndexedInstanced(6, 1, 0, 0, 0);
}

void Sprite::ChangeTexture(const std::string& textureFilePath)
{
	TextureManager::GetInstance()->LoadTexture(textureFilePath);

	// indexを差し替える
	textureIndex =
		TextureManager::GetInstance()->GetTextureIndexByFilePath(textureFilePath);
}

void Sprite::AdjustTextureSize()
{
	// テクスチャメタデータ取得
	const DirectX::TexMetadata& metadata = TextureManager::GetInstance()->GetMetadata(textureIndex);

	textureSize.x = static_cast<float>(metadata.width);
	textureSize.y = static_cast<float>(metadata.height);
	// 画像サイズをテクスチャサイズに合わせる
	size = textureSize;
}
