// Copyright: R. Saxifrage, 2023. All rights reserved.

// Common definitions.

#define ERR_LOG(O, M) GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, FString::Printf(TEXT("%s: %s"), *O->GetName(), TEXT(M)));

#define DEBUG_AI true
#define DEBUG_DETECTION true
#define DEBUG_IK true
