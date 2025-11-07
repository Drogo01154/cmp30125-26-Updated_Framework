#pragma once

#include "../DXFramework/DXF.h"

class renderPass {
public:
	virtual void Render() = 0;
	virtual ~renderPass() {};
};