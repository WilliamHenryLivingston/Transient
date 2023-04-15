#define ERR_LOG(O, M) if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, FString::Printf(TEXT("%s: %s"), *O->GetName(), TEXT(M)))

#ifdef DEBUG_MODE

#include "DrawDebugHelpers.h"

#define DEBUG_LOG_S(M) if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT(M));
#define DEBUG_LOG(M) if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, M);

#else

#define DEBUG_LOG_S(M) do { } while (false) 
#define DEBUG_LOG(M) do { } while (false)

#endif
