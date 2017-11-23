#include "myUSB.h"

/*USBD_Callbacks_TypeDef callbacks =
{
  .usbReset        = NULL,
  .usbStateChange  = StateChangeEvent,
  .setupCmd        = SetupCmd,
  .isSelfPowered   = NULL,
  .sofInt          = NULL
};

 USBD_Init_TypeDef usbInitStruct =
{
  .deviceDescriptor    = &USBDESC_deviceDesc,
  .configDescriptor    = USBDESC_configDesc,
  .stringDescriptors   = USBDESC_strings,
  .numberOfStrings     = sizeof(USBDESC_strings)/sizeof(void*),
  .callbacks           = &callbacks,
  .bufferingMultiplier = USBDESC_bufferingMultiplier,
  .reserved            = 0
};
*/

int dataSentCallback(USB_Status_TypeDef status, uint32_t xferred, uint32_t remaining)
{
  /* Remove warnings for unused variables */
  (void)xferred;
  (void)remaining;

  if ( status != USB_STATUS_OK )
  {
    /* Handle error */
  }
  return USB_STATUS_OK;
}
