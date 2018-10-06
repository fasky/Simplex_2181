#include "AppClass.h"
void Application::InitVariables(void)
{
	//init the meshes
	//m_pMesh1 = new MyMesh();
	//m_pMesh1->GenerateCube(1.0f, C_BLACK);
	//cubes.push_back(m_pMesh1);
	//m_pMesh2 = new MyMesh();
	//m_pMesh2->GenerateCube(1.0f, C_BLACK);
	//cubes.push_back(m_pMesh2);

	for (int i = 0;i < 46; i++) {
		m_pMesh1 = new MyMesh();
		m_pMesh1->GenerateCube(1.0f, C_BLACK);
		cubes.push_back(m_pMesh1);
	}

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
	
	//matrix4 m4Scale = glm::scale(IDENTITY_M4, vector3(2.0f,2.0f,2.0f));
	static float value = 0.0f;
	//matrix4 m4Translate1 = glm::translate(IDENTITY_M4, vector3(0.0f, 2.0f, 3.0f));
	value += 0.01f;

	//matrix4 m4Model = m4Translate * m4Scale;
	//matrix4 m4Model1 = m4Translate1;

	//make shape
	cubes[0]->Render(m4Projection, m4View, glm::translate(IDENTITY_M4, vector3(-2.0f + value, 0.0f, 0.0f)));
	cubes[1]->Render(m4Projection, m4View, glm::translate(IDENTITY_M4, vector3(-1.0f + value, 0.0f, 0.0f)));
	cubes[2]->Render(m4Projection, m4View, glm::translate(IDENTITY_M4, vector3(1.0f + value, 0.0f, 0.0f)));
	cubes[3]->Render(m4Projection, m4View, glm::translate(IDENTITY_M4, vector3(2.0f + value, 0.0f, 0.0f)));
	cubes[4]->Render(m4Projection, m4View, glm::translate(IDENTITY_M4, vector3(-5.0f + value, 1.0f, 0.0f)));
	cubes[5]->Render(m4Projection, m4View, glm::translate(IDENTITY_M4, vector3(-3.0f + value, 1.0f, 0.0f)));
	cubes[6]->Render(m4Projection, m4View, glm::translate(IDENTITY_M4, vector3(3.0f + value, 1.0f, 0.0f)));
	cubes[7]->Render(m4Projection, m4View, glm::translate(IDENTITY_M4, vector3(5.0f + value, 1.0f, 0.0f)));
	cubes[8]->Render(m4Projection, m4View, glm::translate(IDENTITY_M4, vector3(-5.0f + value, 2.0f, 0.0f)));
	cubes[9]->Render(m4Projection, m4View, glm::translate(IDENTITY_M4, vector3(-3.0f + value, 2.0f, 0.0f)));
	cubes[10]->Render(m4Projection, m4View, glm::translate(IDENTITY_M4, vector3(-2.0f + value, 2.0f, 0.0f)));
	cubes[11]->Render(m4Projection, m4View, glm::translate(IDENTITY_M4, vector3(-1.0f + value, 2.0f, 0.0f)));
	cubes[12]->Render(m4Projection, m4View, glm::translate(IDENTITY_M4, vector3(0.0f + value, 2.0f, 0.0f)));
	cubes[13]->Render(m4Projection, m4View, glm::translate(IDENTITY_M4, vector3(1.0f + value, 2.0f, 0.0f)));
	cubes[14]->Render(m4Projection, m4View, glm::translate(IDENTITY_M4, vector3(2.0f + value, 2.0f, 0.0f)));
	cubes[15]->Render(m4Projection, m4View, glm::translate(IDENTITY_M4, vector3(3.0f + value, 2.0f, 0.0f)));
	cubes[16]->Render(m4Projection, m4View, glm::translate(IDENTITY_M4, vector3(5.0f + value, 2.0f, 0.0f)));
	cubes[17]->Render(m4Projection, m4View, glm::translate(IDENTITY_M4, vector3(-5.0f + value, 3.0f, 0.0f)));
	cubes[18]->Render(m4Projection, m4View, glm::translate(IDENTITY_M4, vector3(-4.0f + value, 3.0f, 0.0f)));
	cubes[19]->Render(m4Projection, m4View, glm::translate(IDENTITY_M4, vector3(-3.0f + value, 3.0f, 0.0f)));
	cubes[20]->Render(m4Projection, m4View, glm::translate(IDENTITY_M4, vector3(-2.0f + value, 3.0f, 0.0f)));
	cubes[21]->Render(m4Projection, m4View, glm::translate(IDENTITY_M4, vector3(-1.0f + value, 3.0f, 0.0f)));
	cubes[22]->Render(m4Projection, m4View, glm::translate(IDENTITY_M4, vector3(0.0f + value, 3.0f, 0.0f)));
	cubes[23]->Render(m4Projection, m4View, glm::translate(IDENTITY_M4, vector3(1.0f + value, 3.0f, 0.0f)));
	cubes[24]->Render(m4Projection, m4View, glm::translate(IDENTITY_M4, vector3(2.0f + value, 3.0f, 0.0f)));
	cubes[25]->Render(m4Projection, m4View, glm::translate(IDENTITY_M4, vector3(3.0f + value, 3.0f, 0.0f)));
	cubes[26]->Render(m4Projection, m4View, glm::translate(IDENTITY_M4, vector3(4.0f + value, 3.0f, 0.0f)));
	cubes[27]->Render(m4Projection, m4View, glm::translate(IDENTITY_M4, vector3(5.0f + value, 3.0f, 0.0f)));
	cubes[28]->Render(m4Projection, m4View, glm::translate(IDENTITY_M4, vector3(-4.0f + value, 4.0f, 0.0f)));
	cubes[29]->Render(m4Projection, m4View, glm::translate(IDENTITY_M4, vector3(-3.0f + value, 4.0f, 0.0f)));
	cubes[30]->Render(m4Projection, m4View, glm::translate(IDENTITY_M4, vector3(-1.0f + value, 4.0f, 0.0f)));
	cubes[31]->Render(m4Projection, m4View, glm::translate(IDENTITY_M4, vector3(0.0f + value, 4.0f, 0.0f)));
	cubes[32]->Render(m4Projection, m4View, glm::translate(IDENTITY_M4, vector3(1.0f + value, 4.0f, 0.0f)));
	cubes[33]->Render(m4Projection, m4View, glm::translate(IDENTITY_M4, vector3(3.0f + value, 4.0f, 0.0f)));
	cubes[34]->Render(m4Projection, m4View, glm::translate(IDENTITY_M4, vector3(4.0f + value, 4.0f, 0.0f)));
	cubes[35]->Render(m4Projection, m4View, glm::translate(IDENTITY_M4, vector3(-3.0f + value, 5.0f, 0.0f)));
	cubes[36]->Render(m4Projection, m4View, glm::translate(IDENTITY_M4, vector3(-2.0f + value, 5.0f, 0.0f)));
	cubes[37]->Render(m4Projection, m4View, glm::translate(IDENTITY_M4, vector3(-1.0f + value, 5.0f, 0.0f)));
	cubes[38]->Render(m4Projection, m4View, glm::translate(IDENTITY_M4, vector3(0.0f + value, 5.0f, 0.0f)));
	cubes[39]->Render(m4Projection, m4View, glm::translate(IDENTITY_M4, vector3(1.0f + value, 5.0f, 0.0f)));
	cubes[40]->Render(m4Projection, m4View, glm::translate(IDENTITY_M4, vector3(2.0f + value, 5.0f, 0.0f)));
	cubes[41]->Render(m4Projection, m4View, glm::translate(IDENTITY_M4, vector3(3.0f + value, 5.0f, 0.0f)));
	cubes[42]->Render(m4Projection, m4View, glm::translate(IDENTITY_M4, vector3(-2.0f + value, 6.0f, 0.0f)));
	cubes[43]->Render(m4Projection, m4View, glm::translate(IDENTITY_M4, vector3(2.0f + value, 6.0f, 0.0f)));
	cubes[44]->Render(m4Projection, m4View, glm::translate(IDENTITY_M4, vector3(-3.0f + value, 7.0f, 0.0f)));
	cubes[45]->Render(m4Projection, m4View, glm::translate(IDENTITY_M4, vector3(3.0f + value, 7.0f, 0.0f)));
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
	for (int i = 0; i < 46; i++) {
		SafeDelete(cubes[i]);
	}
	//SafeDelete(m_pMesh2);

	//release GUI
	ShutdownGUI();
}