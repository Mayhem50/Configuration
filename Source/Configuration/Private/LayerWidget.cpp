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

FReply SLayersWidget::OnRemoveButtonPressed(){
    SelectedMaterialLayer = ListViewWidget->GetSelectedItems()[0];
    
    if(SelectedMaterialLayer.IsValid()){
        LayerManager->RemoveLayer(SelectedMaterialLayer);
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
	TSharedPtr<SWidget> Image;
	

	if (Item->IsEnabled ())
	{
		SAssignNew (Image, SImage).Image (
			new FSlateImageBrush (FPaths::Combine (*FPaths::EngineContentDir (), TEXT ("Editor"), TEXT ("Slate"), TEXT ("Icons"), TEXT ("icon_levels_visible_16px.png")), FVector2D (16, 16))
		);
	}
	else
	{
		SAssignNew (Image, SImage).Image (
			new FSlateImageBrush (FPaths::Combine (*FPaths::EngineContentDir (), TEXT ("Editor"), TEXT ("Slate"), TEXT ("Icons"), TEXT ("icon_levels_invisible_16px.png")), FVector2D (16, 16))
		);
	}	

    return
    SNew(STableRow<TSharedPtr<FMaterialLayer>>, OwnerTable)
    .Padding(2.0)
    [
     SNew(SHorizontalBox)
     +SHorizontalBox::Slot()
     [
      SNew(SEditableText)
		.Text (FText::FromString (Item->GetName ()))
		.OnTextChanged(FOnTextChanged::CreateRaw (this, &SLayersWidget::OnTextChanged, Item))
      ]
     +SHorizontalBox::Slot()
     [
		 SNew (SBox)
		 .WidthOverride (16)
		 .HeightOverride (16)
		 [
			 SNew(SButton)
			 .ButtonStyle (FCoreStyle::Get (), "NoBorder")
		 .OnClicked (FOnClicked::CreateRaw(this, &SLayersWidget::OnToggleLayerVisibility, Item))
		 .HAlign (HAlign_Center)
		 .VAlign (VAlign_Center)
		 .ForegroundColor (FSlateColor::UseForeground ())
		 [
			 //Button Content Image
			 Image.ToSharedRef()
		 ]
		 ]
      ]
     ];
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








