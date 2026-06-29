#pragma once
#include "../include/InspectEngine.h"

class Engine
{

public:
	int Run(const unsigned char* gray, int w, int h, int stride);
	void FillResult(InspResult* out);
	const char* LastError() const;

private:
	InspResult result_;
	char	   lastError_[256];
};

