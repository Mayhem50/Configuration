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
    FReply OnRemoveButtonPressed();
    FReply OnDuplicateButtonPressed();
    FReply OnSave();    
	FReply OnToggleLayerVisibility (TSharedPtr<FMaterialLayer> Item);

    void OnSelectionChanged(TSharedPtr<FMaterialLayer> Item, ESelectInfo::Type SelectionType);    
    void OnTextChanged(const FText& InText, TSharedPtr<FMaterialLayer> Item);
    
    TSharedRef<ITableRow> OnGenerateRowForList(TSharedPtr<FMaterialLayer> Item, const TSharedRef<STableViewBase>& OwnerTable);
    
private:
    TSharedPtr<FLayerManager> LayerManager;
    TSharedPtr<SListView<TSharedPtr<FMaterialLayer>>> ListViewWidget;
    
    TSharedPtr<FMaterialLayer> SelectedMaterialLayer;
    
    TSharedPtr<SEditableText> EditableText;
};

#endif /* LayersWidget_h */
