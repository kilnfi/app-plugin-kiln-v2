#include "kiln_plugin.h"
#include <ctype.h>

/*
 * Compare two addresses
 *
 * @param a: first address
 * @param b: second address
 *
 * @return true if the addresses are the same
 */
bool compare_addresses(const char a[ADDRESS_STR_LEN], const char b[ADDRESS_STR_LEN]) {
    for (size_t i = 0; i < ADDRESS_STR_LEN; i += 1) {
        if (tolower((unsigned char) a[i]) != tolower((unsigned char) b[i])) {
            return false;
        }
    }
    return true;
}

/*
 * If address is a known erc20, update lr display context with its name
 * otherwise set it to unkwown (-1)
 *
 * @param address: address to compare
 *
 * @returns index of the erc20 in the context or -1 if not found
 */
int find_lr_known_erc20(const char address[ADDRESS_STR_LEN]) {
    for (size_t i = 0; i < LR_STRATEGIES_COUNT; i++) {
        if (compare_addresses(address, lr_erc20_addresses[i])) {
            return i;
        }
    }
    // if unknown erc20, indicate it
    return -1;
}

/*
 * If address is a known strategy, update lr display context with its name
 * otherwise set it to unkwown (-1)
 *
 * @param address: address to compare
 *
 * @returns index of the strategy in the context or -1 if not found
 */
int find_lr_known_strategy(const char address[ADDRESS_STR_LEN]) {
    for (size_t i = 0; i < LR_STRATEGIES_COUNT; i++) {
        if (compare_addresses(address, lr_strategy_addresses[i])) {
            return i;
        }
    }
    // if unknown strategy, indicate it
    return -1;
}

/*
 * Handle the parameters for the depositIntoStrategy(strategy,erc20,amount)
 * selector
 *
 * @param msg: message containing the parameter
 * @param context: context to update
 *
 */
void handle_lr_deposit_into_strategy(ethPluginProvideParameter_t *msg, context_t *context) {
    uint8_t buffer[ADDRESS_LENGTH];
    char address_buffer[ADDRESS_STR_LEN];

    switch (context->next_param) {
        case LR_DEPOSIT_INTO_STRATEGY_STRATEGY:
            copy_address(buffer, msg->parameter, sizeof(buffer));
            getEthDisplayableAddress(buffer,
                                     address_buffer,
                                     sizeof(address_buffer),
                                     msg->pluginSharedRW->sha3,
                                     0);
            context->param_data.lr_deposit.strategy_to_display =
                find_lr_known_strategy(address_buffer);

            context->next_param = LR_DEPOSIT_INTO_STRATEGY_TOKEN;
            break;
        case LR_DEPOSIT_INTO_STRATEGY_TOKEN:
            copy_address(buffer, msg->parameter, sizeof(buffer));
            getEthDisplayableAddress(buffer,
                                     address_buffer,
                                     sizeof(address_buffer),
                                     msg->pluginSharedRW->sha3,
                                     0);
            context->param_data.lr_deposit.erc20_to_display = find_lr_known_erc20(address_buffer);

            context->next_param = LR_DEPOSIT_INTO_STRATEGY_AMOUNT;
            break;
        case LR_DEPOSIT_INTO_STRATEGY_AMOUNT:
            copy_parameter(context->param_data.lr_deposit.erc20_amount_to_display,
                           msg->parameter,
                           sizeof(context->param_data.lr_deposit.erc20_amount_to_display));
            context->next_param = LR_DEPOSIT_INTO_STRATEGY_UNEXPECTED_PARAMETER;
            break;
        default:
            PRINTF("Param not supported: %d\n", context->next_param);
            msg->result = ETH_PLUGIN_RESULT_ERROR;
            return;
    }
    msg->result = ETH_PLUGIN_RESULT_OK;
}

void handle_lr_queue_withdrawal(ethPluginProvideParameter_t *msg, context_t *context) {
    // queueWithdrawal(uint256[],address[],uint256[],address,bool
    // example for queue withdrawal with 2 strategies indexes, contracts and shares
    // [0] selector
    // [4] strat_i_offset -- starting here
    // [36] strat_offset
    // [68] shares_offset
    // [100] withdrawal
    // [132] undelegateIfPossible
    // [164] strat_i_length
    // [196] strat_i[0]
    // [228] strat_i[1]
    // [260] strat_length
    // [292] strat[0]
    // [324] strat[1]
    // [356] shares_length
    // [388] shares[0]
    // [420] shares[1]

    uint8_t buffer[ADDRESS_LENGTH];
    lr_queue_withdrawal_t *params = &context->param_data.lr_queue_withdrawal;

    if (params->skip_offset != 0 && params->go_to_offset == false &&
        msg->parameterOffset == params->skip_offset + SELECTOR_LENGTH) {
        // if we reach offset, we get the size of the array and skip parsing it
        // in the condition of the default switch case

        // with the example above:
        // before:
        // [356] shares_length -- skip_offset
        // [388] shares[0]
        // [420] shares[1]
        params->skip_offset +=
            U2BE(msg->parameter, PARAMETER_LENGTH - sizeof(params->skip_offset)) * PARAMETER_LENGTH;
        // after:
        // [356] shares_length
        // [388] shares[0]
        // [420] shares[1] -- skip_offset
        params->go_to_offset = true;
    }

    switch (context->next_param) {
        case LR_QUEUE_WITHDRAWAL_STRATEGY_INDEXES_OFFSET:
            params->go_to_offset = false;
            context->next_param = LR_QUEUE_WITHDRAWAL_STRATEGIES_OFFSET;
            break;
        case LR_QUEUE_WITHDRAWAL_STRATEGIES_OFFSET:
            context->next_param = LR_QUEUE_WITHDRAWAL_SHARES_OFFSET;
            break;
        case LR_QUEUE_WITHDRAWAL_SHARES_OFFSET:
            // before:
            // skip_offset = 0
            params->skip_offset =
                U2BE(msg->parameter, PARAMETER_LENGTH - sizeof(params->skip_offset));
            // after:
            // [356] shares_length -- skip_offset
            // [388] shares[0]
            // [420] shares[1]
            context->next_param = LR_QUEUE_WITHDRAWAL_WITHDRAWER;
            break;
        case LR_QUEUE_WITHDRAWAL_WITHDRAWER:
            copy_address(buffer, msg->parameter, sizeof(buffer));
            getEthDisplayableAddress(buffer,
                                     context->param_data.lr_queue_withdrawal.withdrawer,
                                     sizeof(context->param_data.lr_queue_withdrawal.withdrawer),
                                     msg->pluginSharedRW->sha3,
                                     0);
            context->next_param = LR_QUEUE_WITHDRAWAL_UNDELEGATEIFPOSSIBLE;
            break;
        case LR_QUEUE_WITHDRAWAL_UNDELEGATEIFPOSSIBLE:
            context->next_param = LR_QUEUE_WITHDRAWAL_UNEXPECTED_PARAMETER;
            break;
        default:
            if (msg->parameterOffset <= params->skip_offset + SELECTOR_LENGTH) {
                // as we don't want to display the strategy indexes, strategies and shares amount
                // we skip parsing them until skip_offset is reached
                msg->result = ETH_PLUGIN_RESULT_OK;
                return;
            }
            PRINTF("Param not supported: %d\n", context->next_param);
            msg->result = ETH_PLUGIN_RESULT_ERROR;
            return;
    }
    msg->result = ETH_PLUGIN_RESULT_OK;
}

void handle_lr_complete_queued_withdrawal(ethPluginProvideParameter_t *msg, context_t *context) {
    // completeQueuedWithdrawal((address[],uint256[],address,(address,uint96),uint32,address),address[],uint256,bool)
    // example for complete queued withdrawal with 2 tokens
    // [0] selector
    // [4] queuedWithdrawal_offset -- starting here
    // [36] withdrawal_struct_offset
    // [68] token_list_offset
    // [100] middleware_time_index
    // [132] receive_as_tokens
    // [164] withdrawal_struct_offset_length
    // [...] withdrawal_struct elements
    // [...] withdrawal_struct elements
    // [164 + nb_struct_elem * 32] token_list_length
    // [...] token_list elements
    // [...] token_list elements

    lr_complete_queued_withdrawal_t *params = &context->param_data.lr_complete_queued_withdrawal;

    if (params->skip_offset != 0 && params->go_to_offset == false &&
        msg->parameterOffset == params->skip_offset + SELECTOR_LENGTH) {
        // if we reach offset, we get the size of the array and skip parsing it
        // in the condition of the default switch case

        // with the example above:
        // before:
        // [164 + nb_struct_elem * 32] token_list_length -- skip_offset
        // [...] token_list elements
        // [...] token_list elements
        params->skip_offset +=
            U2BE(msg->parameter, PARAMETER_LENGTH - sizeof(params->skip_offset)) * PARAMETER_LENGTH;
        // after:
        // [164 + nb_struct_elem * 32] token_list_length
        // [...] token_list elements
        // [...] token_list elements -- skip_offset

        params->go_to_offset = true;
    }

    switch (context->next_param) {
        case LR_COMPLETE_QUEUED_WITHDRAWAL_QUEUEDWITHDRAWAL_OFFSET:
            context->next_param = LR_COMPLETE_QUEUED_WITHDRAWAL_TOKENS_OFFSET;
            break;
        case LR_COMPLETE_QUEUED_WITHDRAWAL_TOKENS_OFFSET:
            // before:
            // skip_offset = 0
            params->skip_offset =
                U2BE(msg->parameter, PARAMETER_LENGTH - sizeof(params->skip_offset));
            // after:
            // [164 + nb_struct_elem * 32] token_list_length -- skip_offset
            // [...] token_list elements
            // [...] token_list elements
            context->next_param = LR_COMPLETE_QUEUED_WITHDRAWAL_MIDDLEWARETIMEINDEX;
            break;
        case LR_COMPLETE_QUEUED_WITHDRAWAL_MIDDLEWARETIMEINDEX:
            context->next_param = LR_COMPLETE_QUEUED_WITHDRAWAL_RECEIVEASTOKENS;
            break;
        case LR_COMPLETE_QUEUED_WITHDRAWAL_RECEIVEASTOKENS:
            context->next_param = LR_COMPLETE_QUEUED_WITHDRAWAL_UNEXPECTED_PARAMETER;
            break;
        default:
            if (msg->parameterOffset <= params->skip_offset + SELECTOR_LENGTH) {
                // as we don't want to display withdrawals structures
                // we skip parsing them until skip_offset is reached
                msg->result = ETH_PLUGIN_RESULT_OK;
                return;
            }
            PRINTF("Param not supported: %d\n", context->next_param);
            msg->result = ETH_PLUGIN_RESULT_ERROR;
            return;
    }
    msg->result = ETH_PLUGIN_RESULT_OK;
}

void handle_provide_parameter(ethPluginProvideParameter_t *msg) {
    context_t *context = (context_t *) msg->pluginContext;

    PRINTF("plugin provide parameter: offset %d\nBytes: %.*H\n",
           msg->parameterOffset,
           PARAMETER_LENGTH,
           msg->parameter);

    switch (context->selectorIndex) {
        case KILN_V1_DEPOSIT:
            msg->result = ETH_PLUGIN_RESULT_ERROR;
            break;

        case KILN_V1_WITHDRAW:
        case KILN_V1_WITHDRAW_EL:
        case KILN_V1_WITHDRAW_CL:
            msg->result = ETH_PLUGIN_RESULT_OK;
            break;
        case KILN_V1_BATCH_WITHDRAW:
        case KILN_V1_BATCH_WITHDRAW_EL:
        case KILN_V1_BATCH_WITHDRAW_CL:
            msg->result = ETH_PLUGIN_RESULT_OK;
            break;

        case KILN_V1_REQUEST_EXIT:
            msg->result = ETH_PLUGIN_RESULT_OK;
            break;

        case KILN_V2_STAKE:
            msg->result = ETH_PLUGIN_RESULT_ERROR;
            break;
        case KILN_V2_REQUEST_EXIT:
        case KILN_V2_MULTICLAIM:
        case KILN_V2_CLAIM:
            msg->result = ETH_PLUGIN_RESULT_OK;
            break;

        case KILN_LR_DEPOSIT_INTO_STRATEGY:
            handle_lr_deposit_into_strategy(msg, context);
            break;
        case KILN_LR_QUEUE_WITHDRAWAL:
            handle_lr_queue_withdrawal(msg, context);
            break;
        case KILN_LR_COMPLETE_QUEUED_WITHDRAWAL:
            handle_lr_complete_queued_withdrawal(msg, context);
            break;

        default:
            PRINTF("Selector Index not supported: %d\n", context->selectorIndex);
            msg->result = ETH_PLUGIN_RESULT_ERROR;
            break;
    }
}
