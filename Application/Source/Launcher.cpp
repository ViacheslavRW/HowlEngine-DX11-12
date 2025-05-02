#include "../Source/Launch.h"

int main()
{
	HEngine::Launch engineApp;
	engineApp.AppRun();

	if (engineApp.isAppInitialized)
	{
		while (engineApp.isAppRunning)
		{
			engineApp.AppUpdate();
		}
		engineApp.AppExit();
	}
}