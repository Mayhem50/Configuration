//
//  LayerManager.h
//  UE4PluginsDev
//
//  Created by Benjamin Regnier on 06/11/2016.
//  Copyright © 2016 Epic Games, Inc. All rights reserved.
//

#ifndef LayerManager_h
#define LayerManager_h

class FMaterialLayer;
class ULayers;

class FLayerManager : public TSharedFromThis<FLayerManager>
{
public:
    FLayerManager();
    ~FLayerManager();
    
    void Init();
    
    void OnApplyObjectOnActor(UObject* Object, AActor* Actor);
    void OnObjectPropertyChanged(UObject* Object, FPropertyChangedEvent& PropertyChangedEvent);
    void OnObjectModified(UObject* Object);
    
    void OnLayerEnabledChanged();
    
    TArray<TSharedPtr<class FMaterialLayer>>& GetLayers() { return Layers; }

	TSharedPtr<class FMaterialLayer> GetCurrentLayer () { return CurrentLayer; }
	void SetCurentLayer (TSharedPtr<class FMaterialLayer> Layer) { CurrentLayer = Layer; }
    
    void AddLayer();
    void RemoveLayer(TSharedPtr<FMaterialLayer> MaterialLayer);
    void Duplicate(TSharedPtr<FMaterialLayer> MaterialLayer);
    
    void Save();
    
    void DisplayNotification(const FString& String) const;
    
private:
    TArray<TSharedPtr<class FMaterialLayer>> Layers;
    TSharedPtr<class FMaterialLayer> CurrentLayer;
};

#endif /* LayerManager_h */
