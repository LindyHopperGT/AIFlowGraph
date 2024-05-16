// Copyright https://github.com/MothCocoon/FlowGraph/graphs/contributors

#include "AIFlowAsset.h"
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
	DestroyAndUnregisterBlackboardComponent();

	Super::DeinitializeInstance();
}

UBlackboardData* UAIFlowAsset::GetBlackboardAsset() const
{
	return BlackboardAsset;
}

UBlackboardComponent* UAIFlowAsset::GetBlackboardComponent() const
{
	return BlackboardComponent.Get();
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

	TArray<UBlackboardComponent*> BlackboardComponents;
	ActorOwner->GetComponents(BlackboardComponents);

	// Find the desired blackboard component, if it already exists, on the ActorOwner
	const UPackage* BlackboardAssetPackage = BlackboardAsset->GetPackage();
	for (UBlackboardComponent* BlackboardComp : BlackboardComponents)
	{
		if (const UBlackboardData* BlackboardCompAsset = BlackboardComp->GetBlackboardAsset())
		{
			const UPackage* BlackboardCompAssetPackage = BlackboardCompAsset->GetPackage();
		
			if (BlackboardCompAssetPackage == BlackboardAssetPackage)
			{
				BlackboardComponent = BlackboardComp;

				break;
			}
		}
	}

	if (BlackboardComponent.IsValid())
	{
		// Blackboard component has already been setup 
		// (presumably by some other flow graph, or it was built-in to the Actor)

		return;
	}

	// If the desired blackboard component does not already exist, add it to the ActorOwner
	UActorComponent* ComponentInstance = FFlowInjectComponentsHelper::TryCreateComponentInstanceForActorFromClass(*ActorOwner, UBlackboardComponent::StaticClass());
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
