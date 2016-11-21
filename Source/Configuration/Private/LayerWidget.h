//
//  LayersWidget.h
//  UE4PluginsDev
//
//  Created by Benjamin Regnier on 09/11/2016.
//  Copyright © 2016 Epic Games, Inc. All rights reserved.
//

#ifndef LayersWidget_h
#define LayersWidget_h

#include "SlateBasics.h"
#include "SBorder.h"

class SLayersWidget : public SCompoundWidget{
public:
    SLATE_BEGIN_ARGS(SLayersWidget) {}
    
    SLATE_ARGUMENT(class ULayerManager*, LayerManager)
    
    SLATE_END_ARGS()
    
    void Construct(const FArguments& Args);
    
    FReply OnAddButtonPressed();
    FReply OnRemoveButtonPressed(UMaterialLayer* Item);
    FReply OnDuplicateButtonPressed();
    FReply OnSave();    
	FReply OnToggleLayerVisibility (UMaterialLayer* Item);
	TOptional<EItemDropZone> OnCanAcceptDrop (const FDragDropEvent& DragDropEvent, EItemDropZone ItemDropZone, UMaterialLayer* Item);

	void OnLayersManagerPostEditUndo ();

    void OnSelectionChanged(UMaterialLayer* Item, ESelectInfo::Type SelectionType);    
    void OnTextChanged(const FText& InText, UMaterialLayer* Item);

    TSharedRef<ITableRow> OnGenerateRowForList(UMaterialLayer* Item, const TSharedRef<STableViewBase>& OwnerTable);
    
	TSharedPtr<SListView<UMaterialLayer*>> ListViewWidget;
	ULayerManager* LayerManager;

private:
    TSharedPtr<class SWidget> ComputeVisibilityIcon(class UMaterialLayer* Layer);
    
private:
    
    UMaterialLayer* SelectedMaterialLayer;
    
    TSharedPtr<SEditableText> EditableText;
};

class SLayerRowWidget : public SMultiColumnTableRow<UMaterialLayer*>
{
public:
	SLATE_BEGIN_ARGS (SLayerRowWidget)
	{}
	SLATE_ARGUMENT (class SLayersWidget*, LayersWidget)
    SLATE_END_ARGS()

	virtual FReply OnDragDetected (const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;
	virtual void OnDragEnter (const FGeometry& MyGeometry, const FDragDropEvent& DragDropEvent) override;
	virtual void OnDragLeave (const FDragDropEvent& DragDropEvent) override;
	FReply OnDrop (const FGeometry& MyGeometry, const FDragDropEvent& DragDropEvent) override;
    
    void Construct(const FArguments& InArgs, const TSharedRef<STableViewBase>& InOwnerTable, UMaterialLayer* InItem)
    {
        LayersWidget = InArgs._LayersWidget;
        Item = InItem;
        SMultiColumnTableRow<UMaterialLayer*>::Construct(FSuperRowType::FArguments(), InOwnerTable);
    }

	TSharedRef< STableViewBase > GetOwnerTable ()
	{
		TSharedPtr< ITypedTableView<UMaterialLayer*> > OwnerWidget = OwnerTablePtr.Pin ();
		check (OwnerWidget.IsValid ());

		return StaticCastSharedPtr< SListView<UMaterialLayer*> > (OwnerWidget).ToSharedRef ();
	}
    
    TSharedRef<SWidget> GenerateWidgetForColumn(const FName& ColumnName)
    {
        FString IconPath = FPaths::Combine(*FPaths::EngineContentDir (), TEXT ("Editor"), TEXT ("Slate"), TEXT ("Icons"));
        
        if (ColumnName == "Name")
        {
            return SNew(SEditableText)
            .Text (FText::FromString (Item->LayerName))
            .OnTextChanged(FOnTextChanged::CreateRaw (LayersWidget, &SLayersWidget::OnTextChanged, Item));
        }
        else if (ColumnName == "Visibility")
        {            
            if (Item->IsEnabled ())
            {
                return SNew(SButton)
                .ButtonStyle (FCoreStyle::Get (), "NoBorder")
                .OnClicked (FOnClicked::CreateRaw(LayersWidget, &SLayersWidget::OnToggleLayerVisibility, Item))
                .HAlign (HAlign_Center)
                .VAlign (VAlign_Center)
                .ForegroundColor (FSlateColor::UseForeground ())
                [
                 //Button Content Image
                 SNew (SImage).Image (new FSlateImageBrush (FPaths::Combine (*IconPath, TEXT ("icon_levels_visible_16px.png")), FVector2D (16, 16)))
                 ];
            }
            else
            {
                return SNew(SButton)
                .ButtonStyle (FCoreStyle::Get (), "NoBorder")
                .OnClicked (FOnClicked::CreateRaw(LayersWidget, &SLayersWidget::OnToggleLayerVisibility, Item))
                .HAlign (HAlign_Center)
                .VAlign (VAlign_Center)
                .ForegroundColor (FSlateColor::UseForeground ())
                [
                 //Button Content Image
                 SNew (SImage).Image (new FSlateImageBrush (FPaths::Combine (*IconPath, TEXT ("icon_levels_invisible_16px.png")), FVector2D (16, 16)))
                 ];
            }
        }
        else if (ColumnName == "Selected")
        {
            return SNew(STextBlock) .Text(FText::FromString("Selected"))
            .ShadowColorAndOpacity(FLinearColor::Black)
            .ShadowOffset(FIntPoint(-1,1));
        }
        else if(ColumnName == "Trash"){
            return SNew(SButton)
            .ButtonStyle (FCoreStyle::Get (), "NoBorder")
            .OnClicked (FOnClicked::CreateRaw(LayersWidget, &SLayersWidget::OnRemoveButtonPressed, Item))
            .HAlign (HAlign_Center)
            .VAlign (VAlign_Center)
            .ForegroundColor (FSlateColor::UseForeground ())
            [
             //Button Content Image
             SNew (SImage).Image (new FSlateImageBrush (*FPaths::Combine(*IconPath, TEXT("Edit"), TEXT ("icon_Edit_Delete_40x.png")), FVector2D (16, 16)))
             ];
        }
        else
        {
            return SNew(STextBlock);
        }
    }
    
	FLinearColor OldColor;
    UMaterialLayer* Item;
    SLayersWidget* LayersWidget;
};

class FListItemDragDropOpertation : public FDragDropOperation
{
public:
	DRAG_DROP_OPERATOR_TYPE (FColorDragDrop, FDragDropOperation)

	virtual void OnDrop (bool bDropWasHandled, const FPointerEvent& MouseEvent) override;
	virtual void OnDragged (const class FDragDropEvent& DragDropEvent) override; 
	virtual TSharedPtr<SWidget> GetDefaultDecorator () const override;

	static TSharedRef<FListItemDragDropOpertation> New (SLayerRowWidget* Origin, FSimpleDelegate TrashShowCallback = FSimpleDelegate (), FSimpleDelegate TrashHideCallback = FSimpleDelegate (), int32 OriginPosition = 0);

	SLayerRowWidget* OriginWidget;

	/** The origin position */
	int32 OriginBarPosition;

	/** Callback to show the trash when this widget is created */
	FSimpleDelegate ShowTrash;

	/** Callback to hide the trash when this widget is dropped */
	FSimpleDelegate HideTrash;

	/** Flag which ensures that OnDrop will not replace this block in it's origin */
	bool bSetForDeletion;

	/** The size of the drag and drop color block */
	FVector2D BlockSize;
};

#endif /* LayersWidget_h */
