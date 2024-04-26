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

#include "query_contract_ui.h"

/**
 * @brief UI for depositing into an EigenLayer strategy
 *
 * @param msg: message containing the parameter
 * @param context: context with provide_parameter data
 *
 */
bool deposit_into_stragey_ui(ethQueryContractUI_t *msg, context_t *context) {
    bool ret = false;
    lr_deposit_t *params = &context->param_data.lr_deposit;

    switch (msg->screenIndex) {
        case 0:
            strlcpy(msg->title, "EigenLayer", msg->titleLength);
            strlcpy(msg->msg, "Deposit In Strategy", msg->msgLength);
            ret = true;
            break;
        case 1:
            strlcpy(msg->title, "Strategy", msg->titleLength);
            if (params->strategy_to_display == UNKNOW_LR_STRATEGY ||
                params->strategy_to_display >= LR_STRATEGIES_COUNT) {
                strlcpy(msg->msg, "UNKNOWN", msg->msgLength);
            } else {
                strlcpy(msg->msg, lr_tickers[params->strategy_to_display], MAX_TICKER_LEN);
            }
            ret = true;
            break;
        case 2:
            strlcpy(msg->title, "Amount", msg->titleLength);
            amountToString(params->erc20_amount_to_display,
                           sizeof(params->erc20_amount_to_display),
                           ERC20_DECIMALS,
                           params->erc20_to_display == UNKNOW_LR_STRATEGY ||
                                   params->erc20_to_display >= LR_STRATEGIES_COUNT
                               ? "UNKNOWN"
                               : lr_tickers[params->erc20_to_display],
                           msg->msg,
                           msg->msgLength);
            ret = true;
            break;
        default:
            PRINTF("Received an invalid screenIndex\n");
            break;
    }
    return ret;
}

/**
 * @brief UI for queueing withdrawals
 *
 * @param msg: message containing the parameter
 * @param context: context with provide_parameter data
 *
 */
bool queue_withdrawals_ui(ethQueryContractUI_t *msg, context_t *context) {
    bool ret = false;
    lr_queue_withdrawals_t *params = &context->param_data.lr_queue_withdrawals;

    switch (msg->screenIndex) {
        case 0:
            strlcpy(msg->title, "EigenLayer", msg->titleLength);
            strlcpy(msg->msg, "Queue Withdrawals", msg->msgLength);
            ret = true;
            break;

        case 1:
            strlcpy(msg->title, "Withdrawer", msg->titleLength);
            strlcpy(msg->msg, params->withdrawer, msg->msgLength);
            ret = true;
            break;

        default: {
            {
                // removing the first screen to current screen index
                // to get the index of the withdrawal
                uint8_t withdrawal_index = msg->screenIndex - 2;

                if (withdrawal_index < params->strategies_count) {
                    strlcpy(msg->title, "Strategy", msg->titleLength);
                    uint8_t strategy = params->strategies[withdrawal_index];

                    if (strategy == UNKNOW_LR_STRATEGY || strategy - 1 >= LR_STRATEGIES_COUNT) {
                        strlcpy(msg->msg, "UNKNOWN", msg->msgLength);
                    } else {
                        strlcpy(msg->msg, lr_tickers[strategy - 1], msg->msgLength);
                    }
                }

                ret = true;
                break;
            }
            PRINTF("Received an invalid screenIndex\n");
            break;
        }
    }
    return ret;
}

/**
 * @brief UI for completing queued withdrawals
 *
 * @param msg: message containing the parameter
 * @param context: context with provide_parameter data
 *
 */
bool complete_queued_withdrawals_ui(ethQueryContractUI_t *msg, context_t *context) {
    bool ret = false;
    lr_complete_queued_withdrawals_t *params = &context->param_data.lr_complete_queued_withdrawals;

    switch (msg->screenIndex) {
        case 0:
            strlcpy(msg->title, "EigenLayer", msg->titleLength);
            strlcpy(msg->msg, "Claim Withdrawals", msg->msgLength);
            ret = true;
            break;

        case 1:
            strlcpy(msg->title, "Withdrawer", msg->titleLength);
            strlcpy(msg->msg, params->withdrawer, msg->msgLength);
            ret = true;
            break;

        default: {
            {
                // removing the first screen to current screen index
                // to get the index of the withdrawal
                uint8_t withdrawal_index = msg->screenIndex - 2;

                if (withdrawal_index < params->strategies_count) {
                    if (params->is_redelegated[withdrawal_index]) {
                        strlcpy(msg->title, "Redelegate", msg->titleLength);
                    } else {
                        strlcpy(msg->title, "Withdraw", msg->titleLength);
                    }
                    uint8_t strategy = params->strategies[withdrawal_index];

                    if (strategy == UNKNOW_LR_STRATEGY || strategy - 1 >= LR_STRATEGIES_COUNT) {
                        strlcpy(msg->msg, "UNKNOWN", msg->msgLength);
                    } else {
                        strlcpy(msg->msg, lr_tickers[strategy - 1], msg->msgLength);
                    }
                }

                ret = true;
                break;
            }
            PRINTF("Received an invalid screenIndex\n");
            break;
        }
    }
    return ret;
}

/**
 * @brief UI for delegating to an operator
 *
 * @param msg: message containing the parameter
 * @param context: context with provide_parameter data
 *
 */
bool delegate_to_ui(ethQueryContractUI_t *msg, context_t *context) {
    bool ret = false;
    lr_delegate_to_t *params = &context->param_data.lr_delegate_to;

    switch (msg->screenIndex) {
        case 0:
            strlcpy(msg->title, "EigenLayer", msg->titleLength);
            strlcpy(msg->msg, "Delegate To", msg->msgLength);
            ret = true;
            break;
        case 1:
            strlcpy(msg->title, "Operator", msg->titleLength);
            if (params->is_kiln) {
                strlcpy(msg->msg, "Kiln", msg->msgLength);
            } else {
                strlcpy(msg->msg, params->operator_address, msg->msgLength);
            }
            ret = true;
            break;

        default:
            PRINTF("Received an invalid screenIndex\n");
            break;
    }
    return ret;
}
/**
 * @brief UI for undelegating
 *
 * @param msg: message containing the parameter
 *
 */
bool undelegate_ui(ethQueryContractUI_t *msg) {
    bool ret = false;

    switch (msg->screenIndex) {
        case 0:
            strlcpy(msg->title, "EigenLayer", msg->titleLength);
            strlcpy(msg->msg, "Undelegate all", msg->msgLength);
            ret = true;
            break;

        default:
            PRINTF("Received an invalid screenIndex\n");
            break;
    }
    return ret;
}