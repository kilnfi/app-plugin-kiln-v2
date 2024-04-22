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
 * otherwise set it to unkwown (UNKNOW_LR_STRATEGY)
 *
 * @param address: address to compare
 *
 * @returns index of the erc20 in the context or UNKNOW_LR_STRATEGY if not found
 */
int find_lr_known_erc20(const char address[ADDRESS_STR_LEN]) {
    for (size_t i = 0; i < LR_STRATEGIES_COUNT; i++) {
        if (compare_addresses(address, lr_erc20_addresses[i])) {
            return i;
        }
    }
    // if unknown erc20, indicate it
    return UNKNOW_LR_STRATEGY;
}

/*
 * If address is a known strategy, update lr display context with its name
 * otherwise set it to unkwown (UNKNOW_LR_STRATEGY)
 *
 * @param address: address to compare
 *
 * @returns index of the strategy in the context or UNKNOW_LR_STRATEGY if not found
 */
int find_lr_known_strategy(const char address[ADDRESS_STR_LEN]) {
    for (size_t i = 0; i < LR_STRATEGIES_COUNT; i++) {
        if (compare_addresses(address, lr_strategy_addresses[i])) {
            return i;
        }
    }
    // if unknown strategy, indicate it
    return UNKNOW_LR_STRATEGY;
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
            getEthDisplayableAddress(buffer, address_buffer, sizeof(address_buffer), 0);
            context->param_data.lr_deposit.strategy_to_display =
                find_lr_known_strategy(address_buffer);

            context->next_param = LR_DEPOSIT_INTO_STRATEGY_TOKEN;
            break;
        case LR_DEPOSIT_INTO_STRATEGY_TOKEN:
            copy_address(buffer, msg->parameter, sizeof(buffer));
            getEthDisplayableAddress(buffer, address_buffer, sizeof(address_buffer), 0);
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

void handle_lr_queue_withdrawals(ethPluginProvideParameter_t *msg, context_t *context) {
    // queuedWithdrawals = (address strategies[],uint256 shares[],address withdrawer)
    // queueWithdrawals(queuedWithdrawals[])
    // example for 2 queue withdrawals with 2 strategies each (2x2 dimension)
    // [  0] selector
    // [  4] queuedWithdrawals_offset
    // [ 36] queuedWithdrawals_length
    // [ 68] queuedWithdrawals_0_offset
    // [100] queuedWithdrawals_1_offset
    // [132] queuedWithdrawals_0
    //        [132] strategies_offset
    //        [164] shares_offset
    //        [196] withdrawer
    //        [228] strategies_length
    //        [260] strategies_0
    //        [292] strategies_1
    //        [324] shares_length
    //        [356] shares_0
    //        [388] shares_1
    // [388] queuedWithdrawals_1
    //        [388] strategies_offset
    //        [420] shares_offset
    //        [452] withdrawer
    //        [484] strategies_length
    //        [516] strategies_0
    //        [548] strategies_1
    //        [580] shares_length
    //        [612] shares_0
    //        [644] shares_1

    uint8_t buffer[ADDRESS_LENGTH];
    lr_queue_withdrawals_t *params = &context->param_data.lr_queue_withdrawals;

    switch (context->next_param) {
        // 1. queuedWithdrawals array setup
        case LR_QUEUE_WITHDRAWALS_QWITHDRAWALS_OFFSET:
            context->next_param = LR_QUEUE_WITHDRAWALS_QWITHDRAWALS_LENGTH;
            break;
        case LR_QUEUE_WITHDRAWALS_QWITHDRAWALS_LENGTH:
            U2BE_from_parameter(msg->parameter, &params->queued_withdrawals_count);
            params->current_item_count = params->queued_withdrawals_count;
            context->next_param = LR_QUEUE_WITHDRAWALS__QWITHDRAWALS_STRUCT_OFFSET;
            break;

        // 2. entering a queuedWithdrawal
        case LR_QUEUE_WITHDRAWALS__QWITHDRAWALS_STRUCT_OFFSET:
            // we skip all the queuewithdrawal structs offsets
            PRINTF("CURRENT ITEM COUNT: %d\n", params->current_item_count);
            if (params->current_item_count > 0) {
                params->current_item_count -= 1;
            }

            if (params->current_item_count == 0) {
                context->next_param = LR_QUEUE_WITHDRAWALS__QWITHDRAWALS_STRATEGIES_OFFSET;
            }
            break;
        case LR_QUEUE_WITHDRAWALS__QWITHDRAWALS_STRATEGIES_OFFSET:
            context->next_param = LR_QUEUE_WITHDRAWALS__QWITHDRAWALS_SHARES_OFFSET;
            break;
        case LR_QUEUE_WITHDRAWALS__QWITHDRAWALS_SHARES_OFFSET:
            if (params->withdrawer[0] == '\0') {
                context->next_param = LR_QUEUE_WITHDRAWALS__QWITHDRAWALS_WITHDRAWER;
            } else {
                context->next_param = LR_QUEUE_WITHDRAWALS__QWITHDRAWALS_STRATEGIES_LENGTH;
            }
            break;
        case LR_QUEUE_WITHDRAWALS__QWITHDRAWALS_WITHDRAWER:
            // EigenLayer contract does not allow withdrawer to be different than msg.sender
            // https://github.com/Layr-Labs/eigenlayer-contracts/blob/7229f2b426b6f2a24c7795
            // b1a4687a010eac8ef2/src/contracts/core/DelegationManager.sol#L275
            // so we can only copy it once
            if (params->withdrawer[0] == '\0') {
                copy_address(buffer, msg->parameter, sizeof(buffer));
                getEthDisplayableAddress(buffer, params->withdrawer, sizeof(params->withdrawer), 0);
            }
            context->next_param = LR_QUEUE_WITHDRAWALS__QWITHDRAWALS_STRATEGIES_LENGTH;
            break;
        case LR_QUEUE_WITHDRAWALS__QWITHDRAWALS_STRATEGIES_LENGTH:
            // get number of item to parse
            U2BE_from_parameter(msg->parameter, &params->current_item_count);

            context->next_param = LR_QUEUE_WITHDRAWALS__QWITHDRAWALS__STRATEGIES_ITEM;
            break;
        case LR_QUEUE_WITHDRAWALS__QWITHDRAWALS__STRATEGIES_ITEM:
            // get strategy we need to display
            copy_address(buffer, msg->parameter, sizeof(buffer));
            char address_buffer[ADDRESS_STR_LEN];
            getEthDisplayableAddress(buffer, address_buffer, sizeof(address_buffer), 0);

            int strategy_index = find_lr_known_strategy(address_buffer);
            params->strategies[params->strategies_count] =
                (strategy_index != UNKNOW_LR_STRATEGY) ? strategy_index + 1 : UNKNOW_LR_STRATEGY;

            PRINTF("STRATEGY #: %d STRATEGY: %d\n", params->strategies_count, strategy_index);
            // we just processed one strategy item
            params->strategies_count += 1;
            params->current_item_count -= 1;
            if (params->current_item_count == 0) {
                // when we arrive at the end of the strategies array we go to the shares array
                context->next_param = LR_QUEUE_WITHDRAWALS__QWITHDRAWALS_SHARES_LENGTH;
            }
            break;
        case LR_QUEUE_WITHDRAWALS__QWITHDRAWALS_SHARES_LENGTH:
            // get number of items to parse
            U2BE_from_parameter(msg->parameter, &params->current_item_count);

            context->next_param = LR_QUEUE_WITHDRAWALS__QWITHDRAWALS__SHARES_ITEM;
            break;
        case LR_QUEUE_WITHDRAWALS__QWITHDRAWALS__SHARES_ITEM:
            // we skip parsing shares item as they are not needed for clearsigning
            // as not having ETH / ERC20 amount to display would confuse users
            if (params->current_item_count > 0) {
                params->current_item_count -= 1;
            }
            if (params->current_item_count == 0) {
                // here we arrive at the end of the queuedWithdrawal array element

                // check if there are other queuedWithdrawals to parse
                params->queued_withdrawals_count -= 1;
                if (params->queued_withdrawals_count == 0) {
                    // if not we finished parsing
                    context->next_param = LR_QUEUE_WITHDRAWALS_UNEXPECTED_PARAMETER;
                } else {
                    // if there are other queuedWithdrawals we go back to parsing the
                    // next queueWithdrawal struct offset
                    context->next_param = LR_QUEUE_WITHDRAWALS__QWITHDRAWALS_STRUCT_OFFSET;
                }
            }
            break;

        default:
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

void handle_lr_delegate_to(ethPluginProvideParameter_t *msg, context_t *context) {
    // delegateTo(address,(bytes,uint256),bytes32)
    // example
    // [0] selector
    // [4] operator
    // [36] signature_offset
    // [68] approver_salt

    uint8_t buffer[ADDRESS_LENGTH];
    lr_delegate_to_t *params = &context->param_data.lr_delegate_to;

    switch (context->next_param) {
        case LR_DELEGATE_TO_OPERATOR:
            copy_address(buffer, msg->parameter, sizeof(buffer));
            getEthDisplayableAddress(buffer,
                                     params->operator_address,
                                     sizeof(params->operator_address),
                                     0);

            params->is_kiln = false;
            if (compare_addresses((const char *) buffer, lr_kiln_operator_address)) {
                params->is_kiln = true;
            }
            context->next_param = LR_DELEGATE_TO_SIGNATURE_OFFSET;
            break;
        case LR_DELEGATE_TO_SIGNATURE_OFFSET:
            context->next_param = LR_DELEGATE_TO_APPROVER_SALT;
            break;
        case LR_DELEGATE_TO_APPROVER_SALT:
            context->next_param = LR_DELEGATE_TO_UNEXPECTED_PARAMETER;
            break;
        default:
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
        case KILN_LR_QUEUE_WITHDRAWALS:
            handle_lr_queue_withdrawals(msg, context);
            break;
        case KILN_LR_COMPLETE_QUEUED_WITHDRAWAL:
            handle_lr_complete_queued_withdrawal(msg, context);
            break;
        case KILN_LR_DELEGATE_TO:
            handle_lr_delegate_to(msg, context);
            break;
        case KILN_LR_UNDELEGATE:
            msg->result = ETH_PLUGIN_RESULT_OK;
            break;

        default:
            PRINTF("Selector Index not supported: %d\n", context->selectorIndex);
            msg->result = ETH_PLUGIN_RESULT_ERROR;
            break;
    }
}
