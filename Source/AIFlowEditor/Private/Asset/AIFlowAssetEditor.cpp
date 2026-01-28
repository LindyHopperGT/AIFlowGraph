// Copyright https://github.com/MothCocoon/FlowGraph/graphs/contributors

#include "Asset/AIFlowAssetEditor.h"

#include "AIFlowAsset.h"
#include "ToolMenus.h"
#include "Blackboard/SAIFlowWatchBlackboard.h"
#include "Debugger/FlowDebuggerSubsystem.h"
#include "Subsystems/AssetEditorSubsystem.h"
#include "FlowSubsystem.h"

#include "Widgets/Docking/SDockTab.h"
#include "Widgets/Input/SButton.h"
#include "Widgets/Input/SComboBox.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/Layout/SSpacer.h"
#include "Widgets/SBoxPanel.h"
#include "Widgets/Text/STextBlock.h"

#define LOCTEXT_NAMESPACE "AIFlowAssetEditor"

const FName FAIFlowAssetEditor::WatchBlackboardTab(TEXT("WatchBlackboard"));

void FAIFlowAssetEditor::RegisterTabSpawners(const TSharedRef<class FTabManager>& InTabManager)
{
	Super::RegisterTabSpawners(InTabManager);

	// Only add WatchBlackboardTab for AIFlowAsset instances.
	if (Cast<UAIFlowAsset>(FlowAsset))
	{
		InTabManager->RegisterTabSpawner(WatchBlackboardTab, FOnSpawnTab::CreateSP(this, &FAIFlowAssetEditor::SpawnTab_WatchBlackboard))
			.SetDisplayName(LOCTEXT("WatchBlackboardTab", "Watch Blackboard"))
			.SetGroup(WorkspaceMenuCategory.ToSharedRef())
			.SetIcon(FSlateIcon(FAppStyle::GetAppStyleSetName(), "Icons.Audit"));
	}
}

void FAIFlowAssetEditor::InitFlowAssetEditor(const EToolkitMode::Type Mode, const TSharedPtr<class IToolkitHost>& InitToolkitHost, UObject* ObjectToEdit)
{
	Super::InitFlowAssetEditor(Mode, InitToolkitHost, ObjectToEdit);

	// TODO: If WatchBlackboardTab should appear in a specific stack (e.g., alongside ValidationLogTab),
	//       update the editor layout here by overriding the layout from FFlowAssetEditor::InitFlowAssetEditor
	//       and explicitly adding WatchBlackboardTab to the desired stack.
}

TSharedRef<SDockTab> FAIFlowAssetEditor::SpawnTab_WatchBlackboard(const FSpawnTabArgs& Args) const
{
	check(Args.GetTabId() == WatchBlackboardTab);

	return SNew(SDockTab)
		.Label(LOCTEXT("WatchBlackboardTitle", "Watch Blackboard"))
		[
			SNew(SVerticalBox)

				// Header row with buttons and selectors
				+ SVerticalBox::Slot()
				.AutoHeight()
				.Padding(2.0f)
				[
					SNew(SHorizontalBox)

						// Refresh button
						+ SHorizontalBox::Slot()
						.AutoWidth()
						[
							SNew(SButton)
								.Text(LOCTEXT("RefreshBlackboardButton", "Refresh"))
								.ToolTipText(LOCTEXT("RefreshBlackboardTooltip", "Manually refresh the blackboard watch data."))
								.OnClicked(FOnClicked::CreateSP(this, &FAIFlowAssetEditor::OnWatchBlackboardRefreshClicked))
						]

						// Flexible space
						+ SHorizontalBox::Slot()
						.FillWidth(1.0f)
						[
							SNew(SBox)
						]

						// Blackboard selector label
						+ SHorizontalBox::Slot()
						.AutoWidth()
						.VAlign(VAlign_Center)
						.Padding(4.0f, 0.0f)
						[
							SNew(STextBlock)
								.Text(LOCTEXT("WatchBlackboardSelectorLabel", "Blackboard:"))
						]

						// Blackboard selector combo box
						+ SHorizontalBox::Slot()
						.AutoWidth()
						.Padding(4.0f, 0.0f)
						[
							BuildWatchBlackboardSelectorWidget()
						]

						// Small spacer between selectors
						+ SHorizontalBox::Slot()
						.AutoWidth()
						.Padding(8.0f, 0.0f)
						[
							SNew(SSpacer)
						]

						// Display mode selector label
						+ SHorizontalBox::Slot()
						.AutoWidth()
						.VAlign(VAlign_Center)
						.Padding(4.0f, 0.0f)
						[
							SNew(STextBlock)
								.Text(LOCTEXT("BlackboardDisplayModeLabel", "Display As:"))
						]

						// Display mode combo box
						+ SHorizontalBox::Slot()
						.AutoWidth()
						.Padding(4.0f, 0.0f)
						[
							BuildWatchBlackboardDisplayModeWidget()
						]
				]

			// Body: the watch widget details view
			+ SVerticalBox::Slot()
				.FillHeight(1.0f)
				[
					WatchBlackboardWidget.ToSharedRef()
				]
		];
}

void FAIFlowAssetEditor::CreateWidgets()
{
	Super::CreateWidgets();

	// Create WatchBlackboardWidget
	WatchBlackboardWidget = SNew(SAIFlowWatchBlackboard);

	if (UFlowDebuggerSubsystem* DebuggerSubsystem = GEngine->GetEngineSubsystem<UFlowDebuggerSubsystem>())
	{
		// Bind to debugger events to refresh the watch on pause/resume/breakpoint.
		// Store delegate handles so we can unregister safely when the editor is destroyed.
		DebuggerPausedHandle = DebuggerSubsystem->OnDebuggerPaused.AddLambda([this](const UFlowAsset& FlowAssetInstance)
			{
				if (const UAIFlowAsset* AIFlowAsset = Cast<UAIFlowAsset>(&FlowAssetInstance))
				{
					OnDebuggerFlowAssetUpdated(*AIFlowAsset);
				}
			});

		DebuggerResumedHandle = DebuggerSubsystem->OnDebuggerResumed.AddLambda([this](const UFlowAsset& FlowAssetInstance)
			{
				if (const UAIFlowAsset* AIFlowAsset = Cast<UAIFlowAsset>(&FlowAssetInstance))
				{
					OnDebuggerFlowAssetUpdated(*AIFlowAsset);
				}
			});

		DebuggerBreakpointHitHandle = DebuggerSubsystem->OnDebuggerBreakpointHit.AddLambda([this](const UFlowNode* FlowNode)
			{
				if (const UAIFlowAsset* AIFlowAsset = Cast<UAIFlowAsset>(FlowNode->GetFlowAsset()))
				{
					OnDebuggerFlowAssetUpdated(*AIFlowAsset);
				}
			});

		DebuggerTemplateRemovedHandle = DebuggerSubsystem->OnDebuggerFlowAssetTemplateRemoved.AddLambda([this](const UFlowAsset& FlowAssetTemplate)
			{
				if (const UAIFlowAsset* AIFlowAsset = Cast<UAIFlowAsset>(&FlowAssetTemplate))
				{
					OnDebuggerFlowAssetTemplateRemoved(*AIFlowAsset);
				}
			});
	}
}

FAIFlowAssetEditor::~FAIFlowAssetEditor()
{
	if (UFlowDebuggerSubsystem* DebuggerSubsystem = GEngine ? GEngine->GetEngineSubsystem<UFlowDebuggerSubsystem>() : nullptr)
	{
		if (DebuggerPausedHandle.IsValid())
		{
			DebuggerSubsystem->OnDebuggerPaused.Remove(DebuggerPausedHandle);
		}
		
		if (DebuggerResumedHandle.IsValid())
		{
			DebuggerSubsystem->OnDebuggerResumed.Remove(DebuggerResumedHandle);
		}
		
		if (DebuggerBreakpointHitHandle.IsValid())
		{
			DebuggerSubsystem->OnDebuggerBreakpointHit.Remove(DebuggerBreakpointHitHandle);
		}
		
		if (DebuggerTemplateRemovedHandle.IsValid())
		{
			DebuggerSubsystem->OnDebuggerFlowAssetTemplateRemoved.Remove(DebuggerTemplateRemovedHandle);
		}
	}

	DebuggerPausedHandle.Reset();
	DebuggerResumedHandle.Reset();
	DebuggerBreakpointHitHandle.Reset();
	DebuggerTemplateRemovedHandle.Reset();
}

TSharedRef<SWidget> FAIFlowAssetEditor::BuildWatchBlackboardDisplayModeWidget() const
{
	using FModeItemType = TSharedPtr<EAIFlowBlackboardDisplayMode>;

	// Static options list so the pointer passed into OptionsSource stays valid
	static TArray<FModeItemType> ModeItems;
	if (ModeItems.Num() == 0)
	{
		ModeItems.Add(MakeShared<EAIFlowBlackboardDisplayMode>(EAIFlowBlackboardDisplayMode::DisplayEntriesAsText));
		ModeItems.Add(MakeShared<EAIFlowBlackboardDisplayMode>(EAIFlowBlackboardDisplayMode::DisplayEntriesAsFlowPinValues));
	}

	// Determine which item should be initially selected based on current mode
	const EAIFlowBlackboardDisplayMode CurrentMode = GetWatchBlackboardDisplayMode();
	FModeItemType InitiallySelectedItem;
	for (const FModeItemType& Item : ModeItems)
	{
		if (Item.IsValid() && *Item.Get() == CurrentMode)
		{
			InitiallySelectedItem = Item;
			break;
		}
	}

	TSharedRef<SComboBox<FModeItemType>> Combo =
		SNew(SComboBox<FModeItemType>)
		.OptionsSource(&ModeItems)
		.OnGenerateWidget_Lambda([](FModeItemType InItem)
			{
				const EAIFlowBlackboardDisplayMode Mode = InItem.IsValid()
					? *InItem.Get()
					: EAIFlowBlackboardDisplayMode::DisplayEntriesAsText;

				const FText DisplayAsMode = UEnum::GetDisplayValueAsText(Mode);
				return SNew(STextBlock)
					.Text(DisplayAsMode);
			})
		.OnSelectionChanged_Lambda(
			[this](FModeItemType InSelectedItem, ESelectInfo::Type /*SelectInfo*/)
			{
				if (InSelectedItem.IsValid())
				{
					SetWatchBlackboardDisplayMode(*InSelectedItem.Get());
				}
			})
		.InitiallySelectedItem(InitiallySelectedItem)
		[
			SNew(STextBlock)
				.Text_Lambda([this]()
					{
						const FText DisplayAsMode = UEnum::GetDisplayValueAsText(GetWatchBlackboardDisplayMode());
						return DisplayAsMode;
					})
		];

	return Combo;
}

TSharedRef<SWidget> FAIFlowAssetEditor::BuildWatchBlackboardSelectorWidget() const
{
	using FBBItem = TSharedPtr<FAIFlowWatchedBlackboardEntry>;
	static const FText NoBlackboardText = LOCTEXT("NoBlackboardAvailable", "(No Blackboard Available)");

	// Get the underlying array from the watch widget (this array's address must stay valid).
	const TArray<FBBItem>* SourcePtr = nullptr;
	if (WatchBlackboardWidget.IsValid())
	{
		SourcePtr = &WatchBlackboardWidget->GetAvailableBlackboards();
	}

	// If we don't have a widget or it has no items yet, we'll still show a "No Blackboard Available" label.
	auto GetItemText = [SourcePtr](FBBItem InItem) -> FText
		{
			if (InItem.IsValid())
			{
				return InItem->DisplayLabel;
			}

			if (!SourcePtr || SourcePtr->Num() == 0)
			{
				return NoBlackboardText;
			}

			return FText::GetEmpty();
		};

	// Determine initial selection based on the SelectedBlackboard pointer
	FBBItem InitiallySelectedItem;
	if (SourcePtr && WatchBlackboardWidget.IsValid())
	{
		UBlackboardComponent* SelectedBB = WatchBlackboardWidget->GetSelectedBlackboard();

		for (const FBBItem& Item : *SourcePtr)
		{
			if (Item.IsValid() && Item->BlackboardComp.Get() == SelectedBB)
			{
				InitiallySelectedItem = Item;
				break;
			}
		}
	}

	const TArray<FBBItem>* OptionsArray = SourcePtr;
	static const TArray<FBBItem> EmptyArray;
	if (!OptionsArray)
	{
		OptionsArray = &EmptyArray;
	}

	TSharedRef<SComboBox<FBBItem>> Combo =
		SNew(SComboBox<FBBItem>)
		.OptionsSource(OptionsArray)
		.OnGenerateWidget_Lambda([GetItemText](FBBItem InItem)
			{
				return SNew(STextBlock)
					.Text(GetItemText(InItem));
			})
		.OnSelectionChanged_Lambda(
			[this](FBBItem InSelectedItem, ESelectInfo::Type /*SelectInfo*/)
			{
				UBlackboardComponent* SelectedBB = InSelectedItem.IsValid()
					? InSelectedItem->BlackboardComp.Get()
					: nullptr;

				OnWatchBlackboardSelectionChanged(SelectedBB);
			})
		.InitiallySelectedItem(InitiallySelectedItem)
		[
			SNew(STextBlock)
				.Text_Lambda([this, OptionsArray, GetItemText]()
					{
						using FBBItemLocal = TSharedPtr<FAIFlowWatchedBlackboardEntry>;

						static const FText NoBlackboardTextLocal = LOCTEXT("NoBlackboardAvailable_Local", "(No Blackboard Available)");

						if (!WatchBlackboardWidget.IsValid())
						{
							return NoBlackboardTextLocal;
						}

						UBlackboardComponent* SelectedBBLocal = WatchBlackboardWidget->GetSelectedBlackboard();

						for (const FBBItemLocal& Item : *OptionsArray)
						{
							if (Item.IsValid() && Item->BlackboardComp.Get() == SelectedBBLocal)
							{
								return GetItemText(Item);
							}
						}

						if (OptionsArray->Num() == 0)
						{
							return NoBlackboardTextLocal;
						}

						// Default to the first item's text if selection isn't matched
						return GetItemText((*OptionsArray)[0]);
					})
		];

	return Combo;
}

EAIFlowBlackboardDisplayMode FAIFlowAssetEditor::GetWatchBlackboardDisplayMode() const
{
	if (WatchBlackboardWidget.IsValid())
	{
		return WatchBlackboardWidget->GetDisplayMode();
	}

	// Fallback to default if the widget doesn't exist yet.
	return EAIFlowBlackboardDisplayMode::DisplayEntriesAsText;
}

void FAIFlowAssetEditor::SetWatchBlackboardDisplayMode(EAIFlowBlackboardDisplayMode InMode) const
{
	if (WatchBlackboardWidget.IsValid())
	{
		WatchBlackboardWidget->SetDisplayMode(InMode);
	}
}

FReply FAIFlowAssetEditor::OnWatchBlackboardRefreshClicked() const
{
	if (!WatchBlackboardWidget.IsValid())
	{
		return FReply::Handled();
	}

	if (const UAIFlowAsset* Inspected = Cast<UAIFlowAsset>(FlowAsset->GetInspectedInstance()))
	{
		WatchBlackboardWidget->ResetAndRefresh(Inspected);
	}
	else if (const UAIFlowAsset* TemplateAsAI = Cast<UAIFlowAsset>(FlowAsset))
	{
		// Fallback: try using the template as the instance (if it has a blackboard)
		WatchBlackboardWidget->ResetAndRefresh(TemplateAsAI);
	}
	else
	{
		WatchBlackboardWidget->ResetAndRefresh(nullptr);
	}

	return FReply::Handled();
}

void FAIFlowAssetEditor::OnDebuggerFlowAssetUpdated(const UAIFlowAsset& FlowAssetInstance) const
{
	if (!WatchBlackboardWidget.IsValid())
	{
		return;
	}

	WatchBlackboardWidget->ResetAndRefresh(&FlowAssetInstance);
}

void FAIFlowAssetEditor::OnDebuggerFlowAssetTemplateRemoved(const UFlowAsset& FlowAssetTemplate) const
{
	if (!WatchBlackboardWidget.IsValid())
	{
		return;
	}

	if (const UAIFlowAsset* AIFlowAsset = Cast<UAIFlowAsset>(&FlowAssetTemplate))
	{
		WatchBlackboardWidget->OnFlowAssetTemplateRemoved(*AIFlowAsset);
	}
}

void FAIFlowAssetEditor::OnWatchBlackboardSelectionChanged(UBlackboardComponent* SelectedBlackboard) const
{
	if (!WatchBlackboardWidget.IsValid())
	{
		return;
	}

	WatchBlackboardWidget->SetSelectedBlackboard(SelectedBlackboard);

	// Refresh using the currently inspected FlowAsset instance if possible
	if (const UAIFlowAsset* Inspected = Cast<UAIFlowAsset>(FlowAsset->GetInspectedInstance()))
	{
		WatchBlackboardWidget->RefreshList(Inspected);
	}
	else if (const UAIFlowAsset* TemplateAsAI = Cast<UAIFlowAsset>(FlowAsset))
	{
		WatchBlackboardWidget->RefreshList(TemplateAsAI);
	}
}

#undef LOCTEXT_NAMESPACE