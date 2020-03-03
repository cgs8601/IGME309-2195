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
void MyRigidBody::SetModelMatrix(matrix4 a_m4ModelMatrix) { m_m4ToWorld = a_m4ModelMatrix; }
//Allocation
void MyRigidBody::Init(void)
{
	m_pMeshMngr = MeshManager::GetInstance();
	m_bVisible = true;

	m_fRadius = 0.0f;

	m_v3Color = C_WHITE;

	m_v3Center = ZERO_V3;
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

	uint uSize = a_pointList.size();
	if ( uSize < 1 ) {
		return;
	}

	//float minX = a_pointList[0].x;
	//float maxX = a_pointList[0].x;
	//float minY = a_pointList[0].y;
	//float maxY = a_pointList[0].y;
	//float minZ = a_pointList[0].z;
	//float maxZ = a_pointList[0].z;

	vector3 minCoords = a_pointList[0];
	vector3 maxCoords = a_pointList[0];

	for ( uint i = 1; i < uSize; i++ ) {
		vector3 currPoint = a_pointList[i];
		if ( currPoint.x < minCoords.x ) {
			minCoords.x = currPoint.x;
		}
		if ( currPoint.x > maxCoords.x ) {
			maxCoords.x = currPoint.x;
		}
		if ( currPoint.y < minCoords.y ) {
			minCoords.y = currPoint.y;
		}
		if ( currPoint.y > maxCoords.y ) {
			maxCoords.y = currPoint.y;
		}
		if ( currPoint.z < minCoords.z ) {
			minCoords.z = currPoint.z;
		}
		if ( currPoint.z < maxCoords.z ) {
			maxCoords.z = currPoint.z;
		}
	}

	vector3 centerPoint = vector3( ( maxCoords.x + minCoords.x ) / 2.0f, ( maxCoords.y + minCoords.y ) / 2.0f, ( maxCoords.z + minCoords.z ) / 2.0f );
	float maxDistance = glm::distance( a_pointList[0], centerPoint );

	for ( uint i = 1; i < uSize; i++ ) {
		float currDistance = glm::distance( a_pointList[i], centerPoint );
		if ( currDistance > maxDistance ) {
			maxDistance = currDistance;
		}
	}
	m_v3HalfWidth = ( maxCoords - minCoords ) / 2;
	m_fRadius = maxDistance;
	m_v3Center = centerPoint;
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
	// BEGIN SOME CLASS EXPIRIMENTATION
	// (need a non-zero radius to use this)
	matrix4 m4Location = glm::translate( IDENTITY_M4, m_v3Center ) * glm::scale( vector3( m_fRadius ) );
	m_pMeshMngr->AddWireSphereToRenderList( m4Location, C_RED );
	m_pMeshMngr->AddWireCubeToRenderList( glm::translate( IDENTITY_M4, m_v3Center ) * glm::scale( m_v3HalfWidth * 2.0f ), C_BLUE );
	// END SOME CLASS EXPIRIMENTATION
}
bool MyRigidBody::IsColliding(MyRigidBody* const other)
{
	return false;
}