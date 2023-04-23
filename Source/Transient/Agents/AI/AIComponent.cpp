// Copyright: R. Saxifrage, 2023. All rights reserved.

#include "AIExecutor.h"

#include "Actions/BaseAction.h"

#define DEBUG_AI true

#if DEBUG_AI
#define DEBUG_LOG(M) if (Owner->DebugBehavior) GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, TEXT(M));
#define DEBUG_LOG_START(S) if (Owner->DebugBehavior) GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, S->DebugInfo);
#define DEBUG_LOG_END(S) if (Owner->DebugBehavior) GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, S->DebugInfo);
#else
#define DEBUG_LOG(M) do {} while (false);
#define DEBUG_LOG_START(S) do {} while (false);
#define DEBUG_LOG_END(S) do {} while (false);
#endif

void UAIComponent::BeginPlay() {
    Super::BeginPlay();

    this->State = new CAIState();

    this->ActionStack = TArray<CAction*>();
    this->ActionStack.Push(new CBaseAction());
}

void UAIComponent::OnComponentDestroyed(bool FullHierarchy) {
    Super::OnComponentDestroyed(FullHierarchy);

    delete this->State;

    for (int i = 0; i < this->ActionStack.Num(); i++) {
        delete this->ActionStack[i];
    }
    this->ActionStack = TArray<CAction*>();
}

void UAIComponent::TickComponent(float DeltaTime, ELevelTick Type, FActorComponentTickFunction* TickSelf) {
    Super::TickComponent(DeltaTime, Type, TickSelf);

    if (!this->ParentAgent->HasAuthority()) return;

    int StopBeyond = -1;
    CAction* DeferredPush = nullptr;
    for (int i = 0; i < this->ActionStack.Num(); i++) {
        CAction* Action = this->ActionStack[i];

        if (i == this->ActionStack.Num() - 1) {
            FActionTickResult Result = Action->ActionTick(this->ParentAgent, this->State, DeltaTime);

            if (Result.Finished) {
                DEBUG_LOG_END(Action);

                this->ActionStack.Pop(false);
                delete Action;
            }
            if (Result.PushChild != nullptr) {
                DEBUG_LOG_START(Result.PushChild);

                this->ActionStack.Push(Result.PushChild);
            }
        }
        else {
            FActionParentTickResult Result = Action->ActionParentTick(this->ParentAgent, this->State, DeltaTime);

            if (Result.PushChild != nullptr) {
                DeferredPush = Result.PushChild;
            }
            if (Result.StopChildren) {
                StopBeyond = i;
                break;
            }
        }
    }

    if (StopBeyond >= 0) {
        while (this->ActionStack.Num() - 1 > StopBeyond) {
            CAction* Action = this->ActionStack[this->ActionStack.Num() - 1];
            DEBUG_LOG("rollback");
            DEBUG_LOG_END(Action);

            delete Action;
            this->ActionStack.Pop(false);
        }
    }
    if (DeferredPush != nullptr) {
        DEBUG_LOG_START(DeferredPush);

        this->ActionStack.Push(DeferredPush);
    }
}

void UAIComponent::AIPushAction(CAction* Action) {
    this->ActionStack.Push(Action);
}