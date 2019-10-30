for ((i=70; i<510; i+=10)); do
    for ((j=1; j<6; j++)); do
        ./train "$i" ./model_init.txt "./data/train_seq_0$j.txt" "model_0$j.txt";
    done
    ./test modellist.txt ./data/test_seq.txt test.log;
    printf "$i ";
    diff <(cut -d' ' -f1 ./test.log) ./data/test_lbl.txt | grep "^>" | wc -l;
done
