#include "AppClass.h"
void Application::InitVariables( void ) {
    //init the mesh
    m_pMesh = new MyMesh();

    m_pCameraMngr->SetCameraMode( CAM_ORTHO_Z );

    //m_pMesh->GenerateCube(1.0f, C_WHITE);
    m_pMesh->GenerateCube( 1.0f, C_BLACK );
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

    matrix4 m4View = m_pCameraMngr->GetViewMatrix();
    matrix4 m4Projection = m_pCameraMngr->GetProjectionMatrix();

    matrix4 m4Scale = glm::scale( IDENTITY_M4, vector3( 2.0f, 2.0f, 2.0f ) );
    static float value = 0.0f;

    // translate it to make it easier to initially see, as well as moving to the right
    matrix4 m4Translate = glm::translate( IDENTITY_M4, vector3( value - 7, -3.0f, 3.0f ) );
    value += 0.01f;

    //matrix4 m4Model = m4Translate * m4Scale;
    matrix4 m4Model = m4Scale * m4Translate;

    // a map of which pixels to enable to create the invader
    int pixels[8][11] = { { 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0 },
                          { 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0 },
                          { 0, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0 },
                          { 0, 1, 1, 0, 1, 1, 1, 0, 1, 1, 0 },
                          { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
                          { 1, 0, 1, 1, 1, 1, 1, 1, 1, 0, 1 },
                          { 1, 0, 1, 0, 0, 0, 0, 0, 1, 0, 1 },
                          { 0, 0, 0, 1, 1, 0, 1, 1, 0, 0, 0 } };

    // render each pixel from the base mesh
    for ( int y = 0; y < 8; y++ ) {
        for ( int x = 0; x < 11; x++ ) {
            // due to perspective, pixels need to be drawn in horizontally reflected positions.
            if ( pixels[7-y][x] == 1 ) {
                m_pMesh->Render( m4Projection, m4View, glm::translate( m4Model, vector3( x, y, 0.0f ) ) );
            }
        }
    }

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