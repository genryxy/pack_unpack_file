// КДЗ по дисциплине Алгоритмы и структуры данных, 2019-2020 уч.год
// -------------------
// Краснов Александр Андреевич, группа БПИ-185, дата (06.04.2020)
// -------------------
// Среда разработки: CLion (c++11)
// Состав проекта: Huffman.h, Huffman.cpp, LZ77.h, LZ77.cpp, main.cpp
// -------------------
// Сделано: сжатие и распаковка методом Хаффмана / сжатие и распаковка
// методом Шеннона - Фано, сжатие и распаковка методом LZ77, проведен
// вычислительный эксперимент, построены таблицы и графики, для
// измерения времени выполнения использовалось ХХХ, оформлен отчет)
// -------------------
// НЕ сделано: сжатие и распаковка методом LZW

#include <iostream>
#include <cstring>
#include <ctime>
#include "Huffman.h"
#include "LZ77.h"

using std::to_string;

int main(int argc, char* argv[]) {
    const string path = "data/";
    const int number_attempt = 1;

    vector<string> file_names{"1", "2", "3", "4", "5", "6", "7", "8", "9", "10"};
    vector<string> file_extension{".txt", ".docx", ".pptx", ".pdf", ".dll", ".jpg", ".jpg", ".bmp", ".bmp", ".csv"};
    vector<int> dict_size{4 * 1024, 8 * 1024, 16 * 1024};
    vector<int> buffer_size{1 * 1024, 2 * 1024, 4 * 1024};
    Huffman huffman;

    ofstream fout("time_result.txt");
    double first;
    double second;

    cout << "\n\t-----Begin compress Huffman-----\n";
    fout << "\t-----Compress Huffman-----\n";
    for (int j = 0; j < file_names.size(); ++j) {
        string input = path + file_names[j] + file_extension[j];
        string output = path + "compressed/" + file_names[j] + ".haff";
        fout << "\tFile: " << input << " Huffman\n";
        for (int i = 0; i < number_attempt; ++i) {
            first = double(clock());
            huffman.huffmanCompress(input, output);
            second = double(clock());
            fout << i + 1 << ") " << (second - first) / CLOCKS_PER_SEC << " sec\n";
        }
        cout << "Compress " << input << "\n";
    }

    cout << "\n\t-----Begin decompress Huffman-----\n";
    fout << "\t-----Decompress Huffman-----\n";
    for (int j = 0; j < file_names.size(); ++j) {
        string input = path + "compressed/" + file_names[j] + ".haff";
        string output = path + "decompressed/Huffman/" + file_names[j] + "_" + file_extension[j] + ".unhaff";
        fout << "\tFile: " << input << " Huffman\n";
        for (int i = 0; i < number_attempt; ++i) {
            first = double(clock());
            huffman.huffmanDecompress(input, output);
            second = double(clock());
            fout << i + 1 << ") " << (second - first) / CLOCKS_PER_SEC << " sec\n";
        }
        cout << "Decompress " << input << "\n";
    }

    cout << "\n\t-----Begin compress LZ77-----\n";
    fout << "\n\t-----Compress LZ77-----\n";
    for (int k = 0; k < dict_size.size(); ++k) {
        LZ77 lz77 = LZ77(dict_size[k], buffer_size[k]);
        int window_size = (dict_size[k] + buffer_size[k]) / 1024;
        for (int j = 0; j < file_names.size(); ++j) {
            string input = path + file_names[j] + file_extension[j];
            string output = path + "compressed/" + file_names[j] + ".lz77" + to_string(window_size);
            fout << "\tFile: " << input << " LZ77" << window_size << "\n";
            for (int i = 0; i < number_attempt; ++i) {
                first = double(clock());
                lz77.lz77Compress(input, output);
                second = double(clock());
                fout << i + 1 << ") " << (second - first) / CLOCKS_PER_SEC << " sec\n";
            }
            cout << "Window_size:" << window_size << "; Compress " << input << "\n";
        }
    }

    cout << "\n\t-----Begin decompress LZ77-----\n";
    fout << "\t-----Decompress LZ77-----\n";
    for (int k = 0; k < dict_size.size(); ++k) {
        LZ77 lz77 = LZ77(dict_size[k], buffer_size[k]);
        int window_size = (dict_size[k] + buffer_size[k]) / 1024;
        for (int j = 0; j < file_names.size(); ++j) {
            string input = path + "compressed/" + file_names[j] + ".lz77" + to_string(window_size);
            string output = path + "decompressed/LZ77" + to_string(window_size) + "/" + file_names[j] + "_" +
                            file_extension[j] + ".unlz77" + to_string(window_size);
            fout << "\tFile: " << input << " LZ77\n";
            for (int i = 0; i < number_attempt; ++i) {
                first = double(clock());
                lz77.lz77Decompress(input, output);
                second = double(clock());
                fout << i + 1 << ") " << (second - first) / CLOCKS_PER_SEC << " sec\n";
            }
            cout << "Window_size:" << window_size << "; Decompress " << input << "\n";
        }
    }
    fout << "\n\t-----End-----";
    fout.close();

//    auto temp1 = double(clock());
//    huffman.huffmanCompress(argv[3], argv[4]);
//    auto temp2 = double(clock());
//    huffman.huffmanDecompress(argv[4], "outBig.jpg");
//    auto temp3 = double(clock());
//    cout << "Time pack: " << (temp2 - temp1) / CLOCKS_PER_SEC << "sec\n";
//    cout << "Time unpack: " << (temp3 - temp2) / CLOCKS_PER_SEC << "sec\n";
//
//    auto temp1 = double(clock());
//    lz77.lz77Compress(argv[3], argv[4]);
//    auto temp2 = double(clock());
//    lz77.lz77Decompress(argv[4], "my_out.bmp");
//    auto temp3 = double(clock());
//    cout << "Time pack: " << (temp2 - temp1) / CLOCKS_PER_SEC << "sec\n";
//    cout << "Time unpack: " << (temp3 - temp2) / CLOCKS_PER_SEC << "sec\n";

//    if (argc < 5) {
//        std::cout << "Usage: " << argv[0] << " [ALGO] [OPTION] [INPUT_FILE] [OUTPUT_FILE]" << std::endl;
//        std::cout << "Algo:" << std::endl;
//        std::cout << "  haff\tHuffman" << std::endl;
//        std::cout << "  lz77\tLZ77" << std::endl;
//        std::cout << "Options:" << std::endl;
//        std::cout << "  -c\tCompress" << std::endl;
//        std::cout << "  -d\tDecompress" << std::endl;
//    } else {
//        // Huffman
//        if (!std::strcmp(argv[1], "haff") && !std::strcmp(argv[2], "-c")) {
//            Huffman huffman;
//            auto temp1 = double(clock());
//            huffman.huffmanCompress(argv[3], argv[4]);
//            auto temp2 = double(clock());
//            huffman.huffmanDecompress(argv[4], "outBig.jpg");
//            auto temp3 = double(clock());
//            cout << "Time pack: " << (temp2 - temp1) / CLOCKS_PER_SEC << "sec\n";
//            cout << "Time unpack: " << (temp3 - temp2) / CLOCKS_PER_SEC << "sec\n";
//        }
//        if (!std::strcmp(argv[1], "lz77") && !std::strcmp(argv[2], "-c")) {
//            LZ77 lz77 = LZ77(4 * 1024, 1 * 1024);
//            auto temp1 = double(clock());
//            lz77.lz77Compress(argv[3], argv[4]);
//            auto temp2 = double(clock());
//            lz77.lz77Decompress(argv[4], "my_out.bmp");
//            auto temp3 = double(clock());
//            cout << "Time pack: " << (temp2 - temp1) / CLOCKS_PER_SEC << "sec\n";
//            cout << "Time unpack: " << (temp3 - temp2) / CLOCKS_PER_SEC << "sec\n";
//        }
//        if (!std::strcmp(argv[1], "haff") && !std::strcmp(argv[2], "-d")) {
//            Huffman huffman;
//            huffman.huffmanDecompress(argv[3], argv[4]);
//        }
//    }
    return 0;
}