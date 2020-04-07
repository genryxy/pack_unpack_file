#include "Huffman.h"

typedef unsigned char uchar;
typedef unsigned int uint;


Node* Huffman::getNewNode(uchar value, uint frequency, Node* left, Node* right) {
    Node* node = new Node();

    node->value = value;
    node->frequency = frequency;
    node->left = left;
    node->right = right;

    return node;
}

map<uchar, uint> Huffman::createFrequencyTable(ifstream &fin, bool isCompressed) {
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
    }
    return frequency_table;
}

int Huffman::binarySearch(Node** arrNodes, int l, int r, uint nodeFreq) {
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

Node* Huffman::buildHuffmanTree() {
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

void Huffman::createCharCodesFromTree(Node* root) {
    if (root->left != nullptr) {
        temp_code.push_back(0);
        createCharCodesFromTree(root->left);
    }
    if (root->right != nullptr) {
        temp_code.push_back(1);
        createCharCodesFromTree(root->right);
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

void Huffman::writeCompressedHuffmanText(ifstream &fin, ofstream &fout) {
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

void Huffman::createCompressedHuffmanFile(ifstream &fin, ofstream &fout) {
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

    writeCompressedHuffmanText(fin, fout);
}

void Huffman::invertCompressedCharCodesIntoUsualSymbols(ifstream &fin, ofstream &fout, Node* root) {
    uint offset = 0;
    uchar temp_byte;
    Node* temp_node = root;
    int digit;
    unsigned long int i = 0;

    // Читаем первый байт.
    fin.read((char*) &temp_byte, sizeof(temp_byte));
    while (i < num) {

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
        // Получаем значение конкретного бита (в зависимости от смещения). Если
        // digit == 0, то бит равен 0 => для декодирования идём в левую часть
        // поддерева, иначе бит == 1 и тогда переходим в правую часть поддерева.
        // Если дошли до  листа, то можем записать в файл значение  в этом листе
        // и вернуться к корню дерева для декодирования оставшихся символов.
        offset++;
        if (offset == 8) {
            offset = 0;
            // Обработали байт, поэтому читаем следующий.
            fin.read((char*) &temp_byte, sizeof(temp_byte));
        }
    }
}

void Huffman::deleteTree(Node* root) {
    if (root != nullptr) {
        deleteTree(root->left);
        deleteTree(root->right);
        delete root;
    }
}

void Huffman::huffmanCompress(string input, string output) {
    // Открываем для записи и чтения в бинарном режиме.
    ifstream fin(input, std::ios::binary | std::ios::in);
    ofstream fout(output, std::ios::binary | std::ios::out);

    resetValues();
    frequency_table = createFrequencyTable(fin, false);
    Node* tree = buildHuffmanTree();

    createCharCodesFromTree(tree);
    // Сбросить состояние input stream.
    fin.clear();
    // Перемещаем указатель на начало файла.
    fin.seekg(0);
    createCompressedHuffmanFile(fin, fout);

    deleteTree(tree);
    fin.close();
    fout.close();
    // DEBUG!
    // Печатаем коды вершин.
//    for (auto i: codes) {
//        cout << i.first << "-->";
//        for (auto j: i.second) {
//            std::cout << j;
//        }
//        cout << std::endl;
//    }
//    cout << std::endl;
}

void Huffman::huffmanDecompress(string input, string output) {
    // Открываем для записи и чтения в бинарном режиме.
    ifstream fin(input, std::ios::binary | std::ios::in);
    ofstream fout(output, std::ios::binary | std::ios::out);

    resetValues();
    frequency_table = createFrequencyTable(fin, true);
    Node* tree = buildHuffmanTree();
    invertCompressedCharCodesIntoUsualSymbols(fin, fout, tree);

    deleteTree(tree);
    fin.close();
    fout.close();
}