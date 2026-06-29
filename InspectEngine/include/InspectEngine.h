#pragma once
#ifdef INSPECTENGINE_EXPORTS
	#define INSP_API __declspec(dllexport)
#else
	#define INSP_API __declspec(dllimport)
#endif

#ifdef __cplusplus
extern "C" {
#endif

	typedef void* InspHandle;

	typedef struct {
		int		objectCount;
		double	measuredValue[32]; // 픽셀 지름
		int		judgement[32];     // 0=OK, 1=NG
		double	processMs;
		int		resultCode;
	} InspResult;

	INSP_API InspHandle  Insp_Create(void);
	INSP_API int		 Insp_Run(InspHandle h, const unsigned char* gray, int width, int height, int stride);
	INSP_API int		 Insp_GetResult(InspHandle h, InspResult* out);
	INSP_API const char* Insp_GetLastError(InspHandle h);
	INSP_API void		 Insp_Destroy(InspHandle h);

#ifdef __cplusplus
}
#endif
