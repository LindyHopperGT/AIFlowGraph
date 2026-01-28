// Copyright https://github.com/MothCocoon/FlowGraph/graphs/contributors

#pragma once

#include "Widgets/SCompoundWidget.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Engine/TimerHandle.h"
#include "AIFlowBlackboardWatchData.h"

class UAIFlowAsset;
class IDetailsView;

/** Editor-only description of a blackboard we can watch. */
struct FAIFlowWatchedBlackboardEntry
{
	/** Blackboard component we will read values from. */
	TWeakObjectPtr<UBlackboardComponent> BlackboardComp;

	/** User-facing label for the dropdown (e.g. "Owner Actor: MyCharacter (MyBB)"). */
	FText DisplayLabel;

	FAIFlowWatchedBlackboardEntry() = default;
	FAIFlowWatchedBlackboardEntry(UBlackboardComponent* InComp, const FText& InLabel)
		: BlackboardComp(InComp)
		, DisplayLabel(InLabel)
	{}
};

/** Widget for displaying live blackboard values in a details-style view */
class AIFLOWEDITOR_API SAIFlowWatchBlackboard : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SAIFlowWatchBlackboard) {}
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs);

	virtual ~SAIFlowWatchBlackboard() override;

	/**
	* Fully rebuilds available blackboards for the FlowAsset and repopulates the view.
	* Use this when the FlowAsset instance or its owner context changes (e.g., debugger events).
	*/
	void ResetAndRefresh(const UAIFlowAsset* FlowAssetInstance);

	/**
	* Refresh the watch data from the currently selected blackboard.
	* Does NOT rebuild the available-blackboards list; respects current selection.
	*/
	void RefreshList(const UAIFlowAsset* FlowAssetInstance);

	/** Called when a FlowAsset template is removed (to clear the view if needed). */
	void OnFlowAssetTemplateRemoved(const UAIFlowAsset& FlowAssetTemplate);

	/** Get the current display mode, if a watch data object exists. */
	EAIFlowBlackboardDisplayMode GetDisplayMode() const;

	/** Set the current display mode, creating a watch data object if necessary. */
	void SetDisplayMode(EAIFlowBlackboardDisplayMode InMode);

	/** Get the list of available blackboards for the current FlowAsset instance. */
	const TArray<TSharedPtr<FAIFlowWatchedBlackboardEntry>>& GetAvailableBlackboards() const
	{
		return AvailableBlackboards;
	}

	/** Get the currently selected blackboard component (may be null). */
	UBlackboardComponent* GetSelectedBlackboard() const;

	/**
	* Set which blackboard should be used for population.
	* If InBB is not in the current list, selection will be cleared.
	*/
	void SetSelectedBlackboard(UBlackboardComponent* InBB);

private:
	/** Populate the watch data object with current blackboard values */
	void PopulateList(const UAIFlowAsset* FlowAssetInstance);

	/** Check if we have a valid blackboard component to watch */
	bool HasValidBlackboardComponent(const UBlackboardComponent* BlackboardComp) const;

	/** Legacy helper: still used to enforce PIE / general validity via FlowAsset. */
	bool HasValidBlackboard(const UAIFlowAsset* FlowAssetInstance) const;

	void SetDetailsFlowAsset(const UAIFlowAsset* DetailsFlowAsset);
	void EnsureDetailsObjectIsSet(UAIFlowBlackboardWatchData* InWatchDataObject, bool bForceRefresh = true);

	/** Ensure we have a watch data object, creating it if necessary. */
	class UAIFlowBlackboardWatchData* EnsureWatchDataObject();

	/** Rebuild the list of available blackboards for a given FlowAsset instance. */
	void RebuildAvailableBlackboards(const UAIFlowAsset* FlowAssetInstance);

	bool TrySetSelectedBlackboardIfAvailable(UBlackboardComponent* TrySelectBlackboard);

	/** Helper to build a user-facing label for a given context + blackboard data. */
	static FText BuildBlackboardLabel(const FString& ContextLabel, const UBlackboardComponent* BlackboardComp);

	/** Transient watch data object inspected by the details view (strong ref to avoid GC) */
	TStrongObjectPtr<UAIFlowBlackboardWatchData> WatchDataObject = nullptr;

	/** Details view used to display the watch data object */
	TSharedPtr<IDetailsView> DetailsView;

	/** Last FlowAsset this view was populated from. */
	TWeakObjectPtr<const UAIFlowAsset> PopulatedFromFlowAsset = nullptr;

	/** All blackboards currently available to watch for the last FlowAsset. */
	TArray<TSharedPtr<FAIFlowWatchedBlackboardEntry>> AvailableBlackboards;

	/** Which blackboard is currently selected (may be null). */
	TWeakObjectPtr<UBlackboardComponent> SelectedBlackboard;
};