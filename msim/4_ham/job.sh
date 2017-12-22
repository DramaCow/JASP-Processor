rm data
rm bpa

for m in {0..4}
do
  cd ../
  sed -i 's/define\ BP_METHOD\ [0-9][0-9]*/define\ BP_METHOD\ '"$m"'/' src/config.hpp
  make clean
  make
  cd 4_ham

  for k in {1..12}
  do
    for i in {0..9}
    do
      python memgen.py $k
      ../msim ham.x data > out
      echo "ham k=$k done."
    done
  done
  
  mv bpa bpa_$m
done

python plot.py
