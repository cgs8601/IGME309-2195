#include "MyRigidBody.h"
using namespace Simplex;
//Accessors
bool MyRigidBody::GetVisible(void) { return m_bVisible; }
float MyRigidBody::GetRadius(void) { return m_fRadius; }
vector3 MyRigidBody::GetColor(void) { return m_v3Color; }
void MyRigidBody::SetColor(vector3 a_v3Color) { m_v3Color = a_v3Color; }
vector3 MyRigidBody::GetCenterLocal(void) { return m_v3Center; }
vector3 MyRigidBody::GetMinLocal(void) { return m_v3MinL; }
vector3 MyRigidBody::GetMaxLocal(void) { return m_v3MaxL; }
vector3 MyRigidBody::GetCenterGlobal(void){	return vector3(m_m4ToWorld * vector4(m_v3Center, 1.0f)); }
vector3 MyRigidBody::GetMinGlobal(void) { return m_v3MinG; }
vector3 MyRigidBody::GetMaxGlobal(void) { return m_v3MaxG; }
vector3 MyRigidBody::GetHalfWidth(void) { return m_v3HalfWidth; }
matrix4 MyRigidBody::GetModelMatrix(void) { return m_m4ToWorld; }
void MyRigidBody::SetModelMatrix(matrix4 a_m4ModelMatrix) { 
	m_m4ToWorld = a_m4ModelMatrix;

	// BEGIN CLASS EXPERIMENTATION, POSSIBLY ON FINAL PRACTICAL
	globalCenter = a_m4ModelMatrix * vector4( m_v3Center, 1.0f );

	// ARBB
	vector3 corner[8];
	corner[0] = vector3( m_v3MinL.x, m_v3MinL.y, m_v3MinL.z );
	corner[1] = vector3( m_v3MinL.x, m_v3MaxL.y, m_v3MinL.z );
	corner[2] = vector3( m_v3MaxL.x, m_v3MinL.y, m_v3MinL.z );
	corner[3] = vector3( m_v3MaxL.x, m_v3MaxL.y, m_v3MinL.z );

	corner[4] = vector3( m_v3MinL.x, m_v3MinL.y, m_v3MaxL.z );
	corner[5] = vector3( m_v3MinL.x, m_v3MaxL.y, m_v3MaxL.z );
	corner[6] = vector3( m_v3MaxL.x, m_v3MinL.y, m_v3MaxL.z );
	corner[7] = vector3( m_v3MaxL.x, m_v3MaxL.y, m_v3MaxL.z );

	for ( uint i = 0; i < 8; i++ ) {
		corner[i] = m_m4ToWorld * vector4( corner[i], 1.0f );
	}

	m_v3MinG = corner[0];
	m_v3MaxG = corner[0];

	for ( uint i = 1; i < 8; i++ ) {
		m_v3MinG.x = glm::min( m_v3MinG.x, corner[i].x );
		m_v3MinG.x = glm::max( m_v3MaxG.x, corner[i].x );

		m_v3MinG.y = glm::min( m_v3MinG.y, corner[i].y );
		m_v3MinG.y = glm::max( m_v3MaxG.y, corner[i].y );

		m_v3MinG.z = glm::min( m_v3MinG.z, corner[i].z );
		m_v3MinG.z = glm::max( m_v3MaxG.z, corner[i].z );
	}

	m_v3ARBBSize = m_v3MaxG - m_v3MinG;

	// END CLASS EXPERIMENTATION
}
//Allocation
void MyRigidBody::Init(void)
{
	m_pMeshMngr = MeshManager::GetInstance();
	m_bVisible = true;

	m_fRadius = 0.0f;

	m_v3Color = C_WHITE;

	m_v3Center = ZERO_V3;
	globalCenter = ZERO_V3;

	m_v3MinL = ZERO_V3;
	m_v3MaxL = ZERO_V3;

	m_v3MinG = ZERO_V3;
	m_v3MaxG = ZERO_V3;

	m_v3HalfWidth = ZERO_V3;

	m_m4ToWorld = IDENTITY_M4;
}
void MyRigidBody::Swap(MyRigidBody& other)
{
	std::swap(m_pMeshMngr , other.m_pMeshMngr);
	std::swap(m_bVisible , other.m_bVisible);

	std::swap(m_fRadius, other.m_fRadius);

	std::swap(m_v3Color , other.m_v3Color);

	std::swap(m_v3Center , other.m_v3Center);
	std::swap( globalCenter, other.globalCenter );
	std::swap(m_v3MinL , other.m_v3MinL);
	std::swap(m_v3MaxL , other.m_v3MaxL);

	std::swap(m_v3MinG , other.m_v3MinG);
	std::swap(m_v3MaxG , other.m_v3MaxG);

	std::swap(m_v3HalfWidth , other.m_v3HalfWidth);

	std::swap(m_m4ToWorld , other.m_m4ToWorld);
}
void MyRigidBody::Release(void)
{
	m_pMeshMngr = nullptr;
}
//The big 3
MyRigidBody::MyRigidBody(std::vector<vector3> a_pointList)
{
	Init();
}
MyRigidBody::MyRigidBody(MyRigidBody const& other)
{
	m_pMeshMngr = m_pMeshMngr;
	m_bVisible = m_bVisible;

	m_fRadius = m_fRadius;

	m_v3Color = m_v3Color;

	m_v3Center = m_v3Center;
	m_v3MinL = m_v3MinL;
	m_v3MaxL = m_v3MaxL;

	m_v3MinG = m_v3MinG;
	m_v3MaxG = m_v3MaxG;

	m_v3HalfWidth = m_v3HalfWidth;

	m_m4ToWorld = m_m4ToWorld;
}
MyRigidBody& MyRigidBody::operator=(MyRigidBody const& other)
{
	if(this != &other)
	{
		Release();
		Init();
		MyRigidBody temp(other);
		Swap(temp);
	}
	return *this;
}
MyRigidBody::~MyRigidBody(){Release();};

//--- Non Standard Singleton Methods
void MyRigidBody::AddToRenderList(void)
{
	if (!m_bVisible)
		return;

	// STUFF MISSING FROM HERE
	// matrix4 m4Space = m_m4ToWorld * glm::translate();
}
bool MyRigidBody::IsColliding(MyRigidBody* const other)
{
	// BEGIN CLASS EXPERIMENTATION
	float distance = glm::distance( this->m_v3Center, other->m_v3Center );
	if ( distance < ( this->m_fRadius + other->m_fRadius ) ) {
	//	// the expensive test
	//	vector3 thisMin = m_m4ToWorld * vector4( m_v3MinL, 1.0f );
	//	vector3 thisMax = m_m4ToWorld * vector4( m_v3MinL, 1.0f );
	//	vector3 otherMin = other->m_m4ToWorld * vector4( other->m_v3MinL, 1.0f );
	//	vector3 otherMax = other->m_m4ToWorld * vector4( other->m_v3MinL, 1.0f );

		// completely to the left or the right of the other object
		if ( m_v3MinG.x > other->m_v3MaxG.x || m_v3MaxG.x < other->m_v3MinG.x ) {
			return false;
		}
		// completely above or below the other object
		if ( m_v3MinG.y > other->m_v3MaxG.y || m_v3MaxG.y < other->m_v3MinG.y ) {
			return false;
		}
		// completely in front of or behind the other object
		if ( m_v3MinG.z > other->m_v3MaxG.z || m_v3MaxG.z < other->m_v3MinG.z ) {
			return false;
		}
		return true;
	} 
	return false;
	// END CLASS EXPERIMENTATION
}