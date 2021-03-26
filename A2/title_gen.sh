#! /usr/bin/env bash
i=0
for Ki in "1" "4"; do
    for Km in "1.0" "1.5"; do
        for Kn in "0.5" "1.0"; do
            for Kf in "0.1" "0.3"; do
                for Ps in "0.01" "0.0001"; do
                    ((i=i+1))
                    echo '\begin{figure}[H]'
                    echo '\centering'
                    echo '\includegraphics[scale = 0.5]' "{${i}.txt.png}"
                    echo '\caption{'"\$ K_i = ${Ki} , K_m = ${Km} , K_n = ${Kn} , K_f = ${Kf} , P_s = ${Ps} \$"'}'
                    echo '\label{'"${i}"'}'
                    echo '\end{figure}'
                done
            done
        done
    done
done

