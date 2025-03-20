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
 * (c) 2022-2025 Kiln
 *
 * contact@kiln.fi
 ********************************************************************************/

#include "kiln_plugin.h"

bool morpho_claim_ui(ethQueryContractUI_t *msg, context_t *context) {
    bool ret = false;
    morpho_claim_t *params = &context->param_data.morpho_claim;

    switch (msg->screenIndex) {
        case 0:
            strlcpy(msg->title, "Action", msg->titleLength);
            strlcpy(msg->msg, "Claim $MORPHO Rewards", msg->msgLength);
            ret = true;
            break;
        case 1:
            strlcpy(msg->title, "Reward Token", msg->titleLength);
            strlcpy(msg->msg, params->reward_token, msg->msgLength);
            ret = true;
            break;
        case 2:
            strlcpy(msg->title, "Account", msg->titleLength);
            strlcpy(msg->msg, params->account, msg->msgLength);
            ret = true;
            break;
        case 3: {
            strlcpy(msg->title, "Amount", msg->titleLength);
            uint8_t decimals = 18;
            char amount_str[50] = {0};
            if (!amountToString(params->total_amount,
                                sizeof(params->total_amount),
                                decimals,
                                "MORPHO",
                                amount_str,
                                sizeof(amount_str))) {
                strlcpy(msg->msg, "ERROR", msg->msgLength);
            } else {
                strlcpy(msg->msg, amount_str, msg->msgLength);
            }
            ret = true;
            break;
        }
        default:
            PRINTF("Received an invalid screenIndex\n");
            break;
    }

    return ret;
}