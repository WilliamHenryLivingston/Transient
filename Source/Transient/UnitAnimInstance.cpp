#include "UnitAnimInstance.h"

UUnitAnimInstance::UUnitAnimInstance() {
    this->Script_ArmsMode = EUnitAnimArmsMode::Empty;
    this->Script_MovementState = EUnitAnimMovementState::None;
    this->Script_ArmsModifier = EUnitAnimArmsModifier::None;
    this->Script_TimeDilation = 1.0f;
    this->Script_TorsoPitch = 0.0f;
}
