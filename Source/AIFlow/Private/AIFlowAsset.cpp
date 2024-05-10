// Copyright https://github.com/MothCocoon/FlowGraph/graphs/contributors

#include "AIFlowAsset.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "GameFramework/Actor.h"
#include "GameFramework/Controller.h"
#include "GameFramework/Pawn.h"
#include "Misc/DataValidation.h"

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
		EnsureBlackboardComponent();

		SetKeySelfOnBlackboardComponent(BlackboardComponent.Get());
	}
}

UBlackboardData* UAIFlowAsset::GetBlackboardAsset() const
{
	return BlackboardAsset;
}

UBlackboardComponent* UAIFlowAsset::GetBlackboardComponent() const
{
	return BlackboardComponent.Get();
}

void UAIFlowAsset::EnsureBlackboardComponent()
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
	for (UBlackboardComponent* BlackboardComp : BlackboardComponents)
	{
		if (BlackboardComp->GetBlackboardAsset() == BlackboardAsset)
		{
			BlackboardComponent = BlackboardComp;

			break;
		}
	}

	if (!BlackboardComponent.IsValid())
	{
		// If the desired blackboard component does not already exist, add it to the ActorOwner
		const FString BlackboardName = BlackboardAsset->GetName() + TEXT("_") + GetName();
		UBlackboardComponent* NewBlackboardComponent = NewObject<UBlackboardComponent>(ActorOwner, FName(BlackboardName));
		BlackboardComponent = NewBlackboardComponent;

		NewBlackboardComponent->RegisterComponent();

		NewBlackboardComponent->InitializeBlackboard(*BlackboardAsset);
	}
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
