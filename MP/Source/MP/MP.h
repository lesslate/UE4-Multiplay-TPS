// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

DECLARE_LOG_CATEGORY_EXTERN(MP, Log, All);

#define LOG_CALLINFO (FString(__FUNCTION__) + TEXT("(") + FString::FromInt(__LINE__) + TEXT(")"))
#define LOG_S(Verbosity) UE_LOG(MP, Verbosity, TEXT("%s"), *LOG_CALLINFO)
#define LOG(Verbosity, Format, ...) UE_LOG(MP, Verbosity, TEXT("%s %s"), *LOG_CALLINFO, *FString::Printf(Format, ##__VA_ARGS__))

#define CHECK(Expr, ...) {if(!(Expr)) {LOG(Error, TEXT("ASSERTION : %s"), TEXT("'"#Expr"'")); return __VA_ARGS__;}}

#define NETMODE_WORLD (((GEngine == nullptr) || (GetWorld() == nullptr)) ? TEXT("") \
        : (GEngine->GetNetMode(GetWorld()) == NM_Client) ? TEXT("[Client] ") \
        : (GEngine->GetNetMode(GetWorld()) == NM_ListenServer) ? TEXT("[ListenServer] ") \
        : (GEngine->GetNetMode(GetWorld()) == NM_DedicatedServer) ? TEXT("[DedicatedServer] ") \
        : TEXT("[Standalone] "))

