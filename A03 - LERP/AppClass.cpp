#include "AppClass.h"

//Edited by Kyle Fasanella for A03 LERP

void Application::InitVariables(void)
{
	//Change this to your name and email
	m_sProgrammer = "Alberto Bobadilla - labigm@rit.edu";
	
	//Set the position and target of the camera
	//(I'm at [0,0,10], looking at [0,0,0] and up is the positive Y axis)
	m_pCameraMngr->SetPositionTargetAndUpward(AXIS_Z * 20.0f, ZERO_V3, AXIS_Y);

	//if the light position is zero move it
	if (m_pLightMngr->GetPosition(1) == ZERO_V3)
		m_pLightMngr->SetPosition(vector3(0.0f, 0.0f, 3.0f));

	//if the background is cornflowerblue change it to black (its easier to see)
	if (vector3(m_v4ClearColor) == C_BLUE_CORNFLOWER)
	{
		m_v4ClearColor = vector4(ZERO_V3, 1.0f);
	}
	
	//if there are no segments create 7
	if(m_uOrbits < 1)
		m_uOrbits = 7;

	float fSize = 1.0f; //initial size of orbits

	//creating a color using the spectrum 
	uint uColor = 650; //650 is Red
	//prevent division by 0
	float decrements = 250.0f / (m_uOrbits > 1? static_cast<float>(m_uOrbits - 1) : 1.0f); //decrement until you get to 400 (which is violet)
	/*
		This part will create the orbits, it start at 3 because that is the minimum subdivisions a torus can have
	*/
	uint uSides = 3; //start with the minimal 3 sides
	for (uint i = uSides; i < m_uOrbits + uSides; i++)
	{
		vector3 v3Color = WaveLengthToRGB(uColor); //calculate color based on wavelength
		m_shapeList.push_back(m_pMeshMngr->GenerateTorus(fSize, fSize - 0.1f, 3, i, v3Color)); //generate a custom torus and add it to the meshmanager

		//clear current stopsList
		m_stopsList.clear();
		//number of vertices - for determining stops and angle
		int vertexNum = i;

		//point of ref - cos1,sin0 (fsize,0)
		vector3 v3CurrentPos = vector3(fSize - .05f, 0, 0);

		//for circle maths - convert deg to rads
		double angle = 360.00 / vertexNum;
		double radAngle = angle * (PI / 180.00);

		//get stops, push back
		for (int j = 0; j < vertexNum; j++) {

			//move through circle, push vertices to stopsList
			v3CurrentPos = vector3((v3CurrentPos.x * cos(radAngle)) + (v3CurrentPos.y*sin(radAngle)), (-v3CurrentPos.x * sin(radAngle)) + (v3CurrentPos.y * cos(radAngle)), 0);
			m_stopsList.push_back(vector4(v3CurrentPos,i));

		}

		//make list of stop lists to reference
		//push last created stopszlist into list of stopsLists
		m_listOfStopLists.push_back(m_stopsList);

		//add for moving around different num of vertices
		//increment x and y values later to a max value depending on orbit index
		m_stopTrack.push_back(vector2(0, 1));
		//end of list making

		fSize += 0.5f; //increment the size for the next orbit
		uColor -= static_cast<uint>(decrements); //decrease the wavelength
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

	matrix4 m4View = m_pCameraMngr->GetViewMatrix(); //view Matrix
	matrix4 m4Projection = m_pCameraMngr->GetProjectionMatrix(); //Projection Matrix
	matrix4 m4Offset = IDENTITY_M4; //offset of the orbits, starts as the global coordinate system
	/*
		The following offset will orient the orbits as in the demo, start without it to make your life easier.
	*/
	m4Offset = glm::rotate(IDENTITY_M4, 1.5708f, AXIS_Z);

	// draw a shapes
	for (uint i = 0; i < m_uOrbits; ++i)
	{
		m_pMeshMngr->AddMeshToRenderList(m_shapeList[i], glm::rotate(m4Offset, 1.5708f, AXIS_X));

		//current position
		vector3 v3CurrentPos;
		//move with lerp
		static float moveNum = 0.0f;

		//check if current vertex indices are invalid and would cause errors.
		//check against w value of relevant list (same for all points in a list, so I just use 0)
		if (m_stopTrack[i].y == m_listOfStopLists[i][0].w) {
			m_stopTrack[i].y = 0;
		}
		if (m_stopTrack[i].x == m_listOfStopLists[i][0].w) {
			m_stopTrack[i].x = 0;
		}

		//lerp
		v3CurrentPos = glm::lerp(m_listOfStopLists[i][m_stopTrack[i].x], m_listOfStopLists[i][m_stopTrack[i].y], moveNum);

		//changes speed depending on num of orbits. Using the same moveNum for 100 as 5 has unintended consequences - too fast/slow moving spheres
		if (m_uOrbits > 80) {
			moveNum += .0005f;
		}
		else if (m_uOrbits > 20)
		{
			moveNum += .001f;
		}
		else {
			moveNum += 0.005f;
		}

		//if at destination, change stops to move to next one
		if (moveNum >= 1.0f) {
			for (int k = 0; k < m_uOrbits; k++) {
				//updates orbits current stops to lerp with
				m_stopTrack[k].x += 1;
				m_stopTrack[k].y += 1;
			}

			//resest movenum
			moveNum = 0.0f;
		}
		//-------------------

		matrix4 m4Model = glm::translate(m4Offset, v3CurrentPos);

		//draw spheres
		m_pMeshMngr->AddSphereToRenderList(m4Model * glm::scale(vector3(0.1)), C_WHITE);
	}

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
	//release GUI
	ShutdownGUI();
}