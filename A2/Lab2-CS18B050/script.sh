#!/usr/bin/env bash
i=0
mkdir -p outputs
mkdir -p plots
g++ -std=c++11 cw.cpp -o cw
for Ki in "1" "4"; do
    for Km in "1.0" "1.5"; do
        for Kn in "0.5" "1.0"; do
            for Kf in "0.1" "0.3"; do
                for Ps in "0.01" "0.0001"; do
                    ((i=i+1))
                    ./cw -i $Ki -m $Km -n $Kn -f $Kf -s $Ps -T 3000 -o "${Ki}_${Km}_${Kn}_${Kf}_${Ps}"
                done
            done
        done
    done
done
rm cw
