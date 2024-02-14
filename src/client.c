#include "wallet.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include <inttypes.h>
#include <unistd.h>

static struct option long_options[] = {
    {"address", required_argument, NULL, 'a'},
    {"pubkey_hash", required_argument, NULL, 'p'},
    {"detect", no_argument, NULL, 'd'},
    {"testnet", no_argument, NULL, 't'},
    {"regtest", no_argument, NULL, 'r'},
    {"version", no_argument, NULL, 'v'},
    {NULL, 0, NULL, 0}
};

static void print_version() {
    printf("Version: %s %s\n", "spls", "0.0.1");
}

static void print_usage() {
    print_version();
    printf("Usage: spls -c <cmd> (-d|-detect <detect>) (-a|-address <address>) (-p|-pubkey_hash <pubkey_hash>) (-t[--testnet]) (-r[--regtest])\n");
    printf("Available commands:\n");
    printf("convert (requires either -a <address> or -p <pubkey_hash>),\n");
}

static bool showError(const char* er)
    {
    printf("Error: %s\n", er);
    dogecoin_ecc_stop();
    return 1;
    }

int get_index(char* string, char c) {
    char *e = strchr(string, c);
    if (e == NULL) {
        return -1;
    }
    return (int)(e - string);
}

void slice_str(const char *str, char *result, size_t start, size_t end)
{
    strncpy(result, str + start, end - start);
}

void print_utxo(dogecoin_utxo *utxo) {
    printf("%s\n", "----------------------");
    printf("txid:           %s\n", utils_uint8_to_hex(utxo->txid, sizeof utxo->txid));
    printf("vout:           %d\n", utxo->vout);
    printf("address:        %s\n", utxo->address);
    printf("script_pubkey:  %s\n", utxo->script_pubkey);
    printf("height:         %s\n", utxo->height);
    printf("amount:         %s\n", utxo->amount);
    debug_print("confirmations:  %d\n", utxo->confirmations);
    printf("spendable:      %d\n", utxo->spendable);
    printf("solvable:       %d\n", utxo->solvable);
}

int main(int argc, char **argv) {
    int long_index = 0;
    int opt = 0;
    char* pubkey_hash = 0;
    char* address = 0;
    char* cmd = 0;
    char* input = 0;
    char* value = 0;

    memset(&pubkey_hash, 0, sizeof(pubkey_hash));
    const dogecoin_chainparams* chain = &dogecoin_chainparams_main;

    while ((opt = getopt_long_only(argc, argv, "a:p:c:d:f:trv", long_options, &long_index)) != -1) {
        switch (opt) {
            case 'a':
                address = optarg;
                break;
            case 'p':
                pubkey_hash = optarg;
                if (strlen(pubkey_hash) < 20)
                    return showError("pubkey hash must be greater than 20 bytes");
                break;
            case 'c':
                cmd = optarg;
                break;
            case 'd':
                input = optarg;
                break;
            case 'f':
                value = optarg;
                break;
            case 't':
                chain = &dogecoin_chainparams_test;
                break;
            case 'r':
                chain = &dogecoin_chainparams_regtest;
                break;
            case 'v':
                print_version();
                exit(EXIT_SUCCESS);
                break;
            default:
                print_usage();
                exit(EXIT_FAILURE);
        }
    }

    if (!cmd) {
        /* exit if no command was provided */
        print_usage();
        exit(EXIT_FAILURE);
    }

	dogecoin_ecc_start();
    
    if (strcmp(cmd, "detect") == 0) {
        if (input == NULL) {
            printf("No arguments provided!\n");
            exit(EXIT_FAILURE);
        } else {
            char prefix = input[0];
            size_t length = strlen(input);
            if (strcmp(&prefix, "D")==0) {
                if (length == 34) {
                    printf("-a %s\n", input);
                }
            } 
            if (strcmp(&prefix, "7")==0) {
                if (length == 50) {
                    printf("-p %s\n", input);
                }
            }
        }
    } else if (strcmp(cmd, "convert") == 0) {
        char sanitized_pubkey_hash[51];
        
        if (address != NULL) {
            dogecoin_p2pkh_address_to_pubkey_hash(address, sanitized_pubkey_hash);
            printf("%s\n", sanitized_pubkey_hash);
        };

        if (pubkey_hash != NULL) {
            size_t input_length = strlen(pubkey_hash) / 2;
            char p2pkh[P2PKHLEN];

            if (input_length > 20 && input_length <= 25) {
                memcpy(sanitized_pubkey_hash, pubkey_hash, 51);
            } else if (input_length == 20) {
                sprintf(sanitized_pubkey_hash, "%02x%02x%02x%.40s%02x%02x", 118, 169, 20, pubkey_hash, 136, 172);
            }

            sanitized_pubkey_hash[51] = '\0';

            dogecoin_pubkey_hash_to_p2pkh_address(utils_hex_to_uint8(sanitized_pubkey_hash), strlen(sanitized_pubkey_hash) / 2, p2pkh, chain);
            printf("%s\n", sanitized_pubkey_hash);
        }
    } else if (strcmp(cmd, "coins") == 0) {
        if (value == NULL) {
            printf("No arguments provided!\n");
            exit(EXIT_FAILURE);
        } else {
            char* endptr, output[21];
            uint64_t koinu = strtoimax(value, &endptr, 0);
            koinu_to_coins_str(koinu, output);
            printf("%s\n", output);
        }
    } else if (strcmp(cmd, "koinu") == 0) {
        if (value == NULL) {
            printf("No arguments provided!\n");
            exit(EXIT_FAILURE);
        } else {
            uint64_t koinu = coins_to_koinu_str(value);
            printf("%" PRIu64 "\n", koinu);
        }
    } else if (strcmp(cmd, "import")==0) {
        if (address != NULL) {
            FILE* fp;
            fp = fopen(address, "r");
            if (fp == NULL) {
            perror("Failed: ");
            return 1;
            }

            int count = 0;
            char buffer[256];
            dogecoin_utxo *utxo = new_dogecoin_utxo();
            while (fgets(buffer, 256, fp))
            {
                buffer[strcspn(buffer, "\n")] = 0;
                char token[256];
                int index = get_index(buffer, ':');
                uint256 tx_hash;
                if (index != -1) {
                    slice_str(buffer, token, index + 2, 70);
                    char *endptr;
                    switch (count % 6)
                    {
                    case 0:
                        utxo = new_dogecoin_utxo();
                        memcpy(&tx_hash, utils_hex_to_uint8(token), 32);
                        memcpy(utxo->txid, &tx_hash, 32);
                        break;
                    case 1:
                        utxo->vout = strtol(token, &endptr, 10);
                        break;
                    case 2:
                        strcpy(utxo->address, token);
                        break;
                    case 3:
                        memcpy(utxo->script_pubkey, token, 50);
                        break;
                    case 4:
                        memcpy(utxo->height, token, strlen(token));
                        break;
                    case 5:
                        memcpy(utxo->amount, token, strlen(token));
                        print_utxo(utxo);
                        free(utxo);
                        break;
                    default:
                        break;
                    }
                    count++;
                }
            }
            fclose(fp);
            return 0;
        };
    }
    dogecoin_ecc_stop();
    return 0;
}
