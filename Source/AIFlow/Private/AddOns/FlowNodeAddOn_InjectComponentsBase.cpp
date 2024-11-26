// Copyright https://github.com/MothCocoon/FlowGraph/graphs/contributors

#include "AddOns/FlowNodeAddOn_InjectComponentsBase.h"
#include "Components/ActorComponent.h"
#include "Types/FlowInjectComponentsManager.h"
#include "GameFramework/Actor.h"
#include "AIFlowLogChannels.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(FlowNodeAddOn_InjectComponentsBase)

UFlowNodeAddOn_InjectComponentsBase::UFlowNodeAddOn_InjectComponentsBase()
	: Super()
{
#if WITH_EDITOR
	NodeDisplayStyle = FlowNodeStyle::AddOn_PerSpawnedActor;
	Category = TEXT("Per-Spawned Actor");
#endif
}

void UFlowNodeAddOn_InjectComponentsBase::EnsureInjectComponentsManager()
{
	if (IsValid(InjectComponentsManager))
	{
		return;
	}

	InjectComponentsManager = NewObject<UFlowInjectComponentsManager>(this);

	InjectComponentsManager->InitializeRuntime();
	InjectComponentsManager->BeforeActorRemovedDelegate.AddDynamic(this, &UFlowNodeAddOn_InjectComponentsBase::OnBeforeActorRemoved);
}

void UFlowNodeAddOn_InjectComponentsBase::CleanupInjectComponentsManager()
{
	if (IsValid(InjectComponentsManager))
	{
		InjectComponentsManager->ShutdownRuntime();

		InjectComponentsManager->BeforeActorRemovedDelegate.RemoveDynamic(this, &UFlowNodeAddOn_InjectComponentsBase::OnBeforeActorRemoved);
	}

	InjectComponentsManager = nullptr;
}

void UFlowNodeAddOn_InjectComponentsBase::DeinitializeInstance()
{
	CleanupInjectComponentsManager();

	Super::DeinitializeInstance();
}

void UFlowNodeAddOn_InjectComponentsBase::OnBeforeActorRemoved(AActor* RemovedActor)
{
	if (IsValid(RemovedActor))
	{
		OnStopMonitoringActor(*RemovedActor);
	}
}

void UFlowNodeAddOn_InjectComponentsBase::FinishedSpawningActor_Implementation(AActor* SpawnedActor, UFlowNodeBase* SpawningFlowNodeBase)
{
}
