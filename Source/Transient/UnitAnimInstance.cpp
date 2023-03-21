#include "UnitAnimInstance.h"

UnitAnimInstance::UnitAnimInstance() {
    this->Script_ArmsMode = EUnitAnimArmsMode::Empty;
    this->Script_MovementState = EUnitAnimMovementState::None;
    this->Script_Reloading = false;
}
