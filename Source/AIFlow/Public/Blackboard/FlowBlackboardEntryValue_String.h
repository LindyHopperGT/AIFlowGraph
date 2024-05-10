// Copyright https://github.com/MothCocoon/FlowGraph/graphs/contributors

#pragma once

#include "FlowBlackboardEntryValue.h"

#include "FlowBlackboardEntryValue_String.generated.h"

/**
 * Configuration object for setting blackboard entries for UBlackboardKeyType_String entries
 */
UCLASS(BlueprintType, DisplayName = "String Blackboard Value")
class AIFLOW_API UFlowBlackboardEntryValue_String : public UFlowBlackboardEntryValue
{
	GENERATED_BODY()

public:

	//~Begin UFlowBlackboardEntryValue
	virtual void SetOnBlackboardComponent(UBlackboardComponent* BlackboardComponent) const override;
	virtual EBlackboardCompare::Type CompareKeyValues(const UBlackboardComponent* BlackboardComponent, const FName& OtherKeyName) const override;
	virtual TSubclassOf<UBlackboardKeyType> GetSupportedBlackboardKeyType() const override;
#if WITH_EDITOR
	virtual FString GetEditorValueString() const override { return StringValue; }
#endif // WITH_EDITOR
	//~End UFlowBlackboardEntryValue

#if WITH_EDITOR
public:
	//~Begin UFlowNodeBase
	virtual FText BuildNodeConfigText() const override;
	//~End UFlowNodeBase

	//~Begin UObject
	virtual void PostInitProperties() override;
	//~End UObject
#endif // WITH_EDITOR

protected:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Configuration, meta = (DisplayAfter = Key))
	FString StringValue;
};
