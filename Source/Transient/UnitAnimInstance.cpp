#include "UnitAnimInstance.h"

UnitAnimInstance::UnitAnimInstance() {
    this->Script_ArmsMode = EUnitAnimArmsMode::Empty;
    this->Script_MovementState = EUnitAnimMovementState::None;
    this->Script_Reloading = false;
    this->Script_TimeDilation = 1.0f;
    this->Script_TorsoYRotation = 0.0f;
}
