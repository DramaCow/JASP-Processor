rm data
rm bpa

for k in {1..12}
do
  python memgen.py $k
  for i in {0..9}
  do
    ../msim ham.x data > out
    echo "ham k=$k done."
  done
done

python plot.py
