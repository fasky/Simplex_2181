#include "AppClass.h"
void Application::InitVariables(void)
{
	//init the mesh
	m_pMesh = new MyMesh();
	//m_pMesh->GenerateCube(1.0f, C_WHITE);
	m_pMesh->GenerateCone(2.0f, 5.0f, 3, C_WHITE);
}
void Application::Update(void)
{
	//Update the system so it knows how much time has passed since the last call
	m_pSystem->Update();

	//Is the arcball active?
	ArcBall();

	//Is the first person camera active?
	CameraRotation();
}
void Application::Display(void)
{
	// Clear the screen
	ClearScreen();

	matrix4 m4View = m_pCameraMngr->GetViewMatrix();
	matrix4 m4Projection = m_pCameraMngr->GetProjectionMatrix();

	/*
	m_m4Model = glm::rotate(IDENTITY_M4, glm::radians(m_v3Rotation.x), vector3(1.0f, 0.0f, 0.0f));
	m_m4Model = glm::rotate(m_m4Model, glm::radians(m_v3Rotation.y), vector3(0.0f, 1.0f, 0.0f));
	m_m4Model = glm::rotate(m_m4Model, glm::radians(m_v3Rotation.z), vector3(0.0f, 0.0f, 1.0f));
	*/

	quaternion qX;
	quaternion qY;
	quaternion qZ;

	//reset rotation to original
	if (reset) {
		m_m4Model = glm::rotate(IDENTITY_M4, glm::radians(0.0f), vector3(1.0f, 0.0f, 0.0f));
		m_m4Model = glm::rotate(m_m4Model, glm::radians(0.0f), vector3(0.0f, 1.0f, 0.0f));
		m_m4Model = glm::rotate(m_m4Model, glm::radians(0.0f), vector3(0.0f, 0.0f, 1.0f));
		m_pMesh->Render(m4Projection, m4View, ToMatrix4(m_m4Model));
		m_qOrientation = original;
		reset = false;
	}
	else {
		//use quats
		//i just used angle axis with 1 deg change
		//I guess this gets rid of the gimbal lock? It rotates differently than using m_v3Rotation, at least. Local axis vs global?
		//There's probably an easier way to do this
		if (rotateX) {
			qX = glm::angleAxis(glm::radians(1.0f), AXIS_X);
		}
		else if (rotateXNeg) {
			qX = glm::angleAxis(glm::radians(-1.0f), AXIS_X);
		}
		if (rotateY) {
			qY = glm::angleAxis(glm::radians(1.0f), AXIS_Y);
		}
		else if (rotateYNeg) {
			qY = glm::angleAxis(glm::radians(-1.0f), AXIS_Y);
		}
		if (rotateZ) {
			qZ = glm::angleAxis(glm::radians(1.0f), AXIS_Z);
		}
		else if (rotateZNeg) {
			qZ = glm::angleAxis(glm::radians(-1.0f), AXIS_Z);
		}

		//glm::quat q5 = glm::quat(m_v3Rotation);

		//get quat then change toMatrix4 in render a few lines down
		m_qOrientation = m_qOrientation * qZ *qY *qX;
		//m_pMesh->Render(m4Projection, m4View, ToMatrix4(m_m4Model));
		//m_qOrientation = m_qOrientation * glm::angleAxis(glm::radians(1.0f), vector3(1.0f));
		m_pMesh->Render(m4Projection, m4View, ToMatrix4(m_qOrientation));
	}


	
	// draw a skybox
	m_pMeshMngr->AddSkyboxToRenderList();
	
	//render list call
	m_uRenderCallCount = m_pMeshMngr->Render();

	//clear the render list
	m_pMeshMngr->ClearRenderList();
	
	//draw gui
	DrawGUI();
	
	//end the current frame (internally swaps the front and back buffers)
	m_pWindow->display();
}
void Application::Release(void)
{
	SafeDelete(m_pMesh);

	//release GUI
	ShutdownGUI();
}