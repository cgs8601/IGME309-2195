#include "MyCamera.h"
using namespace Simplex;

//Accessors
void Simplex::MyCamera::SetPosition(vector3 a_v3Position) { m_v3Position = a_v3Position; }
vector3 Simplex::MyCamera::GetPosition(void) { return m_v3Position; }
void Simplex::MyCamera::SetPerspective(bool a_bPerspective) { m_bPerspective = a_bPerspective; }
void Simplex::MyCamera::SetFOV(float a_fFOV) { m_fFOV = a_fFOV; }
void Simplex::MyCamera::SetResolution(vector2 a_v2Resolution) { m_v2Resolution = a_v2Resolution; }
void Simplex::MyCamera::SetNearFar(vector2 a_v2NearFar) { m_v2NearFar = a_v2NearFar; }
void Simplex::MyCamera::SetHorizontalPlanes(vector2 a_v2Horizontal) { m_v2Horizontal = a_v2Horizontal; }
void Simplex::MyCamera::SetVerticalPlanes(vector2 a_v2Vertical) { m_v2Vertical = a_v2Vertical; }
matrix4 Simplex::MyCamera::GetProjectionMatrix(void) { return m_m4Projection; }
matrix4 Simplex::MyCamera::GetViewMatrix(void) { CalculateViewMatrix(); return m_m4View; }

Simplex::MyCamera::MyCamera()
{
	Init(); //Init the object with default values
}

Simplex::MyCamera::MyCamera(vector3 a_v3Position, vector3 a_v3Target, vector3 a_v3Upward)
{
	Init(); //Initialize the object
	SetPositionTargetAndUpward(a_v3Position, a_v3Target, a_v3Upward); //set the position, target and upward
}

Simplex::MyCamera::MyCamera(MyCamera const& other)
{
	m_v3Position = other.m_v3Position;

	m_qForward = other.m_qForward;
	m_qUp = other.m_qUp;

	m_bPerspective = other.m_bPerspective;

	m_fFOV = other.m_fFOV;

	m_v2Resolution = other.m_v2Resolution;
	m_v2NearFar = other.m_v2NearFar;

	m_v2Horizontal = other.m_v2Horizontal;
	m_v2Vertical = other.m_v2Vertical;

	m_m4View = other.m_m4View;
	m_m4Projection = other.m_m4Projection;
}

MyCamera& Simplex::MyCamera::operator=(MyCamera const& other)
{
	if (this != &other)
	{
		Release();
		SetPositionTargetAndUpward(other.m_v3Position, other.m_qForward, other.m_qUp );
		MyCamera temp(other);
		Swap(temp);
	}
	return *this;
}

void Simplex::MyCamera::Init(void)
{
	ResetCamera();
	CalculateProjectionMatrix();
	CalculateViewMatrix();
	//No pointers to initialize here
}

void Simplex::MyCamera::Release(void)
{
	//No pointers to deallocate
}

void Simplex::MyCamera::Swap(MyCamera & other)
{
	std::swap(m_v3Position, other.m_v3Position);

	std::swap( m_qForward, other.m_qForward );
	std::swap( m_qUp, other.m_qUp );
	
	std::swap(m_bPerspective, other.m_bPerspective);

	std::swap(m_fFOV, other.m_fFOV);

	std::swap(m_v2Resolution, other.m_v2Resolution);
	std::swap(m_v2NearFar, other.m_v2NearFar);

	std::swap(m_v2Horizontal, other.m_v2Horizontal);
	std::swap(m_v2Vertical, other.m_v2Vertical);

	std::swap(m_m4View, other.m_m4View);
	std::swap(m_m4Projection, other.m_m4Projection);
}

Simplex::MyCamera::~MyCamera(void)
{
	Release();
}

void Simplex::MyCamera::ResetCamera(void)
{
	m_v3Position = vector3(0.0f, 0.0f, 10.0f); //Where my camera is located

	m_qUp = glm::quat( vector3( 0.0f, 1.0f, 0.0f ) ); // What is up 
	m_qForward = glm::quat( glm::normalize( vector3( 0.0f, 0.0f, 0.0f ) - m_v3Position ) ); // What is forward

	m_bPerspective = true; //perspective view? False is Orthographic

	m_fFOV = 45.0f; //Field of View

	m_v2Resolution = vector2(1280.0f, 720.0f); //Resolution of the window
	m_v2NearFar = vector2(0.001f, 1000.0f); //Near and Far planes

	m_v2Horizontal = vector2(-5.0f, 5.0f); //Orthographic horizontal projection
	m_v2Vertical = vector2(-5.0f, 5.0f); //Orthographic vertical projection

	CalculateProjectionMatrix();
	CalculateViewMatrix();
}

void Simplex::MyCamera::SetPositionTargetAndUpward( vector3 a_v3Position, quaternion a_qForward, quaternion a_qUp ) {
	m_v3Position = m_v3Position;
	m_qForward = a_qForward;
	m_qUp = a_qUp;

	CalculateProjectionMatrix();
}

void Simplex::MyCamera::SetPositionTargetAndUpward(vector3 a_v3Position, vector3 a_v3Target, vector3 a_v3Upward)
{
	m_v3Position = a_v3Position;
	m_qForward = glm::quat( glm::normalize( a_v3Target - m_v3Position ) );

	vector3 normalizedAbove = glm::normalize( a_v3Upward );
	m_qUp = glm::quat( normalizedAbove );
	
	//Calculate the Matrix
	CalculateProjectionMatrix();
}

void Simplex::MyCamera::CalculateViewMatrix(void)
{
	// Calculate the look at, most of your assignment will be reflected in this method
	// TODO CGS Handle mouse rotation
	
	matrix4 orientationMatrix = ToMatrix4( m_qForward );
	m_m4View = glm::translate( orientationMatrix, -m_v3Position ); //position, target, upward
}

void Simplex::MyCamera::CalculateProjectionMatrix(void)
{
	float fRatio = m_v2Resolution.x / m_v2Resolution.y;
	if (m_bPerspective) //perspective
	{
		m_m4Projection = glm::perspective(m_fFOV, fRatio, m_v2NearFar.x, m_v2NearFar.y);
	}
	else //Orthographic
	{
		m_m4Projection = glm::ortho(m_v2Horizontal.x * fRatio, m_v2Horizontal.y * fRatio, //horizontal
			m_v2Vertical.x, m_v2Vertical.y, //vertical
			m_v2NearFar.x, m_v2NearFar.y); //near and far
	}
}

void MyCamera::MoveForward( float a_fDistance ) {
	// TODO CGS Make it move in relation to the camera
	vector3 v3ForwardDistance = glm::normalize( glm::eulerAngles( m_qForward ) ) * a_fDistance;
	m_v3Position += v3ForwardDistance;
}

void MyCamera::MoveVertical( float a_fDistance ){
	// TODO CGS Make it move in relation to the camera
	vector3 v3VerticalDistance = glm::normalize( glm::eulerAngles( m_qUp ) ) * a_fDistance;
	m_v3Position += v3VerticalDistance;
}
void MyCamera::MoveSideways( float a_fDistance ) {
	// TODO CGS Make it move in relation to the camera

	vector3 v3HorizontalDistance = glm::cross( glm::normalize( glm::eulerAngles( m_qForward ) ), glm::normalize( glm::eulerAngles( m_qUp ) ) ) * a_fDistance;
	m_v3Position += v3HorizontalDistance;
}

void MyCamera::ChangeYaw( float a_fAngle ) {
	// TODO CGS
	m_qForward = glm::angleAxis( glm::radians( a_fAngle ), AXIS_X ) * m_qForward;
	m_qUp = glm::angleAxis( glm::radians( a_fAngle ), AXIS_X ) * m_qUp;
}

void MyCamera::ChangePitch( float a_fAngle ) {
	// TODO CGS
	m_qForward = glm::angleAxis( glm::radians( -a_fAngle ), AXIS_Y ) * m_qForward;
	m_qUp = glm::angleAxis( glm::radians( -a_fAngle ), AXIS_Y ) * m_qUp;
}

