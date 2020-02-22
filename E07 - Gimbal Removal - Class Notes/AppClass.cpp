#include "AppClass.h"
void Application::InitVariables( void ) {
    //init the mesh
    m_pMesh = new MyMesh();
    //m_pMesh->GenerateCube(1.0f, C_WHITE);
    m_pMesh->GenerateCone( 2.0f, 5.0f, 3, C_WHITE );
}
void Application::Update( void ) {
    //Update the system so it knows how much time has passed since the last call
    m_pSystem->Update();

    //Is the arcball active?
    ArcBall();

    //Is the first person camera active?
    CameraRotation();
}
void Application::Display( void ) {
    // Clear the screen
    ClearScreen();

    // PROJECTION EXPERIMENTING CODE
    matrix4 m4View = m_pCameraMngr->GetViewMatrix();
    m4View = glm::lookAt( vector3( 0.0f, 0.0f, 35.0f ), vector3( 0.0f, 0.0f, 0.0f ), vector3( 0.0f, 1.0f, 0.0f ) );

    // m_pCameraMngr->SetViewMatrix( m4View );

    matrix4 m4Projection = m_pCameraMngr->GetProjectionMatrix();
    //m4Projection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, 0.01f, 1000.0f);
    float fov = glm::radians( 50.0f );
    float fWidth = m_pSystem->GetWindowWidth();
    float fHeight = m_pSystem->GetWindowHeight();
    float aspect = fWidth / fHeight;
    m4Projection = glm::perspective( fov, aspect, 0.01f, 1000.0f );

    // END PROJECTION EXPERIMENTATION

    m_m4Model = glm::rotate( IDENTITY_M4, glm::radians( m_v3Rotation.x ), vector3( 1.0f, 0.0f, 0.0f ) );
    m_m4Model = glm::rotate( m_m4Model, glm::radians( m_v3Rotation.y ), vector3( 0.0f, 1.0f, 0.0f ) );
    m_m4Model = glm::rotate( m_m4Model, glm::radians( m_v3Rotation.z ), vector3( 0.0f, 0.0f, 1.0f ) );

    // EXPERIMENTING CODE
    //static float fAngle = 0.0f;
    // m_m4Model = ToMatrix4( glm::angleAxis( glm::radians( fAngle ), AXIS_Z ) );

    // starting point
    //quaternion q1 = glm::angleAxis( glm::radians( 30.0f ), AXIS_X );
    // ending point
    //quaternion q2 = glm::angleAxis( glm::radians( 120.0f ), AXIS_X );
    // percent to interpolate between the starting and ending points
    //static float fPercent = 0.0f;

    //m_m4Model = ToMatrix4( glm::mix( q1, q2, fPercent ) );

    m_pMesh->Render( m4Projection, m4View, ToMatrix4( m_m4Model ) );

    //fAngle += 1.0f;
    //fPercent += 1.01;
    // END EXPERIMENTING CODE

    //m_qOrientation = m_qOrientation * glm::angleAxis(glm::radians(1.0f), vector3(1.0f));
    //m_pMesh->Render(m4Projection, m4View, ToMatrix4(m_qOrientation));

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
void Application::Release( void ) {
    SafeDelete( m_pMesh );

    //release GUI
    ShutdownGUI();
}