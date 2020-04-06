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
#include <ctime>
#include "Huffman.h"
#include "LZ77.h"

using std::to_string;
using std::replace;

/**
 * 
 * @param name Путь до входного файла.
 * @param fout_frequency Выходной .txt файл со значениями частот.
 * @param fout_size Выходной .csv файл со значениями размеров файлов (исходных + сжатых).
 */
void writeFrequencyAndSizeOriginal(const string &name, ofstream &fout_frequency, ofstream &fout_size) {
    // Открываем для чтения в бинарном режиме.
    ifstream fin(name, std::ios::binary | std::ios::in);
    uchar temp_char;
    uint total = 0;

    map<uchar, uint> frequency_table;
    while (fin.read((char*) &temp_char, sizeof(temp_char))) {
        frequency_table[temp_char]++;
        total++;
    }

    for (int i = 0; i < 256; ++i) {
        auto pos = frequency_table.find(i);
        fout_frequency << /*i << ":" <<*/ (pos == frequency_table.end() ? 0 : pos->second) << "\n";
    }
    fout_frequency << "Total:" << total << "\n";
    // Хотим получить значение в мегабайтах.
    string tmp = to_string(total / 1024.0 / 1024);
    replace(tmp.begin(), tmp.end(), '.', ',');
    fout_size << name << ";" << tmp << "\n";

    fin.close();
}

/**
 * Вычисляем размер каждого сжатого файла. 
 * @param name Путь до входного файла.
 * @param Выходной .csv файл со значениями размеров файлов (исходных + сжатых).
 */
void writeSizeCompressed(const string &name, ofstream &fout_size) {
    ifstream file(name, std::ios::binary | std::ios::in);
    file.seekg(0, std::ios::end);
    // Хотим получить значение в мегабайтах.
    string tmp = to_string(file.tellg() / 1024.0 / 1024);
    replace(tmp.begin(), tmp.end(), '.', ',');
    fout_size << name << ";" << tmp << "\n";

    file.close();
}

int main(int argc, char* argv[]) {
    const string path = "data/";
    // Количество прогонов для каждого файла.
    // Нужно сделать +1, т. к. считаем с 0.
    const int number_attempt = 10;

    vector<string> file_names{"1", "2", "3", "4", "5", "6", "7", "8", "9", "10"};
    vector<string> file_extensions{".txt", ".docx", ".pptx", ".pdf", ".dll", ".jpg", ".jpg", ".bmp", ".bmp", ".csv"};
    vector<string> compressed_extensions{".haff", ".lz775", ".lz7710", ".lz7720"};

    vector<int> dict_size{4 * 1024, 8 * 1024, 16 * 1024};
    vector<int> buffer_size{1 * 1024, 2 * 1024, 4 * 1024};
    Huffman huffman;

    // Вычисляем частоты для символов исходных файлов.
    // Вычисляем размеры исходных файлов.
    ofstream fout_frequency("info/frequency.txt");
    ofstream fout_size("info/size.csv");
    for (int i = 0; i < file_names.size(); ++i) {
        const string name = path + file_names[i] + file_extensions[i];
        writeFrequencyAndSizeOriginal(name, fout_frequency, fout_size);
    }
    fout_frequency.close();


    // Вычисляем время работы алгоритмов на разных файлах.
    for (auto extension:compressed_extensions) {
        for (auto name:file_names) {
            const string full_name = path + "compressed/" + name + extension;
            writeSizeCompressed(full_name, fout_size);
        }
    }
    fout_size.close();

    // Вычисляем время работы каждого из алгоритмов на разных файлах.
    // Сначала время упаковки для Хаффмана, потом время декодирования.
    // Затем аналогично считаем для трёх модификаций алгоритма LZ77.
    // количество прогонов для каждого файла - (number_attempt+1)
    ofstream fout("info/time_result.txt");
    double first;
    double second;
    cout << "\n\t-----Begin compress Huffman-----\n";
    fout << "\t-----Compress Huffman-----\n";
    for (int j = 0; j < file_names.size(); ++j) {
        string input = path + file_names[j] + file_extensions[j];
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
        string output = path + "decompressed/Huffman/" + file_names[j] + "_" + file_extensions[j] + ".unhaff";
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
            string input = path + file_names[j] + file_extensions[j];
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
                            file_extensions[j] + ".unlz77" + to_string(window_size);
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

    return 0;
}