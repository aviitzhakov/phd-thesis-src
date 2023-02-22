#include <string>
#include "core/Matrix.h"

class GraphReaderWriter {

public:

    GraphReaderWriter();

    ~GraphReaderWriter();

    Matrix<int> readGraph(const std::string& inFile, int k);


};