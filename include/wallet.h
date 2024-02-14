#include "libdogecoin.h"
#include "uthash.h"

#include <stdint.h>
#include <stddef.h>

typedef struct dogecoin_utxo_ {
    int index;
    uint256 txid;
    int vout;
    char address[P2PKHLEN];
    char script_pubkey[SCRIPT_PUBKEY_STRINGLEN];
    char height[KOINU_STRINGLEN];
    char amount[KOINU_STRINGLEN];
    int confirmations;
    dogecoin_bool spendable;
    dogecoin_bool solvable;
    UT_hash_handle hh;
} dogecoin_utxo;

DISABLE_WARNING_PUSH
DISABLE_WARNING(-Wunused-variable)
static dogecoin_utxo* utxos = NULL;
DISABLE_WARNING_POP

#define is_spent(x) (((dogecoin_utxo*)x)->spendable == false)

/** wallet utxo functions */
LIBDOGECOIN_API dogecoin_utxo* new_dogecoin_utxo();
LIBDOGECOIN_API dogecoin_utxo* dogecoin_wallet_utxo_new();
LIBDOGECOIN_API int start_dogecoin_utxo();
LIBDOGECOIN_API void add_dogecoin_utxo(dogecoin_utxo* utxo_external);
LIBDOGECOIN_API dogecoin_utxo* find_dogecoin_utxo(int index);
LIBDOGECOIN_API void remove_dogecoin_utxo(dogecoin_utxo* utxo);
LIBDOGECOIN_API void remove_all_utxos();
LIBDOGECOIN_API void dogecoin_wallet_utxo_free(dogecoin_utxo* utxo);
