#include <cstdlib>
#include <iostream>
#include <fstream>

int main(int argc, char *argv[])
{
    if (argc < 4)
    {
        std::cerr << "Not enough arguments\n";
        return -1;
    }
    const char *model_list_path = argv[1];
    const char *seq_path = argv[2];
    const char *output_result_path = argv[3];
    return 0;
}
