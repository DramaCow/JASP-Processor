rm cycles*
rm out

for c in {0..2}
do
  cd ../
  make clean
  sed -i 's/define CACHELEVELS\ [0-9][0-9]*/define\ CACHELEVELS\ '"$c"'/' src/config.hpp
  make
  cd 4_stride

  for s in {2..32..2}
  do
    sed -i 's/movi\ r4\ [0-9][0-9]*/movi\ r4\ '"$s"'/' stride.x
    ../msim stride.x data > out
    echo "stride k=$s done."
  done

  mv cycles cycles_$c
done

python plot.py
