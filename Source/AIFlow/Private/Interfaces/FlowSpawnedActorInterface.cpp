// Copyright https://github.com/MothCocoon/FlowGraph/graphs/contributors

#include "Interfaces/FlowSpawnedActorInterface.h"
#include "GameFramework/Actor.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(FlowSpawnedActorInterface)

bool IFlowSpawnedActorInterface::ImplementsInterfaceSafe(const UObject* Object)
{
	if (!IsValid(Object))
	{
		return false;
	}

	UClass* ObjectClass = Object->GetClass();
	if (ObjectClass->ImplementsInterface(UFlowSpawnedActorInterface::StaticClass()))
	{
		return true;
	}

	return false;
}

void IFlowSpawnedActorInterface::DispatchFinishedSpawningActorFromFlowToActorAndComponents(AActor* SpawnedActor, UFlowNodeBase* SpawningNodeOrAddOn)
{
	if (ImplementsInterfaceSafe(SpawnedActor))
	{
		Execute_FinishedSpawningActorFromFlow(SpawnedActor, SpawnedActor, SpawningNodeOrAddOn);

		const TArray<UActorComponent*> Components = SpawnedActor->GetComponentsByInterface(UFlowSpawnedActorInterface::StaticClass());
		for (UActorComponent* Component : Components)
		{
			Execute_FinishedSpawningActorFromFlow(Component, SpawnedActor, SpawningNodeOrAddOn);
		}
	}
}
