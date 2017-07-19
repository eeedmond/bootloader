#pragma once

#include <stdbool.h>
#include <stdint.h>

#ifndef SHARED_MSG_PACKED
#define SHARED_MSG_PACKED __attribute__((packed))
#endif

#ifndef APP_DESCRIPTOR_ALIGNED_AND_PACKED
#define APP_DESCRIPTOR_ALIGNED_AND_PACKED __attribute__((aligned(8),packed))
#endif

enum shared_msg_t {
    SHARED_MSG_BOOT = 0,
    SHARED_MSG_FIRMWAREUPDATE = 1,
    SHARED_MSG_BOOT_INFO = 2
};

struct shared_canbus_info_s {
    uint8_t local_node_id;
    int32_t baudrate;
} SHARED_MSG_PACKED;

struct shared_boot_msg_s {
    struct shared_canbus_info_s canbus_info;
}  SHARED_MSG_PACKED;

struct shared_firmwareupdate_msg_s {
    struct shared_canbus_info_s canbus_info;
    uint8_t source_node_id;
    char path[201];
} SHARED_MSG_PACKED;

enum shared_periph_cal_data_fmt_t {
    SHARED_PERIPH_CAL_DATA_FMT_NO_DATA = 0,
    SHARED_PERIPH_CAL_DATA_FMT_FLOAT = 1,
};

enum shared_periph_info_pin_function_t {
    SHARED_PERIPH_INFO_PIN_FUNCTION_UNKNOWN = 0,
    SHARED_PERIPH_INFO_PIN_FUNCTION_EXT_OSC,
    SHARED_PERIPH_INFO_PIN_FUNCTION_CAN1_RX,
    SHARED_PERIPH_INFO_PIN_FUNCTION_CAN1_TX,
    SHARED_PERIPH_INFO_PIN_FUNCTION_CAN2_RX,
    SHARED_PERIPH_INFO_PIN_FUNCTION_CAN2_TX,
    SHARED_PERIPH_INFO_PIN_FUNCTION_CAN3_RX,
    SHARED_PERIPH_INFO_PIN_FUNCTION_CAN3_TX,
    SHARED_PERIPH_INFO_PIN_FUNCTION_SPI1_MISO = 21,
    SHARED_PERIPH_INFO_PIN_FUNCTION_SPI1_MOSI,
    SHARED_PERIPH_INFO_PIN_FUNCTION_SPI1_SCLK,
    SHARED_PERIPH_INFO_PIN_FUNCTION_SPI1_NCS,
};

struct shared_onboard_periph_pin_info_s {
    uint16_t function;
    uint8_t port;
    uint8_t pin;
} SHARED_MSG_PACKED;

struct shared_onboard_periph_info_s {
    const char* const name;
    uint8_t rev;
    uint8_t bus_addr;
    uint8_t num_pin_descs;
    const struct shared_onboard_periph_pin_info_s* pin_descs;
    uint8_t cal_data_fmt; // < 127 reserved for standard calibration data formats defined by enum shared_periph_cal_data_fmt_t
    const void* const cal_data;
} SHARED_MSG_PACKED;

struct shared_hw_info_s {
    const char* const hw_name;
    uint8_t hw_major_version;
    uint8_t hw_minor_version;
    uint8_t onboard_periph_desc_fmt;
    uint8_t num_onboard_periph_descs;
    const struct shared_onboard_periph_info_s* const onboard_periph_descs;
} SHARED_MSG_PACKED;

struct shared_boot_info_msg_s {
    struct shared_canbus_info_s canbus_info;
    const struct shared_hw_info_s* hw_info;
} SHARED_MSG_PACKED;

union shared_msg_payload_u {
    struct shared_boot_msg_s boot_msg;
    struct shared_firmwareupdate_msg_s firmwareupdate_msg;
    struct shared_boot_info_msg_s boot_info_msg;
};

#define SHARED_APP_DESCRIPTOR_SIGNATURE "APDesc00"

struct shared_app_descriptor_s {
    char signature[8];
    uint64_t image_crc;
    uint32_t image_size;
    uint32_t vcs_commit;
    uint8_t major_version;
    uint8_t minor_version;
    uint8_t reserved[6];
} APP_DESCRIPTOR_ALIGNED_AND_PACKED;

bool shared_msg_check_and_retreive(enum shared_msg_t* msgid, union shared_msg_payload_u* msg_payload);
void shared_msg_finalize_and_write(enum shared_msg_t msgid, const union shared_msg_payload_u* msg_payload);
void shared_msg_clear(void);

const struct shared_app_descriptor_s* shared_find_app_descriptor(uint8_t* buf, uint32_t buf_len);

const struct shared_onboard_periph_info_s* shared_hwinfo_find_periph_info(const struct shared_hw_info_s* hw_info, const char* periph_name);
const struct shared_onboard_periph_pin_info_s* shared_hwinfo_find_periph_pin_info(const struct shared_onboard_periph_info_s* periph_info, uint16_t pin_function);

uint64_t shared_crc64_we(const uint8_t *buf, uint32_t len, uint64_t crc);