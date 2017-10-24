#include <node.h>
#include <string>
#include "libzerocoin/Zerocoin.h"

using namespace v8;

// utils
Bignum getBignum(Local<Value> arg)
{
    Bignum bn;
    bn.SetHex( std::string(*String::Utf8Value(arg->ToString())) );
    return bn;
}


/*************************************************************************************************
  Mint Coin
    input: modules (length: 3072)
    output: zero coin object
**************************************************************************************************/
void mint(const FunctionCallbackInfo<Value> &args) {
    Isolate *isolate = args.GetIsolate();

    // Check the number of arguments passed.
    if (args.Length() < 1) {
        // Throw an Error that is passed back to JavaScript
        isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(isolate, "Wrong number of arguments")));
        return;
    }

    // Check the argument types
    if (!args[0]->IsString() ) {
        isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(isolate, "Wrong arguments")));
        return;
    }

    try {
        // Set up the Zerocoin Params object
        Bignum modulus = getBignum(args[0]);
        libzerocoin::Params* params = new libzerocoin::Params(modulus);
    
        // mint a newCoin
		libzerocoin::PrivateCoin newCoin(params);
        
        // Get a copy of the 'public' portion of the coin. You should
        // embed this into a Zerocoin 'MINT' transaction along with a series
        // of currency inputs totaling the assigned value of one zerocoin.
        libzerocoin::PublicCoin pubCoin = newCoin.getPublicCoin();
        
        // return this coin
        Local<Object> zcoin = Object::New(isolate);
        zcoin->Set(String::NewFromUtf8(isolate, "serialNumber"), String::NewFromUtf8(isolate, newCoin.getSerialNumber().ToString(16).c_str()));
        zcoin->Set(String::NewFromUtf8(isolate, "randomness"), String::NewFromUtf8(isolate, newCoin.getRandomness().ToString(16).c_str()));
        zcoin->Set(String::NewFromUtf8(isolate, "commitment"), String::NewFromUtf8(isolate, newCoin.getPublicCoin().getValue().ToString(16).c_str()));
        
        //int denomination = (int)newCoin.getPublicCoin().getDenomination();
        //zcoin->Set(String::NewFromUtf8(isolate, "denomination"), Number::New(isolate, denomination));
        
        args.GetReturnValue().Set(zcoin);

    } catch (runtime_error &e) {
        isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(isolate, e.what())));
        return;
    }
}

/*************************************************************************************************
  Validate Coin
    input: 1. modules  2. commitment
    output: true or false
**************************************************************************************************/
void validate(const FunctionCallbackInfo<Value> &args) {
    Isolate *isolate = args.GetIsolate();
    // Check the number of arguments passed.
    if (args.Length() < 1) {
        // Throw an Error that is passed back to JavaScript
        isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(isolate, "Wrong number of arguments")));
        return;
    }
    
    try {
        // Set up the Zerocoin Params object
        Bignum modulus = getBignum( args[0] );
        libzerocoin::Params* params = new libzerocoin::Params(modulus);
    
        libzerocoin::PublicCoin pubCoin(params, getBignum(args[1]));
        
         // return result of validate
         args.GetReturnValue().Set( Boolean::New(isolate, pubCoin.validate()) );

    } catch (runtime_error &e) {
        isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(isolate, e.what())));
        return;
    }
}

/*************************************************************************************************
  Spend Coin
    input: 1. modules  2. zerocoin   3. metadata
    output: zero coin object
**************************************************************************************************/
void spend(const FunctionCallbackInfo<Value> &args) {
    Isolate *isolate = args.GetIsolate();

    // Check the number of arguments passed.
    if (args.Length() < 4) {
        // Throw an Error that is passed back to JavaScript
        isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(isolate, "Wrong number of arguments")));
        return;
    }

    // Check the argument types
    if (!args[0]->IsString() ) {
        isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(isolate, "Wrong arguments")));
        return;
    }

    try {
        // Set up the Zerocoin Params object
        Bignum modulus = getBignum( args[0] );
        libzerocoin::Params* params = new libzerocoin::Params(modulus); 
            
        // reconstruct coin from args[1]
        Local<Object> zc = args[1]->ToObject();
        libzerocoin::PrivateCoin coin(params);
        coin.setSerialNumber(getBignum(zc->Get(String::NewFromUtf8(isolate, "serialNumber"))));
        coin.setRandomness(getBignum(zc->Get(String::NewFromUtf8(isolate, "randomness"))));
        
        const Bignum value = getBignum(zc->Get(String::NewFromUtf8(isolate, "commitment")));
        libzerocoin::PublicCoin pubCoin(params, value);
        coin.setPublicCoin(pubCoin);
        
		// Create an empty accumulator object
		libzerocoin::Accumulator accumulator(params);
        
        // To generate the witness, we start with this accumulator and
		// add the public half of the coin we want to spend.
		libzerocoin::AccumulatorWitness witness(params, accumulator, pubCoin);
        
        // Add the public half of "coin" to the Accumulator itself.
        accumulator += pubCoin;
        
		// Place "transactionHash" and "accumulatorBlockHash" into a new
        // SpendMetaData object.
        uint256 transactionHash = 0;        // args[2]
        uint256 accumulatorID = 0;          // args[3]
		libzerocoin::SpendMetaData metaData(accumulatorID, transactionHash);
        
        // Construct the CoinSpend object. This acts like a signature on the transaction.
        libzerocoin::CoinSpend spend(params, coin, accumulator, witness, metaData);
        
        // return object {status, serialnumber}
        Local<Object> ret = Object::New(isolate);
        
        if (spend.Verify(accumulator, metaData)) {
            ret->Set(String::NewFromUtf8(isolate, "status"), String::NewFromUtf8(isolate, "success"));
            ret->Set(String::NewFromUtf8(isolate, "serialNumber"), String::NewFromUtf8(isolate, spend.getCoinSerialNumber().ToString(16).c_str()));               
        }
        else {
            ret->Set(String::NewFromUtf8(isolate, "status"), String::NewFromUtf8(isolate, "error"));
            ret->Set(String::NewFromUtf8(isolate, "serialNumber"), String::NewFromUtf8(isolate, ""));               
        }
         
        args.GetReturnValue().Set(ret);
        
    } catch (runtime_error &e) {
        isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(isolate, e.what())));
        return;
    }
}

void Initialize(Local<Object> exports) {
    NODE_SET_METHOD(exports, "mint", mint);
    NODE_SET_METHOD(exports, "validate", validate);
    NODE_SET_METHOD(exports, "spend", spend);
}

NODE_MODULE(module_name, Initialize)
