#include "UnitAnimInstance.h"

UUnitAnimInstance::UUnitAnimInstance() {
    this->Script_ArmsState = EUnitAnimArmsState::Empty;
    this->Script_LegsState = EUnitAnimLegsState::None;
    this->Script_ArmsModifier = EUnitAnimArmsModifier::None;
    this->Script_LegsModifier = EUnitAnimLegsModifier::None;
    this->Script_ArmsInteractTarget = EUnitAnimArmsInteractTarget::None;
    this->Script_TimeDilation = 1.0f;
    this->Script_TorsoPitch = 0.0f;
}
