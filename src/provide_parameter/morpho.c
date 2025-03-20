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

void handle_morpho_claim(ethPluginProvideParameter_t *msg, context_t *context) {
    // **************************************************************************
    // FUNCTION TO PARSE
    // **************************************************************************
    //
    // function claim(address rewardToken, address account, uint256 totalAmount, bytes32[]
    // merkleProof)
    //
    // **************************************************************************
    // example
    // [0] selector
    // [4] rewardToken
    // [36] account
    // [68] totalAmount
    // [100] merkleProof (offset)
    // [132] merkleProof (length)
    // [164+] merkleProof (items)
    // **************************************************************************
    morpho_claim_t *params = &context->param_data.morpho_claim;

    switch (context->next_param) {
        case MORPHO_CLAIM_REWARD_TOKEN: {
            uint8_t buffer[ADDRESS_LENGTH];
            copy_address(buffer, msg->parameter, sizeof(buffer));
            getEthDisplayableAddress(buffer, params->reward_token, sizeof(params->reward_token), 0);
            context->next_param = MORPHO_CLAIM_ACCOUNT;
            break;
        }
        case MORPHO_CLAIM_ACCOUNT: {
            uint8_t buffer[ADDRESS_LENGTH];
            copy_address(buffer, msg->parameter, sizeof(buffer));
            getEthDisplayableAddress(buffer, params->account, sizeof(params->account), 0);
            context->next_param = MORPHO_CLAIM_TOTAL_AMOUNT;
            break;
        }
        case MORPHO_CLAIM_TOTAL_AMOUNT:
            copy_parameter(params->total_amount, msg->parameter, sizeof(params->total_amount));
            context->next_param = MORPHO_CLAIM_PROOF_OFFSET;
            break;
        case MORPHO_CLAIM_PROOF_OFFSET:
            // Just store the offset and move to the next parameter
            context->next_param = MORPHO_CLAIM_PROOF_LENGTH;
            break;
        case MORPHO_CLAIM_PROOF_LENGTH:
            // Set the number of proof items from the parameter
            params->current_item_count = U2BE(msg->parameter, 30);
            if (params->current_item_count > 0) {
                context->next_param = MORPHO_CLAIM_PROOF_ITEMS;
            } else {
                // No proof items, move to the end
                context->next_param = MORPHO_CLAIM_UNEXPECTED_PARAMETER;
            }
            break;
        case MORPHO_CLAIM_PROOF_ITEMS:
            params->current_item_count -= 1;
            // If we've processed all proof items, we're done
            if (params->current_item_count == 0) {
                context->next_param = MORPHO_CLAIM_UNEXPECTED_PARAMETER;
            }
            break;
        default:
            PRINTF("Param not supported: %d\n", context->next_param);
            msg->result = ETH_PLUGIN_RESULT_ERROR;
            return;
    }
    msg->result = ETH_PLUGIN_RESULT_OK;
}