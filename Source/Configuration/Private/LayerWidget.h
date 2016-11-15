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

    void OnSelectionChanged(UMaterialLayer* Item, ESelectInfo::Type SelectionType);    
    void OnTextChanged(const FText& InText, UMaterialLayer* Item);
    
    TSharedRef<ITableRow> OnGenerateRowForList(UMaterialLayer* Item, const TSharedRef<STableViewBase>& OwnerTable);
    
private:
    TSharedPtr<class SWidget> ComputeVisibilityIcon(class UMaterialLayer* Layer);
    
private:
    ULayerManager* LayerManager;
    TSharedPtr<SListView<UMaterialLayer*>> ListViewWidget;
    
    UMaterialLayer* SelectedMaterialLayer;
    
    TSharedPtr<SEditableText> EditableText;
};

class SLayerRowWidget : public SMultiColumnTableRow<UMaterialLayer*>
{
public:
    SLATE_BEGIN_ARGS(SLayerRowWidget){}
    SLATE_ARGUMENT(class SLayersWidget*, LayersWidget)
    SLATE_END_ARGS()
    
    void Construct(const FArguments& InArgs, const TSharedRef<STableViewBase>& InOwnerTable, UMaterialLayer* InItem)
    {
        LayersWidget = InArgs._LayersWidget;
        Item = InItem;
        SMultiColumnTableRow<UMaterialLayer*>::Construct(FSuperRowType::FArguments(), InOwnerTable);
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
    
    UMaterialLayer* Item;
    SLayersWidget* LayersWidget;
};

#endif /* LayersWidget_h */
