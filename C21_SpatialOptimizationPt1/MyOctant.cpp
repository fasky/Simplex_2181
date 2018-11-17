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
	std::vector<MyEntity*> l_Entity_List = m_pEntityManager->GetEntityList();
	uint iEntityCount = l_Entity_List.size();
	MyRigidBody* pRB = l_Entity_List[a_uRBIndex]->GetRigidBody();
	if (pRB->IsColliding(m_pRigidBody)) {
		l_Entity_List[a_uRBIndex]->AddDimension(m_iID);
		return true;
	}
	else{
		return false;
	}

}

//default iscolliding
void Simplex::MyOctant::IsColliding(void)
{
	if (IsLeaf()) {
		std::vector<MyEntity*> l_Entity_List = m_pEntityManager->GetEntityList();
		uint iEntityCount = l_Entity_List.size();
		for (uint i = 0; i < iEntityCount; ++i)
		{
			m_pEntityManager->ClearDimensionSet(i);
			MyRigidBody* pRB = l_Entity_List[i]->GetRigidBody();
			if (pRB->IsColliding(m_pRigidBody)) {
				m_EntityList.push_back(i);
				l_Entity_List[i]->AddDimension(m_iID);
			}
		}
	}

}

#pragma region display functions
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
			else{
				if (m_pChild[i]->IsLeaf() != true) {
					m_pChild[i]->Display(a_nIndex, a_v3Color);
				}
			}
		}
	}

}
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

bool Simplex::MyOctant::IsLeaf(void)
{
	if (m_uChildren == 0) {
		return true;
	}
	return false;
}

bool Simplex::MyOctant::ContainsMoreThan(uint a_nEntities)
{
	if (m_EntityList.size() > a_nEntities) {
		return true;
	}
	return false;
}

//construct list of leafs with objects
void Simplex::MyOctant::ConstructList(void)
{
	//loop through kids, check for leaves, add if a leaf with entities in it
}
//void Simplex::MyOctant::ConstructTree(uint a_nMaxLevel)
//{
//
//}

//assign id 
//void Simplex::MyOctant::AssignIDtoEntity(void)
//{
//
//}

//default subdivide
void MyOctant::Subdivide() {
	if (m_uLevel > m_uMaxLevel - 1) {
		return;
	}

	IsColliding();
	if (m_EntityList.size() <= m_uIdealEntityCount) {
		return;
	}

	for (uint i = 0; i < 8; i++) {
		m_pChild[i] = nullptr;
	}
	//vector3 v3Center = m_pRigidBody->GetCenterLocal();
	//vector3 v3HalfWidth = m_pRigidBody->GetHalfWidth();
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

	for (uint i = 0; i < 8; i++) {
		m_pChild[i]->m_uLevel = m_uLevel + 1;
		m_pChild[i]->m_pParent = this;
		//m_pChild[i]->IsColliding();
		//if (m_pChild[i]->m_EntityList.size() > m_uIdealEntityCount) {
		m_pChild[i]->Subdivide();
		//}	
	}

	//for (uint i = 0; i < 8; i++) {
	//	if (m_pChild[i]->IsLeaf()) {
	//		m_pChild[i]->IsColliding();
	//	}
	//}

}

#pragma region The big ones

//constructors
MyOctant::MyOctant(uint a_nMaxLevel, uint a_nIdealEntityCount){
	Init();
	for (uint i = 0; i < 8; i++) {
		m_pChild[i] = nullptr;
	}
	std::vector<MyEntity*> l_Entity_List = m_pEntityManager->GetEntityList();
	uint iEntityCount = l_Entity_List.size();
	std::vector<vector3> v3MaxMin_list;
	for (uint i = 0; i < iEntityCount; ++i)
	{
		MyRigidBody* pRG = l_Entity_List[i]->GetRigidBody();
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

	if (m_uMaxLevel > 0) {
		Subdivide();
	}

}
//void MyOctant::SetLevelIdeal(uint a_nMaxLevel, uint a_nIdealEntityCount)
//{
//	m_uMaxLevel = a_nMaxLevel;
//	m_uIdealEntityCount = a_nIdealEntityCount;
//}

Simplex::MyOctant::MyOctant(vector3 a_v3Center, float a_fSize)
{
	Init();
	//get points from params
	std::vector<vector3> v3MaxMin_list;
	v3MaxMin_list.push_back(a_v3Center - vector3(a_fSize));
	v3MaxMin_list.push_back(a_v3Center + vector3(a_fSize));
	m_pRigidBody = new MyRigidBody(v3MaxMin_list);
	m_uOctantCount++;
	m_iID = m_uOctantCount;
	m_fSize = m_pRigidBody->GetHalfWidth().x;
	m_v3Max = m_pRigidBody->GetMaxLocal();
	m_v3Min = m_pRigidBody->GetMinLocal();
	m_v3Center = m_pRigidBody->GetCenterLocal();
	//SetLevelIdeal(2, 10);
	//IsColliding();
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
	if(this != &other)
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

//destructor
MyOctant::~MyOctant()
{
	Release();
}
//release/clear - call on destruct
void MyOctant::Release(void)
{
	//ClearEntityList();
	SafeDelete(m_pRigidBody);
	if (m_iID != 0) {
		SafeDelete(m_pParent);
	}
	//for (uint i = 0; i < 8; i++) {
	//	SafeDelete(m_pChild[i]);
	//}

}
void Simplex::MyOctant::KillBranches(void)
{
	//for (uint i = 0; i < 8; i++) {
	//	if (m_pChild[i]) {
	//		m_pChild[i]->KillBranches();
	//	}
	//}
	//Release();

}
void Simplex::MyOctant::ClearEntityList(void)
{

}
#pragma endregion