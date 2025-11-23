#pragma once
#include "RenderTexturePass.h"
#include "MiniMapDataModule.h"
#include<optional>
class MiniMapPass : RenderTexturePass {
	MiniMapPass(ShaderManager* shaderManager,
		InstanceManager* instanceManager,
		ID3D11Device* device,
		const RenderTextureData& mainRTData,
		const OrthoMeshData& mainOrthoMeshData,
		std::optional<OrthoMeshData> miniMapOrthoData = std::nullopt) :
		RenderTexturePass(shaderManager, instanceManager, device, mainRTData, mainOrthoMeshData)
	{


		XMFLOAT2 miniMapSize;
		XMFLOAT2 miniMapOffset;

		if (miniMapOrthoData == std::nullopt) {
			miniMapSize = { mainOrthoMeshData.width / 4, mainOrthoMeshData.height / 4 };
			miniMapOffset = { mainOrthoMeshData.width / 2.7f, mainOrthoMeshData.height / 2.7f };
		}
		else {
			miniMapSize = { miniMapOrthoData->width, miniMapOrthoData->height };
		}
		miniMapOrthoMesh = instanceManager->createOrthoMeshInstance(miniMapOrthoID, OrthoMeshData(miniMapSize.x, miniMapSize.y, miniMapOffset.x, miniMapOffset.y));
		screenSize = { mainRTData.screenWidth, mainRTData.screenHeight };


		//Calculate miniMap width and height
		float miniMapWidth = screenSize.x / 4.f;
		float miniMapHeight = screenSize.y / 4.f;

		//Calculate offset to recentre minimmap and then offset to screen position
		projectionOffset.x = (screenSize.x - miniMapSize.x) / 2.f + miniMapOffset.x;
		projectionOffset.y = (screenSize.y - miniMapSize.y) / 2.f - miniMapOffset.y;

		miniMapScaleFactor.x = miniMapSize.x / screenSize.x;
		miniMapScaleFactor.y = miniMapSize.y / screenSize.y;





		miniMapOrthoMesh = instanceManager->createOrthoMeshInstance(miniMapOrthoID, mainOrthoMeshData);

		miniMapCamera = instanceManager->createCamera(cameraID);
		miniMapCamera->camera->m_transform.setEulerAngles(90.f, 0.f, 0.f);
		miniMapCamera->camera->m_transform.setPosition(0.0f, 10.f, 0.0f);
		miniMapCamera->camera->updateGlobals(true);

		miniMapWidth = 20.f;
		miniMapHeight = 20.f;
		miniMapNearZ = 0.1f;
		miniMapFarZ = 100.f;

		playerIconColour = XMFLOAT4(1.f, 0.0f, 0.0f, 1.0f);
		playerIconRadius = 5.f;

		orthographicProjectionMatrix = XMMatrixOrthographicLH(miniMapWidth, miniMapHeight, miniMapNearZ, miniMapFarZ);
	}

	void Render(ID3D11DeviceContext* device, const XMMATRIX& projectionMatrix) {

		XMFLOAT3 currentCameraPos = instanceManager->getActiveCamera()->camera->getGlobalPosition();

		XMVECTOR projectedPos = XMVector3Project(
			XMLoadFloat3(&currentCameraPos),
			0.0f, 0.0f,
			screenSize.x,
			screenSize.y,
			0.0f,
			1.0f,
			orthographicProjectionMatrix,
			miniMapCamera->camera->getViewMatrix(),
			XMMatrixIdentity()
		);

		XMFLOAT3 screenPos;
		XMStoreFloat3(&screenPos, projectedPos);

		//Change target to correct position
		screenPos.x = screenPos.x * miniMapScaleFactor.x + projectionOffset.x;
		screenPos.y = screenPos.y * miniMapScaleFactor.y + projectionOffset.y;

		std::shared_ptr<MatrixDataModule> matrixModule = dynamic_pointer_cast<MatrixDataModule>(matrixDataModule->module);
		std::shared_ptr<CameraDataModule> camModule = dynamic_pointer_cast<CameraDataModule>(cameraDataModule->module);
		std::shared_ptr<MiniMapDataModule> minimapModule = dynamic_pointer_cast<MiniMapDataModule>(miniMapDataModule->module);

		matrixModule->setModuleParamaters(device, miniMapOrthoMesh, projectionMatrix);
		miniMapModule->
		textureModule->setModuleParamaters(device, textureOutput->getShaderResourceView());

		XMMATRIX tworld, tview, tproj;

		
		/*
		

		XMVECTOR projectedPos = XMVector3Project(
			XMLoadFloat3(&cameraPos),
			0.0f, 0.0f,
			screenSize.x,
			screenSize.y,
			0.0f,
			1.0f,
			orthographicProjectionMatrix,
			miniMapCamera->getViewMatrix(),
			XMMatrixIdentity()
		);
		*/

		XMFLOAT3 screenPos;
		*/

		// Transpose the matrices to prepare them for the shader.
		tworld = XMMatrixTranspose(worldMatrix);
		tview = XMMatrixTranspose(viewMatrix);
		tproj = XMMatrixTranspose(projectionMatrix);

		matrixDataModule.setModuleParamaters(deviceContext, worldMatrix, viewMatrix, projectionMatrix);

		RenderTexturePass::Render(device, projectionMatrix)

		miniMapOrthoMesh->mesh->mesh->sendData(ID3D11DeviceContext * device, const XMMATRIX & projectionMatrix)
		orthoMeshTR->sendData(renderer->getDeviceContext());
		miniMapTextureShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, orthoViewMatrix, orthoMatrix, renderTextureTR->getShaderResourceView(), camera, miniMapCamera.get());
		miniMapTextureShader->render(renderer->getDeviceContext(), orthoMeshTR->getIndexCount());
	}

	void ImGuiMenu()
	{
		if (ImGui::CollapsingHeader("MiniMapControl"))
		{
			if (ImGui::TreeNode("ProjectionMatrixSettings")) {
				bool matrixUpdated = false;

				if (ImGui::SliderFloat("orthoWidth: ", &miniMapWidth, 0.0f, 1000.f)) { matrixUpdated = true; }
				if (ImGui::SliderFloat("orthoHeight: ", &miniMapHeight, 0.0f, 1000.f)) { matrixUpdated = true; }
				if (ImGui::SliderFloat("orthoNearZ: ", &miniMapNearZ, 0.1f, 1000.f)) { matrixUpdated = true; }
				if (ImGui::SliderFloat("orthoFarZ: ", &miniMapFarZ, 0.1f, 1000.f)) { matrixUpdated = true; }

				if (matrixUpdated) {
					//Calulcate the orthographic projection matrix
					orthographicProjectionMatrix = XMMatrixOrthographicLH(miniMapWidth, miniMapHeight, miniMapNearZ, miniMapFarZ);
				}
				ImGui::TreePop();
			}
			if (ImGui::TreeNode("Player Icon Control")) {
				if (ImGui::TreeNode("Player Icon Colour")) {
					if (ImGui::ColorPicker4("Icon Colour", &playerIconColour.x)) {}
					ImGui::TreePop();
				}
				if (ImGui::SliderFloat("Icon Radius", &playerIconRadius, 1.f, 50.f)) {}
				ImGui::TreePop();
			}
			/*
			if (ImGui::TreeNode("GreyScale Value")) {
				if (ImGui::SliderFloat("Red Weight", &greyScaleValues.x, 0.0f, 1.0f)) {}
				if (ImGui::SliderFloat("Green Weight", &greyScaleValues.y, 0.0f, 1.0f)) {}
				if (ImGui::SliderFloat("Blue Weight", &greyScaleValues.z, 0.0f, 1.0f)) {}
				ImGui::TreePop();
			}
			*/
		}
	}
private:

	float miniMapWidth;
	float miniMapHeight;
	float miniMapNearZ;
	float miniMapFarZ;
	XMFLOAT4 playerIconColour;
	float playerIconRadius;

	XMMATRIX orthographicProjectionMatrix;
	size_t miniMapOrthoID;
	GeometryInstance miniMapOrthoMesh;
	std::shared_ptr<RenderTexture> miniMapTextureInput; // Pointer to input texture from other render pass;
	size_t cameraID;
	CameraInstance miniMapCamera;
	ModuleInstance miniMapDataModule;

	XMFLOAT2 screenSize;
	XMFLOAT2 projectionOffset;
	XMFLOAT2 miniMapScaleFactor;
}

/*
	//Was in shader 
	XMStoreFloat3(&screenPos, projectedPos);


	//Calculate miniMap width and height
	float miniMapWidth = screenSize.x / 4.f;
	float miniMapHeight = screenSize.y / 4.f;

	//Calculate offset to recentre minimmap
	float xOffset = (screenSize.x - miniMapWidth) / 2.f;
	float yOffset = (screenSize.y - miniMapHeight) / 2.f;

	//Scale down target to minimap size and recentre
	screenPos.x = screenPos.x / 4.f + xOffset;
	screenPos.y = screenPos.y / 4.f + yOffset;

	// Offset to top right of screen
	screenPos.x += screenSize.x / 2.7f;
	screenPos.y -= screenSize.y / 2.7f;

*/


*/