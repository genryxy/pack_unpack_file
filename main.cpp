#include <iostream>
#include <cstring>
#include <ctime>
#include "Huffman.h"
#include "LZ77.h"


int main(int argc, char* argv[]) {
    if (argc < 5) {
        std::cout << "Usage: " << argv[0] << " [ALGO] [OPTION] [INPUT_FILE] [OUTPUT_FILE]" << std::endl;
        std::cout << "Algo:" << std::endl;
        std::cout << "  haff\tHuffman" << std::endl;
        std::cout << "  lz77\tLZ77" << std::endl;
        std::cout << "Options:" << std::endl;
        std::cout << "  -c\tCompress" << std::endl;
        std::cout << "  -d\tDecompress" << std::endl;
    } else {
        // Huffman
        if (!std::strcmp(argv[1], "haff") && !std::strcmp(argv[2], "-c")) {
            Huffman huffman;
            auto temp1 = double(clock());
            huffman.huffmanCompress(argv[3], argv[4]);
            auto temp2 = double(clock());
            huffman.huffmanDecompress(argv[4], "kdz_output.pdf");
            auto temp3 = double(clock());
            cout << "Time pack: " << (temp2 - temp1) / CLOCKS_PER_SEC << "sec\n";
            cout << "Time unpack: " << (temp3 - temp2) / CLOCKS_PER_SEC << "sec\n";
        }
        if (!std::strcmp(argv[1], "lz77") && !std::strcmp(argv[2], "-c")) {
            LZ77 lz77 = LZ77(4 * 1024, 1 * 1024);
            auto temp1 = double(clock());
            lz77.lz77Compress(argv[3], argv[4]);
            auto temp2 = double(clock());
            lz77.lz77Decompress(argv[4], "kdz_output.pdf");
            auto temp3 = double(clock());
            cout << "Time pack: " << (temp2 - temp1) / CLOCKS_PER_SEC << "sec\n";
            cout << "Time unpack: " << (temp3 - temp2) / CLOCKS_PER_SEC << "sec\n";
        }
        if (!std::strcmp(argv[1], "haff") && !std::strcmp(argv[2], "-d")) {
            Huffman huffman;
            huffman.huffmanDecompress(argv[3], argv[4]);
        }
    }
    return 0;
}