#ifdef HAVE_CONFIG_H
#  include "config.h"
#endif // HAVE_CONFIG_H

#include "drivers/vcrm200.h"

#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <sys/select.h>
#include <errno.h>
#include <string.h>
#ifdef _MSC_VER
#include <sys/types.h>
#endif
#include <nfc/nfc.h>

#include "nfc-internal.h"
#include "buses/usbbus.h"
#include "chips/pn53x.h"
#include "chips/pn53x-internal.h"
#include "drivers/pn53x_usb.h"


#define VCRM200_DRIVER_NAME "vcrm200"
#define LOG_CATEGORY "libnfc.driver.vcrm200"
#define LOG_GROUP    NFC_LOG_GROUP_DRIVER

typedef enum {
  UNKNOWN,
  VCRM200
} vcrm200_model;

// Internal data struct
struct vcrm200_data {
  usb_dev_handle *pudh;
  vcrm200_model model;
  uint32_t uiEndPointIn;
  uint32_t uiEndPointOut;
  uint32_t uiMaxPacketSize;
  volatile bool abort_flag;
  bool possibly_corrupted_usbdesc;
};


struct vcrm200_descriptor {
  char *dirname;
  char *filename;
};

static nfc_device* vcrm200_open(const nfc_context *context, const nfc_connstring connstring)
{
	nfc_device *pnd = NULL;
	struct vcrm200_descriptor desc = { NULL, NULL };
	int connstring_decode_level = connstring_decode(connstring, VCRM200_DRIVER_NAME, "usb", &desc.dirname, &desc.filename);
	log_put(LOG_GROUP, LOG_CATEGORY, NFC_LOG_PRIORITY_DEBUG, "%d element(s) have been decoded from \"%s\"", connstring_decode_level, connstring);
	if (connstring_decode_level < 1) {
		goto free_mem;
	}

	if (!pnd) {
		perror("malloc");
		goto error;
	}	
	// pn53x_usb_get_usb_device_name(dev, data.pudh, pnd->name, sizeof(pnd->name));
	
	pnd = nfc_device_new(context, connstring);
	
	pnd->driver = &pn53x_usb_driver;
	
	struct vcrm200_data data = {
		.pudh = NULL,
		.uiEndPointIn = 0,
		.uiEndPointOut = 0,
		.possibly_corrupted_usbdesc = false,
	};
	error:
	// Free allocated structure on error.
	nfc_device_free(pnd);
	pnd = NULL;
	free_mem:
	free(desc.dirname);
	free(desc.filename);
	return pnd;
}

static void vcrm200_close(nfc_device *pnd)
{
}

static int vcrm200_transceive(nfc_device *device, const uint8_t *tx, const size_t tx_len, uint8_t *rx, size_t *rx_len, int timeout) {
    // Code to send and receive data to/from the VCRM200 device
    return 0; // Return 0 on success, -1 on failure
}

int vcrm200_initiator_init(struct nfc_device *pnd)
{
/*
  pn53x_reset_settings(pnd);
  int res;
  if (CHIP_DATA(pnd)->sam_mode != PSM_NORMAL) {
    if ((res = pn532_SAMConfiguration(pnd, PSM_NORMAL, -1)) < 0) {
      return res;
    }
  }

  // Configure the PN53X to be an Initiator or Reader/Writer
  if ((res = pn53x_write_register(pnd, PN53X_REG_CIU_Control, SYMBOL_INITIATOR, 0x10)) < 0)
    return res;

  CHIP_DATA(pnd)->operating_mode = INITIATOR;
*/
  return NFC_SUCCESS;
}


static const nfc_driver vcrm200_driver = {

	.name                             = VCRM200_DRIVER_NAME,
	.scan_type                        = NOT_INTRUSIVE,
	.scan                             = NULL,
	.open                             = vcrm200_open,
	.close                            = vcrm200_close,
	.strerror                         = NULL,
  
	.initiator_init                   = vcrm200_initiator_init,
	.initiator_init_secure_element    = NULL, // No secure-element support
	.initiator_select_passive_target  = NULL, /*vcrm200_initiator_select_passive_target,*/
	.initiator_poll_target            = NULL, /*vcrm200_initiator_poll_target,*/
	.initiator_select_dep_target      = NULL, /*vcrm200_initiator_select_dep_target,*/
	.initiator_deselect_target        = NULL, /*vcrm200_initiator_deselect_target,*/
	.initiator_transceive_bytes       = NULL, /*vcrm200_initiator_transceive_bytes,*/
	.initiator_transceive_bits        = NULL, /*vcrm200_initiator_transceive_bits,*/
	.initiator_transceive_bytes_timed = NULL, /*vcrm200_initiator_transceive_bytes_timed,*/
	.initiator_transceive_bits_timed  = NULL, /*vcrm200_initiator_transceive_bits_timed,*/
	.initiator_target_is_present      = NULL, /*vcrm200_initiator_target_is_present,*/

	.target_init           = NULL, /*vcrm200_target_init,*/
	.target_send_bytes     = NULL, /*vcrm200x_target_send_bytes,*/
	.target_receive_bytes  = NULL, /*vcrm200_target_receive_bytes,*/
	.target_send_bits      = NULL, /*vcrm200_target_send_bits,*/
	.target_receive_bits   = NULL, /*vcrm200_target_receive_bits,*/

	.device_set_property_bool     = NULL, /*vcrm200_usb_set_property_bool,*/
	.device_set_property_int      = NULL, /*vcrm200_set_property_int,*/
	.get_supported_modulation     = NULL, /*vcrm200_get_supported_modulation,*/
	.get_supported_baud_rate      = NULL, /*vcrm200_get_supported_baud_rate,*/
	.device_get_information_about = NULL, /*vcrm200_get_information_about,*/

	.abort_command  = NULL, /*pn53x_usb_abort_command,*/
	.idle           = NULL, /*pn53x_idle,*/
	.powerdown      = NULL, /*pn53x_PowerDown,*/
};


/*
const struct pn53x_io pn53x_usb_io = {
  .send       = pn53x_usb_send,
  .receive    = pn53x_usb_receive,
};

const struct nfc_driver pn53x_usb_driver = {
  .name                             = PN53X_USB_DRIVER_NAME,
  .scan_type                        = NOT_INTRUSIVE,
  .scan                             = pn53x_usb_scan,
  .open                             = pn53x_usb_open,
  .close                            = pn53x_usb_close,
  .strerror                         = pn53x_strerror,

  .initiator_init                   = pn53x_initiator_init,
  .initiator_init_secure_element    = NULL, // No secure-element support
  .initiator_select_passive_target  = pn53x_initiator_select_passive_target,
  .initiator_poll_target            = pn53x_initiator_poll_target,
  .initiator_select_dep_target      = pn53x_initiator_select_dep_target,
  .initiator_deselect_target        = pn53x_initiator_deselect_target,
  .initiator_transceive_bytes       = pn53x_initiator_transceive_bytes,
  .initiator_transceive_bits        = pn53x_initiator_transceive_bits,
  .initiator_transceive_bytes_timed = pn53x_initiator_transceive_bytes_timed,
  .initiator_transceive_bits_timed  = pn53x_initiator_transceive_bits_timed,
  .initiator_target_is_present      = pn53x_initiator_target_is_present,

  .target_init           = pn53x_target_init,
  .target_send_bytes     = pn53x_target_send_bytes,
  .target_receive_bytes  = pn53x_target_receive_bytes,
  .target_send_bits      = pn53x_target_send_bits,
  .target_receive_bits   = pn53x_target_receive_bits,

  .device_set_property_bool     = pn53x_usb_set_property_bool,
  .device_set_property_int      = pn53x_set_property_int,
  .get_supported_modulation     = pn53x_usb_get_supported_modulation,
  .get_supported_baud_rate      = pn53x_get_supported_baud_rate,
  .device_get_information_about = pn53x_get_information_about,

  .abort_command  = pn53x_usb_abort_command,
  .idle           = pn53x_idle,
  .powerdown      = pn53x_PowerDown,
};
*/
