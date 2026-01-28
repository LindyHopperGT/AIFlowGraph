// Copyright https://github.com/MothCocoon/FlowGraph/graphs/contributors

#pragma once

#include "Asset/FlowAssetEditor.h"
#include "Blackboard/AIFlowBlackboardWatchData.h"

class UAIFlowAsset;
class UBlackboardComponent;

class AIFLOWEDITOR_API FAIFlowAssetEditor : public FFlowAssetEditor
{
public:
	virtual ~FAIFlowAssetEditor();

	virtual void InitFlowAssetEditor(const EToolkitMode::Type Mode, const TSharedPtr<class IToolkitHost>& InitToolkitHost, UObject* ObjectToEdit) override;

	TSharedRef<SDockTab> SpawnTab_WatchBlackboard(const FSpawnTabArgs& Args) const;

	static const FName WatchBlackboardTab;

	TSharedPtr<class SAIFlowWatchBlackboard> WatchBlackboardWidget;

private:
	using Super = FFlowAssetEditor;

protected:
	virtual void RegisterTabSpawners(const TSharedRef<class FTabManager>& InTabManager) override;
	virtual void CreateWidgets() override;

	/** Helper to build the display mode combo box for the watch blackboard tab. */
	TSharedRef<SWidget> BuildWatchBlackboardDisplayModeWidget() const;

	/** Helper to build the blackboard selector combo box for the watch blackboard tab. */
	TSharedRef<SWidget> BuildWatchBlackboardSelectorWidget() const;

	/** Utility: get the current display mode from the watch widget (or default if not available). */
	EAIFlowBlackboardDisplayMode GetWatchBlackboardDisplayMode() const;

	/** Utility: set the current display mode on the watch widget. */
	void SetWatchBlackboardDisplayMode(EAIFlowBlackboardDisplayMode InMode) const;

	/** Click handler for the "Refresh" button in the Watch Blackboard tab header. */
	FReply OnWatchBlackboardRefreshClicked() const;

	/** Handler for debugger "paused" / "resumed" / "breakpoint hit" events. */
	void OnDebuggerFlowAssetUpdated(const UAIFlowAsset& FlowAssetInstance) const;

	/** Handler when a FlowAsset template is removed from the debugger subsystem. */
	void OnDebuggerFlowAssetTemplateRemoved(const UFlowAsset& FlowAssetTemplate) const;

	/** When a different blackboard is selected from the header dropdown. */
	void OnWatchBlackboardSelectionChanged(UBlackboardComponent* SelectedBlackboard) const;

	FDelegateHandle DebuggerPausedHandle;
	FDelegateHandle DebuggerResumedHandle;
	FDelegateHandle DebuggerBreakpointHitHandle;
	FDelegateHandle DebuggerTemplateRemovedHandle;
};