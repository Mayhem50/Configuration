//
//  LayerManager.cpp
//  UE4PluginsDev
//
//  Created by Benjamin Regnier on 06/11/2016.
//  Copyright © 2016 Epic Games, Inc. All rights reserved.
//

#include "ConfigurationPrivatePCH.h"

#include "LayerManager.h"

#include "Framework/Notifications/NotificationManager.h"
#include "Widgets/Notifications/SNotificationList.h"

#include "EditorUndoClient.h"


FLayerManager::FLayerManager()
{
    //MaterialLayer = MakeShareable(new FMaterialLayer);
}

FLayerManager::~FLayerManager()
{
    
}

void FLayerManager::Init(){    
    //ULayers* _DataAsset = LoadObject<ULayers>(nullptr, TEXT("Configuration/Content/Layers.Layers"));
    
    //if(_DataAsset){
        // Load
        //Layers = TWeakObjectPtr<ULayers>(_DataAsset);
    //}
    //else{
        // Create
    //}
    TArray<uint8> BinaryArray;
	BinaryArray.Init (0, 0);
    
    FString FilePath = FPaths::Combine(*FPaths::GamePluginsDir(), TEXT("Configuration"), TEXT("Content"), TEXT("ConfData.dat"));
    
    DisplayNotification(FilePath);
    
    if(FPaths::FileExists(FilePath)){
        FFileHelper::LoadFileToArray(BinaryArray, *FilePath);
        
        if(BinaryArray.Num() > 0){
            FMemoryReader Reader = FMemoryReader(BinaryArray, true);
            
            while(!Reader.AtEnd()){
                FMaterialLayer* Layer = new FMaterialLayer;
                Reader << *Layer;
                Layer->OnEnabledChanged.BindRaw(this, &FLayerManager::OnLayerEnabledChanged);
                Layers.Add(MakeShareable(Layer));
            }
        }
    }
    else{
        AddLayer();
        CurrentLayer = Layers[0];
        Save();
    }
}

void FLayerManager::Save(){
    FBufferArchive BufferArchive;
    
    for(auto Layer : Layers){
        BufferArchive << *Layer.Get();
    }
    
    if(BufferArchive.Num() > 0){
        FString FilePath = FPaths::Combine(*FPaths::GamePluginsDir(), TEXT("Configuration"), TEXT("Content"), TEXT("ConfData.dat"));
        DisplayNotification(FilePath);
        FFileHelper::SaveArrayToFile(BufferArchive, *FilePath);
    }
}

void FLayerManager::AddLayer(){
    TSharedPtr<FMaterialLayer> Layer = MakeShareable(new FMaterialLayer("Layer_" + FString::FromInt(Layers.Num())));
    Layer->OnEnabledChanged.BindRaw(this, &FLayerManager::OnLayerEnabledChanged);
    Layers.Add(Layer);
}

void FLayerManager::RemoveLayer(TSharedPtr<FMaterialLayer> MaterialLayer){
    if(!MaterialLayer.IsValid() || Layers.Num() == 1){ return; }
    
    Layers.Remove(MaterialLayer);
}

void FLayerManager::Duplicate(TSharedPtr<FMaterialLayer> MaterialLayer){
    if(!MaterialLayer.IsValid()){ return; }
    
    Layers.Add(MaterialLayer->Clone());
}

void FLayerManager::OnObjectModified(UObject* Object){
    if(!CurrentLayer.IsValid()){ return; }

	bool IsWanted = Cast<UStaticMeshComponent> (Object) || Cast<USkeletalMeshComponent> (Object);

	if (!IsWanted) { 
		return;
	}
    
	UPrimitiveComponent* Actor = Cast<UPrimitiveComponent>(Object);
    
    if(Actor){
        CurrentLayer->Update(Actor);
        DisplayNotification("On Object Modify " + Object->GetName());
    }
    
    Save();
}

void FLayerManager::OnLayerEnabledChanged(){
    for(TSharedPtr<FMaterialLayer> Layer : Layers){
        if(Layer->IsEnabled()){
            Layer->Apply();
        }
    }
}

void FLayerManager::OnApplyObjectOnActor(UObject* Object, AActor* Actor)
{
    if(!CurrentLayer.IsValid()){ return; }
    
    UMaterialInterface* Material = Cast<UMaterialInterface>(Object);
    
    if(Material)
    {
        CurrentLayer->Update(Actor);
        DisplayNotification("On Apply Object On Actor " + GEditor->GetTransactionName().ToString());
    }
    
    Save();
}

void FLayerManager::OnObjectPropertyChanged(UObject* Object, FPropertyChangedEvent& PropertyChangedEvent)
{
    FName PropertyName = (PropertyChangedEvent.Property != nullptr) ? PropertyChangedEvent.Property->GetFName() : NAME_None;
    
    if(PropertyName == "OverrideMaterials"){
        DisplayNotification("On Object Property Changed");
    }
    
    Save();
}

void FLayerManager::DisplayNotification(const FString& String) const{
    FNotificationInfo Info(FText::FromString(String));
    Info.FadeInDuration = 0.1f;
    Info.FadeOutDuration = 0.5f;
    Info.ExpireDuration = 5.5f;
    Info.bUseThrobber = false;
    Info.bUseSuccessFailIcons = true;
    Info.bUseLargeFont = true;
    Info.bFireAndForget = false;
    Info.bAllowThrottleWhenFrameRateIsLow = false;
    auto NotificationItem = FSlateNotificationManager::Get().AddNotification( Info );
    NotificationItem->SetCompletionState(SNotificationItem::CS_Success);
    NotificationItem->ExpireAndFadeout();
}


