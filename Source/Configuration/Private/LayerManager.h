//
//  LayerManager.h
//  UE4PluginsDev
//
//  Created by Benjamin Regnier on 06/11/2016.
//  Copyright © 2016 Epic Games, Inc. All rights reserved.
//

#pragma once

#include "LayerManager.generated.h"

UCLASS()
class CONFIGURATION_API ULayerManager : public UObject
{
	GENERATED_BODY()

public:
    ULayerManager();
    ~ULayerManager();
    
    void Init();
    
    void OnApplyObjectOnActor(UObject* Object, AActor* Actor);
	void OnUndoRedo (struct FUndoSessionContext Context, bool bCanRedo);
    void OnObjectModified(UObject* Object);
	void OnObjectPropertyChanged (UObject* Object, FPropertyChangedEvent& PropertyChangedEvent);
	    
    void OnLayerEnabledChanged();

	void ApplyDisplayedLayers ();
    
    TArray<class UMaterialLayer*>& GetLayers() { return Layers; }

	class UMaterialLayer* GetCurrentLayer () { return CurrentLayer; }
	void SetCurentLayer (class UMaterialLayer* Layer);
    
    void AddLayer();
    void RemoveLayer(class UMaterialLayer* MaterialLayer);
    void Duplicate(class UMaterialLayer* MaterialLayer);
    
    void Save();

	bool SwapMaterials (UMaterialLayer* Layer1, UMaterialLayer* Layer2);

	DECLARE_DELEGATE (FOnCreateNewLayerFromDrag)
		FOnCreateNewLayerFromDrag OnCreateNewLayerFromDrag;
		    
private:
	void ParseAllActors (class UMaterialLayer* Layer);
	void ShouldCreateLayer ();

	bool ActorExistInCurrentLayer (AActor* Actor);

	UPROPERTY()
		TArray<class UMaterialLayer*> Layers;

	UPROPERTY()
		class UMaterialLayer* CurrentLayer;
};
