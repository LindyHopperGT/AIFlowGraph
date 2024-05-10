// Copyright https://github.com/MothCocoon/FlowGraph/graphs/contributors

#pragma once

#include "Modules/ModuleInterface.h"

class FAIFlowModule final : public IModuleInterface
{
public:
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
};
