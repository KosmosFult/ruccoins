#!/bin/bash


if [ "$#" -ne 1 ]; then
    echo "Usage: $0 <node_index>"
    exit 1
fi


node_index=$1


bft_bin="/home/flt/workspace/bitcoin/cmake-build-debug/ruccoin/bft_main"
coin_bin="/home/flt/workspace/bitcoin/cmake-build-debug/ruccoin/coin_node"
conf="/home/flt/workspace/bitcoin/ruccoin/config_json/config.json"


echo "Starting BFT node with index $node_index..."
$bft_bin -i "$node_index" -f "$conf" &

sleep 1


echo "Starting Coin node with index $node_index..."
$coin_bin -i "$node_index" -f "$conf" &

echo "Node $node_index started."