#include "MyCamera.h"
using namespace Simplex;

//Accessors
void Simplex::MyCamera::SetPosition(vector3 a_v3Position) { m_v3Position = a_v3Position; }
vector3 Simplex::MyCamera::GetPosition(void) { return m_v3Position; }
void Simplex::MyCamera::SetTarget(vector3 a_v3Target) { m_v3Target = a_v3Target; }
vector3 Simplex::MyCamera::GetTarget(void) { return m_v3Target; }
void Simplex::MyCamera::SetAbove(vector3 a_v3Above) { m_v3Above = a_v3Above; }
vector3 Simplex::MyCamera::GetAbove(void) { return m_v3Above; }
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
	m_v3Target = other.m_v3Target;
	m_v3Above = other.m_v3Above;

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
		SetPositionTargetAndUpward(other.m_v3Position, other.m_v3Target, other.m_v3Above);
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
	std::swap(m_v3Target, other.m_v3Target);
	std::swap(m_v3Above, other.m_v3Above);
	
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
	m_v3Target = vector3(0.0f, 0.0f, 0.0f); //What I'm looking at
	m_v3Above = vector3(0.0f, 1.0f, 0.0f); //What is above the camera


	m_bPerspective = true; //perspective view? False is Orthographic

	m_fFOV = 45.0f; //Field of View

	m_v2Resolution = vector2(1280.0f, 720.0f); //Resolution of the window
	m_v2NearFar = vector2(0.001f, 1000.0f); //Near and Far planes

	m_v2Horizontal = vector2(-5.0f, 5.0f); //Orthographic horizontal projection
	m_v2Vertical = vector2(-5.0f, 5.0f); //Orthographic vertical projection

	CalculateProjectionMatrix();
	CalculateViewMatrix();
}

void Simplex::MyCamera::SetPositionTargetAndUpward(vector3 a_v3Position, vector3 a_v3Target, vector3 a_v3Upward)
{
	m_v3Position = a_v3Position;
	m_v3Target = a_v3Target;

	m_v3Above = a_v3Position + glm::normalize(a_v3Upward);
	
	//Calculate the Matrix
	CalculateProjectionMatrix();
}

void Simplex::MyCamera::CalculateViewMatrix(void)
{
	// Calculate the look at, most of your assignment will be reflected in this method
	// I'll admit, I'm not sure what was supposed to be implemented here.
	m_m4View = glm::lookAt(m_v3Position, m_v3Target, glm::normalize(m_v3Above - m_v3Position)); //position, target, upward
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
	// Move in the direction of the forward vector
	vector3 v3ForwardDistance = glm::normalize( m_v3Target - m_v3Position ) * a_fDistance;
	m_v3Position += v3ForwardDistance;
	m_v3Target += v3ForwardDistance;
	m_v3Above += v3ForwardDistance;
}

void MyCamera::MoveVertical( float a_fDistance ){
	// Move in the direction of the above vector
	// Since we don't need to rotate the above vector for an FPS camera (under Proffesor's guidance for this assigment),
	// we need to calculate where the upward should be, i.e. rotating the target vector 90 degrees around the rightward vector.
	vector3 rightward = glm::normalize( glm::cross( glm::normalize( m_v3Target - m_v3Position ),
													glm::normalize( m_v3Above - m_v3Position ) ) );

	vector3 v3VerticalDistance = glm::normalize( glm::rotate( glm::normalize( m_v3Target - m_v3Position), 
															  glm::radians( 90.0f ),
															  rightward ) ) * a_fDistance;
	m_v3Position += v3VerticalDistance;
	m_v3Target += v3VerticalDistance;
	m_v3Above += v3VerticalDistance;
}
void MyCamera::MoveSideways( float a_fDistance ) {
	// Move in the plane of the right vector
	// The right vector is the cross product of the target and the above 
	// (normalized so that it is constant no matter the angle between the above and the target).
	vector3 v3HorizontalDistance = glm::normalize( glm::cross( glm::normalize( m_v3Target - m_v3Position ),
															   glm::normalize( m_v3Above - m_v3Position ) ) ) * a_fDistance;
	m_v3Position += v3HorizontalDistance;
	m_v3Target += v3HorizontalDistance;
	m_v3Above += v3HorizontalDistance;
}

void MyCamera::ChangeYaw( float a_fAngle ) {
	// Gets the vector represented by target/above in relation to position,
	// then rotates the x of that vector by the angle, and adds position to form the new target/above
	vector3 rightward = glm::normalize( glm::cross( glm::normalize( m_v3Target - m_v3Position ),
													glm::normalize( m_v3Above - m_v3Position ) ) );

	m_v3Target = glm::rotate( m_v3Target - m_v3Position,
							  -a_fAngle,
							  rightward ) + m_v3Position;

	// no need to change upward for rightward rotation
}

void MyCamera::ChangePitch( float a_fAngle ) {
	// Gets the vector represented by target/above in relation to position,
	// then rotates the y of that vector by the angle, and adds position to form the new target/above
	vector3 oldTarget = m_v3Target;
	m_v3Target = glm::rotate( m_v3Target - m_v3Position,
							  a_fAngle,
							  glm::normalize( m_v3Above - m_v3Position ) ) + m_v3Position;

	// I stopped trying to rotate the above after Prof said that keeping the initial above is fine for an FPS camera
	//m_v3Above = glm::rotate( m_v3Above - m_v3Position,
	//						 -a_fAngle,
	//						 glm::normalize( oldTarget - m_v3Position ) ) + m_v3Position;
}

