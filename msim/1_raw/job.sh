for i in {0..6}
do
  ../msim raw$i.x ../data > out
  echo "raw$i done."
done

mv epc epc_raw
mv cycles cycles_raw

for i in {0..6}
do
  ../msim uraw$i.x ../data > out
  echo "uraw$i done."
done

mv epc epc_uraw
mv cycles cycles_uraw

for i in {0..6}
do
  ../msim u2raw$i.x ../data > out
  echo "u2raw$i done."
done

mv epc epc_u2raw
mv cycles cycles_u2raw

for i in {0..6}
do
  ../msim curaw$i.x ../data > out
  echo "curaw$i done."
done

mv epc epc_curaw
mv cycles cycles_curaw

for i in {0..6}
do
  ../msim short_raw$i.x ../data > out
  echo "short_raw$i done."
done

mv epc epc_short_raw
mv cycles cycles_short_raw

for i in {0..6}
do
  ../msim short_uraw$i.x ../data > out
  echo "short_uraw$i done."
done

mv epc epc_short_uraw
mv cycles cycles_short_uraw

for i in {0..6}
do
  ../msim short_u2raw$i.x ../data > out
  echo "short_u2raw$i done."
done

mv epc epc_short_u2raw
mv cycles cycles_short_u2raw

for i in {0..6}
do
  ../msim short_curaw$i.x ../data > out
  echo "short_curaw$i done."
done

mv epc epc_short_curaw
mv cycles cycles_short_curaw

python plot.py

rm out
rm cycles_*
rm epc_*
