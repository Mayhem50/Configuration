//
//  LayerWidget.cpp
//  UE4PluginsDev
//
//  Created by Benjamin Regnier on 09/11/2016.
//  Copyright © 2016 Epic Games, Inc. All rights reserved.
//

#include "ConfigurationPrivatePCH.h"

#include "LayerWidget.h"

void SLayersWidget::Construct(const SLayersWidget::FArguments &Args){
    LayerManager = Args._LayerManager;
    
    ChildSlot
    [
     SNew(SHorizontalBox)
     +SHorizontalBox::Slot()
     [
      SNew(SScrollBox)
      + SScrollBox::Slot()
      [
       SNew(SHorizontalBox)
       +SHorizontalBox::Slot()
       .Padding(FMargin(20, 15))
       [
        SNew(SButton)
        .Text(FText::FromString("Add Layer"))
        .OnClicked(this, &SLayersWidget::OnAddButtonPressed)
        ]
       +SHorizontalBox::Slot()
       .Padding(FMargin(20, 15))
       [
        SNew(SButton)
        .Text(FText::FromString("Duplicate Layer"))
        .OnClicked(this, &SLayersWidget::OnDuplicateButtonPressed)
        ]
       [
        SNew(SButton)
        .Text(FText::FromString("Save"))
        .OnClicked(this, &SLayersWidget::OnSave)
        ]
       ]
      
      +SScrollBox::Slot()
      [
       SAssignNew(ListViewWidget, SListView<TSharedPtr<FMaterialLayer>>)
       .ItemHeight(24)
       .ListItemsSource(&(LayerManager->GetLayers()))
       .OnGenerateRow(this, &SLayersWidget::OnGenerateRowForList)
       .OnSelectionChanged(this, &SLayersWidget::OnSelectionChanged)
       .HeaderRow(
                  SNew(SHeaderRow)
                  +SHeaderRow::Column("Dummy").DefaultLabel(FText::FromString("")).FixedWidth(50)
                  +SHeaderRow::Column("Name").DefaultLabel(FText::FromString("Name"))
                  +SHeaderRow::Column("Visibility").DefaultLabel(FText::FromString("")).FixedWidth(24)
                  +SHeaderRow::Column("Selected").DefaultLabel(FText::FromString("Selected")).FixedWidth(24)
                  +SHeaderRow::Column("Trash").DefaultLabel(FText::FromString("Trash")).FixedWidth(24)
                  )
       ]
      ]
     +SHorizontalBox::Slot()
     [
      SNew(SHorizontalBox)
      +SHorizontalBox::Slot()
      [
       SNew(STextBlock)
       .Text(FText::FromString("Future Text"))
       ]
      ]
     
     ];
    
    if (LayerManager->GetCurrentLayer ().IsValid ())
    {
        ListViewWidget->SetSelection (LayerManager->GetCurrentLayer (), ESelectInfo::Direct);
    }
}

FReply SLayersWidget::OnAddButtonPressed(){
    LayerManager->AddLayer();
    ListViewWidget->RequestListRefresh();
    ListViewWidget->SetSelection(LayerManager->GetLayers().Last(), ESelectInfo::Direct);
    return FReply::Handled();
}

FReply SLayersWidget::OnRemoveButtonPressed(TSharedPtr<FMaterialLayer> Layer){
    if(Layer.IsValid()){
        LayerManager->RemoveLayer(Layer);
        ListViewWidget->SetSelection(LayerManager->GetLayers().Last(), ESelectInfo::Direct);
        ListViewWidget->RequestListRefresh();
    }
    return FReply::Handled();
}

FReply SLayersWidget::OnDuplicateButtonPressed(){
    LayerManager->Duplicate(SelectedMaterialLayer);
    ListViewWidget->RequestListRefresh();
    return FReply::Handled();
}

FReply SLayersWidget::OnSave(){
    LayerManager->Save();
    
    return FReply::Handled();
}

TSharedRef<ITableRow> SLayersWidget::OnGenerateRowForList(TSharedPtr<FMaterialLayer> Item, const TSharedRef<STableViewBase> &OwnerTable){    
    return
    SNew(SLayerRowWidget, OwnerTable, Item).LayersWidget(this);
}

void SLayersWidget::OnSelectionChanged(TSharedPtr<FMaterialLayer> Item, ESelectInfo::Type SelectionType){
    SelectedMaterialLayer = Item;
    
    if(!SelectedMaterialLayer.IsValid()){ return; }
    LayerManager->SetCurentLayer (SelectedMaterialLayer);
}

void SLayersWidget::OnTextChanged(const FText &InText, TSharedPtr<FMaterialLayer> Item){
    SelectedMaterialLayer = Item;
    
    if(!Item.IsValid()){ return; }
    
    Item->SetName(InText.ToString());
}

FReply SLayersWidget::OnToggleLayerVisibility (TSharedPtr<FMaterialLayer> Item)
{
    Item->SetIsEnabled (!Item->IsEnabled ());
    ListViewWidget->RebuildList ();
    return FReply::Handled ();
}








