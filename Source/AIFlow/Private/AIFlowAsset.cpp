// Copyright https://github.com/MothCocoon/FlowGraph/graphs/contributors

#include "AIFlowAsset.h"
#include "AIFlowLogChannels.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "GameFramework/Actor.h"
#include "GameFramework/Controller.h"
#include "GameFramework/Pawn.h"
#include "Misc/DataValidation.h"
#include "Types/FlowInjectComponentsHelper.h"
#include "Types/FlowInjectComponentsManager.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(AIFlowAsset)

UAIFlowAsset::UAIFlowAsset(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	BlackboardComponentClass = UBlackboardComponent::StaticClass();
}

void UAIFlowAsset::InitializeInstance(const TWeakObjectPtr<UObject> InOwner, UFlowAsset* InTemplateAsset)
{
	Super::InitializeInstance(InOwner, InTemplateAsset);

	check(Owner == InOwner.Get());

	if (UObject* FlowOwnerActor = TryFindActorOwner())
	{
		CreateAndRegisterBlackboardComponent();

		SetKeySelfOnBlackboardComponent(BlackboardComponent.Get());
	}
}

void UAIFlowAsset::DeinitializeInstance()
{
	Super::DeinitializeInstance();

	// We want to keep the blackboard around until we have deinitialized everything else. 
	DestroyAndUnregisterBlackboardComponent();
}

UBlackboardData* UAIFlowAsset::GetBlackboardAsset() const
{
	return BlackboardAsset;
}

UBlackboardComponent* UAIFlowAsset::GetBlackboardComponent() const
{
	return BlackboardComponent.Get();
}

UBlackboardComponent* UAIFlowAsset::TryFindBlackboardComponentOnActor(AActor& Actor, UBlackboardData* OptionalBlackboardData)
{
	TArray<UBlackboardComponent*> BlackboardComponents;
	Actor.GetComponents(BlackboardComponents);

	if (IsValid(OptionalBlackboardData))
	{
		// Find the desired blackboard component, if it exists, on the ActorOwner
		const UPackage* BlackboardAssetPackage = OptionalBlackboardData->GetPackage();
		for (UBlackboardComponent* BlackboardComp : BlackboardComponents)
		{
			if (const UBlackboardData* BlackboardCompAsset = BlackboardComp->GetBlackboardAsset())
			{
				const UPackage* BlackboardCompAssetPackage = BlackboardCompAsset->GetPackage();

				if (BlackboardCompAssetPackage == BlackboardAssetPackage)
				{
					return BlackboardComp;
				}
			}
		}

		return nullptr;
	}
	else if (BlackboardComponents.Num() > 0)
	{
		if (BlackboardComponents.Num() > 1)
		{
			UE_LOG(
				LogAIFlow,
				Error,
				TEXT("UAIFlowAsset::TryFindBlackboardComponentOnActor found multiple blackboard components (%d) on actor %s, but no OptionalBlackboardData was specified to filter which is desired.  Returning the 0th, but this may not be the desired blackboard."),
				BlackboardComponents.Num(),
				*Actor.GetName());
		}

		return BlackboardComponents[0];
	}

	return nullptr;
}

void UAIFlowAsset::CreateAndRegisterBlackboardComponent()
{
	if (!IsValid(BlackboardAsset))
	{
		return;
	}

	AActor* ActorOwner = TryFindActorOwner();
	if (!IsValid(ActorOwner))
	{
		return;
	}

	BlackboardComponent = TryFindBlackboardComponentOnActor(*ActorOwner, BlackboardAsset);

	if (BlackboardComponent.IsValid())
	{
		// Blackboard component has already been setup 
		// (presumably by some other flow graph, or it was built-in to the Actor)

		return;
	}

	// If the desired blackboard component does not already exist, add it to the ActorOwner
	const FName InstanceBaseName = FName(FString(TEXT("Comp_") + BlackboardAsset->GetName()));
	UActorComponent* ComponentInstance = FFlowInjectComponentsHelper::TryCreateComponentInstanceForActorFromClass(*ActorOwner, BlackboardComponentClass, InstanceBaseName);
	BlackboardComponent = CastChecked<UBlackboardComponent>(ComponentInstance);

	// Create the manager object if we're injecting a component
	InjectComponentsManager = NewObject<UFlowInjectComponentsManager>(this);
	InjectComponentsManager->InitializeRuntime();

	// Inject the desired component
	InjectComponentsManager->InjectComponentOnActor(*ActorOwner, *ComponentInstance);

	// Ensure the Runtime BlackboardData is instanced (if subclasses need to instance it)
	UBlackboardData* RuntimeBlackboard = EnsureRuntimeBlackboardData();

	BlackboardComponent->InitializeBlackboard(*RuntimeBlackboard);
}

void UAIFlowAsset::DestroyAndUnregisterBlackboardComponent()
{
	if (IsValid(InjectComponentsManager))
	{
		InjectComponentsManager->ShutdownRuntime();
	}

	InjectComponentsManager = nullptr;

	BlackboardComponent = nullptr;
}

void UAIFlowAsset::SetKeySelfOnBlackboardComponent(UBlackboardComponent* BlackboardComp) const
{
	if (!IsValid(BlackboardComp))
	{
		return;
	}

	// Set the SelfActor key to the ActorOwner
	AActor* ActorSelf = TryFindActorOwner();;

	AController* ControllerOwner = Cast<AController>(ActorSelf);
	if (IsValid(ControllerOwner))
	{
		// If the ActorOwner is a Controller, use the PossessedPawn 
		// (if available) instead of the Controller for KeySelf
		APawn* PossessedPawn = ControllerOwner->GetPawn();
		if (IsValid(PossessedPawn))
		{
			ActorSelf = PossessedPawn;
		}
	}

	BlackboardComp->SetValueAsObject(FBlackboard::KeySelf, ActorSelf);
}
