#include <iostream>
#include <fstream>
#include <map>
#include <vector>
#include <algorithm>
#include <cstring>
#include <ctime>


using std::string;
using std::map;
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

// Количество символов в файле.
uint num = 0;

/**
 * Вершина дерева.
 */
struct Node {
    uchar value = '\0';
    uint frequency = 0;
    Node* left = nullptr;
    Node* right = nullptr;
};

/**
 * Метод для создания новой вершины с заданными значениями.
 * @param value Значение символа.
 * @param frequency Количество символов в тексте.
 * @param left Левый ребёнок.
 * @param right Правый ребёнок.
 * @return Новая вершина с заданными значениями.
 */
Node* getNewNode(uchar value, uint frequency, Node* left, Node* right) {
    Node* node = new Node();

    node->value = value;
    node->frequency = frequency;
    node->left = left;
    node->right = right;

    return node;
}

/**
 * Переменная для хранения кода символа.
 */
vector<int> temp_code;

/**
 * Компаратор для сортировки пар по второму значению в порядке убывания.
 * (ключ, значение) - (символ, сколько раз встречался)
 * @param first Первая пара для сравнения.
 * @param second Вторая пара.
 * @return Первая больше второй?
 */
bool comparator(const pair<uchar, uint> &first, const pair<uchar, uint> &second) {
    return second.second < first.second;
}

/**
 * Создаём таблицу частот. В режиме compressed == true, поток
 * ifstream открывается в бинаром режиме.
 * @param fin Путь до файла.
 * @param isCompressed Открыть файл для чтения с закодированными данными.
 * @return Словарь вида (символ, частота).
 */
map<uchar, uint> createFrequencyTable(ifstream &fin, bool isCompressed) {
    // Словарь с количеством символов каждого вида.
    map<uchar, uint> frequency_table;
    uchar temp_char;
    uint table_size;
    uint temp_freq;

    if (isCompressed) {
        // Считываем количество символов.
        fin.read((char*) &num, sizeof(num));
        // Считываем размер таблицы с количеством символов каждого вида.
        fin.read((char*) &table_size, sizeof(table_size));
        for (int i = 0; i < table_size; ++i) {
            fin.read((char*) &temp_char, sizeof(temp_char));
            fin.read((char*) &temp_freq, sizeof(temp_freq));
            frequency_table[temp_char] = temp_freq;
        }
        // DEBUG!
//        cout << "number:" << num << " " << frequency_table.size() << "\n";
    } else {
        while (fin.read((char*) &temp_char, sizeof(temp_char))) {
            frequency_table[temp_char]++;
            num++;
        }
        // DEBUG!
//        int i = 0;
//        for (auto myPair : frequency_table) {
//            std::cout << i++ << ": " << myPair.first << ":" << myPair.second << "\n";
//        }
//        // DEBUG!
//        cout << "number:" << num << " " << frequency_table.size() << "\n";
    }
    return frequency_table;
}

/**
 * Метод для поиска индекса, на который нужно вставить вершину.
 * @param arrNodes Вектор, содержащий вершины.
 * @param l Левая граница.
 * @param r Правая граница.
 * @param nodeFreq Значение, полученное из суммы двух меньших (по
 * значению frequency) вершин.
 * @return Позиция в векторе, на которую нужно вставить созданную вершину.
 */
int binarySearch(Node** arrNodes, int l, int r, uint nodeFreq) {
    if (r >= l) {
        int mid = l + (r - l) / 2;

        if (arrNodes[mid]->frequency < nodeFreq && arrNodes[mid - 1]->frequency >= nodeFreq)
            return mid;

        if (arrNodes[mid]->frequency >= nodeFreq)
            return binarySearch(arrNodes, mid + 1, r, nodeFreq);

        return binarySearch(arrNodes, l, mid - 1, nodeFreq);
    }
    return -1;
}

/**
 * Строим дерево Хаффмана на основе таблице частот.
 * @param table - таблица частот
 * @return Node* Корень дерева.
 */
Node* buildHuffmanTree(map<uchar, uint> frequency_table) {
    // Вектор, содержащий вершины.
    vector<Node*> nodesVec;
    // Вектор содержащий, пары (символ, количество), для сортировки по неубыванию.
    vector<pair<uchar, uint>> frequencyVector(frequency_table.begin(), frequency_table.end());
    sort(frequencyVector.begin(), frequencyVector.end(), comparator);

    nodesVec.reserve(frequencyVector.size());
    // Заполняем вектор вершин.
    // std::pair<uchar, uint>
    for (auto pair : frequencyVector) {
        nodesVec.emplace_back(getNewNode(pair.first, pair.second, nullptr, nullptr));
    }

    // Строим дерево.
    while (nodesVec.size() > 1) {
        // Получаем две вершины с наименьшей частотой.
        Node* left = nodesVec.back();
        nodesVec.pop_back();
        Node* right = nodesVec.back();
        nodesVec.pop_back();

        // Создаём новую вершину на основе двух полученных.
        uint sum = left->frequency + right->frequency;
        if (nodesVec[0]->frequency < sum) {
            nodesVec.insert(nodesVec.begin(), getNewNode('\0', sum, left, right));
        } else if (nodesVec.back()->frequency >= sum) {
            nodesVec.insert(nodesVec.end(), getNewNode('\0', sum, left, right));
        } else {
            nodesVec.insert(nodesVec.begin() + binarySearch(nodesVec.data(), 0, nodesVec.size(), sum),
                            getNewNode('\0', sum, left, right));
        }
    }

    return nodesVec.back();
}

/**
 * Получаем коды символов на основе построенного дерева.
 * @param node Корень дерева Хаффмана.
 * @param codes Словарь вида (символ, код этого символа)
 */
void createCharCodesFromTree(Node* root, map<uchar, vector<int>> &codes) {
    if (root->left != nullptr) {
        temp_code.push_back(0);
        createCharCodesFromTree(root->left, codes);
    }
    if (root->right != nullptr) {
        temp_code.push_back(1);
        createCharCodesFromTree(root->right, codes);
    }

    // Записываем код (из вектора, объявленного глобально выше)
    // в словарь, если вершина - НЕ лист.
    if (root->left == nullptr && root->right == nullptr) {
        codes[root->value] = temp_code;
    }

    // Удаляем последнюю цифру из кода, так как поднимаемся
    // на один уровень выше (к предыдущей вершине).
    temp_code.pop_back();
}

/**
  * Записываем исходный текст в сжатом виде, используя коды символов.
 * @param fin Входной .* файл.
 * @param fout Выходной .haff файл.
 * @param codes Коды символов. Словарь вида (символ, код символа).
 */
void writeCompressedHuffmanText(ifstream &fin, ofstream &fout, map<uchar, vector<int>> codes) {
    uint offset = 0;
    uchar temp_char;
    uchar temp_byte = 0;

    while (fin.read((char*) &temp_char, sizeof(temp_char))) {
        // Пишем символы, используя полученную кодировку.
        for (int i : codes[temp_char]) {
            // Чтобы записать несколько бит в один байт.
            temp_byte = temp_byte | (i << (7 - offset));
            offset++;
            if (offset == 8) {
                fout.write((char*) &temp_byte, sizeof(temp_byte));
                offset = 0;
                temp_byte = 0;
            }
        }
    }
    // Очищаем буфер.
    fout.write((char*) &temp_byte, sizeof(temp_byte));
}

/**
 * Создаем сжатый .haff файл.
 * @param fin Входной .* файл.
 * @param fout Выходной .haff файл.
 * @param frequency_table Таблица частот. Словарь вида (символ, частота).
 * @param codes Коды символов. Словарь вида (символ, код символа).
 */
void createCompressedHuffmanFile(ifstream &fin, ofstream &fout,
                                 const map<uchar, uint> &frequency_table,
                                 map<uchar, vector<int>> codes) {
    uint table_size = frequency_table.size();

    // Записываем количество символов.
    fout.write((char*) &num, sizeof(num));
    // Записываем размер таблицы с количеством символов каждого вида.
    fout.write((char*) &table_size, sizeof(table_size));
    // Записываем символ и его количество, чтобы потом можно было расшифровать.
    for (auto i : frequency_table) {
        fout.write((char*) &i.first, sizeof(i.first));
        fout.write((char*) &i.second, sizeof(i.second));
    }

    writeCompressedHuffmanText(fin, fout, codes);
}

/**
 * Декодируем сжатый файл (.haff -> .*).
 * @param fin Входной .haff файл.
 * @param fout Выходной .* файл.
 */
void invertCompressedCharCodesIntoUsualSymbols(ifstream &fin, ofstream &fout, Node* root) {
    uint offset = 0;
    uchar temp_byte;
    Node* temp_node = root;
    int digit;
    unsigned long int i = 0;

    // Читаем первый байт.
    fin.read((char*) &temp_byte, sizeof(temp_byte));
    while (i < num) {
        // Получаем значение конкретного бита (в зависимости от смещения). Если digit == 0,
        // то бит равен 0 => для декодирования идём в левую часть поддерева, иначе
        // бит == 1 и тогда переходим в правую часть поддерева. Если дошли до  листа, то
        // можем записать в файл значение  в этом листе и вернуться к корню дерева для
        // декодирования оставшихся символов.
        digit = temp_byte & (1 << (7 - offset));
        if (digit == 0) {
            temp_node = temp_node->left;
        } else {
            temp_node = temp_node->right;
        }

        if (temp_node->left == nullptr && temp_node->right == nullptr) {
            fout << temp_node->value;
            temp_node = root;
            i++;
        }

        offset++;
        if (offset == 8) {
            offset = 0;
            // Обработали байт, поэтому читаем следующий.
            fin.read((char*) &temp_byte, sizeof(temp_byte));
        }
    }
}

/**
 * Очищаем память, занятую деревом.
 * @param root Корень дерева.
 */
void deleteTree(Node* root) {
    if (root != nullptr) {
        deleteTree(root->left);
        deleteTree(root->right);
        delete root;
    }
}

/**
 * Функция сжатия Хаффмана.
 * @param input Путь до .txt файла.
 * @param output Путь до .haff файла.
 */
void huffmanCompress(string input, string output) {
    // Открываем для записи и чтения в бинарном режиме.
    ifstream fin(input, std::ios::binary | std::ios::in);
    ofstream fout(output, std::ios::binary | std::ios::out);

    num = 0;
    map<uchar, uint> frequency_table = createFrequencyTable(fin, false);
    Node* tree = buildHuffmanTree(frequency_table);

    map<uchar, vector<int> > codes;
    temp_code.clear();
    createCharCodesFromTree(tree, codes);
    // Сбросить состояние input stream.
    fin.clear();
    // Перемещаем указатель на начало файла.
    fin.seekg(0);
    createCompressedHuffmanFile(fin, fout, frequency_table, codes);

    deleteTree(tree);
    fin.close();
    fout.close();
    // Print codes
//    for (auto i: codes) {
//        cout << i.first << "-->";
//        for (auto j: i.second) {
//            std::cout << j;
//        }
//        cout << std::endl;
//    }
//    cout << std::endl;
}

/**
 * Функция распаковки Хаффмана.
 * @param input Путь до .haff файла.
 * @param output Путь до .txt файла.
 */
void huffmanDecompress(string input, string output) {
    // Открываем для записи и чтения в бинарном режиме.
    ifstream fin(input, std::ios::binary | std::ios::in);
    ofstream fout(output, std::ios::binary | std::ios::out);

    num = 0;
    map<uchar, uint> frequency_table = createFrequencyTable(fin, true);
    Node* tree = buildHuffmanTree(frequency_table);
    invertCompressedCharCodesIntoUsualSymbols(fin, fout, tree);
    deleteTree(tree);
}

pair<ushort, ushort> getMatching(const vector<uchar> &dictionary, const vector<uchar> &buffer) {
    ushort offset = 0;
    ushort length = 0;

    int dictSize = dictionary.size();
    int bufferSize = buffer.size();

    for (int i = 0; i < dictionary.size(); i++) {
        int tmpLength = 0;
        while (tmpLength < min(bufferSize, dictSize - i) && dictionary[i + tmpLength] == buffer[tmpLength]) {
            tmpLength++;
        }
        // Возможно, что в буфере находится продолжение.
        if (tmpLength == dictSize - i) {
            int bufferIndex = 0;
            while (tmpLength < bufferSize && buffer[bufferIndex] == buffer[tmpLength]) {
                bufferIndex++;
                tmpLength++;
            }
        }
        if (tmpLength > length) {
            length = tmpLength;
            offset = dictSize - i;
        }
    }

    return make_pair(offset, length);
}

vector<uchar> getSlice(const vector<uchar> &vec, int start, int end) {
    vector<uchar> new_vec;
    end += start;
    for (int i = start; i < end; i++) {
        new_vec.emplace_back(vec[i]);
    }
    return new_vec;
}

void lz77Pack(int dictSize, int bufferSize, vector<uchar> &input, vector<ushort> &offset_vec,
              vector<ushort> &length_vec, vector<uchar> &next_char) {
    int pos = 0;

    while (pos < input.size()) {
        vector<uchar> dict = getSlice(input, max(0, pos - dictSize), min(pos, dictSize));
        vector<uchar> buffer = getSlice(input, pos, min(bufferSize, (int) input.size() - pos));
        if (buffer.empty()) {
            offset_vec.emplace_back(0);
            length_vec.emplace_back(0);
            pos++;
        } else {
            auto offsetLength = getMatching(dict, buffer);
            offset_vec.emplace_back(offsetLength.first);
            length_vec.emplace_back(offsetLength.second);
            pos += offsetLength.second + 1;
        }
        if (pos >= input.size()) {
            next_char.emplace_back('#');
        } else {
            next_char.emplace_back(input[pos - 1]);
        }
    }
}

void lz77Unpack(ofstream &fout, vector<ushort> &offset_vec,
                vector<ushort> &length_vec, vector<uchar> &next_char) {
    string res;

    for (int i = 0; i < offset_vec.size(); ++i) {
        auto start = res.length() - offset_vec[i];
        for (int j = 0; j < length_vec[i]; ++j) {
            res += res[start + j];
            fout.write((char*) &res[start + j], sizeof(next_char[0]));
        }
        res += next_char[i];
        fout.write((char*) &next_char[i], sizeof(next_char[0]));
    }
}

vector<uchar> createVectorWithCharsLZ77(ifstream &fin, bool isCompressed, vector<ushort> &offset_vec,
                                        vector<ushort> &length_vec, vector<uchar> &next_char) {
    // Словарь с количеством символов каждого вида.
    vector<uchar> input_chars;
    uchar temp_char;
    uint number_triples;
    ushort temp_ushort;

    if (isCompressed) {
        // Считываем количество троек в закодированном файле.
        fin.read((char*) &number_triples, sizeof(number_triples));
        for (int i = 0; i < number_triples; ++i) {
            fin.read((char*) &temp_ushort, sizeof(temp_ushort));
            offset_vec.emplace_back(temp_ushort);
            fin.read((char*) &temp_ushort, sizeof(temp_ushort));
            length_vec.emplace_back(temp_ushort);
            fin.read((char*) &temp_char, sizeof(temp_char));
            next_char.emplace_back(temp_char);
        }
    } else {
        while (fin.read((char*) &temp_char, sizeof(temp_char))) {
            input_chars.emplace_back(temp_char);
            num++;
        }
    }
    return input_chars;
}

void createCompressedLZ77File(ofstream &fout, vector<ushort> &offset_vec, vector<ushort> &length_vec,
                              vector<uchar> &next_char) {
    uint number_triple = offset_vec.size();
    // Записываем количество троек.
    fout.write((char*) &number_triple, sizeof(number_triple));

    for (int i = 0; i < offset_vec.size(); ++i) {
        // Записываем.
        fout.write((char*) &offset_vec[i], sizeof(offset_vec[0]));
        // Записываем.
        fout.write((char*) &length_vec[i], sizeof(length_vec[0]));
        fout.write((char*) &next_char[i], sizeof(next_char[0]));
    }
}

/**
 * Функция сжатия LZ77.
 * @param input Путь до .txt файла.
 * @param output Путь до .lz77* файла.
 */
void lz77Compress(string input, string output, int dictSize, int bufferSize) {
    // Открываем для записи и чтения в бинарном режиме.
    ifstream fin(input, std::ios::binary | std::ios::in);
    ofstream fout(output, std::ios::binary | std::ios::out);

    vector<ushort> offset_vec;
    vector<ushort> length_vec;
    vector<uchar> next_char;
    // Считываем символы из файла.
    vector<uchar> input_chars = createVectorWithCharsLZ77(fin, false, offset_vec, length_vec, next_char);
    // Кодируем считанные символы, записывая результаты в переданные вектора.
    lz77Pack(dictSize, bufferSize, input_chars, offset_vec, length_vec, next_char);
    // Создаём файлик
    createCompressedLZ77File(fout, offset_vec, length_vec, next_char);

    fin.close();
    fout.close();
}

void lz77Decompress(string input, string output) {
    // Открываем для записи и чтения в бинарном режиме.
    ifstream fin(input, std::ios::binary | std::ios::in);
    ofstream fout(output, std::ios::binary | std::ios::out);

    vector<ushort> offset_vec;
    vector<ushort> length_vec;
    vector<uchar> next_char;

    // Считываем символы из файла.
    createVectorWithCharsLZ77(fin, true, offset_vec, length_vec, next_char);
    lz77Unpack(fout, offset_vec, length_vec, next_char);
    fin.close();
    fout.close();
}

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
            auto temp1 = double(clock());
            huffmanCompress(argv[3], argv[4]);
            auto temp2 = double(clock());
            huffmanDecompress(argv[4], "output2.png");
            auto temp3 = double(clock());
            cout << "Time pack: " << (temp2 - temp1) / CLOCKS_PER_SEC << "sec\n";
            cout << "Time unpack: " << (temp3 - temp2) / CLOCKS_PER_SEC << "sec\n";
        }
        if (!std::strcmp(argv[1], "lz77") && !std::strcmp(argv[2], "-c")) {
            auto temp1 = double(clock());
            lz77Compress(argv[3], argv[4], 4 * 1024, 1 * 1024);
            auto temp2 = double(clock());
            lz77Decompress(argv[4], "kdz_output.pdf");
            auto temp3 = double(clock());
            cout << "Time pack: " << (temp2 - temp1) / CLOCKS_PER_SEC << "sec\n";
            cout << "Time unpack: " << (temp3 - temp2) / CLOCKS_PER_SEC << "sec\n";
        }
        if (!std::strcmp(argv[1], "haff") && !std::strcmp(argv[2], "-d"))
            huffmanDecompress(argv[3], argv[4]);
    }
    return 0;
}