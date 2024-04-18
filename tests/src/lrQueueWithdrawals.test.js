import 'core-js/stable';
import 'regenerator-runtime/runtime';
import {
  waitForAppScreen,
  kilnJSON,
  zemu,
  genericTx,
  nano_models,
  SPECULOS_ADDRESS,
  txFromEtherscan,
} from './test.fixture';
import { ethers } from 'ethers';
import { parseEther, parseUnits } from 'ethers/lib/utils';
import { ledgerService } from '@ledgerhq/hw-app-eth';

const contractAddr = '0x858646372cc42e1a627fce94aa7a7033e7cf075a'; // strategy manager

const pluginName = 'Kiln';
const abi_path = `../cal/abis/${contractAddr}.json`;
const abi = require(abi_path);

nano_models.forEach(function (model) {
  test(
    '[Nano ' + model.letter + '] LR Queue Withdrawals 1-2 dimension',
    zemu(model, async (sim, eth) => {
      const contract = new ethers.Contract(contractAddr, abi);

      const { data } = await contract.populateTransaction.queueWithdrawals([
        {
          strategies: [
            '0x54945180dB7943c0ed0FEE7EdaB2Bd24620256bc', // cbETH
            '0x13760F50a9d7377e4F20CB8CF9e4c26586c658ff', // ankrETH
          ],
          shares: [parseEther('0.1'), parseEther('0.2')],
          withdrawer: '0xd8dA6BF26964aF9D7eEd9e03E53415D37aA96045',
        },
      ]);

      let unsignedTx = genericTx;

      unsignedTx.to = contractAddr;
      unsignedTx.data = data;
      unsignedTx.value = parseEther('0');

      const serializedTx = ethers.utils
        .serializeTransaction(unsignedTx)
        .slice(2);
      const resolution = await ledgerService.resolveTransaction(
        serializedTx,
        eth.loadConfig,
        {
          externalPlugins: true,
        }
      );
      const tx = eth.signTransaction("44'/60'/0'/0", serializedTx, resolution);
      const right_clicks = model.letter === 'S' ? 7 : 5;

      await waitForAppScreen(sim);
      await sim.navigateAndCompareSnapshots(
        '.',
        model.name + '_lrQueueWithdrawals_1_2_dimension',
        [right_clicks, 0]
      );
      await tx;
    }),
    30000
  );

  test(
    '[Nano ' + model.letter + '] LR Queue Withdrawals 2-3 dimension',
    zemu(model, async (sim, eth) => {
      const contract = new ethers.Contract(contractAddr, abi);

      const { data } = await contract.populateTransaction.queueWithdrawals([
        {
          strategies: [
            '0x54945180dB7943c0ed0FEE7EdaB2Bd24620256bc',  // cbETH
            '0x13760F50a9d7377e4F20CB8CF9e4c26586c658ff',  // ankrETH
            '0x57ba429517c3473B6d34CA9aCd56c0e735b94c02',  // osETH
          ],
          shares: [parseEther('0.1'), parseEther('0.2'), parseEther('0.3')],
          withdrawer: '0xd8dA6BF26964aF9D7eEd9e03E53415D37aA96045',
        },
        {
          strategies: [
            '0xd5F7838F5C461fefF7FE49ea5ebaF7728bB0ADfa',  // mETH
            '0x54945180dB7943c0ed0FEE7EdaB2Bd24620256bc',  // cbETH
            '0x856c4Efb76C1D1AE02e20CEB03A2A6a08b0b8dC3',  // OETH
          ],
          shares: [parseEther('0.5'), parseEther('0.1'), parseEther('0.2')],
          withdrawer: '0xd8dA6BF26964aF9D7eEd9e03E53415D37aA96045',
        },
      ]);

      let unsignedTx = genericTx;

      unsignedTx.to = contractAddr;
      unsignedTx.data = data;
      unsignedTx.value = parseEther('0');

      const serializedTx = ethers.utils
        .serializeTransaction(unsignedTx)
        .slice(2);
      const resolution = await ledgerService.resolveTransaction(
        serializedTx,
        eth.loadConfig,
        {
          externalPlugins: true,
        }
      );
      const tx = eth.signTransaction("44'/60'/0'/0", serializedTx, resolution);
      const right_clicks = model.letter === 'S' ? 7 : 5;

      await waitForAppScreen(sim);
      await sim.navigateAndCompareSnapshots(
        '.',
        model.name + '_lrQueueWithdrawals_2_3_dimension',
        [right_clicks, 0]
      );
      await tx;
    }),

});
