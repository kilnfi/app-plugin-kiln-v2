/*******************************************************************************
 *
 * ██╗  ██╗██╗██╗     ███╗   ██╗
 * ██║ ██╔╝██║██║     ████╗  ██║
 * █████╔╝ ██║██║     ██╔██╗ ██║
 * ██╔═██╗ ██║██║     ██║╚██╗██║
 * ██║  ██╗██║███████╗██║ ╚████║
 * ╚═╝  ╚═╝╚═╝╚══════╝╚═╝  ╚═══╝
 *
 * Kiln Ethereum Ledger App
 * (c) 2022-2024 Kiln
 *
 * contact@kiln.fi
 ********************************************************************************/

#include "provide_parameter.h"

/**
 * @brief Compare two addresses
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

/**
 * @brief If address is a known erc20, update lr display context with its name
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

/**
 * @brief If address is a known strategy, update lr display context with its name
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

/**
 * @brief Handle the parameters for the depositIntoStrategy(strategy,erc20,amount)
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

/**
 * @brief Handle the parameters for the queueWithdrawals(queuedWithdrawals[]) selector
 *
 * @param msg: message containing the parameter
 * @param context: context to update
 *
 */
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
            context->next_param = LR_QUEUE_WITHDRAWALS__QWITHDRAWALS_WITHDRAWER;
            break;
        case LR_QUEUE_WITHDRAWALS__QWITHDRAWALS_WITHDRAWER:
            // EigenLayer contract does not allow withdrawer to be different than msg.sender
            // https://github.com/Layr-Labs/eigenlayer-contracts/blob/7229f2b426b6f2a24c7795
            // b1a4687a010eac8ef2/src/contracts/core/DelegationManager.sol#L275
            // so we can only copy it once
            if (params->withdrawer[0] == '\0') {
                uint8_t buffer[ADDRESS_LENGTH];
                copy_address(buffer, msg->parameter, sizeof(buffer));
                getEthDisplayableAddress(buffer, params->withdrawer, sizeof(params->withdrawer), 0);
            }
            PRINTF("WITHDRAWER: %s\n", params->withdrawer);
            context->next_param = LR_QUEUE_WITHDRAWALS__QWITHDRAWALS_STRATEGIES_LENGTH;
            break;
        case LR_QUEUE_WITHDRAWALS__QWITHDRAWALS_STRATEGIES_LENGTH:
            // get number of item to parse
            U2BE_from_parameter(msg->parameter, &params->current_item_count);
            PRINTF("STRATEGIES LENGTH: %d\n", params->current_item_count);

            if (params->current_item_count == 0) {
                // if no strategies we go to the shares array
                context->next_param = LR_QUEUE_WITHDRAWALS__QWITHDRAWALS_SHARES_LENGTH;
            } else {
                context->next_param = LR_QUEUE_WITHDRAWALS__QWITHDRAWALS__STRATEGIES_ITEM;
            }
            break;
        case LR_QUEUE_WITHDRAWALS__QWITHDRAWALS__STRATEGIES_ITEM: {
            // get strategy we need to display
            {
                uint8_t buffer[ADDRESS_LENGTH];
                copy_address(buffer, msg->parameter, sizeof(buffer));
                char address_buffer[ADDRESS_STR_LEN];
                getEthDisplayableAddress(buffer, address_buffer, sizeof(address_buffer), 0);

                int strategy_index = find_lr_known_strategy(address_buffer);
                params->strategies[params->strategies_count] =
                    (strategy_index != UNKNOW_LR_STRATEGY) ? strategy_index + 1
                                                           : UNKNOW_LR_STRATEGY;

                PRINTF("STRATEGY #: %d STRATEGY: %d\n", params->strategies_count, strategy_index);
            }

            // we just processed one strategy item
            params->strategies_count += 1;
            params->current_item_count -= 1;
            if (params->current_item_count == 0) {
                // when we arrive at the end of the strategies array we go to the shares array
                context->next_param = LR_QUEUE_WITHDRAWALS__QWITHDRAWALS_SHARES_LENGTH;
            }
            break;
        }
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
                    // next queueWithdrawal struct
                    context->next_param = LR_QUEUE_WITHDRAWALS__QWITHDRAWALS_STRATEGIES_OFFSET;
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

/**
 * @brief Handle the parameters for the completeQueuedWithdrawals(Withdrawal[] withdrawals,
 * address[][] tokens, uint256[] middlewareTimesIndexes, bool[] receiveAsTokens) selector
 * @param msg: message containing the parameter
 * @param context: context to update
 *
 */
void handle_lr_complete_queued_withdrawals(ethPluginProvideParameter_t *msg, context_t *context) {
    // **************************************************************************
    // FUNCTION TO PARSE
    // **************************************************************************
    //
    //  struct Withdrawal {
    // 	    address staker;
    // 	    address delegatedTo;
    // 	    address withdrawer;
    // 	    uint256 nonce;
    // 	    uint32 startBlock;
    // 	    address[] strategies;
    // 	    uint256[] shares;
    // }
    //
    // function completeQueuedWithdrawals(
    // 	    Withdrawal[] calldata withdrawals,
    // 	    address[][] calldata tokens,
    // 	    uint256[] calldata middlewareTimesIndexes,
    // 	    bool[] calldata receiveAsTokens
    // ) external
    //
    // **************************************************************************
    // example for 2 complete queued withdrawals with 2 tokens each
    // [  0] selector
    // [  4] withdrawals_offset
    // [ 36] tokens_offset
    // [ 68] middlewareTimesIndexes_offset
    // [100] receiveAsTokens_offset
    // [132] withdrawals_length
    // [164] withdrawals_0_offset
    // [196] withdrawals_1_offset
    // [228] withdrawals_0
    //        [228] staker
    //        [260] delegatedTo
    //        [292] withdrawer
    //        [324] nonce
    //        [356] startBlock
    //        [388] strategies_offset
    //        [420] shares_offset
    //        [452] strategies_length
    //        [484] strategies_0
    //        [516] strategies_1
    //        [548] shares_length
    //        [580] shares_0
    //        [612] shares_1
    // [644] withdrawals_1
    //        [644] staker
    //        [676] delegatedTo
    //        [708] withdrawer
    //        [740] nonce
    //        [772] startBlock
    //        [804] strategies_offset
    //        [836] shares_offset
    //        [868] strategies_length
    //        [900] strategies_0
    //        [932] strategies_1
    //        [964] shares_length
    //        [996] shares_0
    //        [1028] shares_1
    // [1060] tokens_length
    // [1092] tokens_0_offset
    // [1124] tokens_1_offset
    // [1156] tokens_0
    //        [1156] tokens_0_length
    //        [1188] tokens_0_0
    //        [1220] tokens_0_1
    // [1252] tokens_1
    //        [1252] tokens_1_length
    //        [1284] tokens_1_0
    //        [1316] tokens_1_1
    // [1348] middlewareTimesIndexes_length
    // [1380] middlewareTimesIndexes_0
    // [1412] middlewareTimesIndexes_1
    // [1444] receiveAsTokens_length
    // [1476] receiveAsTokens_0
    // [1508] receiveAsTokens_1
    lr_complete_queued_withdrawals_t *params = &context->param_data.lr_complete_queued_withdrawals;

    switch (context->next_param) {
        // ********************************************************************
        // FUNCTION PARAMETERS PARSING
        // ********************************************************************
        case LRCQW_WITHDRAWALS_OFFSET:
            context->next_param = LRCQW_TOKENS_OFFSET;
            break;
        case LRCQW_TOKENS_OFFSET:
            context->next_param = LRCQW_MIDDLEWARE_TIMES_INDEXES_OFFSET;
            break;
        case LRCQW_MIDDLEWARE_TIMES_INDEXES_OFFSET:
            context->next_param = LRCQW_RECEIVE_AS_TOKENS_OFFSET;
            break;
        case LRCQW_RECEIVE_AS_TOKENS_OFFSET:
            context->next_param = LRCQW_WITHDRAWALS_LENGTH;
            break;

        // ********************************************************************
        // WITHDRAWAL[] PARSING
        // ********************************************************************
        case LRCQW_WITHDRAWALS_LENGTH:

            U2BE_from_parameter(msg->parameter, &params->parent_item_count);
            params->current_item_count = params->parent_item_count;
            PRINTF("LRCQW_WITHDRAWALS_LENGTH: %d\n", params->parent_item_count);

            if (params->parent_item_count == 0) {
                context->next_param = LRCQW_TOKENS_LENGTH;
            } else {
                context->next_param = LRCQW_WITHDRAWALS__OFFSET_ITEMS;
            }
            break;
        case LRCQW_WITHDRAWALS__OFFSET_ITEMS:

            params->current_item_count -= 1;
            if (params->current_item_count == 0) {
                context->next_param = LRCQW_WITHDRAWALS__ITEM__STAKER;
            }
            break;

        // ********************************************************************
        // WITHDRAWAL STRUCT PARSING
        // ********************************************************************
        case LRCQW_WITHDRAWALS__ITEM__STAKER:
            context->next_param = LRCQW_WITHDRAWALS__ITEM__DELEGATED_TO;
            break;
        case LRCQW_WITHDRAWALS__ITEM__DELEGATED_TO:
            context->next_param = LRCQW_WITHDRAWALS__ITEM__WITHDRAWER;
            break;
        case LRCQW_WITHDRAWALS__ITEM__WITHDRAWER:
            // withdrawer is the same for all queuedWithdrawals
            // so we only copy it once
            if (params->withdrawer[0] == '\0') {
                uint8_t buffer[ADDRESS_LENGTH];
                copy_address(buffer, msg->parameter, sizeof(buffer));
                getEthDisplayableAddress(buffer, params->withdrawer, sizeof(params->withdrawer), 0);
            }
            PRINTF("WITHDRAWER: %s\n", params->withdrawer);

            context->next_param = LRCQW_WITHDRAWALS__ITEM__NONCE;
            break;
        case LRCQW_WITHDRAWALS__ITEM__NONCE:
            context->next_param = LRCQW_WITHDRAWALS__ITEM__START_BLOCK;
            break;
        case LRCQW_WITHDRAWALS__ITEM__START_BLOCK:
            context->next_param = LRCQW_WITHDRAWALS__ITEM__STRATEGIES_OFFSET;
            break;
        case LRCQW_WITHDRAWALS__ITEM__STRATEGIES_OFFSET:
            context->next_param = LRCQW_WITHDRAWALS__ITEM__SHARES_OFFSET;
            break;
        case LRCQW_WITHDRAWALS__ITEM__SHARES_OFFSET:
            context->next_param = LRCQW_WITHDRAWALS__ITEM__STRATEGIES_LENGTH;
            break;
        case LRCQW_WITHDRAWALS__ITEM__STRATEGIES_LENGTH:

            U2BE_from_parameter(msg->parameter, &params->current_item_count);
            PRINTF("LRCQW_WITHDRAWALS__ITEM__STRATEGIES_LENGTH: %d\n", params->current_item_count);

            if (params->current_item_count == 0) {
                context->next_param = LRCQW_WITHDRAWALS__ITEM__SHARES_LENGTH;
            } else {
                context->next_param = LRCQW_WITHDRAWALS__ITEM__STRATEGIES__ITEMS;
            }
            break;
        case LRCQW_WITHDRAWALS__ITEM__STRATEGIES__ITEMS: {
            // get strategy we need to display
            {
                uint8_t buffer[ADDRESS_LENGTH];
                copy_address(buffer, msg->parameter, sizeof(buffer));
                char address_buffer[ADDRESS_STR_LEN];
                getEthDisplayableAddress(buffer, address_buffer, sizeof(address_buffer), 0);

                int strategy_index = find_lr_known_strategy(address_buffer);
                if (params->strategies_count < MAX_DISPLAYABLE_LR_STRATEGIES_COUNT) {
                    params->strategies[params->strategies_count] =
                        (strategy_index != UNKNOW_LR_STRATEGY) ? strategy_index + 1
                                                               : UNKNOW_LR_STRATEGY;

                    PRINTF("STRATEGY #: %d STRATEGY: %d\n",
                           params->strategies_count,
                           strategy_index);
                }
            }

            // we just processed one strategy item
            params->strategies_count += 1;
            params->current_item_count -= 1;
            if (params->current_item_count == 0) {
                // when we arrive at the end of the strategies array we go to the shares array
                context->next_param = LRCQW_WITHDRAWALS__ITEM__SHARES_LENGTH;
            }
            break;
        }
        case LRCQW_WITHDRAWALS__ITEM__SHARES_LENGTH:

            U2BE_from_parameter(msg->parameter, &params->current_item_count);
            PRINTF("LRCQW_WITHDRAWALS__ITEM__SHARES_LENGTH: %d\n", params->current_item_count);

            if (params->current_item_count == 0 && params->parent_item_count > 0) {
                // shares array is empty AND we have other queuedWithdrawals to parse
                context->next_param = LRCQW_WITHDRAWALS__ITEM__STAKER;
            } else {
                context->next_param = LRCQW_WITHDRAWALS__ITEM__SHARES__ITEMS;
            }
            break;
        case LRCQW_WITHDRAWALS__ITEM__SHARES__ITEMS:

            // we don't need to parse shares items as they are not needed for clearsigning
            // as not having ETH / ERC20 amount to display would confuse users
            params->current_item_count -= 1;
            if (params->current_item_count == 0) {
                // we arrive at the end of the Withdrawal struct
                params->parent_item_count -= 1;
                if (params->parent_item_count == 0) {
                    // we arrive at the end of the Withdrawals array
                    context->next_param = LRCQW_TOKENS_LENGTH;
                } else {
                    // we have other Withdrawals to parse
                    context->next_param = LRCQW_WITHDRAWALS__ITEM__STAKER;
                }
            }
            break;

        // ********************************************************************
        // TOKENS[][] PARSING
        //       ^
        // ********************************************************************
        case LRCQW_TOKENS_LENGTH:

            U2BE_from_parameter(msg->parameter, &params->parent_item_count);
            params->current_item_count = params->parent_item_count;
            PRINTF("LRCQW_TOKENS_LENGTH: %d\n", params->parent_item_count);

            if (params->parent_item_count == 0) {
                context->next_param = LRCQW_MIDDLEWARE_TIMES_INDEXES_LENGTH;
            } else {
                context->next_param = LRCQW_TOKENS__OFFSET_ITEMS;
            }
            break;
        case LRCQW_TOKENS__OFFSET_ITEMS:

            params->current_item_count -= 1;
            if (params->current_item_count == 0) {
                context->next_param = LRCQW_TOKENS__ITEM__LENGTH;
            }
            break;

        // ********************************************************************
        // TOKENS[][] PARSING
        //         ^
        // ********************************************************************
        case LRCQW_TOKENS__ITEM__LENGTH:

            U2BE_from_parameter(msg->parameter, &params->current_item_count);
            PRINTF("LRCQW_TOKENS__ITEM__LENGTH: %d\n", params->current_item_count);

            if (params->current_item_count == 0) {
                context->next_param = LRCQW_MIDDLEWARE_TIMES_INDEXES_LENGTH;
            } else {
                context->next_param = LRCQW_TOKENS__ITEM__ITEMS;
            }

            break;
        case LRCQW_TOKENS__ITEM__ITEMS:
            params->current_item_count -= 1;
            if (params->current_item_count == 0) {
                // we arrive at the end of the tokens array
                if (params->parent_item_count == 0) {
                    // if we don't have other Withdrawals to parse
                    context->next_param = LRCQW_MIDDLEWARE_TIMES_INDEXES_LENGTH;
                } else {
                    // if we have other Withdrawals to parse
                    context->next_param = LRCQW_TOKENS__ITEM__LENGTH;
                }
            }
            break;

        // ********************************************************************
        //  MIDDLEWARETIMESINDEXES[] PARSING
        // ********************************************************************
        case LRCQW_MIDDLEWARE_TIMES_INDEXES_LENGTH:

            U2BE_from_parameter(msg->parameter, &params->current_item_count);
            PRINTF("LRCQW_MIDDLEWARE_TIMES_INDEXES_LENGTH: %d\n", params->current_item_count);

            if (params->current_item_count == 0) {
                context->next_param = LRCQW_RECEIVE_AS_TOKENS_LENGTH;
            } else {
                context->next_param = LRCQW_MIDDLEWARE_TIMES_INDEXES__ITEMS;
            }
            break;
        case LRCQW_MIDDLEWARE_TIMES_INDEXES__ITEMS:

            params->current_item_count -= 1;
            if (params->current_item_count == 0) {
                context->next_param = LRCQW_RECEIVE_AS_TOKENS_LENGTH;
            }
            break;

        // ********************************************************************
        //  RECEIVEASTOKENS[] PARSING
        // ********************************************************************
        case LRCQW_RECEIVE_AS_TOKENS_LENGTH:

            U2BE_from_parameter(msg->parameter, &params->current_item_count);
            // we save the number of redelegations to parse
            params->relegations_count = params->current_item_count;
            PRINTF("LRCQW_RECEIVE_AS_TOKENS_LENGTH: %d\n", params->current_item_count);

            if (params->current_item_count == 0) {
                // reached the end
                context->next_param = LRCQW_UNEXPECTED_PARAMETER;
            } else {
                context->next_param = LRCQW_RECEIVE_AS_TOKENS__ITEMS;
            }
            break;
        case LRCQW_RECEIVE_AS_TOKENS__ITEMS: {
            {
                uint16_t index = params->relegations_count - params->current_item_count;

                // if false, token is redelegated
                params->is_redelegated[index] = msg->parameter[0] == 0;
            }
            params->current_item_count -= 1;
            if (params->current_item_count == 0) {
                // reached the end
                context->next_param = LRCQW_UNEXPECTED_PARAMETER;
            }
            break;
        }

        default:
            PRINTF("Param not supported: %d\n", context->next_param);
            msg->result = ETH_PLUGIN_RESULT_ERROR;
            return;
    }
}

/**
 * @brief Handle the parameters for the delegateTo(address,(bytes,uint256),bytes32) selector
 *
 * @param msg: message containing the parameter
 * @param context: context to update
 *
 */
void handle_lr_delegate_to(ethPluginProvideParameter_t *msg, context_t *context) {
    // delegateTo(address,(bytes,uint256),bytes32)
    // example
    // [0] selector
    // [4] operator
    // [36] signature_offset
    // [68] approver_salt

    lr_delegate_to_t *params = &context->param_data.lr_delegate_to;

    switch (context->next_param) {
        case LR_DELEGATE_TO_OPERATOR: {
            {
                uint8_t buffer[ADDRESS_LENGTH];
                copy_address(buffer, msg->parameter, sizeof(buffer));
                getEthDisplayableAddress(buffer,
                                         params->operator_address,
                                         sizeof(params->operator_address),
                                         0);

                params->is_kiln = false;
                if (compare_addresses((const char *) buffer, lr_kiln_operator_address)) {
                    params->is_kiln = true;
                }
            }
            context->next_param = LR_DELEGATE_TO_SIGNATURE_OFFSET;
            break;
        }
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