#include "AppClass.h"
void Application::InitVariables( void ) {
    //Make MyMesh object
    m_pMesh = new MyMesh();
    m_pMesh->GenerateCube( 2.0f, C_BROWN );

    m_pCameraMngr->SetPositionTargetAndUpward(
        vector3( 0.0f, 0.0f, 10.0f ),
        vector3( 0.0f, 0.0f, 0.0f ),
        vector3( 0.0f, 1.0f, 0.0f )
    );

    m_pCameraMngr->SetCameraMode( CAM_ORTHO_Z ); // Might actually be ortho y or x
    //Make MyMesh object
    m_pMesh1 = new MyMesh();
    m_pMesh1->GenerateCube( 1.0f, C_WHITE );
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

    matrix4 m4Projection = m_pCameraMngr->GetProjectionMatrix();
    matrix4 m4View = m_pCameraMngr->GetViewMatrix();
    matrix4 m4Model = ToMatrix4( m_qArcBall );

    // EXPERIMENTING IN CLASS
    vector4 v4C0 = m4Model[0];
    vector4 v4C1 = m4Model[1];
    vector4 v4C2 = m4Model[2];
    vector4 v4C3 = m4Model[3];

    // Playing around with the vectors 
    // hard set the size to 2,4,1
    v4C0[0] = 2.0f;
    v4C1[1] = 4.0f;
    v4C2[2] = 1.0f;

    // tranform it 2 right, 1 up, 1 backwards
    v4C3 = vector4( 2, 1, -1, 1 );

    //

    // set the model to the new values
    m4Model[0] = v4C0;
    m4Model[1] = v4C1;
    m4Model[2] = v4C2;
    m4Model[3] = v4C3;

    // does the same as the above
    matrix4 m4ScalingMatrix = glm::scale( IDENTITY_M4, vector3( 2.0f, 4.0f, 1.0f ) );
    m4Model = glm::translate( IDENTITY_M4, vector3( 2.0f, 1.0f, -1.0f ) );
    m4Model = m4Model * m4ScalingMatrix;

    // equivalent to just above:
    // m4Model = glm::translate( IDENTITY_M4, vector3( 2.0f, 1.0f, -1.0f ) );
    // matrix4 m4ScalingMatrix = glm::scale( m4Matrix, vector3( 2.0f, 4.0f, 1.0f ) );
    // m4Model = ToMatrix4(ArcBall) * m4Model;

    // rotation
    static float fAngle = 0.0f;
    m4Model = ToMatrix4( glm::angleAxis( glm::radians( fAngle ), AXIS_Z ) );
    // NOTE THE DELTA TIME IS CURRENTLY NOT WORKING FOR SOME REASON
    //static DWORD lastTime = GetTickCount();
    //static DWORD startTime = GetTickCount();
    //DWORD currTime = GetTickCount();

    //matrix4 m4RotX = glm::rotate( IDENTITY_M4, glm::radians( v3Rotation.x ), AXIS_X );
    //matrix4 m4RotY = glm::rotate( IDENTITY_M4, glm::radians( v3Rotation.y ), AXIS_Y );
    //matrix4 m4RotZ = glm::rotate( IDENTITY_M4, glm::radians( v3Rotation.z ), AXIS_Z );
    m4Model = ToMatrix4( glm::angleAxis( glm::radians( fAngle ), AXIS_Z ) );

    // m4Model = m4RotX * m4RotY * m4RotZ;
    quaternion q1 = glm::angleAxis( glm::radians( 30.0f ), AXIS_Z );
    quaternion q2 = glm::angleAxis( glm::radians( 120.0f ), AXIS_Z );
    static float fPercent = 0.0f;

    m4Model = ToMatrix4( glm::mix( q1, q2, fPercent ) );

    // m4Model = glm::rotate( IDENTITY_M4, glm::radians( fAngle ), AXIS_Z );
    //fAngle += ( currTime - lastTime ) / 1000.0f;

    fAngle += 1.0f;
    fPercent += 1.01;

    // END EXPERIMENTING

    m_pMesh->Render( m4Projection, m4View, m4Model );
    // could tie m_pMesh1 to the first mesh by putting in m4Model instead of IDENTITY_M4
    // m_pMesh1->Render( m4Projection, m4View, glm::translate( IDENTITY_M4, vector3( 3.0f, 0.0f, 0.0f ) ) );

     // draw a skybox
    m_pMeshMngr->AddSkyboxToRenderList();

    // BEING EXPERIMENTING, doesn't work, don't know why
#pragma region Debugging Information2
    //Print info on the screen
    uint nEmptyLines = 21;
    for ( uint i = 0; i < nEmptyLines; ++i )
        m_pMeshMngr->PrintLine( "" );//Add a line on top
    //m_pMeshMngr->Print("						");
    m_pMeshMngr->PrintLine( m_pSystem->GetAppName(), C_YELLOW );

    //m_pMeshMngr->Print("						");
    m_pMeshMngr->Print( "FPS:" );
    m_pMeshMngr->PrintLine( std::to_string( m_pSystem->GetFPS() ), C_RED );

    m_pMeshMngr->Print( "Timer" );
    m_pMeshMngr->PrintLine( std::to_string( ( currTime - startTime ) / 1000.0f ), C_RED );

    m_pMeshMngr->Print( "Delta Time" );
    m_pMeshMngr->PrintLine( std::to_string( ( currTime - lastTime ) / 1000.0f ), C_RED );
#pragma endregion
    // END EXPERIMENTING

    //render list call
    m_uRenderCallCount = m_pMeshMngr->Render();

    //clear the render list
    m_pMeshMngr->ClearRenderList();

    //draw gui
    DrawGUI();

    //end the current frame (internally swaps the front and back buffers)
    m_pWindow->display();

    lastTime = GetTickCount();
}
void Application::Release( void ) {
    if ( m_pMesh != nullptr ) {
        delete m_pMesh;
        m_pMesh = nullptr;
    }
    SafeDelete( m_pMesh1 );
    //release GUI
    ShutdownGUI();
}