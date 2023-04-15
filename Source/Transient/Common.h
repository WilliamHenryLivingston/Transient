#define ERR_LOG(O, M) GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, FString::Printf(TEXT("%s: %s"), *O->GetName(), TEXT(M)));
