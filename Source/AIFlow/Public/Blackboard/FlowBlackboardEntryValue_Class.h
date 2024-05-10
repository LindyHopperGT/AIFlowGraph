// Copyright https://github.com/MothCocoon/FlowGraph/graphs/contributors

#pragma once

#include "FlowBlackboardEntryValue.h"

#include "FlowBlackboardEntryValue_Class.generated.h"

/**
 * Configuration object for setting blackboard entries for UBlackboardKeyType_Class entries
 */
UCLASS(BlueprintType, DisplayName = "Class Blackboard Value")
class UFlowBlackboardEntryValue_Class : public UFlowBlackboardEntryValue
{
	GENERATED_BODY()

public:

	//~Begin UFlowBlackboardEntryValue
	virtual void SetOnBlackboardComponent(UBlackboardComponent* BlackboardComponent) const override;
	virtual EBlackboardCompare::Type CompareKeyValues(const UBlackboardComponent* BlackboardComponent, const FName& OtherKeyName) const override;
	virtual TSubclassOf<UBlackboardKeyType> GetSupportedBlackboardKeyType() const override;
#if WITH_EDITOR
	virtual bool TryReconfigureFromBlackboardKeyType(const UBlackboardKeyType& KeyType) override;
	virtual FString GetEditorValueString() const override;
#endif // WITH_EDITOR
	//~End UFlowBlackboardEntryValue

#if WITH_EDITOR
	//~Begin UFlowNodeBase
	virtual FText BuildNodeConfigText() const override;
	//~End UFlowNodeBase

	//~Begin UObject
	virtual void PostInitProperties() override;
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
	//~End UObject
#endif // WITH_EDITOR

protected:
#if WITH_EDITOR
	void EnsureClassInstanceIsCompatibleWithBaseClass();
#endif // WITH_EDITOR

protected:
#if WITH_EDITORONLY_DATA
	// The BaseClass for the Object to set
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Configuration, meta = (AllowAbstract = "true"))
	TObjectPtr<UClass> BaseClass = nullptr;
#endif // WITH_EDITORONLY_DATA

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Configuration, meta = (DisplayAfter = Key, AllowAbstract = "true", EditCondition = "BaseClass != nullptr"))
	TObjectPtr<UClass> ClassInstance = nullptr;
};
