//
//  MaterialLayer.hpp
//  UE4PluginsDev
//
//  Created by Benjamin Regnier on 06/11/2016.
//  Copyright © 2016 Epic Games, Inc. All rights reserved.
//

#ifndef MaterialLayer_h
#define MaterialLayer_h

class UMaterialInterface;

class FMaterialLayer : public TSharedFromThis<FMaterialLayer>
{
public:
    FMaterialLayer(const FString& InName = "");
    ~FMaterialLayer();
    
    TSharedPtr<FMaterialLayer> Clone();
    
    void Update(AActor* Actor);
    void Update(UPrimitiveComponent* Primitive);
    void Apply();
    
    bool IsEnabled() const { return Enabled; }
    void SetIsEnabled(bool IsEnabled) {
        Enabled = IsEnabled;
        OnEnabledChanged.ExecuteIfBound();
    }
    
    FString GetName() const { return Name; }
    void SetName(const FString& InName) { Name = InName; }
    
    TMap<FString, TArray<FName>>& GetPrimitiveToMaterialsMap() { return PrimitiveMaterialsMap; }
    void SetPrimitiveToMaterialsMap(const TMap<FString, TArray<FName>>& InMap) { PrimitiveMaterialsMap = InMap; }
    
    DECLARE_DELEGATE(FOnEnabledChanged)
    FOnEnabledChanged OnEnabledChanged;
    
private:
    void ApplyToStaticMesh();
    void ApplyToSkeletalMesh();
    
    void ApplyToPrimitiveComponent(UPrimitiveComponent* Primitive, const TArray<UMaterialInterface*> Materials);
    
private:
    TMap<FString, TArray<FName>> PrimitiveMaterialsMap;
    bool Enabled;
    FString Name;
};

FArchive& operator<<(FArchive &Ar, FMaterialLayer &MaterialLayer);

#endif /* MaterialLayer_h */
