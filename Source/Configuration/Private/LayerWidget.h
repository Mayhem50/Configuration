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
    
    SLATE_ARGUMENT(TSharedPtr<class FLayerManager>, LayerManager)
    
    SLATE_END_ARGS()
    
    void Construct(const FArguments& Args);
    
    FReply OnAddButtonPressed();
    FReply OnRemoveButtonPressed(TSharedPtr<FMaterialLayer> Item);
    FReply OnDuplicateButtonPressed();
    FReply OnSave();    
	FReply OnToggleLayerVisibility (TSharedPtr<FMaterialLayer> Item);

    void OnSelectionChanged(TSharedPtr<FMaterialLayer> Item, ESelectInfo::Type SelectionType);    
    void OnTextChanged(const FText& InText, TSharedPtr<FMaterialLayer> Item);
    
    TSharedRef<ITableRow> OnGenerateRowForList(TSharedPtr<FMaterialLayer> Item, const TSharedRef<STableViewBase>& OwnerTable);
    
private:
    TSharedPtr<class SWidget> ComputeVisibilityIcon(TSharedPtr<class FMaterialLayer> Layer);
    
private:
    TSharedPtr<FLayerManager> LayerManager;
    TSharedPtr<SListView<TSharedPtr<FMaterialLayer>>> ListViewWidget;
    
    TSharedPtr<FMaterialLayer> SelectedMaterialLayer;
    
    TSharedPtr<SEditableText> EditableText;
};

class SLayerRowWidget : public SMultiColumnTableRow<TSharedPtr<FMaterialLayer>>
{
public:
    SLATE_BEGIN_ARGS(SLayerRowWidget){}
    SLATE_ARGUMENT(class SLayersWidget*, LayersWidget)
    SLATE_END_ARGS()
    
    void Construct(const FArguments& InArgs, const TSharedRef<STableViewBase>& InOwnerTable, TSharedPtr<FMaterialLayer> InItem)
    {
        LayersWidget = InArgs._LayersWidget;
        Item = InItem;
        SMultiColumnTableRow<TSharedPtr<FMaterialLayer>>::Construct(FSuperRowType::FArguments(), InOwnerTable);
    }
    
    TSharedRef<SWidget> GenerateWidgetForColumn(const FName& ColumnName)
    {
        FString IconPath = FPaths::Combine(*FPaths::EngineContentDir (), TEXT ("Editor"), TEXT ("Slate"), TEXT ("Icons"));
        
        if (ColumnName == "Name")
        {
            return SNew(SEditableText)
            .Text (FText::FromString (Item->GetName ()))
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
    
    TSharedPtr<FMaterialLayer> Item;
    SLayersWidget* LayersWidget;
};

#endif /* LayersWidget_h */
