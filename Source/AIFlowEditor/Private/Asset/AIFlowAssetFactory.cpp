// Copyright https://github.com/MothCocoon/FlowGraph/graphs/contributors

#include "AIFlowAssetFactory.h"
#include "AIFlowAsset.h"
#include "Graph/FlowGraphSchema.h"
#include "Graph/FlowGraph.h"

#define LOCTEXT_NAMESPACE "AIFlowAssetFactory"

UAIFlowAssetFactory::UAIFlowAssetFactory(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	SupportedClass = UAIFlowAsset::StaticClass();
}

bool UAIFlowAssetFactory::ConfigureProperties()
{
	const FText TitleText = LOCTEXT("CreateEncounterFlowAssetOptions", "Pick AI Flow Asset Class");

	return ConfigurePropertiesInternal(TitleText);
}

UObject* UAIFlowAssetFactory::FactoryCreateNew(UClass* Class, UObject* InParent, FName Name, EObjectFlags Flags,
	UObject* Context, FFeedbackContext* Warn)
{
	UAIFlowAsset* NewFlowAsset = NewObject<UAIFlowAsset>(InParent, Class, Name, Flags | RF_Transactional, Context);

	UFlowGraph::CreateGraph(NewFlowAsset, UFlowGraphSchema::StaticClass());

	return NewFlowAsset;
}

#undef LOCTEXT_NAMESPACE
