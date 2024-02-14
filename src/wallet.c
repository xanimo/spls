#include "wallet.h"

dogecoin_utxo* dogecoin_wallet_utxo_new() {
    dogecoin_utxo* utxo = calloc(1, sizeof(dogecoin_utxo));
    memset(utxo->txid, 0, 32);
    utxo->vout = 0;
    memset(&utxo->address, 0, sizeof(utxo->address));
    memset(&utxo->script_pubkey, 0, sizeof(utxo->script_pubkey));
    memset(&utxo->height, 0, sizeof(utxo->height));
    memset(&utxo->amount, 0, sizeof(utxo->amount));
    utxo->confirmations = 0;
    utxo->spendable = true;
    utxo->solvable = true;
    return utxo;
}

dogecoin_utxo* new_dogecoin_utxo() {
    dogecoin_utxo* utxo = dogecoin_wallet_utxo_new();
    utxo->index = HASH_COUNT(utxos) + 1;
    return utxo;
}

int start_dogecoin_utxo() {
    dogecoin_utxo* m = new_dogecoin_utxo();
    add_dogecoin_utxo(m);
    return m->index;
}

void add_dogecoin_utxo(dogecoin_utxo* utxo_external) {
    dogecoin_utxo* utxo_internal;
    HASH_FIND_INT(utxos, &utxo_external->index, utxo_internal);
    if (utxo_internal == NULL) {
        HASH_ADD_INT(utxos, index, utxo_external);
    } else {
        HASH_REPLACE_INT(utxos, index, utxo_external, utxo_internal);
    }
    dogecoin_free(utxo_internal);
}

dogecoin_utxo* find_dogecoin_utxo(int index) {
    dogecoin_utxo* utxo;
    HASH_FIND_INT(utxos, &index, utxo);
    return utxo;
}

void remove_dogecoin_utxo(dogecoin_utxo* utxo) {
    HASH_DEL(utxos, utxo);
    dogecoin_free(utxo);
}

void remove_all_utxos() {
    dogecoin_utxo* utxo;
    dogecoin_utxo* tmp;
    HASH_ITER(hh, utxos, utxo, tmp) {
        remove_dogecoin_utxo(utxo);
    }
}

void dogecoin_wallet_utxo_free(dogecoin_utxo* utxo) {
    dogecoin_free(utxo);
}
