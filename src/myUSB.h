#include "em_usb.h"
#include "msdd.h"
#include "msddmedia.h"
#include "vud.h"
#include "cdc.h"
#include "descriptors.h"

int dataSentCallback(USB_Status_TypeDef status, uint32_t xferred, uint32_t remaining);
