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

#include "kiln_plugin.h"

void handle_query_contract_id(ethQueryContractID_t *msg) {
    const context_t *context = (const context_t *) msg->pluginContext;

    strlcpy(msg->name, PLUGIN_NAME, msg->nameLength);

    msg->result = ETH_PLUGIN_RESULT_OK;

    switch (context->selectorIndex) {
        case KILN_V1_DEPOSIT:
            strlcpy(msg->version, "Native Staking", msg->versionLength);
            break;

        case KILN_V1_WITHDRAW:
        case KILN_V1_WITHDRAW_EL:
        case KILN_V1_WITHDRAW_CL:
            strlcpy(msg->version, "Native Staking", msg->versionLength);
            break;

        case KILN_V1_BATCH_WITHDRAW:
        case KILN_V1_BATCH_WITHDRAW_EL:
        case KILN_V1_BATCH_WITHDRAW_CL:
            strlcpy(msg->version, "Native Stakingn", msg->versionLength);
            break;

        case KILN_V1_REQUEST_EXIT:
            strlcpy(msg->version, "Native Stakingn", msg->versionLength);
            break;

        case KILN_V2_STAKE:
            strlcpy(msg->version, "Pooled Staking", msg->versionLength);
            break;

        case KILN_V2_REQUEST_EXIT:
            strlcpy(msg->version, "Pooled Staking", msg->versionLength);
            break;

        case KILN_V2_MULTICLAIM:
            strlcpy(msg->version, "Pooled Staking", msg->versionLength);
            break;

        case KILN_V2_CLAIM:
            strlcpy(msg->version, "Pooled Staking", msg->versionLength);
            break;

        case KILN_LR_DEPOSIT_INTO_STRATEGY:
            strlcpy(msg->version, "EigenLayer", msg->versionLength);
            break;

        case KILN_LR_QUEUE_WITHDRAWALS:
            strlcpy(msg->version, "EigenLayer", msg->versionLength);
            break;

        case KILN_LR_COMPLETE_QUEUED_WITHDRAWALS:
            strlcpy(msg->version, "EigenLayer", msg->versionLength);
            break;

        case KILN_LR_DELEGATE_TO:
            strlcpy(msg->version, "EigenLayer", msg->versionLength);
            break;

        case KILN_LR_UNDELEGATE:
            strlcpy(msg->version, "EigenLayer", msg->versionLength);
            break;

        default:
            PRINTF("Selector Index not supported: %d\n", context->selectorIndex);
            msg->result = ETH_PLUGIN_RESULT_ERROR;
            break;
    }
}
