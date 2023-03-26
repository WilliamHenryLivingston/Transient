#include "TakeCoverAction.h"

#include "Kismet/GameplayStatics.h"

#include "../AIUnit.h"
#include "../AINavManager.h"
#include "MoveToPointAction.h"

CTakeCoverAction::CTakeCoverAction() {}

CTakeCoverAction::~CTakeCoverAction() {}

FAIActionExecutionResult CTakeCoverAction::AIActionTick(AActor* RawOwner, float DeltaTime) {
    AAIUnit* Owner = Cast<AAIUnit>(RawOwner);
    
    AActor* Threat = Owner->AIGetAgroTarget();
    if (Threat == nullptr) return this->Finished;
    FVector ThreatLocation = Threat->GetActorLocation();

    AAINavManager* NavManager = Cast<AAINavManager>(UGameplayStatics::GetActorOfClass(Owner->GetWorld(), AAINavManager::StaticClass()));

    AAINavNode* Cover = nullptr;
    TArray<AAINavNode*> CheckSet = NavManager->NavGetNearestNodes(Owner, 20);
    for (int i = 0; i < CheckSet.Num(); i++) {
        AAINavNode* Check = CheckSet[i];

        if (!Check->CoverPosition) continue;

        // Conceptually; Switch space to Check at origin with it's rotation applied, see if Threat is +X
        FVector AdjustedThreatLocation = Check->GetActorRotation().RotateVector(ThreatLocation - Check->GetActorLocation());

        if (AdjustedThreatLocation.X > 0.0f) {
            Cover = Check;
            break;
        }
    }

    if (Cover == nullptr) return this->Finished;

    return FAIActionExecutionResult(false, new CMoveToPointAction(Cover, 100.0f));
}
