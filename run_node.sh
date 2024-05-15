bft_bin=/home/flt/workspace/bitcoin/cmake-build-debug/ruccoin/bft_main
coin_bin=/home/flt/workspace/bitcoin/cmake-build-debug/ruccoin/coin_node
conf=/home/flt/workspace/bitcoin/ruccoin/config_json/config.json
for n in {0..3}
do
  $bft_bin -i "$n" -f $conf &
  $coin_bin -i "$n" -f $conf &
done
