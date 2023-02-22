#include "core/GraphReaderWriter.h"
#include <fstream>

GraphReaderWriter::GraphReaderWriter() {

}

GraphReaderWriter::~GraphReaderWriter() {

}


Matrix<int> GraphReaderWriter::readGraph(const std::string& infile, int k) {
    std::ifstream input(infile.c_str());
    int i = 1;
    std::string line;

    while ( i++ < k && std::getline(input, line) );

    int sum = 0;
    int n = 1;

    while (sum < line.size()) {
        sum += n;
        n++;
    }
    Matrix<int> adj(n, n);
    for(int j=0; j < line.size(); j++) {
        int r = j / n;
        int c = j % n;
        adj.set(r,c, line[j] == '0' ? 0 : 1);
    }

    return adj;

}