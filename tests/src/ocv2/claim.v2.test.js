import 'core-js/stable';
import 'regenerator-runtime/runtime';
import {
  waitForAppScreen,
  zemu,
  genericTx,
  nano_models,
} from '../test.fixture';
import { ethers } from 'ethers';
import { parseEther } from 'ethers/lib/utils';
import { ledgerService } from '@ledgerhq/hw-app-eth';

const contractAddr = '0x8d6fd650500f82c7d978a440348e5a9b886943bf';

const pluginName = 'Kiln';
const abi_path = `../cal/ethereum/abis/${contractAddr}.json`;
const abi = require(abi_path);

nano_models.forEach(function (model) {
  test(
    '[Nano ' + model.letter + '] Claim V2 Eth',
    zemu(model, async (sim, eth) => {
      const contract = new ethers.Contract(contractAddr, abi);

      const { data } = await contract.populateTransaction.claim(
        [150, 2],
        [0, 1],
        132
      );

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
      const right_clicks = 4;

      await waitForAppScreen(sim);
      await sim.navigateAndCompareSnapshots('.', model.name + '_claimv2', [
        right_clicks,
        0,
      ]);
      await tx;
    }),
    30000
  );
});
