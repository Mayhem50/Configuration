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


ULayerManager::ULayerManager()
{
	Layers.Init (nullptr, 0);
}

ULayerManager::~ULayerManager()
{
    
}

void ULayerManager::Init(){   
    TArray<uint8> BinaryArray;
	BinaryArray.Init (0, 0);
    
    FString FilePath = FPaths::Combine(*FPaths::GamePluginsDir(), TEXT("Configuration"), TEXT("Content"), TEXT("ConfData.dat"));
    
    DisplayNotification(FilePath);
    
    if(FPaths::FileExists(FilePath)){
        FFileHelper::LoadFileToArray(BinaryArray, *FilePath);
        
        if(BinaryArray.Num() > 0){
            FMemoryReader Reader = FMemoryReader(BinaryArray, true);
            
            while(!Reader.AtEnd()){
                UMaterialLayer* Layer = NewObject<UMaterialLayer>();
                Reader << *Layer;
				//Layer->Serialize (Reader);
                Layer->OnEnabledChanged.BindUObject (this, &ULayerManager::OnLayerEnabledChanged);
                Layers.Add(Layer);
            }
        }
    }
    else{
        AddLayer();
        CurrentLayer = Layers[0];
        Save();
    }
}

void ULayerManager::Save(){
    FBufferArchive BufferArchive;
    
    for(auto Layer : Layers){
        BufferArchive << *Layer;
		//Layer->Serialize (BufferArchive);
    }
    
    if(BufferArchive.Num() > 0){
        FString FilePath = FPaths::Combine(*FPaths::GamePluginsDir(), TEXT("Configuration"), TEXT("Content"), TEXT("ConfData.dat"));
        DisplayNotification(FilePath);
        FFileHelper::SaveArrayToFile(BufferArchive, *FilePath);
    }
}

void ULayerManager::AddLayer(){
    UMaterialLayer* Layer = NewObject<UMaterialLayer>();
	Layer->Init (ELayerEnum::MATERIAL, "Layer_" + FString::FromInt (Layers.Num ()));
    Layer->OnEnabledChanged.BindUObject(this, &ULayerManager::OnLayerEnabledChanged);
    Layers.Add(Layer);
}

void ULayerManager::RemoveLayer(UMaterialLayer* MaterialLayer){
    if(MaterialLayer == nullptr || Layers.Num() == 1){ return; }
    
    Layers.Remove(MaterialLayer);
}

void ULayerManager::Duplicate(UMaterialLayer* MaterialLayer){
    if(!MaterialLayer){ return; }
    
    Layers.Add(MaterialLayer->Clone());
}

void ULayerManager::OnObjectModified(UObject* Object){
    if(!CurrentLayer){ return; }

	bool IsWanted = Cast<AStaticMeshActor> (Object) || Cast<ASkeletalMeshActor> (Object);

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

void ULayerManager::OnLayerEnabledChanged(){
    for(UMaterialLayer* Layer : Layers){
        if(Layer->IsEnabled()){
            Layer->Apply();
        }
    }
}

void ULayerManager::OnApplyObjectOnActor(UObject* Object, AActor* Actor)
{
    if(!CurrentLayer){ return; }
    
    UMaterialInterface* Material = Cast<UMaterialInterface>(Object);
    
    if(Material)
    {
        CurrentLayer->Update(Actor);
        DisplayNotification("On Apply Object On Actor " + GEditor->GetTransactionName().ToString());
    }
    
    Save();
}

void ULayerManager::OnObjectPropertyChanged(UObject* Object, FPropertyChangedEvent& PropertyChangedEvent)
{
    FName PropertyName = (PropertyChangedEvent.Property != nullptr) ? PropertyChangedEvent.Property->GetFName() : NAME_None;
    
    if(PropertyName == "OverrideMaterials"){
        DisplayNotification("On Object Property Changed");
    }
    
    Save();
}

void ULayerManager::DisplayNotification(const FString& String) const{
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


