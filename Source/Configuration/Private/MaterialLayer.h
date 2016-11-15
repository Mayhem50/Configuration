//
//  MaterialLayer.hpp
//  UE4PluginsDev
//
//  Created by Benjamin Regnier on 06/11/2016.
//  Copyright © 2016 Epic Games, Inc. All rights reserved.
//

#pragma once

#include "BaseLayer.h"
#include "MaterialLayer.generated.h"

UCLASS()
class CONFIGURATION_API UMaterialLayer : public UBaseLayer
{
	GENERATED_BODY()

public:
    UMaterialLayer(const FObjectInitializer& objectInitializer);
    virtual ~UMaterialLayer();
    
    UMaterialLayer* Clone();
    
    void Update(AActor* Actor);
    void Update(UPrimitiveComponent* Primitive);

	UFUNCTION (BlueprintCallable, Category = "Functions")
		virtual void Apply() override;
    
    TMap<FString, TArray<FName>>& GetPrimitiveToMaterialsMap() { return PrimitiveMaterialsMap; }
    void SetPrimitiveToMaterialsMap(const TMap<FString, TArray<FName>>& InMap) { PrimitiveMaterialsMap = InMap; }

	friend FArchive& operator<<(FArchive &Ar, UMaterialLayer &MaterialLayer)
	{
		Ar << MaterialLayer.LayerName;
		Ar << MaterialLayer.bEnabled;
		Ar << (uint8&)MaterialLayer.Type;
		Ar << MaterialLayer.PrimitiveMaterialsMap;

		return Ar;
	}
    
private:
    void ApplyToStaticMesh();
    void ApplyToSkeletalMesh();
    
    void ApplyToPrimitiveComponent(UPrimitiveComponent* Primitive, const TArray<class UMaterialInterface*> Materials);
    
private:
    TMap<FString, TArray<FName>> PrimitiveMaterialsMap;
};
