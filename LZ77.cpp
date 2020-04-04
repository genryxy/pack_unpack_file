//
// Created by Admin on 05.04.2020.
//
#include "LZ77.h"

pair<ushort, ushort> LZ77::getMatching(const vector<uchar> &dictionary, const vector<uchar> &buffer) {
    ushort offset = 0;
    ushort length = 0;

    int dictSize = dictionary.size();
    int bufferSize = buffer.size();

    for (int i = 0; i < dictionary.size(); i++) {
        int tmpLength = 0;
        while (tmpLength < min(bufferSize, dictSize - i) && dictionary[i + tmpLength] == buffer[tmpLength]) {
            tmpLength++;
        }
        // Возможно, что в буфере предпросмотра находится продолжение.
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

vector<uchar> LZ77::getSlice(const vector<uchar> &vec, int start, int end) {
    vector<uchar> new_vec;
    end += start;
    for (int i = start; i < end; i++) {
        new_vec.emplace_back(vec[i]);
    }
    return new_vec;
}

void LZ77::lz77Pack() {
    int pos = 0;

    while (pos < input_chars.size()) {
        vector<uchar> dict = getSlice(input_chars, max(0, pos - dictSize), min(pos, dictSize));
        vector<uchar> buffer = getSlice(input_chars, pos, min(bufferSize, (int) input_chars.size() - pos));
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
        if (pos >= input_chars.size()) {
            next_char.emplace_back('#');
        } else {
            next_char.emplace_back(input_chars[pos - 1]);
        }
    }
}

void LZ77::lz77Unpack(ofstream &fout) {
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

vector<uchar> LZ77::createVectorWithCharsLZ77(ifstream &fin, bool isCompressed) {
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
        }
    }
    return input_chars;
}

void LZ77::createCompressedLZ77File(ofstream &fout) {
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

void LZ77::lz77Compress(string input, string output) {
    // Открываем для записи и чтения в бинарном режиме.
    ifstream fin(input, std::ios::binary | std::ios::in);
    ofstream fout(output, std::ios::binary | std::ios::out);

    resetValues();
    // Считываем символы из файла.
    input_chars = createVectorWithCharsLZ77(fin, false);
    // Кодируем считанные символы, записывая результаты в переданные вектора.
    lz77Pack();
    // Создаём файл, содержащий тройки.
    createCompressedLZ77File(fout);

    fin.close();
    fout.close();
}

void LZ77::lz77Decompress(string input, string output) {
    // Открываем для записи и чтения в бинарном режиме.
    ifstream fin(input, std::ios::binary | std::ios::in);
    ofstream fout(output, std::ios::binary | std::ios::out);

    resetValues();
    // Считываем символы из файла.
    createVectorWithCharsLZ77(fin, true);
    lz77Unpack(fout);
    fin.close();
    fout.close();
}
