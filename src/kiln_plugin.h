#pragma once

#include <string.h>

#include "eth_plugin_interface.h"

#define PLUGIN_NAME        "Kiln"
#define VALIDATOR_KEY_SIZE 48

// Available selectors:
//
// V1 selectors
// --- 0. deposit()
// --- 1. withdraw(validator_pubkey)
// --- 2. withdrawEL(validator_pubkey)
// --- 3. withdrawCL(validator_pubkey)
// --- 4. batchWithdraw(validator_pubkeys)
// --- 5. batchWithdrawEL(validator_pubkeys)
// --- 6. batchWithdrawCL(validator_pubkeys)
// --- 7. requestValidatorsExit(validator_pubkeys)
//
// V2 selectors
// --- 8. stake()
// --- 9. requestExit(shares_count)
// --- 10. multiClaim(exit_queues, ticket_ids, cask_ids)
// --- 11. claim(uint256[],uint32[],uint16)
//
// LR selectors
// --- 12. depositIntoStrategy(address,address,uint256)
// --- 13. queueWithdrawals((address[],uint256[],address)[])
// --- 14.
// completeQueuedWithdrawal((address,address,address,uint256,uint32,address[],uint256[]),address[],uint256,bool)
// --- 15. delegateTo(address,(bytes,uint256),bytes32)
// --- 16. undelegate(address)
//
#define NUM_SELECTORS 17

// Selectors available (see mapping above).
typedef enum {
    KILN_V1_DEPOSIT = 0,
    KILN_V1_WITHDRAW,
    KILN_V1_WITHDRAW_EL,
    KILN_V1_WITHDRAW_CL,
    KILN_V1_BATCH_WITHDRAW,
    KILN_V1_BATCH_WITHDRAW_EL,
    KILN_V1_BATCH_WITHDRAW_CL,
    KILN_V1_REQUEST_EXIT,
    KILN_V2_STAKE,
    KILN_V2_REQUEST_EXIT,
    KILN_V2_MULTICLAIM,
    KILN_V2_CLAIM,
    KILN_LR_DEPOSIT_INTO_STRATEGY,
    KILN_LR_QUEUE_WITHDRAWALS,
    KILN_LR_COMPLETE_QUEUED_WITHDRAWALS,
    KILN_LR_DELEGATE_TO,
    KILN_LR_UNDELEGATE,
} selector_t;

extern const uint32_t KILN_SELECTORS[NUM_SELECTORS];

// ADDRESS_STR_LEN is 0x + addr + \0
#define ADDRESS_STR_LEN 43

// All supported ERC20 tokens have 18 decimals on mainnet.
#define ERC20_DECIMALS 18

// Parameters for deposit selector.
typedef enum {
    DEPOSIT_UNEXPECTED_PARAMETER,
} deposit_parameters;

// Parameters for LR deposit into strategy selector.
typedef enum {
    LR_DEPOSIT_INTO_STRATEGY_STRATEGY = 0,
    LR_DEPOSIT_INTO_STRATEGY_TOKEN,
    LR_DEPOSIT_INTO_STRATEGY_AMOUNT,
    LR_DEPOSIT_INTO_STRATEGY_UNEXPECTED_PARAMETER,
} lr_deposit_into_strategy_parameters;

// Parameters for LR queue withdrawals selector.
typedef enum {
    LR_QUEUE_WITHDRAWALS_QWITHDRAWALS_OFFSET = 0,
    LR_QUEUE_WITHDRAWALS_QWITHDRAWALS_LENGTH,
    LR_QUEUE_WITHDRAWALS__QWITHDRAWALS_STRUCT_OFFSET,
    LR_QUEUE_WITHDRAWALS__QWITHDRAWALS_STRATEGIES_OFFSET,
    LR_QUEUE_WITHDRAWALS__QWITHDRAWALS_SHARES_OFFSET,
    LR_QUEUE_WITHDRAWALS__QWITHDRAWALS_WITHDRAWER,
    LR_QUEUE_WITHDRAWALS__QWITHDRAWALS_STRATEGIES_LENGTH,
    LR_QUEUE_WITHDRAWALS__QWITHDRAWALS__STRATEGIES_ITEM,
    LR_QUEUE_WITHDRAWALS__QWITHDRAWALS_SHARES_LENGTH,
    LR_QUEUE_WITHDRAWALS__QWITHDRAWALS__SHARES_ITEM,
    LR_QUEUE_WITHDRAWALS_UNEXPECTED_PARAMETER
} lr_queue_withdrawals_parameters;

// Parameters for LR complete queued withdrawals selector.
typedef enum {
    LRCQW_WITHDRAWALS_OFFSET = 0,
    LRCQW_TOKENS_OFFSET,
    LRCQW_MIDDLEWARE_TIMES_INDEXES_OFFSET,
    LRCQW_RECEIVE_AS_TOKENS_OFFSET,

    LRCQW_WITHDRAWALS_LENGTH,
    LRCQW_WITHDRAWALS__OFFSET_ITEMS,

    LRCQW_WITHDRAWALS__ITEM__STAKER,
    LRCQW_WITHDRAWALS__ITEM__DELEGATED_TO,
    LRCQW_WITHDRAWALS__ITEM__WITHDRAWER,
    LRCQW_WITHDRAWALS__ITEM__NONCE,
    LRCQW_WITHDRAWALS__ITEM__START_BLOCK,
    LRCQW_WITHDRAWALS__ITEM__STRATEGIES_OFFSET,
    LRCQW_WITHDRAWALS__ITEM__SHARES_OFFSET,
    LRCQW_WITHDRAWALS__ITEM__STRATEGIES_LENGTH,
    LRCQW_WITHDRAWALS__ITEM__STRATEGIES__ITEMS,
    LRCQW_WITHDRAWALS__ITEM__SHARES_LENGTH,
    LRCQW_WITHDRAWALS__ITEM__SHARES__ITEMS,

    LRCQW_TOKENS_LENGTH,
    LRCQW_TOKENS__OFFSET_ITEMS,

    LRCQW_TOKENS__ITEM__LENGTH,
    LRCQW_TOKENS__ITEM__ITEMS,

    LRCQW_MIDDLEWARE_TIMES_INDEXES_LENGTH,
    LRCQW_MIDDLEWARE_TIMES_INDEXES__ITEMS,

    LRCQW_RECEIVE_AS_TOKENS_LENGTH,
    LRCQW_RECEIVE_AS_TOKENS__ITEMS,

    LRCQW_UNEXPECTED_PARAMETER

} lr_complete_queued_withdrawals_parameters;

// Parameters for LR delegate to selector.
typedef enum {
    LR_DELEGATE_TO_OPERATOR = 0,
    LR_DELEGATE_TO_SIGNATURE_OFFSET,
    LR_DELEGATE_TO_APPROVER_SALT,
    LR_DELEGATE_TO_UNEXPECTED_PARAMETER
} lr_delegate_to_parameters;

#define LR_STRATEGIES_COUNT 11

extern const char lr_strategy_addresses[LR_STRATEGIES_COUNT][ADDRESS_STR_LEN];
extern const char lr_erc20_addresses[LR_STRATEGIES_COUNT][ADDRESS_STR_LEN];
extern const char lr_tickers[LR_STRATEGIES_COUNT][MAX_TICKER_LEN];
extern const char lr_kiln_operator_address[ADDRESS_STR_LEN];

// max number of strategies / erc20 to display
#define MAX_DISPLAY_COUNT 3
#define SELECTOR_LENGTH   4

typedef struct {
    int strategy_to_display;
    int erc20_to_display;
    uint8_t erc20_amount_to_display[INT256_LENGTH];
} lr_deposit_t;

typedef struct {
    char operator_address[ADDRESS_STR_LEN];
    bool is_kiln;
} lr_delegate_to_t;

#define UNKNOW_LR_STRATEGY                  255
#define MAX_DISPLAYABLE_LR_STRATEGIES_COUNT (LR_STRATEGIES_COUNT * 3)

typedef struct {
    //  -- utils
    uint16_t queued_withdrawals_count;
    uint16_t current_item_count;

    // -- display
    uint8_t strategies_count;
    char withdrawer[ADDRESS_STR_LEN];
    // list of strategies indexes **INCREMENTED BY 1** to display in the UI
    // 0 is reserved for end of array
    // UNKNOW_LR_STRATEGY is used to display the "unknown" strategy
    // (i) in practice, we should not encounter more than
    //      LR_STRATEGIES_COUNT +~ a few potential unsupported
    //      strategies in the plugin. So * 3 should be a good enough buffer.
    // (ii) in practice there should not be more than (2 ** 8) - 2 known strategies
    uint8_t strategies[MAX_DISPLAYABLE_LR_STRATEGIES_COUNT];
} lr_queue_withdrawals_t;

typedef struct {
    // -- utils
    uint16_t parent_item_count;
    uint16_t current_item_count;
    uint16_t relegations_count;

    // -- display
    uint16_t strategies_count;
    char withdrawer[ADDRESS_STR_LEN];
    // list of strategies indexes **INCREMENTED BY 1** to display in the UI
    // 0 is reserved for end of array
    // UNKNOW_LR_STRATEGY is used to display the "unknown" strategy
    // (i) in practice, we should not encounter more than
    //      LR_STRATEGIES_COUNT +~ a few potential unsupported
    //      strategies in the plugin. So * 3 should be a good enough buffer.
    // (ii) in practice there should not be more than (2 ** 8) - 2 known strategies
    uint8_t strategies[MAX_DISPLAYABLE_LR_STRATEGIES_COUNT];
    // follows the indexes of the strategies array in this structure
    bool is_redelegated[MAX_DISPLAYABLE_LR_STRATEGIES_COUNT];
} lr_complete_queued_withdrawals_t;

typedef struct context_t {
    uint8_t next_param;

    union {
        lr_deposit_t lr_deposit;
        lr_delegate_to_t lr_delegate_to;
        lr_queue_withdrawals_t lr_queue_withdrawals;
        lr_complete_queued_withdrawals_t lr_complete_queued_withdrawals;
    } param_data;

    selector_t selectorIndex;
} context_t;

_Static_assert(sizeof(context_t) <= 5 * 32, "Structure of parameters too big.");
