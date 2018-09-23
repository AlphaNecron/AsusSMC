//
//  KeyImplementations.cpp
//  AsusSMC
//
//  Copyright © 2018 Le Bao Hiep. All rights reserved.
//

#include "KeyImplementations.hpp"

SMC_RESULT SMCALSValue::readAccess() {
    auto value = reinterpret_cast<Value *>(data);
    uint32_t lux = atomic_load_explicit(currentLux, memory_order_acquire);
    uint8_t bits = forceBits->bits();

    if (lux == 0xFFFFFFFF) {
        value->valid = false;
    } else {
        value->valid = true;
        if (!(bits & ALSForceBits::kALSForceHighGain))
            value->highGain = true;
        if (!(bits & ALSForceBits::kALSForceChan))
            value->chan0 = OSSwapHostToBigInt16(lux);
        if (!(bits & ALSForceBits::kALSForceLux))
            value->roomLux = OSSwapHostToBigInt32(lux << 14);
    }

    return SmcSuccess;
}

SMC_RESULT SMCKBrdBLightValue::writeAccess() {
    auto value = reinterpret_cast<lkb *>(data);
    if (atkDevice) {
        uint16_t tval = (value->val1 << 4) | (value->val2 >> 4);
        DBGLOG("alsd", "LKSB writeAccess val1 %d val2 %d", value->val1, value->val2);
        DBGLOG("alsd", "LKSB writeAccess %d", tval);
        tval = tval / 16;
        OSObject * params[1];
        OSObject * ret = NULL;
        params[0] = OSNumber::withNumber(tval, sizeof(tval)*8);

        atkDevice->evaluateObject("SKBV", &ret, params, 1);
    }
    return SmcSuccess;
}
