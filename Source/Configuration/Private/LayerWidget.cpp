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
        .Text(FText::FromString("Remove Layer"))
        .OnClicked(this, &SLayersWidget::OnRemoveButtonPressed)
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
       .OnSelectionChanged(this, &SLayersWidget::OnSelectionChanged)       ]
      ]
     +SHorizontalBox::Slot()
     [
      SNew(SHorizontalBox)
      +SHorizontalBox::Slot()
      [
       SNew(STextBlock)
       .Text(FText::FromString("Layer Name: "))
       ]
      +SHorizontalBox::Slot()
      [
       SAssignNew(EditableText, SEditableText)
       .OnTextChanged(this, &SLayersWidget::OnTextChanged)
       ]
      +SHorizontalBox::Slot()
      [
       SNew(SButton)
       .Text(FText::FromString("Up"))
       ]
      ]
     
     ];
}

FReply SLayersWidget::OnAddButtonPressed(){
    LayerManager->AddLayer();
    ListViewWidget->RequestListRefresh();
    ListViewWidget->SetSelection(LayerManager->GetLayers().Last(), ESelectInfo::Direct);
    return FReply::Handled();
}

FReply SLayersWidget::OnRemoveButtonPressed(){
    SelectedMaterialLayer = ListViewWidget->GetSelectedItems()[0];
    
    if(SelectedMaterialLayer.IsValid()){
        LayerManager->RemoveLayer(SelectedMaterialLayer);
        ListViewWidget->RequestListRefresh();
    }
    return FReply::Handled();
}

FReply SLayersWidget::OnDuplicateButtonPressed(){
    LayerManager->Duplicate(SelectedMaterialLayer);
    ListViewWidget->RequestListRefresh();
    ListViewWidget->SetSelection(LayerManager->GetLayers().Last(), ESelectInfo::Direct);
    return FReply::Handled();
}

FReply SLayersWidget::OnSave(){
    LayerManager->Save();
    
    return FReply::Handled();
}

TSharedRef<ITableRow> SLayersWidget::OnGenerateRowForList(TSharedPtr<FMaterialLayer> Item, const TSharedRef<STableViewBase> &OwnerTable){
    return
    SNew(STableRow<TSharedPtr<FMaterialLayer>>, OwnerTable)
    .Padding(2.0)
    [
     SNew(SHorizontalBox)
     +SHorizontalBox::Slot()
     [
      SNew(STextBlock).Text(FText::FromString(Item->GetName()))
      ]
     +SHorizontalBox::Slot()
     [
      SNew(STextBlock).Text(FText::FromString(Item->GetName()))
      ]
     ];
}

void SLayersWidget::OnSelectionChanged(TSharedPtr<FMaterialLayer> Item, ESelectInfo::Type SelectionType){
    SelectedMaterialLayer = Item;
    
    if(!SelectedMaterialLayer.IsValid()){ return; }
    
    EditableText->SetText(FText::FromString(SelectedMaterialLayer->GetName()));
}

void SLayersWidget::OnTextChanged(const FText &InText){
    SelectedMaterialLayer = ListViewWidget->GetSelectedItems()[0];
    
    if(!SelectedMaterialLayer.IsValid()){ return; }
    
    SelectedMaterialLayer->SetName(InText.ToString());
    ListViewWidget->RebuildList();
}









