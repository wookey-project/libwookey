#ifndef WOOKEY_IPC_H_
#define WOOKEY_IPC_H_

/* IPC based synchronization toolkit */

#include "libc/types.h"

/*
 * generic synchronization specific binary IPC API
 */

/*
 *  ipc_command:
 *
 *  |----|----|
 *  |MGC |STA |
 *  |----|----|
 *
 *  ipc_command_data:
 *
 *  |----|----|----|----|
 *  |MGC |STA |   SIZE  |
 *  |----|----|----|----|
 *  |DATA|DATA|DATA|DATA|
 *  |----|----|----|----|
 *  |DATA|DATA|DATA|DATA|
 *  |----|----|----|----|
 *  |DATA|DATA|DATA|DATA|
 *  |----|----|----|----|
 *  |DATA|DATA|DATA|DATA|
 *  |----|----|----|----|
 *  |DATA|DATA|DATA|DATA|
 *  |----|----|----|----|
 *  |DATA|DATA|DATA|DATA|
 *  |----|----|----|----|
 *  |DATA|DATA|DATA|DATA|
 *  |----|----|----|----|
 *  |DATA|DATA|DATA|DATA|
 *  |----|----|----|----|
 *
 * data is a union between:
 * 32 uint8_t,
 * 16 uint16_t,
 * 8 uint32_t,
 * t_sc_request structure
 *
 */


enum sync_magic {
    /* Dataplane read & write requests */
    MAGIC_DATA_WR_DMA_REQ              = 0x01,
    MAGIC_DATA_WR_DMA_ACK              = 0x02,
    MAGIC_DATA_RD_DMA_REQ              = 0x03,
    MAGIC_DATA_RD_DMA_ACK              = 0x04,
    /** task state request command and response */
    MAGIC_TASK_STATE_CMD               = 0x42,
    MAGIC_TASK_STATE_RESP              = 0x43,
    /** cryptography interaction command and response */
    MAGIC_CRYPTO_INJECT_CMD            = 0x52,
    MAGIC_CRYPTO_INJECT_RESP           = 0x53,
    /** pin and pet name interaction command and response */
    MAGIC_CRYPTO_PIN_CMD               = 0x62,
    MAGIC_CRYPTO_PIN_RESP              = 0x63,
    /** is authentication sequence passed ? */
    MAGIC_AUTH_STATE_PASSED            = 0x64,
    /** DMA 'buffer ready' command and response */
    MAGIC_DMA_SHM_INFO_CMD             = 0x70,
    MAGIC_DMA_SHM_INFO_RESP            = 0x71,
    MAGIC_DMA_BUF_READY_CMD            = 0x72,
    MAGIC_DMA_BUF_READY_RESP           = 0x73,
    /** USB vs storage synchronization control plane */
    MAGIC_STORAGE_SCSI_BLOCK_SIZE_CMD  = 0x82,
    MAGIC_STORAGE_SCSI_BLOCK_SIZE_RESP = 0x83,
    MAGIC_STORAGE_SCSI_BLOCK_NUM_CMD   = 0x84,
    MAGIC_STORAGE_SCSI_BLOCK_NUM_RESP  = 0x85,
    MAGIC_STORAGE_EJECTED              = 0x86,
    /** user menu settigs requests magics */
    MAGIC_SETTINGS_CMD                 = 0x90,
    MAGIC_SETTINGS_LOCK                = 0x91,
    /** DFU metadata exchange */
    MAGIC_DFU_DWNLOAD_STARTED          = 0xa0,
    MAGIC_DFU_HEADER_SEND              = 0xa1,
    MAGIC_DFU_HEADER_VALID             = 0xa2,
    MAGIC_DFU_HEADER_INVALID           = 0xa3,
    MAGIC_DFU_DWNLOAD_FINISHED         = 0xa4,
    MAGIC_DFU_WRITE_FINISHED           = 0xa5,
    MAGIC_DFU_GET_FW_VERSION           = 0xa6,
    MAGIC_REBOOT_REQUEST               = 0xb0,
    /* informational transmission */
    MAGIC_INFORMATIONAL_MSG            = 0xc0,
    /** finishing with invalid */
    MAGIC_INVALID                      = 0xff,
};

enum sync_init_state {
    SYNC_READY        = 0,
    SYNC_ASK_FOR_DATA = 1,
    SYNC_WAIT         = 2,
    SYNC_DONE         = 3,
    SYNC_ACKNOWLEDGE  = 4,
    SYNC_UNKNOWN      = 5,
    SYNC_BADFILE      = 6,
    SYNC_FAILURE      = 7
};

/* Smart vs Pin structure, mapped in data.u8 */
enum sc_field_type {
    SC_USER_PIN = 1,
    SC_PET_PIN  = 2,
    SC_PET_NAME = 3
};

enum sc_field_request {
    SC_REQ_AUTHENTICATE = 1,
    SC_REQ_MODIFY       = 2
};

typedef struct  __attribute__((packed)) {
    enum sc_field_type sc_type;
    enum sc_field_request sc_req;
    char sc_petname[24];
} t_sc_request;

/**/
union data_block {
    char     c[64];
    uint8_t  u8[64];
    uint16_t u16[32];
    uint32_t u32[16];
    uint64_t u64[8];
    t_sc_request req;
};

struct sync_command {
    uint8_t magic;
    uint8_t state;
} __attribute__((packed));

struct sync_command_data {
    uint8_t          magic;
    uint8_t          state;
    uint16_t          data_size;
    union data_block data;
} __attribute__((packed));

/*
 * Dataplane specific IPC commands
 * This define the SCSI based communication protocol for requiring read/write
 * commands between tasks
 */

struct dataplane_command {
    uint8_t  magic;
    uint8_t  state;
    uint32_t sector_address;
    uint32_t num_sectors;
};

typedef union {
    struct  dataplane_command dataplane_cmd;
    struct  sync_command      sync_cmd;
    struct  sync_command_data sync_cmd_data;
    uint8_t                   magic; // first field of the two above
} t_ipc_command;

#endif /*! WOOKEY_IPC_H_*/
