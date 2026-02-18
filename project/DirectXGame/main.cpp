#include <Windows.h>
#include <cstdint>
#include <string>
#include <d3d12.h>
#include <dxgi1_6.h>
#include <cassert>
#include <fstream>
#include <sstream>

#include "engine/io/Input.h"
#include "WinApp.h"
#include "Logger.h"
#include "TextureManager.h"
#include "DirectXCommon.h"
#include "StringUtility.h"

#include "externals/imgui/imgui.h"
#include "externals/imgui/imgui_impl_dx12.h"
#include "externals/imgui/imgui_impl_win32.h"

#include "D3DResourceLeakChecker.h"
#include <engine/2d/SpriteCommon.h>
#include <engine/2d/Sprite.h>

using namespace StringUtility;
using namespace Logger;

#include <engine/math/Vector2.h>
#include <engine/math/Vector3.h>
#include <engine/math/Vector4.h>
#include <engine/math/Matrix4x4.h>
#include <engine/math/MyMath.h>

#pragma comment(lib,"dxcompiler.lib")

struct VertexData {
	Vector4 position;
	Vector2 texcoord;
};

struct MaterialData {
	std::string textureFilePath;
};

struct ModelData {
	std::vector<VertexData> vertices;
	MaterialData material;
};

MaterialData LoadMaterialTemplateFile(const std::string& directoryPath, const std::string& filename)
{

	// 1. 中で必要となる変数の宣言
	MaterialData materialData; // 構築するMaterialData
	std::string line; // ファイルから読んだ1行を格納するもの
	// 2. ファイルを開く
	std::ifstream file(directoryPath + "/" + filename); // ファイルを開く
	assert(file.is_open()); // とりあえず開けなかったら止める
	// 3. 実際にファイルを読み、MaterialDataを構築していく
	while (std::getline(file, line)) {
		std::string identifier;
		std::istringstream s(line);
		s >> identifier;
		// identifierに応じた処理
		if (identifier == "map_Kd") {
			std::string textureFilename;
			s >> textureFilename;
			// 連結してファイルパスにする
			materialData.textureFilePath = directoryPath + "/" + textureFilename;
		}
	}
	// 4. MaterialData を返す
	return materialData;

}

ModelData LoadObjFile(const std::string& directoryPath, const std::string filename) {
	ModelData modelData; // 構築するModelData
	std::vector<Vector4> positions; // 位置
	std::vector<Vector3> normals; // 法線
	std::vector<Vector2> texcoords; // テクスチャ座標
	std::string line; // ファイルから読んだ1行を格納するもの

	std::ifstream file(directoryPath + "/" + filename); // ファイルを開く
	assert(file.is_open());
	// とりあえず開けなかったら止める

	while (std::getline(file, line)) {
		std::string identifier;
		std::istringstream s(line);
		s >> identifier; // 先頭の識別子を読む
		//identifierに応じた処理

		if (identifier == "v") {
			Vector4 position;
			s >> position.x >> position.y >> position.z;
			position.w = 1.0f;
			positions.push_back(position);
		} else if (identifier == "vt") {
			Vector2 texcoord;
			s >> texcoord.x >> texcoord.y;
			texcoords.push_back(texcoord);
		} else if (identifier == "vn") {
			Vector3 normal;
			s >> normal.x >> normal.y >> normal.z;
			normals.push_back(normal);
		} else if (identifier == "f") {
			VertexData triangle[3];
			// 面は三角形限定。その他は未対応
			for (int32_t faceVertex = 0; faceVertex < 3; ++faceVertex) {
				std::string vertexDefinition;
				s >> vertexDefinition;
				// 頂点の要素へのIndexは「位置/UV/法線」で格納されているので、分解してIndexを取得する
				std::istringstream v(vertexDefinition);
				uint32_t elementIndices[3];
				for (int32_t element = 0; element < 3; ++element) {
					std::string index;
					std::getline(v, index, '/'); // /区切りでインデックスを読んでいく
					elementIndices[element] = std::stoi(index);
				}
				// 要素へのIndexから、実際の要素の値を取得して、頂点を構築する
				Vector4 position = positions[elementIndices[0] - 1];
				position.x *= -1.0f;
				Vector2 texcoord = texcoords[elementIndices[1] - 1];
				texcoord.y = 1.0f - texcoord.y;
				Vector3 normal = normals[elementIndices[2] - 1];
				//normal,x *= -1.0f;
				VertexData vertex = { position, texcoord };//5章でとばした
				//modelData.vertices.push_back(vertex);
				triangle[faceVertex] = { position,texcoord };//5章でとばした
			}
			modelData.vertices.push_back(triangle[2]);
			modelData.vertices.push_back(triangle[1]);
			modelData.vertices.push_back(triangle[0]);
		} else if (identifier == "mtllib") {
			std::string materialFilename;
			s >> materialFilename;
			// 基本的にobjファイルと同一階層にmtlは存在させるので、ディレクトリ名とファイル名を渡す
			modelData.material = LoadMaterialTemplateFile(directoryPath, materialFilename);
		}
	}

	return modelData;
}

//Windoesアプリでのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {

	//ポインタ
	WinApp* winApp = nullptr;

	DirectXCommon* dxCommon = nullptr;

	//windowsの初期化
	winApp = new WinApp();
	winApp->initialize();

	//DirectXの初期化
	dxCommon = new DirectXCommon();
	dxCommon->Initialize(winApp);

	//ポインタ
	Input* input = nullptr;

	//入力の初期化
	input = new Input();
	input->Initialize(winApp);

	//テクスチャマネージャー
	TextureManager::GetInstance()->Initialize(dxCommon);

	MSG msg{};

	// ポインタ
	SpriteCommon* spriteCommon = nullptr;
	// スプライト共通部の初期化
	spriteCommon = new SpriteCommon();
	spriteCommon->Initialize(dxCommon);

	//モデルの読み込み
	//ModelData modelData = LoadObjFile("resources", "plane.obj");

	Sprite* sprite = new Sprite;
	sprite->Initialize(spriteCommon, dxCommon, "Resources/uvChecker.png");

	Vector2 position = sprite->GetPosition();
	float rotation = sprite->GetRotation();
	Vector4 color = sprite->GetColor();
	Vector2 size = sprite->GetSize();

	dxCommon->ExcuteCommandList();
	dxCommon->WaitForGpu();

	//ウィンドウのXボタンが押されるまでループ
	while (true) {
		//Windowにメッセージが来てたら最優先で処理させる
		if (winApp->ProcessMessage()) {
			break;
		}

		input->Update();

		//ゲームの処理
		ImGui_ImplDX12_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();

		//長押し判定
		if (input->PushKey(DIK_0))
		{
			OutputDebugStringA("Press 0\n");//Hit0表示
		}

		if (input->TriggerKey(DIK_1))
		{
			OutputDebugStringA("Press 1\n");//Hit0表示
		}

		//開発用のUIの処理
		ImGui::ShowDemoWindow();

		ImGui::Begin("Settings");
		ImGui::DragFloat2("Position", &position.x, 1.0f);
		ImGui::DragFloat("Rotation", &rotation, 0.1f);
		ImGui::DragFloat2("Size", &size.x, 1.0f);
		ImGui::ColorEdit4("Color", &color.x);

		ImGui::End();

		sprite->SetPosition(position);
		sprite->SetRotation(rotation);
		sprite->SetColor(color);
		sprite->SetSize(size);

		sprite->Update();

		dxCommon->PreDraw();
		spriteCommon->SetCommonPipelineState();

		sprite->Draw();
		ImGui::Render();
		// 実際のcommandListのImGuiの描画コマンドを積む
		ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), dxCommon->GetCommandList());

		dxCommon->PostDraw();


	}

	//解放処理

	ImGui_ImplDX12_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();

	CoUninitialize();

	D3DResourceLeakChecker leakChecker;
	//入力解放
	delete input;

	delete dxCommon;

#pragma region windowsAPIの終了
	winApp->finalize();
#pragma endregion WindowsAPIの終了

#pragma region テクスチャマネージャーの終了
	TextureManager::GetInstance()->Finalize();
#pragma endregion テクスチャマネージャーの終了


	delete winApp;

	winApp = nullptr;

	delete sprite;

	delete spriteCommon;

	return 0;
}