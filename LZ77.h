//
// Created by Admin on 05.04.2020.
//

#ifndef KRASNOV_KDZ_COMPRESSION_LZ77_H
#define KRASNOV_KDZ_COMPRESSION_LZ77_H


#include <iostream>
#include <fstream>
#include <map>
#include <vector>
#include <algorithm>
#include <cstring>


using std::string;
using std::vector;
using std::ifstream;
using std::ofstream;
using std::sort;
using std::pair;
using std::cout;
using std::min;
using std::max;
using std::make_pair;

typedef unsigned char uchar;
typedef unsigned int uint;
typedef unsigned short ushort;

class LZ77 {

private:
    vector<ushort> offset_vec;
    vector<ushort> length_vec;
    vector<uchar> next_char;
    vector<uchar> input_chars;
    int dictSize;
    int bufferSize;

public:

    /**
     * Конструктор класса. dictSize + bufferSize == скользящее окно.
     * @param dictSize Размер словаря.
     * @param bufferSize Размер буфера предпросмотра.
     */
    LZ77(int dictSize, int bufferSize) {
        this->dictSize = dictSize;
        this->bufferSize = bufferSize;
    }

    /**
    * Функция сжатия LZ77.
    * @param input Путь до .* файла.
    * @param output Путь до .lz77* файла.
    */
    void lz77Compress(string input, string output);

    /**
    * Функция распаковки LZ77.
    * @param input Путь до .lz77* файла.
    * @param output Путь до .* файла.
    */
    void lz77Decompress(string input, string output);

private:

    /**
     * Получаем срез элементов вектора.
     * @param vec Вектор, срезы которого нужно получить.
     * @param start Начальная позиция.
     * @param end Индекс элемента, следующего за последним записанным.
     * @return Вектор, содержащий элементы вектора vec из заданного диапазона.
     */
    static vector<uchar> getSlice(const vector<uchar> &vec, int start, int end);

    /**
     * Ищем наибольшое совпадение символов из буфера предпросмотра с символами
     * из словаря (при наложении и из буфера тоже) в скользящем окне.
     * @param dictionary Словарь.
     * @param buffer Буфер предпросмотра.
     * @return Пара вида (смещение, длина совпадения).
     */
    static pair<ushort, ushort> getMatching(const vector<uchar> &dictionary, const vector<uchar> &buffer);

    /**
     * Кодируем прочитанные символы тройками вида (смещение, длина совпадения, символ), где
     * символ - первый символ буфера предпросмотра после найденного фрагмента
     */
    void lz77Pack();

    /**
     * Декодируем тройки, записанные в полях класса. Результат записываем
     * в выходной файл.
     * @param fout Выходной .* файл.
     */
    void lz77Unpack(ofstream &fout);

    /**
     * Создаём вектор с символами из файла. В режиме compressed == true читаем данные
     * из закодированного файла.
     * @param fin Путь до файла.
     * @param isCompressed Открыть файл для чтения с закодированными данными.
     * @return Вектор содержащий символы из файла. Если isCompressed, то пустой вектор,
     * так как тогда считывание происходит в поля из класса.
     */
    vector<uchar> createVectorWithCharsLZ77(ifstream &fin, bool isCompressed);

    /**
     * Создаем сжатый .lz77* файл.
     * @param fout Выходной .lz77* файл.
     */
    void createCompressedLZ77File(ofstream &fout);

    /**
     * Сбрасываем значения полей класса.
     */
    void resetValues() {
        offset_vec.clear();
        length_vec.clear();
        next_char.clear();
        input_chars.clear();
    }
};


#endif //KRASNOV_KDZ_COMPRESSION_LZ77_H
