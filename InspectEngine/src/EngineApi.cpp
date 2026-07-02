#include "Engine.h"
#include "../include/InspectEngine.h"

INSP_API InspHandle Insp_Create(void)
{
	Engine* e = new Engine();
	return static_cast<InspHandle>(e);
}

INSP_API int Insp_Run(InspHandle h, const unsigned char* gray, int width, int height, int stride)
{
	try {
		Engine* e = static_cast<Engine*>(h);

		int result = e->Run(gray, width, height, stride);

		if (result != 0)
		{
			return -1;
		}

		return 0;
	}
	catch (...) {

		return -1;
	}
}

INSP_API int Insp_GetResult(InspHandle h, InspResult* out)
{
	Engine* e = static_cast<Engine*>(h);
	e->FillResult(out);
	return 0;
}

INSP_API const char* Insp_GetLastError(InspHandle h)
{
	Engine* e = static_cast<Engine*>(h);
	return e->LastError();
}

INSP_API void Insp_Destroy(InspHandle h)
{
	delete static_cast<Engine*>(h);
}