// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "K2Node.h"
#include "K2Node_AddPinInterface.h"
#include "K2Node_CallFunction.h"
#include "Kismet2/Kismet2NameValidators.h"
#include "KismetNodes/SGraphNodeK2Base.h"
#include "GESEditorCommon.h"
#include "K2Node_GESDispatchEvent.generated.h"

UCLASS()
class GLOBALEVENTSYSTEMEDITOR_API UK2Node_GESDispatchEvent : public UK2Node, public IK2Node_AddPinInterface
{
	GENERATED_BODY()
	
public:
	
	UK2Node_GESDispatchEvent();

#if WITH_EDITOR
	virtual TSharedPtr<SGraphNode> CreateVisualWidget() override;
#endif
	
	// Right Menu
	virtual void GetMenuActions(FBlueprintActionDatabaseRegistrar& ActionRegistrar) const override;

	// Context Menu
	virtual void GetNodeContextMenuActions(UToolMenu* Menu, UGraphNodeContextMenuContext* Context) const override;

	// Title
	// virtual FText GetNodeTitle(ENodeTitleType::Type TitleType) const override { return FText::FromString(TEXT("CustomNode")); }
	// Menu Category. Use "BaseCategory|Subcategory1|..." to create subcategory
	virtual FText GetMenuCategory() const override { return FText::FromString(TEXT("GlobalEventSystem")); }
	// Tooltip Heading
	virtual FText GetToolTipHeading() const override { return FText::FromString(TEXT("GlobalEventSystem_DispatchEvent")); }
	// Tooltip
	virtual FText GetTooltipText() const override { return FText::FromString(TEXT("Dispatch event by GlobalEventSystem")); }
	// Search Keywords
	virtual FText GetKeywords() const override { return FText::FromString(TEXT("GES,DE,DPE")); }

	// Color
	virtual FLinearColor GetNodeTitleColor() const override { return FLinearColor::Yellow; }
	virtual FLinearColor GetNodeBodyTintColor() const override { return FLinearColor::Red; }

	// Compact Node
	virtual bool ShouldDrawCompact() const override { return false; }
	virtual FText GetCompactNodeTitle() const override { return FText::FromString(TEXT("DispatchEventCompactNode")); }

	// Bead Node. Always locate in the middle of graph
	virtual bool ShouldDrawAsBead() const override { return false; }

	// Variable Node. The title is hidden and only the pins are visible
	virtual bool DrawNodeAsVariable() const override { return false; }

	// Control Point / Knot
	virtual bool ShouldDrawNodeAsControlPointOnly(int32& OutInputPinIndex, int32& OutOutputPinIndex) const override
	{
		OutInputPinIndex = 0;
		OutOutputPinIndex = 1;
		return true;
	}

	// Icons
	virtual FSlateIcon GetIconAndTint(FLinearColor& OutColor) const override
	{
		static const FSlateIcon Icon = FSlateIcon("EditorStyle", "GraphEditor.Default_16x");
		return Icon;
	}
	virtual bool ShowPaletteIconOnNode() const override { return false; }
	// virtual FName GetCornerIcon() const override { return TEXT("Graph.Latent.LatentIcon"); }

	// Rename Node. Set bCanRenameNode as true in the constructor function, and do followings
	UPROPERTY()
	FString UserDefinedTitle;
	
	virtual void OnRenameNode(const FString& NewName) override
	{
		UserDefinedTitle = NewName;
	}
	virtual TSharedPtr<INameValidatorInterface> MakeNameValidator() const override
	{
		// Use the default name validator. Custom validators will need to derive from FKismetNameValidator.
		return MakeShareable(new FKismetNameValidator(GetBlueprint()));
	}
	// virtual FText GetNodeTitle(ENodeTitleType::Type TitleType) const override
	// {
	// 	if (TitleType == ENodeTitleType::EditableTitle && !UserDefinedTitle.IsEmpty())
	// 	{
	// 		return FText::FromString(UserDefinedTitle);
	// 	}
	// 	
	// 	return FText::FromString(TEXT("CustomNode"));
	// }
	/////////////////////////////////////////////////////////////////////////////////////////

	// Pure function node
	virtual bool IsNodePure() const override { return false; }

	// Node Details. Show all editable(EditAnywhere) or visible(VisibleAnywhere) property of this node when select it
	UPROPERTY(EditAnywhere)
	int32 Int_NodeDetails;
	virtual bool ShouldShowNodeProperties() const override { return false; }

	// Text Caching
	FNodeTextCache NodeTitleCache;
	virtual FText GetNodeTitle(ENodeTitleType::Type TitleType) const override
	{
		if (NodeTitleCache.IsOutOfDate(this))
		{
			NodeTitleCache.SetCachedText(NSLOCTEXT("GlobalEventSystem", "GESDispatchEvent", "GESDispatchEvent"), this);
		}
		return NodeTitleCache;
	}

	// Create default pins
	virtual void AllocateDefaultPins() override;

	// UK2Node_AddPinInterface
	virtual void AddInputPin() override;
	// UK2Node_AddPinInterface

	UEdGraphPin* GetThenPin() const
	{
		UEdGraphPin* Pin = FindPin(UEdGraphSchema_K2::PN_Then);
		check(Pin == nullptr || Pin->Direction == EGPD_Output); // If pin exists, it must be output
		return Pin;
	}

	void CreateNextParamPin(const int32 Index = INDEX_NONE, const bool bInitial = false);
	FName GetUniquePinName(const int32 Index = INDEX_NONE) const;
	static FString GetPinNameByIndex(const int32 Index)
	{
		return FString::Printf(TEXT("%s%d"), *GESEditorConstant::EventDataPinPrefixName, Index);
	}

	void InsertBeforeInputPin(UEdGraphPin* Pin);
	void InsertAfterInputPin(UEdGraphPin* Pin);
	void RemoveInputPin(UEdGraphPin* Pin);

	int32 GetParamPinIndex(const UEdGraphPin* Pin) const;
	void FlushInputPinNames(const int32 StartIndex, const int32 EndIndex);

	virtual void ExpandNode(FKismetCompilerContext& CompilerContext, UEdGraph* SourceGraph) override;

private:
	static constexpr int32 DynamicPinIndexStart = 3;

	UPROPERTY()
	int32 ParamPinCount;
};

class SGraphNodeK2_GESDispatchEvent : public SGraphNodeK2Base
{
public:
	SLATE_BEGIN_ARGS(SGraphNodeK2_GESDispatchEvent){}
	SLATE_END_ARGS()
	
	void Construct( const FArguments& InArgs, UK2Node* InNode );

protected:
	// SGraphNode interface
	virtual void CreateOutputSideAddButton(TSharedPtr<SVerticalBox> OutputBox) override;
	virtual FReply OnAddPin() override;
	virtual EVisibility IsAddPinButtonVisible() const override;
	// End of SGraphNode interface
};