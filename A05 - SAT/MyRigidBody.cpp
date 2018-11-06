#include "MyRigidBody.h"
using namespace Simplex;
//Allocation
void MyRigidBody::Init(void)
{
	m_pMeshMngr = MeshManager::GetInstance();
	m_bVisibleBS = false;
	m_bVisibleOBB = true;
	m_bVisibleARBB = false;

	m_fRadius = 0.0f;

	m_v3ColorColliding = C_RED;
	m_v3ColorNotColliding = C_WHITE;

	m_v3Center = ZERO_V3;
	m_v3MinL = ZERO_V3;
	m_v3MaxL = ZERO_V3;

	m_v3MinG = ZERO_V3;
	m_v3MaxG = ZERO_V3;

	m_v3HalfWidth = ZERO_V3;
	m_v3ARBBSize = ZERO_V3;

	m_m4ToWorld = IDENTITY_M4;
}
void MyRigidBody::Swap(MyRigidBody& a_pOther)
{
	std::swap(m_pMeshMngr, a_pOther.m_pMeshMngr);
	std::swap(m_bVisibleBS, a_pOther.m_bVisibleBS);
	std::swap(m_bVisibleOBB, a_pOther.m_bVisibleOBB);
	std::swap(m_bVisibleARBB, a_pOther.m_bVisibleARBB);

	std::swap(m_fRadius, a_pOther.m_fRadius);

	std::swap(m_v3ColorColliding, a_pOther.m_v3ColorColliding);
	std::swap(m_v3ColorNotColliding, a_pOther.m_v3ColorNotColliding);

	std::swap(m_v3Center, a_pOther.m_v3Center);
	std::swap(m_v3MinL, a_pOther.m_v3MinL);
	std::swap(m_v3MaxL, a_pOther.m_v3MaxL);

	std::swap(m_v3MinG, a_pOther.m_v3MinG);
	std::swap(m_v3MaxG, a_pOther.m_v3MaxG);

	std::swap(m_v3HalfWidth, a_pOther.m_v3HalfWidth);
	std::swap(m_v3ARBBSize, a_pOther.m_v3ARBBSize);

	std::swap(m_m4ToWorld, a_pOther.m_m4ToWorld);

	std::swap(m_CollidingRBSet, a_pOther.m_CollidingRBSet);
}
void MyRigidBody::Release(void)
{
	m_pMeshMngr = nullptr;
	ClearCollidingList();
}
//Accessors
bool MyRigidBody::GetVisibleBS(void) { return m_bVisibleBS; }
void MyRigidBody::SetVisibleBS(bool a_bVisible) { m_bVisibleBS = a_bVisible; }
bool MyRigidBody::GetVisibleOBB(void) { return m_bVisibleOBB; }
void MyRigidBody::SetVisibleOBB(bool a_bVisible) { m_bVisibleOBB = a_bVisible; }
bool MyRigidBody::GetVisibleARBB(void) { return m_bVisibleARBB; }
void MyRigidBody::SetVisibleARBB(bool a_bVisible) { m_bVisibleARBB = a_bVisible; }
float MyRigidBody::GetRadius(void) { return m_fRadius; }
vector3 MyRigidBody::GetColorColliding(void) { return m_v3ColorColliding; }
vector3 MyRigidBody::GetColorNotColliding(void) { return m_v3ColorNotColliding; }
void MyRigidBody::SetColorColliding(vector3 a_v3Color) { m_v3ColorColliding = a_v3Color; }
void MyRigidBody::SetColorNotColliding(vector3 a_v3Color) { m_v3ColorNotColliding = a_v3Color; }
vector3 MyRigidBody::GetCenterLocal(void) { return m_v3Center; }
vector3 MyRigidBody::GetMinLocal(void) { return m_v3MinL; }
vector3 MyRigidBody::GetMaxLocal(void) { return m_v3MaxL; }
vector3 MyRigidBody::GetCenterGlobal(void){	return vector3(m_m4ToWorld * vector4(m_v3Center, 1.0f)); }
vector3 MyRigidBody::GetMinGlobal(void) { return m_v3MinG; }
vector3 MyRigidBody::GetMaxGlobal(void) { return m_v3MaxG; }
vector3 MyRigidBody::GetHalfWidth(void) { return m_v3HalfWidth; }
matrix4 MyRigidBody::GetModelMatrix(void) { return m_m4ToWorld; }
void MyRigidBody::SetModelMatrix(matrix4 a_m4ModelMatrix)
{
	//to save some calculations if the model matrix is the same there is nothing to do here
	if (a_m4ModelMatrix == m_m4ToWorld)
		return;

	//Assign the model matrix
	m_m4ToWorld = a_m4ModelMatrix;

	//Calculate the 8 corners of the cube
	//Back square
	v3Corner[0] = m_v3MinL;
	v3Corner[1] = vector3(m_v3MaxL.x, m_v3MinL.y, m_v3MinL.z);
	v3Corner[2] = vector3(m_v3MinL.x, m_v3MaxL.y, m_v3MinL.z);
	v3Corner[3] = vector3(m_v3MaxL.x, m_v3MaxL.y, m_v3MinL.z);

	//Front square
	v3Corner[4] = vector3(m_v3MinL.x, m_v3MinL.y, m_v3MaxL.z);
	v3Corner[5] = vector3(m_v3MaxL.x, m_v3MinL.y, m_v3MaxL.z);
	v3Corner[6] = vector3(m_v3MinL.x, m_v3MaxL.y, m_v3MaxL.z);
	v3Corner[7] = m_v3MaxL;

	//Place them in world space
	for (uint uIndex = 0; uIndex < 8; ++uIndex)
	{
		v3Corner[uIndex] = vector3(m_m4ToWorld * vector4(v3Corner[uIndex], 1.0f));
	}

	//Identify the max and min as the first corner
	m_v3MaxG = m_v3MinG = v3Corner[0];

	//get the new max and min for the global box
	for (uint i = 1; i < 8; ++i)
	{
		if (m_v3MaxG.x < v3Corner[i].x) m_v3MaxG.x = v3Corner[i].x;
		else if (m_v3MinG.x > v3Corner[i].x) m_v3MinG.x = v3Corner[i].x;

		if (m_v3MaxG.y < v3Corner[i].y) m_v3MaxG.y = v3Corner[i].y;
		else if (m_v3MinG.y > v3Corner[i].y) m_v3MinG.y = v3Corner[i].y;

		if (m_v3MaxG.z < v3Corner[i].z) m_v3MaxG.z = v3Corner[i].z;
		else if (m_v3MinG.z > v3Corner[i].z) m_v3MinG.z = v3Corner[i].z;
	}

	//we calculate the distance between min and max vectors
	m_v3ARBBSize = m_v3MaxG - m_v3MinG;
}
bool Simplex::MyRigidBody::IsZeroVector(vector3 m)
{
	float tolerance = .001f;
	if (m.x < tolerance || m.x > -tolerance || m.y < tolerance || m.y > -tolerance || m.z < tolerance || m.z > -tolerance) {
		return true;
	}
	else {
		return false;
	}
}
//The big 3
MyRigidBody::MyRigidBody(std::vector<vector3> a_pointList)
{
	Init();
	//Count the points of the incoming list
	uint uVertexCount = a_pointList.size();

	//If there are none just return, we have no information to create the BS from
	if (uVertexCount == 0)
		return;

	//Max and min as the first vector of the list
	m_v3MaxL = m_v3MinL = a_pointList[0];

	//Get the max and min out of the list
	for (uint i = 1; i < uVertexCount; ++i)
	{
		if (m_v3MaxL.x < a_pointList[i].x) m_v3MaxL.x = a_pointList[i].x;
		else if (m_v3MinL.x > a_pointList[i].x) m_v3MinL.x = a_pointList[i].x;

		if (m_v3MaxL.y < a_pointList[i].y) m_v3MaxL.y = a_pointList[i].y;
		else if (m_v3MinL.y > a_pointList[i].y) m_v3MinL.y = a_pointList[i].y;

		if (m_v3MaxL.z < a_pointList[i].z) m_v3MaxL.z = a_pointList[i].z;
		else if (m_v3MinL.z > a_pointList[i].z) m_v3MinL.z = a_pointList[i].z;
	}

	//with model matrix being the identity, local and global are the same
	m_v3MinG = m_v3MinL;
	m_v3MaxG = m_v3MaxL;

	//with the max and the min we calculate the center
	m_v3Center = (m_v3MaxL + m_v3MinL) / 2.0f;

	//we calculate the distance between min and max vectors
	m_v3HalfWidth = (m_v3MaxL - m_v3MinL) / 2.0f;

	//Get the distance between the center and either the min or the max
	m_fRadius = glm::distance(m_v3Center, m_v3MinL);
}
MyRigidBody::MyRigidBody(MyRigidBody const& a_pOther)
{
	m_pMeshMngr = a_pOther.m_pMeshMngr;

	m_bVisibleBS = a_pOther.m_bVisibleBS;
	m_bVisibleOBB = a_pOther.m_bVisibleOBB;
	m_bVisibleARBB = a_pOther.m_bVisibleARBB;

	m_fRadius = a_pOther.m_fRadius;

	m_v3ColorColliding = a_pOther.m_v3ColorColliding;
	m_v3ColorNotColliding = a_pOther.m_v3ColorNotColliding;

	m_v3Center = a_pOther.m_v3Center;
	m_v3MinL = a_pOther.m_v3MinL;
	m_v3MaxL = a_pOther.m_v3MaxL;

	m_v3MinG = a_pOther.m_v3MinG;
	m_v3MaxG = a_pOther.m_v3MaxG;

	m_v3HalfWidth = a_pOther.m_v3HalfWidth;
	m_v3ARBBSize = a_pOther.m_v3ARBBSize;

	m_m4ToWorld = a_pOther.m_m4ToWorld;

	m_CollidingRBSet = a_pOther.m_CollidingRBSet;
}
MyRigidBody& MyRigidBody::operator=(MyRigidBody const& a_pOther)
{
	if (this != &a_pOther)
	{
		Release();
		Init();
		MyRigidBody temp(a_pOther);
		Swap(temp);
	}
	return *this;
}
MyRigidBody::~MyRigidBody() { Release(); };
//--- a_pOther Methods
void MyRigidBody::AddCollisionWith(MyRigidBody* a_pOther)
{
	/*
		check if the object is already in the colliding set, if
		the object is already there return with no changes
	*/
	auto element = m_CollidingRBSet.find(a_pOther);
	if (element != m_CollidingRBSet.end())
		return;
	// we couldn't find the object so add it
	m_CollidingRBSet.insert(a_pOther);
}
void MyRigidBody::RemoveCollisionWith(MyRigidBody* a_pOther)
{
	m_CollidingRBSet.erase(a_pOther);
}
void MyRigidBody::ClearCollidingList(void)
{
	m_CollidingRBSet.clear();
}
bool MyRigidBody::IsColliding(MyRigidBody* const a_pOther)
{
	//check if spheres are colliding as pre-test
	bool bColliding = (glm::distance(GetCenterGlobal(), a_pOther->GetCenterGlobal()) < m_fRadius + a_pOther->m_fRadius);
	
	//if they are colliding check the SAT
	if (bColliding)
	{
		if(SAT(a_pOther) != eSATResults::SAT_NONE)
			bColliding = false;// reset to false
	}

	if (bColliding) //they are colliding
	{
		this->AddCollisionWith(a_pOther);
		a_pOther->AddCollisionWith(this);
	}
	else //they are not colliding
	{
		this->RemoveCollisionWith(a_pOther);
		a_pOther->RemoveCollisionWith(this);
	}

	return bColliding;
}
void MyRigidBody::AddToRenderList(void)
{
	if (m_bVisibleBS)
	{
		if (m_CollidingRBSet.size() > 0)
			m_pMeshMngr->AddWireSphereToRenderList(glm::translate(m_m4ToWorld, m_v3Center) * glm::scale(vector3(m_fRadius)), C_BLUE_CORNFLOWER);
		else
			m_pMeshMngr->AddWireSphereToRenderList(glm::translate(m_m4ToWorld, m_v3Center) * glm::scale(vector3(m_fRadius)), C_BLUE_CORNFLOWER);
	}
	if (m_bVisibleOBB)
	{
		if (m_CollidingRBSet.size() > 0)
			m_pMeshMngr->AddWireCubeToRenderList(glm::translate(m_m4ToWorld, m_v3Center) * glm::scale(m_v3HalfWidth * 2.0f), m_v3ColorColliding);
		else
			m_pMeshMngr->AddWireCubeToRenderList(glm::translate(m_m4ToWorld, m_v3Center) * glm::scale(m_v3HalfWidth * 2.0f), m_v3ColorNotColliding);
	}
	if (m_bVisibleARBB)
	{
		if (m_CollidingRBSet.size() > 0)
			m_pMeshMngr->AddWireCubeToRenderList(glm::translate(GetCenterGlobal()) * glm::scale(m_v3ARBBSize), C_YELLOW);
		else
			m_pMeshMngr->AddWireCubeToRenderList(glm::translate(GetCenterGlobal()) * glm::scale(m_v3ARBBSize), C_YELLOW);
	}
}

uint MyRigidBody::SAT(MyRigidBody* const a_pOther)
{

	//it doesn't work idk what im doing - commented out attempts (show i didnt not try)

	//get distance between centers - cB - cA
	vector3 distance =  glm::abs(a_pOther->GetCenterGlobal() - GetCenterGlobal());

	vector3 halfA = GetHalfWidth();
	vector3 halfB = glm::rotate(a_pOther->GetHalfWidth(), glm::radians(-55.0f), AXIS_Z);

	/* Compute a tentative separating axis for ab and cd
	Vector m = Cross(ab, cd);
	if (!IsZeroVector(m)) {
		// Edges ab and cd not parallel, continue with m as a potential separating axis
		...
	}
	else {
		// Edges ab and cd must be (near) parallel, and therefore lie in some plane P.
		// Thus, as a separating axis try an axis perpendicular to ab and lying in P
		Vector n = Cross(ab, c - a);
		m = Cross(ab, n);
		if (!IsZeroVector(m)) {
			// Continue with m as a potential separating axis
			...
		}
		// ab and ac are parallel too, so edges must be on a line. Ignore testing
		// the axis for this combination of edges as it won’t be a separating axis.
		// (Alternatively, test if edges overlap on this line, in which case the
		// objects are overlapping.)
		...
	}*/

	//z = point 4 - 0, y = point 2 - 0, x = point 1 - 0
	zAxis = glm::abs(vector3(m_v3MinG.x, m_v3MinG.y, m_v3MaxG.z) - m_v3MinG);
	yAxis = glm::abs(vector3(m_v3MinG.x, m_v3MaxG.y, m_v3MinG.z) - m_v3MinG);
	xAxis = glm::abs(vector3(m_v3MaxG.x, m_v3MinG.y, m_v3MinG.z) - m_v3MinG);
	a_pOther->zAxis = glm::abs(vector3(a_pOther->m_v3MinG.x, a_pOther->m_v3MinG.y, a_pOther->m_v3MaxG.z) - a_pOther->m_v3MinG);
	a_pOther->yAxis = glm::abs(vector3(a_pOther->m_v3MinG.x, a_pOther->m_v3MaxG.y, a_pOther->m_v3MinG.z) - a_pOther->m_v3MinG);
	a_pOther->xAxis = glm::abs(vector3(a_pOther->m_v3MaxG.x, a_pOther->m_v3MinG.y, a_pOther->m_v3MinG.z) - a_pOther->m_v3MinG);


	if ((glm::abs(halfA.x) + glm::abs(halfB.x)) < distance.x) {
		return eSATResults::SAT_AX;
	}

	if ((glm::abs(halfA.y) + glm::abs(halfB.y)) < distance.y) {
		return eSATResults::SAT_AY;
	}

	if ((glm::abs(halfA.z) + glm::abs(halfB.z)) < distance.z) {
		return eSATResults::SAT_AZ;
	}
	/*
	if(CheckEdges(v3Corner[0], v3Corner[1], a_pOther->v3Corner[0], a_pOther->v3Corner[1],distance,halfA,halfB)) {
		return 1;
	}
	if (CheckEdges(v3Corner[0], v3Corner[2], a_pOther->v3Corner[0], a_pOther->v3Corner[2], distance, halfA, halfB)) {
		return 1;
	}
	if (CheckEdges(v3Corner[0], v3Corner[4], a_pOther->v3Corner[0], a_pOther->v3Corner[4], distance, halfA, halfB)) {
		return 1;
	}
	if (CheckEdges(v3Corner[4], v3Corner[6], a_pOther->v3Corner[4], a_pOther->v3Corner[6], distance, halfA, halfB)) {
		return 1;
	}
	if (CheckEdges(v3Corner[4], v3Corner[5], a_pOther->v3Corner[4], a_pOther->v3Corner[5], distance, halfA, halfB)) {
		return 1;
	}
	if (CheckEdges(v3Corner[5], v3Corner[7], a_pOther->v3Corner[5], a_pOther->v3Corner[7], distance, halfA, halfB)) {
		return 1;
	}
	if (CheckEdges(v3Corner[5], v3Corner[1], a_pOther->v3Corner[5], a_pOther->v3Corner[1], distance, halfA, halfB)) {
		return 1;
	}
	if (CheckEdges(v3Corner[1], v3Corner[3], a_pOther->v3Corner[1], a_pOther->v3Corner[3], distance, halfA, halfB)) {
		return 1;
	}
	if (CheckEdges(v3Corner[6], v3Corner[7], a_pOther->v3Corner[6], a_pOther->v3Corner[7], distance, halfA, halfB)) {
		return 1;
	}
	if (CheckEdges(v3Corner[6], v3Corner[2], a_pOther->v3Corner[6], a_pOther->v3Corner[2], distance, halfA, halfB)) {
		return 1;
	}
	if (CheckEdges(v3Corner[3], v3Corner[7], a_pOther->v3Corner[3], a_pOther->v3Corner[7], distance, halfA, halfB)) {
		return 1;
	}
	if (CheckEdges(v3Corner[3], v3Corner[2], a_pOther->v3Corner[3], a_pOther->v3Corner[2], distance, halfA, halfB)) {
		return 1;
	}*/

	/*
	//z = point 4 - 0, y = point 2 - 0, x = point 1 - 0
	zAxis = glm::normalize(vector3(m_v3MinG.x, m_v3MinG.y, m_v3MaxG.z) - m_v3MinG);
	yAxis = glm::normalize(vector3(m_v3MinG.x, m_v3MaxG.y, m_v3MinG.z) - m_v3MinG);
	xAxis = glm::normalize(vector3(m_v3MaxG.x, m_v3MinG.y, m_v3MinG.z) - m_v3MinG);
	a_pOther->zAxis = glm::normalize(vector3(a_pOther->m_v3MinG.x, a_pOther->m_v3MinG.y, a_pOther->m_v3MaxG.z) - a_pOther->m_v3MinG);
	a_pOther->yAxis = glm::normalize(vector3(a_pOther->m_v3MinG.x, a_pOther->m_v3MaxG.y, a_pOther->m_v3MinG.z) - a_pOther->m_v3MinG);
	a_pOther->xAxis = glm::normalize(vector3(a_pOther->m_v3MaxG.x, a_pOther->m_v3MinG.y, a_pOther->m_v3MinG.z) - a_pOther->m_v3MinG);
	
	std::vector<vector3> axes;

	axes.push_back(xAxis); //aX axes[0]
	axes.push_back(yAxis); //aY 1
	axes.push_back(zAxis); //aZ 2
	axes.push_back(a_pOther->xAxis); //bX 3
	axes.push_back(a_pOther->yAxis); //bY 4
	axes.push_back(a_pOther->zAxis); //bZ axes[5]


	//for Ax //if true, seperated
	if (glm::abs(glm::dot(distance, axes[0])) > (halfA.x + glm::abs(halfB.x*glm::dot(axes[0],axes[3])) + glm::abs(halfB.y*glm::dot(axes[0], axes[4])) + glm::abs(halfB.z*glm::dot(axes[0], axes[5])))) {

		return eSATResults::SAT_AX;
	}

	//for Ay //if true, seperated
	else if (glm::abs(glm::dot(distance, axes[1])) > (halfA.y + glm::abs(halfB.x*glm::dot(axes[1], axes[3])) + glm::abs(halfB.y*glm::dot(axes[1], axes[4])) + glm::abs(halfB.z*glm::dot(axes[1], axes[5])))) {
		return eSATResults::SAT_AY;
	}
	//for Az //if true, seperated
	else if (glm::abs(glm::dot(distance, axes[2])) > (halfA.z + glm::abs(halfB.x*glm::dot(axes[2], axes[3])) + glm::abs(halfB.y*glm::dot(axes[2], axes[4])) + glm::abs(halfB.z*glm::dot(axes[2], axes[5])))) {
		return eSATResults::SAT_AZ;
	}
	//for Bx //if true, seperated
	else if (glm::abs(glm::dot(distance, axes[3])) > (halfB.x + glm::abs(halfA.x*glm::dot(axes[0], axes[3])) + glm::abs(halfA.y*glm::dot(axes[1], axes[3])) + glm::abs(halfA.z*glm::dot(axes[2], axes[3])))) {
		return eSATResults::SAT_BX;
	}
	//for By //if true, seperated
	else if (glm::abs(glm::dot(distance, axes[4])) > (halfB.y + glm::abs(halfA.x*glm::dot(axes[0], axes[4])) + glm::abs(halfA.y*glm::dot(axes[1], axes[4])) + glm::abs(halfA.z*glm::dot(axes[2], axes[4])))) {
		return eSATResults::SAT_BY;
	}
	//for Bz //if true, seperated
	else if (glm::abs(glm::dot(distance, axes[5])) > (halfB.z + glm::abs(halfA.x*glm::dot(axes[0], axes[5])) + glm::abs(halfA.y*glm::dot(axes[1], axes[5])) + glm::abs(halfA.z*glm::dot(axes[2], axes[5])))) {
		return eSATResults::SAT_BZ;
	}
	//for AxBx //if true, seperated
	else if (glm::abs((glm::dot(distance, axes[2])*glm::dot(axes[1],axes[3])) - (glm::dot(distance, axes[1])*glm::dot(axes[2], axes[3]))) > (glm::abs(halfA.y*glm::dot(axes[2], axes[3])) + glm::abs(halfA.z*glm::dot(axes[1], axes[3])) + glm::abs(halfB.y*glm::dot(axes[0], axes[5]))+ glm::abs(halfB.z*glm::dot(axes[0],axes[4])))) {
		return eSATResults::SAT_AXxBX;
	}
	//for AxBy //if true, seperated
	else if (glm::abs((glm::dot(distance, axes[2])*glm::dot(axes[1], axes[4])) - (glm::dot(distance, axes[1])*glm::dot(axes[2], axes[4]))) > (glm::abs(halfA.y*glm::dot(axes[2], axes[4])) + glm::abs(halfA.z*glm::dot(axes[1], axes[4])) + glm::abs(halfB.x*glm::dot(axes[0], axes[5])) + glm::abs(halfB.z*glm::dot(axes[0], axes[3])))) {
		return eSATResults::SAT_AXxBY;
	}
	//for AxBz //if true, seperated
	else if (glm::abs((glm::dot(distance, axes[2])*glm::dot(axes[1], axes[5])) - (glm::dot(distance, axes[1])*glm::dot(axes[2], axes[5]))) > (glm::abs(halfA.y*glm::dot(axes[2], axes[5])) + glm::abs(halfA.z*glm::dot(axes[1], axes[5])) + glm::abs(halfB.x*glm::dot(axes[0], axes[4])) + glm::abs(halfB.y*glm::dot(axes[0], axes[3])))) {
		return eSATResults::SAT_AXxBZ;
	}
	//for AyBx //if true, seperated
	else if (glm::abs((glm::dot(distance, axes[0])*glm::dot(axes[2], axes[3])) - (glm::dot(distance, axes[2])*glm::dot(axes[0], axes[3]))) > (glm::abs(halfA.x*glm::dot(axes[2], axes[3])) + glm::abs(halfA.z*glm::dot(axes[0], axes[3])) + glm::abs(halfB.y*glm::dot(axes[1], axes[5])) + glm::abs(halfB.z*glm::dot(axes[1], axes[4])))) {
		return eSATResults::SAT_AYxBX;
	}
	//for AyBy //if true, seperated
	else if (glm::abs((glm::dot(distance, axes[0])*glm::dot(axes[2], axes[4])) - (glm::dot(distance, axes[2])*glm::dot(axes[0], axes[4]))) > (glm::abs(halfA.x*glm::dot(axes[2], axes[4])) + glm::abs(halfA.z*glm::dot(axes[0], axes[4])) + glm::abs(halfB.x*glm::dot(axes[1], axes[5])) + glm::abs(halfB.z*glm::dot(axes[1], axes[3])))) {
		return eSATResults::SAT_AYxBY;
	}
	//for AyBz //if true, seperated
	else if (glm::abs((glm::dot(distance, axes[0])*glm::dot(axes[2], axes[5])) - (glm::dot(distance, axes[2])*glm::dot(axes[0], axes[5]))) > (glm::abs(halfA.x*glm::dot(axes[2], axes[5])) + glm::abs(halfA.z*glm::dot(axes[0], axes[5])) + glm::abs(halfB.x*glm::dot(axes[1], axes[4])) + glm::abs(halfB.y*glm::dot(axes[1], axes[3])))) {
		return eSATResults::SAT_AYxBZ;
	}
	//for AzBx //if true, seperated
	else if (glm::abs((glm::dot(distance, axes[1])*glm::dot(axes[0], axes[3])) - (glm::dot(distance, axes[0])*glm::dot(axes[1], axes[3]))) > (glm::abs(halfA.x*glm::dot(axes[1], axes[3])) + glm::abs(halfA.y*glm::dot(axes[0], axes[3])) + glm::abs(halfB.y*glm::dot(axes[2], axes[5])) + glm::abs(halfB.z*glm::dot(axes[2], axes[4])))) {
		return eSATResults::SAT_AZxBX;
	}
	//for AzBy //if true, seperated
	else if (glm::abs((glm::dot(distance, axes[1])*glm::dot(axes[0], axes[4])) - (glm::dot(distance, axes[0])*glm::dot(axes[1], axes[4]))) > (glm::abs(halfA.x*glm::dot(axes[1], axes[4])) + glm::abs(halfA.y*glm::dot(axes[0], axes[4])) + glm::abs(halfB.x*glm::dot(axes[2], axes[5])) + glm::abs(halfB.z*glm::dot(axes[2], axes[3])))) {
		return eSATResults::SAT_AZxBY;
	}
	//for AzBz //if true, seperated
	else if (glm::abs((glm::dot(distance, axes[1])*glm::dot(axes[0], axes[5])) - (glm::dot(distance, axes[0])*glm::dot(axes[1], axes[5]))) > (glm::abs(halfA.x*glm::dot(axes[1], axes[5])) + glm::abs(halfA.y*glm::dot(axes[0], axes[5])) + glm::abs(halfB.x*glm::dot(axes[2], axes[4])) + glm::abs(halfB.y*glm::dot(axes[2], axes[3])))) {
		return eSATResults::SAT_AZxBZ;
	}*/
	
	//there is no axis test that separates this two objects
	return eSATResults::SAT_NONE;
}

bool Simplex::MyRigidBody::CheckEdges(vector3 a, vector3 b, vector3 c, vector3 d, vector3 distance, vector3 hA, vector3 hB)
{
	//calculate edge vectors
	vector3 ab = b - a;
	vector3 cd = d - c;

	//get axis
	vector3 m = glm::cross(ab, cd);
	if (!IsZeroVector(m)) {
		// Edges ab and cd not parallel, continue with m as a potential separating axis
		hA = glm::proj(hA, m);
		hB = glm::proj(hB, m);
		distance = glm::proj(distance, m);
		if (glm::length(distance) > glm::length(ab) + glm::length(cd)) {
			return true;
		}
		else {
			return false;
		}
	}
	else {
		// Edges ab and cd must be (near) parallel, and therefore lie in some plane P.
		// Thus, as a separating axis try an axis perpendicular to ab and lying in P
		vector3 n = glm::cross(ab, c - a);
		m = glm::cross(ab, n);
		if (!IsZeroVector(m)) {
			// Continue with m as a potential separating axis
			hA = glm::proj(hA, m);
			hB = glm::proj(hB, m);
			distance = glm::proj(distance, m);
			if (glm::length(distance) > glm::length(ab) + glm::length(cd)) {
				return true;
			}
			else {
				return false;
			}
		}
		// ab and ac are parallel too, so edges must be on a line. Ignore testing
		// the axis for this combination of edges as it won’t be a separating axis.
		// (Alternatively, test if edges overlap on this line, in which case the
		// objects are overlapping.)
		return false;
	}
	return false;
}
