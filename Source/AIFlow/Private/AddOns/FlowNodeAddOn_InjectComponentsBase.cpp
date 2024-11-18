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

void UFlowNodeAddOn_InjectComponentsBase::InitializeInstance()
{
	Super::InitializeInstance();

	check(!InjectComponentsManager);
	InjectComponentsManager = NewObject<UFlowInjectComponentsManager>(this);

	InjectComponentsManager->InitializeRuntime();
	InjectComponentsManager->BeforeActorRemovedDelegate.AddDynamic(this, &UFlowNodeAddOn_InjectComponentsBase::OnBeforeActorRemoved);
}

void UFlowNodeAddOn_InjectComponentsBase::DeinitializeInstance()
{
	if (IsValid(InjectComponentsManager))
	{
		InjectComponentsManager->ShutdownRuntime();

		InjectComponentsManager->BeforeActorRemovedDelegate.RemoveDynamic(this, &UFlowNodeAddOn_InjectComponentsBase::OnBeforeActorRemoved);
	}

	InjectComponentsManager = nullptr;

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
	if (!IsValid(SpawnedActor))
	{
		// It is possible to fail spawning, and we will still get a nullptr call to this function
		// (in case the AddOn is simply counting spawn attempts, etc.)

		return;
	}

	check(IsValid(InjectComponentsManager));
	const TArray<UActorComponent*> ComponentInstances = InjectComponentsHelper.CreateComponentInstancesForActor(*SpawnedActor);
	if (!ComponentInstances.IsEmpty())
	{
		InjectComponentsManager->InjectComponentsOnActor(*SpawnedActor, ComponentInstances);

		// Inform subclasses that we are monitoring this Actor
		OnStartMonitoringActor(*SpawnedActor);
	}
}

void UFlowNodeAddOn_InjectComponentsBase::UpdateNodeConfigText_Implementation()
{
#if WITH_EDITOR
	FTextBuilder TextBuilder;

	for (UActorComponent* ComponentTemplate : InjectComponentsHelper.ComponentTemplates)
	{
		if (IsValid(ComponentTemplate))
		{
			TextBuilder.AppendLine(FText::FromString(*ComponentTemplate->GetName()));
		}
	}

	for (TSubclassOf<UActorComponent> ComponentClass : InjectComponentsHelper.ComponentClasses)
	{
		if (IsValid(ComponentClass))
		{
			TextBuilder.AppendLine(FText::FromString(*ComponentClass->GetName()));
		}
	}

	SetNodeConfigText(TextBuilder.ToText());
#endif // WITH_EDITOR
}

