
let addon = require('./build/Release/asch-private'); 

let MODULES = "a8852ebf7c49f01cd196e35394f3b74dd86283a07f57e0a262928e7493d4a3961d93d93c90ea3369719641d626d28b9cddc6d9307b9aabdbffc40b6d6da2e329d079b4187ff784b2893d9f53e9ab913a04ff02668114695b07d8ce877c4c8cac1b12b9beff3c51294ebe349eca41c24cd32a6d09dd1579d3947e5c4dcc30b2090b0454edb98c6336e7571db09e0fdafbd68d8f0470223836e90666a5b143b73b9cd71547c917bf24c0efc86af2eba046ed781d9acb05c80f007ef5a0a5dfca23236f37e698e8728def12554bc80f294f71c040a88eff144d130b24211016a97ce0f5fe520f477e555c9997683d762aff8bd1402ae6938dd5c994780b1bf6aa7239e9d8101630ecfeaa730d2bbc97d39beb057f016db2e28bf12fab4989c0170c2593383fd04660b5229adcd8486ba78f6cc1b558bcd92f344100dff239a8c00dbc4c2825277f24bdd04475bcc9a8c39fd895eff97c1967e434effcb9bd394e0577f4cf98c30d9e6b54cd47d6e447dcf34d67e48e4421691dbe4a7d9bd503abb9"

//
// 1 Mint Transaction
console.log("1. Start Mint TRANSACTION..."); 

// 1.1 mint coin
let zcoin = addon.mint(MODULES);

console.log("1.1 Mint coin")
console.log("    私密部分之SerialNumber: " + zcoin.serialNumber);
console.log("    私密部分之Randomness: " + zcoin.randomness);  
console.log("    公开部分之Commitment: " + zcoin.commitment);

// 1.2 进行实际Mint交易(需要Asch-Core 支持)
console.log("1.2 Transaction(implentment by asch-core...)"); 
// 1.3 Miner validate commitment of zero coin
console.log("1.3 Miner validate"); 
if (addon.validate(MODULES, zcoin.commitment))         
    console.log("    Success.");
else
    console.log("    Error!");

//
// 2. Spend Transaction
console.log("\n2. Start Spend Transaction...");

// At this point we should generate a ZEROCOIN_SPEND transaction to
// send to the network. This network should include a set of outputs
// totalling to the value of one zerocoin (minus transaction fees).
//
// The format of this transaction is up to the implementer. Here we'll
// assume you've formatted this transaction and placed the hash into
// "transactionHash". We'll also assume "accumulatorHash" contains the
// hash of the last block whose transactions are in the accumulator.
let transactionHash = 0;    // DUMMY_TRANSACTION_HASH; 
let accumulatorID = 0;    // DUMMY_ACCUMULATOR_Hash; will be implment by asch-core

let coinSpend = addon.spend(MODULES, zcoin, transactionHash, accumulatorID);

console.log("    " + coinSpend.status);
console.log("    SN: " + coinSpend.serialNumber);
