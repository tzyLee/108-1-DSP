OUTPUT_DIR=./output

echo "Mine";
time for input in ./test_data/*.txt.seg; do
    ./mydisambig $input ZhuYin-Big5.map languageModel "$OUTPUT_DIR/$(basename $input).out" 2> /dev/null
done

echo "=====================";

echo "Reference";
time for input in ./test_data/*.txt.seg; do
    disambig -text $input -map ZhuYin-Big5.map -lm languageModel -order 2 > "$OUTPUT_DIR/$(basename $input).ref" 2> /dev/null
done

echo "=====================";

for input in ./test_data/*.txt.seg; do
    base="$OUTPUT_DIR/$(basename $input)";
    diff "$base.out" "$base.ref" || echo "$base.out and $base.ref differs!";
    echo "=====================";
done