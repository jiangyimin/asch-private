{
    "targets":[
        {
            "target_name":"asch-private",
            "sources": ["src/addon.cc", 
                "src/libzerocoin/Coin.cpp", "src/libzerocoin/CoinSpend.cpp", "src/libzerocoin/Commitment.cpp", 
                "src/libzerocoin/Params.cpp", "src/libzerocoin/ParamGeneration.cpp", 
                "src/libzerocoin/SerialNumberSignatureOfKnowledge.cpp", "src/libzerocoin/SpendMetaData.cpp", 
                "src/libzerocoin/Accumulator.cpp", "src/libzerocoin/AccumulatorProofOfKnowledge.cpp"],
            'cflags_cc': ['-fexceptions'],
            'link_settings': {
                'libraries': ['-lboost_system','-lcrypto'],
            }
        }
    ]
}