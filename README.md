# Badges

[![Code style check](https://github.com/LedgerHQ/app-plugin-kiln/actions/workflows/lint-workflow.yml/badge.svg)](https://github.com/LedgerHQ/app-plugin-kiln/actions/workflows/lint-workflow.yml)
[![Compilation & tests](https://github.com/LedgerHQ/app-plugin-kiln/actions/workflows/ci-workflow.yml/badge.svg)](https://github.com/LedgerHQ/app-plugin-kiln/actions/workflows/ci-workflow.yml)

# app-plugin-kiln

![Kiln Logo](https://uploads-ssl.webflow.com/625db3caa8abd6c22d5f0ce3/6260572336892a801afd71e3_Kiln_Logo-Transparent-Dark.svg)

This plug-in is compatible with Nano S / X devices, it enables to
interact in a secure wait with the Kiln deposit smart contract to
stake Ethereum.

## Prerequisite

Clone the plugin to a new folder.

```shell
git clone https://github.com/LedgerHQ/app-plugin-kiln.git
```

Then in the same folder clone two more repositories, which is the plugin-tools and app-ethereum.

```shell
git clone https://github.com/LedgerHQ/plugin-tools.git                          #plugin-tools
git clone --recurse-submodules https://github.com/LedgerHQ/app-ethereum.git     #app-ethereum
```

## Documentation

Need more information about the interface, the architecture, or general stuff about ethereum plugins? You can find more about them in the [ethereum-app documentation](https://github.com/LedgerHQ/app-ethereum/blob/master/doc/ethapp_plugins.asc).

## Smart Contracts

Smart contracts covered by this plugin are:

| Network | Version | Smart Contract                               |
| ------- | ------- | -------------------------------------------- |
| Goerli  | V0      | `0xe8Ff2a04837aac535199eEcB5ecE52b2735b3543` |

## Build

Go to the plugin-tools folder and run the "./start" script.

```shell
cd plugin-tools  # go to plugin folder
./start.sh       # run the script start.sh
```

The script will build a docker image and attach a console.
When the docker image is running go to the "app-plugin-kiln" folder and build the ".elf" files.

```shell
cd app-plugin-kiln/tests        # go to the tests folder in app-plugin-kiln
./build_local_test_elfs.sh      # run the script build_local_test_elfs.sh
```

## Tests

To test the plugin go to the tests folder from the "app-plugin-kiln/tests" and run the script "test"

```shell
cd app-plugin-kiln/tests        # go to the tests folder in app-plugin-kiln
yarn                            # install dependencies
yarn test                       # run the script test
```

## Continuous Integration

The flow processed in [GitHub Actions](https://github.com/features/actions) is the following:

- Code formatting with [clang-format](http://clang.llvm.org/docs/ClangFormat.html)
- Compilation of the application for Ledger Nano S in [ledger-app-builder](https://github.com/LedgerHQ/ledger-app-builder)

# Flows

## NANO S

### Deposit

![](/tests/snapshots/nanos_deposit/00000.png) ![](/tests/snapshots/nanos_deposit/00001.png) ![](/tests/snapshots/nanos_deposit/00002.png) ![](/tests/snapshots/nanos_deposit/00003.png) ![](/tests/snapshots/nanos_deposit/00004.png) ![](/tests/snapshots/nanos_deposit/00005.png)

### Withdraw

![](/tests/snapshots/nanos_withdraw/00000.png) ![](/tests/snapshots/nanos_withdraw/00001.png) ![](/tests/snapshots/nanos_withdraw/00002.png) ![](/tests/snapshots/nanos_withdraw/00003.png) ![](/tests/snapshots/nanos_withdraw/00004.png) ![](/tests/snapshots/nanos_withdraw/00005.png)

### Withdraw EL

![](/tests/snapshots/nanos_withdrawEL/00000.png) ![](/tests/snapshots/nanos_withdrawEL/00001.png) ![](/tests/snapshots/nanos_withdrawEL/00002.png) ![](/tests/snapshots/nanos_withdrawEL/00003.png) ![](/tests/snapshots/nanos_withdrawEL/00004.png) ![](/tests/snapshots/nanos_withdrawEL/00005.png)

### Withdraw CL

![](/tests/snapshots/nanos_withdrawCL/00000.png) ![](/tests/snapshots/nanos_withdrawCL/00001.png) ![](/tests/snapshots/nanos_withdrawCL/00002.png) ![](/tests/snapshots/nanos_withdrawCL/00003.png) ![](/tests/snapshots/nanos_withdrawCL/00004.png) ![](/tests/snapshots/nanos_withdrawCL/00005.png)

### Batch Withdraw

![](/tests/snapshots/nanos_batchWithdraw/00000.png) ![](/tests/snapshots/nanos_batchWithdraw/00001.png) ![](/tests/snapshots/nanos_batchWithdraw/00002.png) ![](/tests/snapshots/nanos_batchWithdraw/00003.png) ![](/tests/snapshots/nanos_batchWithdraw/00004.png) ![](/tests/snapshots/nanos_batchWithdraw/00005.png)

### Batch Withdraw EL

![](/tests/snapshots/nanos_batchWithdrawEL/00000.png) ![](/tests/snapshots/nanos_batchWithdrawEL/00001.png) ![](/tests/snapshots/nanos_batchWithdrawEL/00002.png) ![](/tests/snapshots/nanos_batchWithdrawEL/00003.png) ![](/tests/snapshots/nanos_batchWithdrawEL/00004.png) ![](/tests/snapshots/nanos_batchWithdrawEL/00005.png)

### Batch WithdrawCL

![](/tests/snapshots/nanos_batchWithdrawCL/00000.png) ![](/tests/snapshots/nanos_batchWithdrawCL/00001.png) ![](/tests/snapshots/nanos_batchWithdrawCL/00002.png) ![](/tests/snapshots/nanos_batchWithdrawCL/00003.png) ![](/tests/snapshots/nanos_batchWithdrawCL/00004.png) ![](/tests/snapshots/nanos_batchWithdrawCL/00005.png)

## Request Exits

![](/tests/snapshots/nanos_requestValidatorsExit/00000.png) ![](/tests/snapshots/nanos_requestValidatorsExit/00001.png) ![](/tests/snapshots/nanos_requestValidatorsExit/00002.png) ![](/tests/snapshots/nanos_requestValidatorsExit/00003.png) ![](/tests/snapshots/nanos_requestValidatorsExit/00004.png) ![](/tests/snapshots/nanos_requestValidatorsExit/00005.png)

### Stake v2

![](/tests/snapshots/nanos_stakev2/00000.png) ![](/tests/snapshots/nanos_stakev2/00001.png) ![](/tests/snapshots/nanos_stakev2/00002.png) ![](/tests/snapshots/nanos_stakev2/00003.png) ![](/tests/snapshots/nanos_stakev2/00004.png) ![](/tests/snapshots/nanos_stakev2/00005.png)

### RequestExit V2

![](/tests/snapshots/nanos_requestExitv2/00000.png) ![](/tests/snapshots/nanos_requestExitv2/00001.png) ![](/tests/snapshots/nanos_requestExitv2/00002.png) ![](/tests/snapshots/nanos_requestExitv2/00003.png) ![](/tests/snapshots/nanos_requestExitv2/00004.png) ![](/tests/snapshots/nanos_requestExitv2/00005.png)

### MultiClaim V2

![](/tests/snapshots/nanos_multiClaimv2/00000.png) ![](/tests/snapshots/nanos_multiClaimv2/00001.png) ![](/tests/snapshots/nanos_multiClaimv2/00002.png) ![](/tests/snapshots/nanos_multiClaimv2/00003.png) ![](/tests/snapshots/nanos_multiClaimv2/00004.png) ![](/tests/snapshots/nanos_multiClaimv2/00005.png)

### Claim V2

![](/tests/snapshots/nanos_claimv2/00000.png) ![](/tests/snapshots/nanos_claimv2/00001.png) ![](/tests/snapshots/nanos_claimv2/00002.png) ![](/tests/snapshots/nanos_claimv2/00003.png) ![](/tests/snapshots/nanos_claimv2/00004.png) ![](/tests/snapshots/nanos_claimv2/00005.png)

### LR Deposit Into Strategy

![](/tests/snapshots/nanos_lrDepositIntoStrategyNormal/00000.png) ![](/tests/snapshots/nanos_lrDepositIntoStrategyNormal/00001.png) ![](/tests/snapshots/nanos_lrDepositIntoStrategyNormal/00002.png) ![](/tests/snapshots/nanos_lrDepositIntoStrategyNormal/00003.png) ![](/tests/snapshots/nanos_lrDepositIntoStrategyNormal/00004.png) ![](/tests/snapshots/nanos_lrDepositIntoStrategyNormal/00005.png) ![](/tests/snapshots/nanos_lrDepositIntoStrategyNormal/00006.png) ![](/tests/snapshots/nanos_lrDepositIntoStrategyNormal/00007.png)

### Delegate To

![](/tests/snapshots/nanos_delegate_to_is_kiln/00000.png) ![](/tests/snapshots/nanos_delegate_to_is_kiln/00001.png) ![](/tests/snapshots/nanos_delegate_to_is_kiln/00002.png) ![](/tests/snapshots/nanos_delegate_to_is_kiln/00003.png) ![](/tests/snapshots/nanos_delegate_to_is_kiln/00004.png) ![](/tests/snapshots/nanos_delegate_to_is_kiln/00005.png) ![](/tests/snapshots/nanos_delegate_to_is_kiln/00006.png)

### Undelegate

![](/tests/snapshots/nanos_lr_undelegate/00000.png) ![](/tests/snapshots/nanos_lr_undelegate/00001.png) ![](/tests/snapshots/nanos_lr_undelegate/00002.png) ![](/tests/snapshots/nanos_lr_undelegate/00003.png) ![](/tests/snapshots/nanos_lr_undelegate/00004.png) ![](/tests/snapshots/nanos_lr_undelegate/00005.png)

### Queue Withdrawals

![](/tests/snapshots/nanos_lrQueueWithdrawals_1_2_dimension/00000.png) ![](/tests/snapshots/nanos_lrQueueWithdrawals_1_2_dimension/00001.png) ![](/tests/snapshots/nanos_lrQueueWithdrawals_1_2_dimension/00002.png) ![](/tests/snapshots/nanos_lrQueueWithdrawals_1_2_dimension/00003.png) ![](/tests/snapshots/nanos_lrQueueWithdrawals_1_2_dimension/00004.png) ![](/tests/snapshots/nanos_lrQueueWithdrawals_1_2_dimension/00005.png) ![](/tests/snapshots/nanos_lrQueueWithdrawals_1_2_dimension/00006.png) ![](/tests/snapshots/nanos_lrQueueWithdrawals_1_2_dimension/00007.png)

### LR Complete Queued Withdrawals

![](/tests/snapshots/nanos_lrCompleteQueuedWithdrawals/00000.png) ![](/tests/snapshots/nanos_lrCompleteQueuedWithdrawals/00001.png) ![](/tests/snapshots/nanos_lrCompleteQueuedWithdrawals/00002.png) ![](/tests/snapshots/nanos_lrCompleteQueuedWithdrawals/00003.png) ![](/tests/snapshots/nanos_lrCompleteQueuedWithdrawals/00004.png) ![](/tests/snapshots/nanos_lrCompleteQueuedWithdrawals/00005.png) ![](/tests/snapshots/nanos_lrCompleteQueuedWithdrawals/00006.png) ![](/tests/snapshots/nanos_lrCompleteQueuedWithdrawals/00007.png) ![](/tests/snapshots/nanos_lrCompleteQueuedWithdrawals/00008.png) ![](/tests/snapshots/nanos_lrCompleteQueuedWithdrawals/00009.png) ![](/tests/snapshots/nanos_lrCompleteQueuedWithdrawals/000010.png)

## NANO X

### Deposit

![](/tests/snapshots/nanox_deposit/00000.png) ![](/tests/snapshots/nanox_deposit/00001.png) ![](/tests/snapshots/nanox_deposit/00002.png) ![](/tests/snapshots/nanox_deposit/00003.png) ![](/tests/snapshots/nanox_deposit/00004.png) ![](/tests/snapshots/nanox_deposit/00005.png)

### Withdraw

![](/tests/snapshots/nanox_withdraw/00000.png) ![](/tests/snapshots/nanox_withdraw/00001.png) ![](/tests/snapshots/nanox_withdraw/00002.png) ![](/tests/snapshots/nanox_withdraw/00003.png) ![](/tests/snapshots/nanox_withdraw/00004.png) ![](/tests/snapshots/nanox_withdraw/00005.png)

### Withdraw EL

![](/tests/snapshots/nanox_withdrawEL/00000.png) ![](/tests/snapshots/nanox_withdrawEL/00001.png) ![](/tests/snapshots/nanox_withdrawEL/00002.png) ![](/tests/snapshots/nanox_withdrawEL/00003.png) ![](/tests/snapshots/nanox_withdrawEL/00004.png) ![](/tests/snapshots/nanox_withdrawEL/00005.png)

### Withdraw CL

![](/tests/snapshots/nanox_withdrawCL/00000.png) ![](/tests/snapshots/nanox_withdrawCL/00001.png) ![](/tests/snapshots/nanox_withdrawCL/00002.png) ![](/tests/snapshots/nanox_withdrawCL/00003.png) ![](/tests/snapshots/nanox_withdrawCL/00004.png) ![](/tests/snapshots/nanox_withdrawCL/00005.png)

### Batch Withdraw

![](/tests/snapshots/nanox_batchWithdraw/00000.png) ![](/tests/snapshots/nanox_batchWithdraw/00001.png) ![](/tests/snapshots/nanox_batchWithdraw/00002.png) ![](/tests/snapshots/nanox_batchWithdraw/00003.png) ![](/tests/snapshots/nanox_batchWithdraw/00004.png) ![](/tests/snapshots/nanox_batchWithdraw/00005.png)

### Batch Withdraw EL

![](/tests/snapshots/nanox_batchWithdrawEL/00000.png) ![](/tests/snapshots/nanox_batchWithdrawEL/00001.png) ![](/tests/snapshots/nanox_batchWithdrawEL/00002.png) ![](/tests/snapshots/nanox_batchWithdrawEL/00003.png) ![](/tests/snapshots/nanox_batchWithdrawEL/00004.png) ![](/tests/snapshots/nanox_batchWithdrawEL/00005.png)

### Batch WithdrawCL

![](/tests/snapshots/nanox_batchWithdrawCL/00000.png) ![](/tests/snapshots/nanox_batchWithdrawCL/00001.png) ![](/tests/snapshots/nanox_batchWithdrawCL/00002.png) ![](/tests/snapshots/nanox_batchWithdrawCL/00003.png) ![](/tests/snapshots/nanox_batchWithdrawCL/00004.png) ![](/tests/snapshots/nanox_batchWithdrawCL/00005.png)

### Request Exits

![](/tests/snapshots/nanox_requestValidatorsExit/00000.png) ![](/tests/snapshots/nanox_requestValidatorsExit/00001.png) ![](/tests/snapshots/nanox_requestValidatorsExit/00002.png) ![](/tests/snapshots/nanox_requestValidatorsExit/00003.png) ![](/tests/snapshots/nanox_requestValidatorsExit/00004.png) ![](/tests/snapshots/nanox_requestValidatorsExit/00005.png)

### Stake V2

![](/tests/snapshots/nanox_stakev2/00000.png) ![](/tests/snapshots/nanox_stakev2/00001.png) ![](/tests/snapshots/nanox_stakev2/00002.png) ![](/tests/snapshots/nanox_stakev2/00003.png) ![](/tests/snapshots/nanox_stakev2/00004.png) ![](/tests/snapshots/nanox_stakev2/00005.png)

### RequestExit V2

![](/tests/snapshots/nanox_requestExitv2/00000.png) ![](/tests/snapshots/nanox_requestExitv2/00001.png) ![](/tests/snapshots/nanox_requestExitv2/00002.png) ![](/tests/snapshots/nanox_requestExitv2/00003.png) ![](/tests/snapshots/nanox_requestExitv2/00004.png) ![](/tests/snapshots/nanox_requestExitv2/00005.png)

### MultiClaim V2

![](/tests/snapshots/nanox_multiClaimv2/00000.png) ![](/tests/snapshots/nanox_multiClaimv2/00001.png) ![](/tests/snapshots/nanox_multiClaimv2/00002.png) ![](/tests/snapshots/nanox_multiClaimv2/00003.png) ![](/tests/snapshots/nanox_multiClaimv2/00004.png) ![](/tests/snapshots/nanox_multiClaimv2/00005.png)

### Claim V2

![](/tests/snapshots/nanox_claimv2/00000.png) ![](/tests/snapshots/nanox_claimv2/00001.png) ![](/tests/snapshots/nanox_claimv2/00002.png) ![](/tests/snapshots/nanox_claimv2/00003.png) ![](/tests/snapshots/nanox_claimv2/00004.png) ![](/tests/snapshots/nanox_claimv2/00005.png)

# EigenLayer

### LR Deposit Into Strategy

![](/tests/snapshots/nanox_lrDepositIntoStrategyNormal/00000.png) ![](/tests/snapshots/nanox_lrDepositIntoStrategyNormal/00001.png) ![](/tests/snapshots/nanox_lrDepositIntoStrategyNormal/00002.png) ![](/tests/snapshots/nanox_lrDepositIntoStrategyNormal/00003.png) ![](/tests/snapshots/nanox_lrDepositIntoStrategyNormal/00004.png) ![](/tests/snapshots/nanox_lrDepositIntoStrategyNormal/00005.png) ![](/tests/snapshots/nanox_lrDepositIntoStrategyNormal/00006.png) ![](/tests/snapshots/nanox_lrDepositIntoStrategyNormal/00007.png)

### Delegate To

![](/tests/snapshots/nanox_delegate_to_is_kiln/00000.png) ![](/tests/snapshots/nanox_delegate_to_is_kiln/00001.png) ![](/tests/snapshots/nanox_delegate_to_is_kiln/00002.png) ![](/tests/snapshots/nanox_delegate_to_is_kiln/00003.png) ![](/tests/snapshots/nanox_delegate_to_is_kiln/00004.png) ![](/tests/snapshots/nanox_delegate_to_is_kiln/00005.png) ![](/tests/snapshots/nanox_delegate_to_is_kiln/00006.png)

### Undelegate

![](/tests/snapshots/nanox_lr_undelegate/00000.png) ![](/tests/snapshots/nanox_lr_undelegate/00001.png) ![](/tests/snapshots/nanox_lr_undelegate/00002.png) ![](/tests/snapshots/nanox_lr_undelegate/00003.png) ![](/tests/snapshots/nanox_lr_undelegate/00004.png) ![](/tests/snapshots/nanox_lr_undelegate/00005.png)

### Queue Withdrawals

![](/tests/snapshots/nanox_lrQueueWithdrawals_1_2_dimension/00000.png) ![](/tests/snapshots/nanox_lrQueueWithdrawals_1_2_dimension/00001.png) ![](/tests/snapshots/nanox_lrQueueWithdrawals_1_2_dimension/00002.png) ![](/tests/snapshots/nanox_lrQueueWithdrawals_1_2_dimension/00003.png) ![](/tests/snapshots/nanox_lrQueueWithdrawals_1_2_dimension/00004.png) ![](/tests/snapshots/nanox_lrQueueWithdrawals_1_2_dimension/00005.png) ![](/tests/snapshots/nanox_lrQueueWithdrawals_1_2_dimension/00006.png) ![](/tests/snapshots/nanox_lrQueueWithdrawals_1_2_dimension/00007.png)

### LR Complete Queued Withdrawals

![](/tests/snapshots/nanox_lrCompleteQueuedWithdrawals/00000.png) ![](/tests/snapshots/nanox_lrCompleteQueuedWithdrawals/00001.png) ![](/tests/snapshots/nanox_lrCompleteQueuedWithdrawals/00002.png) ![](/tests/snapshots/nanox_lrCompleteQueuedWithdrawals/00003.png) ![](/tests/snapshots/nanox_lrCompleteQueuedWithdrawals/00004.png) ![](/tests/snapshots/nanox_lrCompleteQueuedWithdrawals/00005.png) ![](/tests/snapshots/nanox_lrCompleteQueuedWithdrawals/00006.png) ![](/tests/snapshots/nanox_lrCompleteQueuedWithdrawals/00007.png) ![](/tests/snapshots/nanox_lrCompleteQueuedWithdrawals/00008.png) ![](/tests/snapshots/nanox_lrCompleteQueuedWithdrawals/00009.png)
