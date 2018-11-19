#include "MyOctant.h"
using namespace Simplex;
uint MyOctant::m_uOctantCount;
uint MyOctant::m_uMaxLevel;
uint MyOctant::m_uIdealEntityCount;

//  MyOctant
// initialize
void MyOctant::Init(void)
{
	m_pMeshMngr = MeshManager::GetInstance();
	m_pEntityManager = MyEntityManager::GetInstance();
	for (uint i = 0; i < 8; i++) {
		m_pChild[i] = nullptr;
	}
	//m_uOctantCount = 0;
}

#pragma region gets
//gets ---
float Simplex::MyOctant::GetSize(void)
{
	return m_fSize;
}
vector3 Simplex::MyOctant::GetCenterGlobal(void)
{
	return m_v3Center;
}
vector3 Simplex::MyOctant::GetMinGlobal(void)
{
	return m_v3Min;
}
vector3 Simplex::MyOctant::GetMaxGlobal(void)
{
	return m_v3Max;
}
uint Simplex::MyOctant::GetOctantCount(void)
{
	uint count = m_uOctantCount;
	return count;
}
MyOctant * Simplex::MyOctant::GetChild(uint a_nChild)
{
	return m_pChild[a_nChild];
}

MyOctant * Simplex::MyOctant::GetParent(void)
{
	return m_pParent;
}
#pragma endregion

//is there collision for certain index
bool Simplex::MyOctant::IsColliding(uint a_uRBIndex)
{
	MyRigidBody* pRB = m_pEntityManager->GetRigidBody(a_uRBIndex);
	if (pRB->IsColliding(m_pRigidBody)) {
		m_pEntityManager->ClearDimensionSet(a_uRBIndex);
		m_pEntityManager->AddDimension(a_uRBIndex, m_iID);
		return true;
	}
	else {
		return false;
	}

}

//default iscolliding
void Simplex::MyOctant::IsColliding(void)
{
	//if (IsLeaf()) {
	//get entity count, use to loop and check all cubes against octant
	uint iEntityCount = m_pEntityManager->GetEntityCount();
	for (uint i = 0; i < iEntityCount; ++i)
	{
		MyRigidBody* pRB = m_pEntityManager->GetRigidBody(i);
		//if colliding with octant, reset dimension
		if (pRB->IsColliding(m_pRigidBody)) {
			for (uint j = 0; j < iEntityCount; ++j)
			{
				MyRigidBody* pRBTwo = m_pEntityManager->GetRigidBody(j);
				if (pRB->IsColliding(pRBTwo)) {
					m_pEntityManager->AddDimension(j, m_iID);
				}
			}
			m_pEntityManager->ClearDimensionSet(i); //biggest optimization for me - framerate jumped up after
			m_EntityList.push_back(i);
			m_pEntityManager->AddDimension(i, m_iID);
	
		}
	}
	//}

}

#pragma region display functions
//display certain octant
void Simplex::MyOctant::Display(uint a_nIndex, vector3 a_v3Color)
{

	if (m_iID == a_nIndex) {
		m_pRigidBody->SetColorNotColliding(a_v3Color);
		m_pRigidBody->SetColorColliding(a_v3Color);
		m_pRigidBody->AddToRenderList();
	}
	else {

		for (uint i = 0; i < 8; i++) {
			if (m_pChild[i]->m_iID == a_nIndex) {
				m_pChild[i]->Display();
			}
			else {
				if (m_pChild[i]->IsLeaf() != true) {
					m_pChild[i]->Display(a_nIndex, a_v3Color);
				}
			}
		}
	}

}
//display all octants
void Simplex::MyOctant::Display(vector3 a_v3Color)
{

	m_pRigidBody->SetColorNotColliding(a_v3Color);
	m_pRigidBody->SetColorColliding(a_v3Color);
	m_pRigidBody->AddToRenderList();
	if (!IsLeaf()) {
		for (uint i = 0; i < 8; i++) {
			if (m_pChild[i]) {
				m_pChild[i]->Display();
			}
		}
	}
}
//display leaf octants - ones with no kids
void Simplex::MyOctant::DisplayLeafs(vector3 a_v3Color)
{
	if (IsLeaf()) {
		m_pRigidBody->SetColorNotColliding(a_v3Color);
		m_pRigidBody->SetColorColliding(a_v3Color);
		m_pRigidBody->AddToRenderList();
	}
	else {
		for (uint i = 0; i < 8; i++) {
			if (m_pChild[i]) {
				m_pChild[i]->DisplayLeafs();
			}
		}
	}
}
#pragma endregion

//if no children, its a leaf
bool Simplex::MyOctant::IsLeaf(void)
{
	if (m_uChildren == 0) {
		return true;
	}
	return false;
}

//if the entities are greater than the value to check against, return true
bool Simplex::MyOctant::ContainsMoreThan(uint a_nEntities)
{
	if (m_EntityList.size() > a_nEntities) {
		return true;
	}
	return false;
}

//ignore these
////construct list of leafs with objects
//void Simplex::MyOctant::ConstructList(void)
//{
//	//loop through kids, check for leaves, add if a leaf with entities in it
//}
//void Simplex::MyOctant::ConstructTree(uint a_nMaxLevel)
//{
//
//}

//assign id 
//void Simplex::MyOctant::AssignIDtoEntity(void)
//{
//
//}

//subdivide
void MyOctant::Subdivide() {
	//check if max level is reached
	if (m_uLevel > m_uMaxLevel - 1) {
		return;
	}

	//check collisions
	IsColliding();
	//if generated entity list from iscolliding has a count smaller than the ideal entity count, return and don't keep subdividing
	if (m_EntityList.size() <= m_uIdealEntityCount) {
		return;
	}

	//otherwise set up children
	float fSize = (m_fSize) / 2.0f;
	float fCenters = fSize;
	m_pChild[0] = new MyOctant(m_v3Center + vector3(fCenters, fCenters, fCenters), fSize);
	m_pChild[1] = new MyOctant(m_v3Center + vector3(-fCenters, fCenters, fCenters), fSize);
	m_pChild[2] = new MyOctant(m_v3Center + vector3(-fCenters, -fCenters, fCenters), fSize);
	m_pChild[3] = new MyOctant(m_v3Center + vector3(fCenters, -fCenters, fCenters), fSize);

	m_pChild[4] = new MyOctant(m_v3Center + vector3(fCenters, fCenters, -fCenters), fSize);
	m_pChild[5] = new MyOctant(m_v3Center + vector3(-fCenters, fCenters, -fCenters), fSize);
	m_pChild[6] = new MyOctant(m_v3Center + vector3(-fCenters, -fCenters, -fCenters), fSize);
	m_pChild[7] = new MyOctant(m_v3Center + vector3(fCenters, -fCenters, -fCenters), fSize);

	m_uChildren = 8;

	//and try subidivind them
	for (uint i = 0; i < 8; i++) {
		m_pChild[i]->m_uLevel = m_uLevel + 1;
		m_pChild[i]->m_pParent = this;
		//m_pChild[i]->IsColliding();
		//if (m_pChild[i]->m_EntityList.size() > m_uIdealEntityCount) {
		m_pChild[i]->Subdivide();
		//}	
	}

}

#pragma region The big ones

//constructors
//root set up - init and set values, set rigidbody cube
MyOctant::MyOctant(uint a_nMaxLevel, uint a_nIdealEntityCount) {
	Init();
	uint iEntityCount = m_pEntityManager->GetEntityCount();
	std::vector<vector3> v3MaxMin_list;

	for (uint i = 0; i < iEntityCount; ++i)
	{
		MyRigidBody* pRG = m_pEntityManager->GetRigidBody(i);
		vector3 v3Min = pRG->GetMinGlobal();
		vector3 v3Max = pRG->GetMaxGlobal();
		v3MaxMin_list.push_back(v3Min);
		v3MaxMin_list.push_back(v3Max);
	}

	m_pRigidBody = new MyRigidBody(v3MaxMin_list);
	m_pRigidBody->MakeCubic();
	m_fSize = m_pRigidBody->GetHalfWidth().x;
	m_v3Max = m_pRigidBody->GetMaxLocal();
	m_v3Min = m_pRigidBody->GetMinLocal();
	m_v3Center = m_pRigidBody->GetCenterLocal();
	m_uMaxLevel = a_nMaxLevel;
	m_uIdealEntityCount = a_nIdealEntityCount;
	m_uOctantCount = 0;

	//subdivide if appropriate
	if (m_uMaxLevel > 0) {
		Subdivide();
	}

}
//ignore
//void MyOctant::SetLevelIdeal(uint a_nMaxLevel, uint a_nIdealEntityCount)
//{
//	m_uMaxLevel = a_nMaxLevel;
//	m_uIdealEntityCount = a_nIdealEntityCount;
//}

//for children - set values - called when subdividing
Simplex::MyOctant::MyOctant(vector3 a_v3Center, float a_fSize)
{
	Init();
	//get points from params
	std::vector<vector3> v3MaxMin_list;
	v3MaxMin_list.push_back(a_v3Center - vector3(a_fSize));
	v3MaxMin_list.push_back(a_v3Center + vector3(a_fSize));
	m_pRigidBody = new MyRigidBody(v3MaxMin_list);
	m_uOctantCount = m_uOctantCount + 1;
	m_iID = m_uOctantCount;
	m_fSize = m_pRigidBody->GetHalfWidth().x;
	m_v3Max = m_pRigidBody->GetMaxLocal();
	m_v3Min = m_pRigidBody->GetMinLocal();
	m_v3Center = m_pRigidBody->GetCenterLocal();

	//uint entColCount = 0;
	//uint entCount = m_pEntityManager->GetEntityCount();
	//for (uint j = 0; j < entCount; j++) {
	//	bool isCol = IsColliding(j);

	//	if (isCol) {
	//		entColCount++;
	//	}

	//	if (entColCount > m_uIdealEntityCount) {
	//		//m_pEntityManager->ClearDimensionSet(j);
	//		if (m_uLevel > m_uMaxLevel - 1) {
	//			return;
	//		}
	//		Subdivide();
	//	}
	//}
}

//copy cons
MyOctant::MyOctant(MyOctant const& other)
{
	m_uOctantCount = other.m_uOctantCount;
	m_uMaxLevel = other.m_uMaxLevel;
	m_uIdealEntityCount = other.m_uIdealEntityCount;

	m_iID = other.m_iID;
	m_uLevel = other.m_uLevel;
	m_uChildren = other.m_uChildren;

	m_fSize = other.m_fSize;

	m_pMeshMngr = other.m_pMeshMngr;
	m_pEntityManager = other.m_pEntityManager;
	m_pRigidBody = other.m_pRigidBody;

	m_v3Center = other.m_v3Center;
	m_v3Min = other.m_v3Min;
	m_v3Max = other.m_v3Max;

	m_pParent = other.m_pParent;
	m_pChild[8] = other.m_pChild[8];

	m_EntityList = other.m_EntityList;

	m_pRoot = other.m_pRoot;
	m_lChild = other.m_lChild;
}

//copy assignment op
MyOctant& MyOctant::operator=(MyOctant const& other)
{
	if (this != &other)
	{
		Release();
		Init();
		MyOctant temp(other);
		Swap(temp);
	}
	return *this;
}

//swap
void MyOctant::Swap(MyOctant& other)
{
	std::swap(m_uOctantCount, other.m_uOctantCount);
	std::swap(m_uMaxLevel, other.m_uMaxLevel);
	std::swap(m_uIdealEntityCount, other.m_uIdealEntityCount);

	std::swap(m_iID, other.m_iID);
	std::swap(m_uLevel, other.m_uLevel);
	std::swap(m_uChildren, other.m_uChildren);

	std::swap(m_fSize, other.m_fSize);

	std::swap(m_pMeshMngr, other.m_pMeshMngr);
	std::swap(m_pEntityManager, other.m_pEntityManager);
	std::swap(m_pRigidBody, other.m_pRigidBody);

	std::swap(m_v3Center, other.m_v3Center);
	std::swap(m_v3Min, other.m_v3Min);
	std::swap(m_v3Max, other.m_v3Max);

	std::swap(m_pParent, other.m_pParent);
	std::swap(m_pChild[8], other.m_pChild[8]);

	std::swap(m_EntityList, other.m_EntityList);

	std::swap(m_pRoot, other.m_pRoot);
	std::swap(m_lChild, other.m_lChild);
}

#pragma region try to take care of memory leaks
//destructor
MyOctant::~MyOctant()
{
	//just call release
	Release();
}
//release/clear - call on destruct
void MyOctant::Release(void)
{		
	//clear entity list, set pointers to null, delete children and rb
	ClearEntityList();
	m_pRoot = nullptr;
	m_pMeshMngr = nullptr;
	m_pEntityManager = nullptr;
	m_pParent = nullptr;

	//delete rb
	SafeDelete(m_pRigidBody);
	//delete children
	KillBranches();
}

//kill children
void Simplex::MyOctant::KillBranches(void)
{
	//delete children
	for (uint i = 0; i < 8; i++) {
		if (m_pChild[i]) {
			SafeDelete(m_pChild[i]);
		}
	}

}

//clear ent list
void Simplex::MyOctant::ClearEntityList(void)
{
	m_EntityList.clear();
	for (uint i = 0; i < 8; i++) {
		if (m_pChild[i]) {
			m_pChild[i]->ClearEntityList();
		}
	}
}
#pragma endregion

#pragma endregion