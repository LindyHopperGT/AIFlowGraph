// Copyright https://github.com/MothCocoon/FlowGraph/graphs/contributors

#include "Interfaces/FlowPerSpawnedActorInterface.h"
#include "AddOns/FlowNodeAddOn.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(FlowPerSpawnedActorInterface)

bool IFlowPerSpawnedActorInterface::ImplementsInterfaceSafe(const UFlowNodeAddOn* AddOnTemplate)
{
	if (!IsValid(AddOnTemplate))
	{
		return false;
	}

	UClass* AddOnClass = AddOnTemplate->GetClass();
	if (AddOnClass->ImplementsInterface(UFlowPerSpawnedActorInterface::StaticClass()))
	{
		return true;
	}

	return false;
}
