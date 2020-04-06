#ifndef KRASNOV_KDZ_COMPRESSION_HUFFMAN_H
#define KRASNOV_KDZ_COMPRESSION_HUFFMAN_H


#include <iostream>
#include <fstream>
#include <map>
#include <vector>
#include <algorithm>
#include <cstring>


using std::string;
using std::map;
using std::vector;
using std::ifstream;
using std::ofstream;
using std::sort;
using std::pair;
using std::cout;

typedef unsigned char uchar;
typedef unsigned int uint;

/**
 * Вершина дерева.
 */
struct Node {
    uchar value = '\0';
    uint frequency = 0;
    Node* left = nullptr;
    Node* right = nullptr;
};

class Huffman {

private:
    // Количество символов в файле.
    uint num = 0;
    /**
    * Переменная для хранения кода символа.
    */
    vector<int> temp_code;
    // Таблица частот. Словарь вида (символ, частота).
    map<uchar, uint> frequency_table;
    // Коды символов. Словарь вида (символ, код символа).
    map<uchar, vector<int>> codes;

public:

    /**
    * Функция сжатия Хаффмана.
    * @param input Путь до .* файла.
    * @param output Путь до .haff файла.
    */
    void huffmanCompress(string input, string output);

    /**
    * Функция распаковки Хаффмана.
    * @param input Путь до .haff файла.
    * @param output Путь до .* файла.
    */
    void huffmanDecompress(string input, string output);

private:

    /**
    * Метод для создания новой вершины с заданными значениями.
    * @param value Значение символа.
    * @param frequency Количество символов в тексте.
    * @param left Левый ребёнок.
    * @param right Правый ребёнок.
    * @return Новая вершина с заданными значениями.
    */
    static Node* getNewNode(uchar value, uint frequency, Node* left, Node* right);

    /**
    * Метод для поиска индекса, на который нужно вставить вершину.
    * @param arrNodes Вектор, содержащий вершины.
    * @param l Левая граница.
    * @param r Правая граница.
    * @param nodeFreq Значение, полученное из суммы двух меньших (по
    * значению frequency) вершин.
    * @return Позиция в векторе, на которую нужно вставить созданную вершину.
    */
    static int binarySearch(Node** arrNodes, int l, int r, uint nodeFreq);

    /**
    * Создаём таблицу частот. В режиме compressed == true читаем данные
    * из закодированного файла.
    * @param fin Входной .* файл.
    * @param isCompressed Открыть файл для чтения с закодированными данными.
    * @return Словарь вида (символ, частота).
    */
    map<uchar, uint> createFrequencyTable(ifstream &fin, bool isCompressed);

    /**
    * Строим дерево Хаффмана на основе таблице частот.
    * @return Node* Корень дерева.
    */
    Node* buildHuffmanTree();

    /**
    * Получаем коды символов на основе построенного дерева.
    * @param root Корень дерева Хаффмана.
    */
    void createCharCodesFromTree(Node* root);

    /**
    * Записываем исходный текст в сжатом виде, используя коды символов.
    * @param fin Входной .* файл.
    * @param fout Выходной .haff файл.
    */
    void writeCompressedHuffmanText(ifstream &fin, ofstream &fout);

    /**
    * Создаем сжатый .haff файл.
    * @param fin Входной .* файл.
    * @param fout Выходной .haff файл.
    */
    void createCompressedHuffmanFile(ifstream &fin, ofstream &fout);

    /**
    * Декодируем сжатый файл (.haff -> .*).
    * @param fin Входной .haff файл.
    * @param fout Выходной .* файл.
    * @param root Корень дерева Хаффмана.
    */
    void invertCompressedCharCodesIntoUsualSymbols(ifstream &fin, ofstream &fout, Node* root);

    /**
    * Очищаем память, занятую деревом.
    * @param root Корень дерева.
    */
    void deleteTree(Node* root);

    /**
     * Сбрасываем значения полей класса.
     */
    void resetValues() {
        frequency_table.clear();
        codes.clear();
        temp_code.clear();
        num = 0;
    }

    /**
    * Компаратор для сортировки пар по второму значению в порядке убывания.
    * (ключ, значение) - (символ, сколько раз встречался)
    * @param first Первая пара для сравнения.
    * @param second Вторая пара.
    * @return Первая больше второй?
    */
    static bool comparator(const pair<uchar, uint> &first, const pair<uchar, uint> &second) {
        return second.second < first.second;
    }
};


#endif //KRASNOV_KDZ_COMPRESSION_HUFFMAN_H
