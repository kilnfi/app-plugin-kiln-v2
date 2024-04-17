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

const contractAddr = '0x39053d51b77dc0d36036fc1fcc8cb819df8ef37a'; // delegation manager

const pluginName = 'Kiln';
const abi_path = `../cal/abis/${contractAddr}.json`;
const abi = require(abi_path);

nano_models.forEach(function (model) {
  test(
    '[Nano ' + model.letter + '] LR Delegate To - is Kiln',
    zemu(model, async (sim, eth) => {
      const contract = new ethers.Contract(contractAddr, abi);

      const { data } = await contract.populateTransaction.delegateTo(
        '0x1f8c8b1d78d01bcc42ebdd34fae60181bd697662', // kiln operator
        {
          signature: '0x',
          expiry: 0,
        },
        0
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
      const right_clicks = model.letter === 'S' ? 7 : 5;

      await waitForAppScreen(sim);
      await sim.navigateAndCompareSnapshots(
        '.',
        model.name + '_delegate_to_is_kiln',
        [right_clicks, 0]
      );
      await tx;
    }),
    30000
  );
});
