// Include standard headers
#include "Main.h"

void wrapper() {
	Application *pApp = new Application();
	pApp->Init( "", RES_C_1280x720_16x9_HD, false, false );
	pApp->Run();
	SafeDelete( pApp );
}
int main(void)
{
	wrapper();
	_CrtSetDbgFlag( _CRTDBG_LEAK_CHECK_DF | _CRTDBG_REPORT_FLAG );
	return 0;
}