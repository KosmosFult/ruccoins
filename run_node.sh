bft_bin=/home/flt/workspace/bitcoin/cmake-build-debug/ruccoin/bft_main
conf=/home/flt/workspace/bitcoin/ruccoin/config_json/config.json
for n in {1..3}
do
  $bft_bin -i "$n" -f $conf &
done
