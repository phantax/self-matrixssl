#!/bin/bash


# The number of AFL runs
N=2

# The number of generations per AFL run
M=1000


for n in $(seq $N); do

    DIR=output-$n
    INST=afl-fuzz-$n

    # Launch AFL
    echo "Launching AFL (-> $DIR) ..."
    screen -d -m -S $INST ./launch-afl-fuzz.sh $DIR

    while true; do

        sleep 5
        echo "* check ..."

        m=$(cat stimuli_*.txt | wc -l)

        if [ "$m" -ge $M ]; then
            echo "  ... stop ($m/$M)!"
            screen -X -S $INST stuff "^C"
            sleep 1
            rm -r $DIR
            break
        fi

    done


    DIR=$(printf 'iteration-%03i' $n)

    echo "* collecting stimuli (-> $DIR) ..."
    mkdir $DIR

    if [[ "$(ls stimuli_0*.txt 2>/dev/null)" != "" ]]; then
        MAX=$(ls stimuli_0*.txt | cut -d "_" -f2 | cut -d "." -f1 | sort -nr | head -n1)
        NEXT=$((10#$MAX + 1))
    else
        NEXT=0
    fi
    mv stimuli_head.txt $(printf 'stimuli_%08i.txt' $NEXT)

    mv stimuli_*.txt $DIR

done
