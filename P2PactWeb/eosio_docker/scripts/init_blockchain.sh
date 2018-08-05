#!/usr/bin/env bash

echo "=== setup blockchain accounts and smart contract ==="

# change to executable directory
cd "/opt/eosio/bin"

set -m

# start nodeos ( local node of blockchain )
# run it in a background job such that docker run could continue
nodeos -e -p eosio -d /mnt/dev/data --config-dir /mnt/dev/config --http-validate-host=false --plugin eosio::wallet_api_plugin --plugin eosio::wallet_plugin --plugin eosio::producer_plugin --plugin eosio::history_plugin --plugin eosio::chain_api_plugin --plugin eosio::history_api_plugin --plugin eosio::http_plugin --http-server-address=0.0.0.0:8888 --access-control-allow-origin=* --contracts-console &
sleep 1s
  until curl localhost:8888/v1/chain/get_info
do
  sleep 1s
done

# Sleep for 2 to allow time 4 blocks to be created so we have blocks to reference when sending transactions
sleep 2s
echo "=== setup wallet: eosiomain ==="
# First key import is for eosio system account
./cleos wallet create -n eosiomain | tail -1 | sed -e 's/^"//' -e 's/"$//' > eosiomain_wallet_password.txt
./cleos wallet import -n eosiomain --private-key 5KQwrPbwdL6PhXujxW37FSSQZ1JiwsST4cqQzDeyXtP79zkvFD3

echo "=== setup wallet: notechainwal ==="
# key for eosio account and export the generated password to a file for unlocking wallet later
./cleos wallet create -n p2pactwal | tail -1 | sed -e 's/^"//' -e 's/"$//' > p2pact_wallet_password.txt
# Owner key for notechainwal wallet
./cleos wallet import -n p2pactwal --private-key 5JpWT4ehouB2FF9aCfdfnZ5AwbQbTtHBAwebRXt94FmjyhXwL4K
# Active key for notechainwal wallet
./cleos wallet import -n p2pactwal --private-key 5JD9AGTuTeD5BXZwGQ5AtwBqHK21aHmYnTetHgk1B3pjj7krT8N

# import two more keys for the eosio.token account
./cleos wallet import -n p2pactwal --private-key 5JK96uyjnEw51FUuhA9GDX5WxkXW1f5TwzxNwQBYgvpu95TNcMT
./cleos wallet import -n p2pactwal --private-key 5KX1HpUF8jC5QMJn6Zx3MZHaAmhopgSyQgwPDEF5FRJfRK3edhE

# * Replace "notechainwal" by your own wallet name when you start your own project

# create account for notechainacc with above wallet's public keys
./cleos create account eosio p2pactacc EOS6PUh9rs7eddJNzqgqDx1QrspSHLRxLMcRdwHZZRL4tpbtvia5B EOS8BCgapgYA2L4LJfCzekzeSr3rzgSTUXRXwNi8bNRoz31D14en9

# create another account for the eosio.token contract
./cleos create account eosio eosio.token EOS8PFimbFFcV8DTCsJ2JXDrfDUT2dAz11nFseAxCaHUf82FphrpZ EOS6QGacB2S8C6knqYDTQ4H4CS4w5SUJptB4p3jrjCDdD5hQLQC9F

# * Replace "notechainacc" by your own account name when you start your own project

echo "=== deploy smart contract ==="
# $1 smart contract name
# $2 account holder name of the smart contract
# $3 wallet for unlock the account
# $4 password for unlocking the wallet
./scripts/deploy_contract.sh Proposals p2pactacc p2pactwal $(cat p2pact_wallet_password.txt)
./scripts/deploy_contract.sh eosio.token eosio.token p2pactwal $(cat p2pact_wallet_password.txt)

./cleos push action eosio.token create "[ \"eosio\", \"1000000000.0000 TOK\"]" -p eosio.token

echo "=== create user accounts ==="
# script for create data into blockchain
./scripts/create_accounts.sh

# * Replace the script with different form of data that you would pushed into the blockchain when you start your own project

echo "=== end of setup blockchain accounts and smart contract ==="
# create a file to indicate the blockchain has been initialized
touch "/mnt/dev/data/initialized"

# put the background nodeos job to foreground for docker run
fg %1
