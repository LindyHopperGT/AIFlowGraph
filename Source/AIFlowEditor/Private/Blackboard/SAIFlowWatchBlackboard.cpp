// Copyright https://github.com/MothCocoon/FlowGraph/graphs/contributors

#include "Blackboard/SAIFlowWatchBlackboard.h"
#include "AIFlowAsset.h"
#include "AIFlowActorBlackboardHelper.h"
#include "BehaviorTree/BlackboardData.h"
#include "Types/FlowDataPinValue.h"
#include "Editor.h"
#include "Blackboard/AIFlowBlackboardWatchData.h"

#include "PropertyEditorModule.h"
#include "Modules/ModuleManager.h"
#include "IDetailsView.h"
#include "GameFramework/Actor.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/Controller.h"
#include "GameFramework/GameStateBase.h"
#include "Engine/World.h"

#define LOCTEXT_NAMESPACE "SAIFlowWatchBlackboard"

void SAIFlowWatchBlackboard::Construct(const FArguments& InArgs)
{
	// Create the details view for the watch data object
	FPropertyEditorModule& PropertyEditorModule = FModuleManager::LoadModuleChecked<FPropertyEditorModule>("PropertyEditor");

	FDetailsViewArgs DetailsViewArgs;
	DetailsViewArgs.bHideSelectionTip = true;
	DetailsViewArgs.bLockable = false;
	DetailsViewArgs.bUpdatesFromSelection = false;
	DetailsViewArgs.NotifyHook = nullptr;
	DetailsViewArgs.bShowOptions = false;
	DetailsViewArgs.bAllowSearch = true;

	DetailsView = PropertyEditorModule.CreateDetailView(DetailsViewArgs);

	ChildSlot
		[
			DetailsView.ToSharedRef()
		];
}

SAIFlowWatchBlackboard::~SAIFlowWatchBlackboard()
{
	WatchDataObject.Reset();

	if (DetailsView)
	{
		DetailsView->SetObject(nullptr);
		DetailsView->ForceRefresh();

		DetailsView = nullptr;
	}
}

UAIFlowBlackboardWatchData* SAIFlowWatchBlackboard::EnsureWatchDataObject()
{
	if (!WatchDataObject.IsValid())
	{
		WatchDataObject.Reset(
			NewObject<UAIFlowBlackboardWatchData>(
				GetTransientPackage(),
				UAIFlowBlackboardWatchData::StaticClass(),
				NAME_None,
				RF_Transient | RF_DuplicateTransient));
	}

	return WatchDataObject.Get();
}

void SAIFlowWatchBlackboard::RebuildAvailableBlackboards(const UAIFlowAsset* FlowAssetInstance)
{
	AvailableBlackboards.Reset();
	UBlackboardComponent* PrevSelectedBlackboard = SelectedBlackboard.Get();
	SelectedBlackboard = nullptr;

	if (!IsValid(FlowAssetInstance))
	{
		return;
	}

	// Keep track of BBs we've already added
	TSet<UBlackboardComponent*> Seen;

	auto AddBlackboard = [this, &Seen](UBlackboardComponent* BBComp, const FString& ContextLabel)
		{
			if (!IsValid(BBComp) || Seen.Contains(BBComp))
			{
				return;
			}

			Seen.Add(BBComp);

			const FText Label = BuildBlackboardLabel(ContextLabel, BBComp);
			AvailableBlackboards.Add(MakeShared<FAIFlowWatchedBlackboardEntry>(BBComp, Label));
		};

	// 1) Flow asset's own blackboard
	UBlackboardComponent* FlowAssetBB = FlowAssetInstance->GetBlackboardComponent();
	if (IsValid(FlowAssetBB))
	{
		AddBlackboard(FlowAssetBB, TEXT("Flow Asset"));
	}

	// Helper to add all BB components from a given Actor
	auto AddActorBlackboards = [&AddBlackboard](AActor* Actor, const FString& ContextPrefix)
		{
			if (!IsValid(Actor))
			{
				return;
			}

			TArray<UBlackboardComponent*> Components;
			Actor->GetComponents(Components);

			for (UBlackboardComponent* Comp : Components)
			{
				if (IsValid(Comp))
				{
					AddBlackboard(Comp, ContextPrefix);
				}
			}
		};

	// 2) Owner actor
	AActor* OwnerActor = FlowAssetInstance->TryFindActorOwner();
	AddActorBlackboards(OwnerActor, TEXT("Owner Actor"));

	// 3) Owner controller, if the owner is a Pawn
	if (APawn* OwnerPawn = Cast<APawn>(OwnerActor))
	{
		if (AController* Controller = OwnerPawn->GetController())
		{
			AddActorBlackboards(Controller, TEXT("Owner Controller"));
		}
	}

	// 4) Game state blackboards
	if (const UWorld* World = FlowAssetInstance->GetWorld())
	{
		if (AGameStateBase* GameState = World->GetGameState<AGameStateBase>())
		{
			AddActorBlackboards(GameState, TEXT("Game State"));
		}
	}

	// Choose default selection:
	//  - Prefer maintain existing selection if in available list
	//  - Prefer the Flow Asset's own BB if valid
	//  - Otherwise any Game State BB (first one with "Game State" context)
	//  - Otherwise first available
	if (!AvailableBlackboards.IsEmpty())
	{
		if (TrySetSelectedBlackboardIfAvailable(PrevSelectedBlackboard))
		{
			//  - Prefer maintain existing selection if in available list
		}
		else if (TrySetSelectedBlackboardIfAvailable(FlowAssetBB))
		{
			//  - Prefer the Flow Asset's own BB if valid
		}
		else
		{
			// Try Game State BB if Flow Asset BB wasn't found
			for (const TSharedPtr<FAIFlowWatchedBlackboardEntry>& Entry : AvailableBlackboards)
			{
				if (!Entry.IsValid())
				{
					continue;
				}

				const UBlackboardComponent* BBComp = Entry->BlackboardComp.Get();
				if (!IsValid(BBComp))
				{
					continue;
				}

				const FString Label = Entry->DisplayLabel.ToString();
				if (Label.Contains(TEXT("Game State")))
				{
					SelectedBlackboard = const_cast<UBlackboardComponent*>(BBComp);
					break;
				}
			}

			if (!SelectedBlackboard.IsValid())
			{
				// Fallback: first available
				const TSharedPtr<FAIFlowWatchedBlackboardEntry>& First = AvailableBlackboards[0];
				if (First.IsValid())
				{
					SelectedBlackboard = First->BlackboardComp.Get();
				}
			}
		}
	}
}

bool SAIFlowWatchBlackboard::TrySetSelectedBlackboardIfAvailable(UBlackboardComponent* BlackboardToTryToSelect)
{
	if (!IsValid(BlackboardToTryToSelect))
	{
		return false;
	}

	for (const TSharedPtr<FAIFlowWatchedBlackboardEntry>& Entry : AvailableBlackboards)
	{
		if (Entry.IsValid() && Entry->BlackboardComp.Get() == BlackboardToTryToSelect)
		{
			SelectedBlackboard = BlackboardToTryToSelect;

			return true;
		}
	}

	return false;
}

FText SAIFlowWatchBlackboard::BuildBlackboardLabel(const FString& ContextLabel, const UBlackboardComponent* BlackboardComp)
{
	const UBlackboardData* BBData = IsValid(BlackboardComp) ? BlackboardComp->GetBlackboardAsset() : nullptr;
	const FString AssetName = IsValid(BBData) ? BBData->GetName() : TEXT("None");

	if (!ContextLabel.IsEmpty())
	{
		return FText::FromString(FString::Printf(TEXT("%s (%s)"), *ContextLabel, *AssetName));
	}

	return FText::FromString(AssetName);
}

void SAIFlowWatchBlackboard::PopulateList(const UAIFlowAsset* FlowAssetInstance)
{
	if (!IsValid(FlowAssetInstance))
	{
		SetDetailsFlowAsset(nullptr);
		EnsureDetailsObjectIsSet(nullptr);

		return;
	}

	// Ensure we have a selection based on the currently available blackboards
	if (!SelectedBlackboard.IsValid())
	{
		RebuildAvailableBlackboards(FlowAssetInstance);
	}

	UBlackboardComponent* BBComp = GetSelectedBlackboard();
	if (!HasValidBlackboardComponent(BBComp))
	{
		SetDetailsFlowAsset(nullptr);
		EnsureDetailsObjectIsSet(nullptr);

		return;
	}

	const UBlackboardData* BBData = BBComp->GetBlackboardAsset();
	if (!IsValid(BBData))
	{
		SetDetailsFlowAsset(nullptr);
		EnsureDetailsObjectIsSet(nullptr);

		return;
	}

	// Create the transient watch data object and hold a strong reference to avoid GC
	UAIFlowBlackboardWatchData* WatchDataPtr = EnsureWatchDataObject();

	// Populate the watch data object from blackboard entries
	WatchDataPtr->NamedProperties.Reset();
	WatchDataPtr->NamedProperties.Reserve(BBData->Keys.Num());

	for (const FBlackboardEntry& Entry : BBData->Keys)
	{
		TInstancedStruct<FFlowDataPinValue> OutValue;
		const EFlowDataPinResolveResult ResolveResult =
			FAIFlowActorBlackboardHelper::TryProvideFlowDataPinPropertyFromBlackboardEntry(
				Entry.EntryName, Entry.KeyType, BBComp, OutValue);

		if (FlowPinType::IsSuccess(ResolveResult))
		{
			FFlowNamedDataPinProperty& NewProperty = WatchDataPtr->NamedProperties.Emplace_GetRef();
			NewProperty.Name = Entry.EntryName;
			NewProperty.DataPinValue = OutValue;
		}
	}

	// Refresh the details view to reflect new data
	SetDetailsFlowAsset(FlowAssetInstance);
	EnsureDetailsObjectIsSet(WatchDataObject.Get());
}

void SAIFlowWatchBlackboard::SetDetailsFlowAsset(const UAIFlowAsset* DetailsFlowAsset)
{
	PopulatedFromFlowAsset = DetailsFlowAsset;
}

void SAIFlowWatchBlackboard::EnsureDetailsObjectIsSet(UAIFlowBlackboardWatchData* InWatchDataObject, bool bForceRefresh /*= true*/)
{
	if (!DetailsView.IsValid())
	{
		return;
	}

	DetailsView->SetObject(InWatchDataObject);

	if (bForceRefresh)
	{
		DetailsView->ForceRefresh();
	}
}

void SAIFlowWatchBlackboard::ResetAndRefresh(const UAIFlowAsset* FlowAssetInstance)
{
	if (!IsValid(FlowAssetInstance))
	{
		SetDetailsFlowAsset(nullptr);
		EnsureDetailsObjectIsSet(nullptr);

		AvailableBlackboards.Reset();
		SelectedBlackboard = nullptr;

		return;
	}

	RebuildAvailableBlackboards(FlowAssetInstance);
	PopulateList(FlowAssetInstance);
}

void SAIFlowWatchBlackboard::RefreshList(const UAIFlowAsset* FlowAssetInstance)
{
	if (!IsValid(FlowAssetInstance))
	{
		SetDetailsFlowAsset(nullptr);
		EnsureDetailsObjectIsSet(nullptr);

		return;
	}

	// Do NOT rebuild available BBs here; respect current SelectedBlackboard.
	PopulateList(FlowAssetInstance);
}

void SAIFlowWatchBlackboard::OnFlowAssetTemplateRemoved(const UAIFlowAsset& FlowAssetTemplate)
{
	// If the last populated flow asset is being removed, then we need to stop watching it.
	const UAIFlowAsset* PopulatedFromFlowAssetPtr = PopulatedFromFlowAsset.Get();
	if (PopulatedFromFlowAssetPtr && PopulatedFromFlowAssetPtr->GetTemplateAsset() == &FlowAssetTemplate)
	{
		SetDetailsFlowAsset(nullptr);
		EnsureDetailsObjectIsSet(nullptr);
	}
}

bool SAIFlowWatchBlackboard::HasValidBlackboardComponent(const UBlackboardComponent* BlackboardComp) const
{
	if (!BlackboardComp)
	{
		return false;
	}

	if (!BlackboardComp->HasValidAsset())
	{
		return false;
	}

	if (!GEditor->PlayWorld)
	{
		return false;
	}

	return true;
}

bool SAIFlowWatchBlackboard::HasValidBlackboard(const UAIFlowAsset* FlowAssetInstance) const
{
	if (!FlowAssetInstance)
	{
		return false;
	}

	const UBlackboardComponent* BlackboardComp = FlowAssetInstance->GetBlackboardComponent();
	return HasValidBlackboardComponent(BlackboardComp);
}

EAIFlowBlackboardDisplayMode SAIFlowWatchBlackboard::GetDisplayMode() const
{
	if (!WatchDataObject.IsValid())
	{
		// If there is no watch data yet, return the default mode.
		return EAIFlowBlackboardDisplayMode::DisplayEntriesAsText;
	}

	return WatchDataObject->DisplayMode;
}

void SAIFlowWatchBlackboard::SetDisplayMode(EAIFlowBlackboardDisplayMode InMode)
{
	UAIFlowBlackboardWatchData* WatchDataPtr = EnsureWatchDataObject();
	if (!WatchDataPtr)
	{
		return;
	}

	if (WatchDataPtr->DisplayMode != InMode)
	{
		WatchDataPtr->DisplayMode = InMode;

		// Force the details view to pick up the new mode.
		if (DetailsView.IsValid())
		{
			DetailsView->ForceRefresh();
		}
	}
}

UBlackboardComponent* SAIFlowWatchBlackboard::GetSelectedBlackboard() const
{
	return SelectedBlackboard.Get();
}

void SAIFlowWatchBlackboard::SetSelectedBlackboard(UBlackboardComponent* InBB)
{
	if (!InBB)
	{
		SelectedBlackboard = nullptr;
		return;
	}

	// Ensure the requested BB is in the current list
	for (const TSharedPtr<FAIFlowWatchedBlackboardEntry>& Entry : AvailableBlackboards)
	{
		if (Entry.IsValid() && Entry->BlackboardComp.Get() == InBB)
		{
			SelectedBlackboard = InBB;
			return;
		}
	}

	// Not found in the current list; clear selection
	SelectedBlackboard = nullptr;
}

#undef LOCTEXT_NAMESPACE