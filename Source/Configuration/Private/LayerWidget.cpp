//
//  LayerWidget.cpp
//  UE4PluginsDev
//
//  Created by Benjamin Regnier on 09/11/2016.
//  Copyright © 2016 Epic Games, Inc. All rights reserved.
//

#include "ConfigurationPrivatePCH.h"

#include "LayerWidget.h"

#include "Classes/Editor/Transactor.h"
#include "ScopedTransaction.h"

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
       SAssignNew(ListViewWidget, SListView<UMaterialLayer*>)
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
    
    if (LayerManager->GetCurrentLayer ())
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

FReply SLayersWidget::OnRemoveButtonPressed(UMaterialLayer* Layer){
    if(Layer){
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

TSharedRef<ITableRow> SLayersWidget::OnGenerateRowForList(UMaterialLayer* Item, const TSharedRef<STableViewBase> &OwnerTable){  
	return
		SNew (SLayerRowWidget, OwnerTable, Item)
		.LayersWidget (this);
}

void SLayersWidget::OnSelectionChanged(UMaterialLayer* Item, ESelectInfo::Type SelectionType){
    SelectedMaterialLayer = Item;
    
	if (!SelectedMaterialLayer)
	{
		return;
	}

	LayerManager->SetCurentLayer (SelectedMaterialLayer);    
	LayerManager->ApplyDisplayedLayers ();
}

void SLayersWidget::OnTextChanged(const FText &InText, UMaterialLayer* Item){
    SelectedMaterialLayer = Item;
    
    if(!Item){ return; }
    
    Item->LayerName = InText.ToString ();
}

FReply SLayersWidget::OnToggleLayerVisibility (UMaterialLayer* Item)
{
    Item->SetEnabled (!Item->IsEnabled ());
	LayerManager->ApplyDisplayedLayers ();
    ListViewWidget->RebuildList ();
    return FReply::Handled ();
}


FReply SLayerRowWidget::OnDragDetected (const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
	TSharedRef<FListItemDragDropOpertation> Operation = FListItemDragDropOpertation::New(this);
	return FReply::Handled ().BeginDragDrop(Operation);
}

void SLayerRowWidget::OnDragEnter (const FGeometry& MyGeometry, const FDragDropEvent& DragDropEvent)
{
	TSharedPtr<FListItemDragDropOpertation> DragDropOperation = DragDropEvent.GetOperationAs<FListItemDragDropOpertation> ();

	if (DragDropOperation.IsValid ())
	{
		this->OnMouseEnter(MyGeometry, DragDropEvent);
		return;
	}
}

FReply SLayerRowWidget::OnDrop (const FGeometry& MyGeometry, const FDragDropEvent& DragDropEvent)
{
	TSharedPtr<FListItemDragDropOpertation> DragDropOperation = DragDropEvent.GetOperationAs<FListItemDragDropOpertation> ();

	if (DragDropOperation.IsValid ())
	{
		if (LayersWidget->LayerManager->SwapMaterials (this->Item, DragDropOperation->OriginWidget->Item))
		{
			LayersWidget->LayerManager->ApplyDisplayedLayers ();
			LayersWidget->ListViewWidget->RebuildList ();
		}

		return FReply::Handled ();
	}
	else
	{
		return FReply::Unhandled ();
	}
}

void SLayerRowWidget::OnDragLeave (const FDragDropEvent& DragDropEvent)
{
	TSharedPtr<FListItemDragDropOpertation> DragDropOperation = DragDropEvent.GetOperationAs<FListItemDragDropOpertation> ();

	if (DragDropOperation.IsValid ())
	{
		this->OnMouseLeave (DragDropEvent);
		return;
	}

}

void FListItemDragDropOpertation::OnDrop (bool bDropWasHandled, const FPointerEvent& MouseEvent)
{
	HideTrash.ExecuteIfBound ();

	FDragDropOperation::OnDrop (bDropWasHandled, MouseEvent);
}

void FListItemDragDropOpertation::OnDragged (const class FDragDropEvent& DragDropEvent)
{
	if (CursorDecoratorWindow.IsValid ())
	{
		CursorDecoratorWindow->MoveWindowTo (DragDropEvent.GetScreenSpacePosition () - BlockSize * 0.5f);
	}
}

TSharedPtr<SWidget> FListItemDragDropOpertation::GetDefaultDecorator () const
{
	return SNew (SBorder).Cursor (EMouseCursor::GrabHandClosed)
		[
			OriginWidget->AsWidget ()
		];
}

TSharedRef<FListItemDragDropOpertation> FListItemDragDropOpertation::New (SLayerRowWidget* Origin, FSimpleDelegate TrashShowCallback, FSimpleDelegate TrashHideCallback, int32 OriginPosition)
{
	TSharedRef<FListItemDragDropOpertation> Operation = MakeShareable (new FListItemDragDropOpertation);

	Operation->OriginWidget = Origin;
	Operation->OriginBarPosition = OriginPosition;
	Operation->ShowTrash = TrashShowCallback;
	Operation->HideTrash = TrashHideCallback;
	Operation->bSetForDeletion = false;
	Operation->BlockSize = FVector2D (32, 32);

	Operation->ShowTrash.ExecuteIfBound ();

	Operation->Construct ();

	return Operation;
}





