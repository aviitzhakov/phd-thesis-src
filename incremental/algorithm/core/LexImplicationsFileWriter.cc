#include "core/LexImplicationsFileWriter.h"
#include <cstdlib>
#include <stdio.h>
#include <fstream>
#include <iostream>

LexImplicationsFileWriter::LexImplicationsFileWriter() {
    
}

LexImplicationsFileWriter::~LexImplicationsFileWriter() {
    
}

void LexImplicationsFileWriter::write( int nvertices,
                                       const std::vector <LexImplication>& leximps, 
                                       const std::string& fileName,
                                       double compTime,
                                       CompType compType,
                                       CanonicalIndexing indexing ) const {

    std::ofstream outfile;
    outfile.open( fileName.c_str() );
    outfile << "res(";
    outfile << nvertices << ", ";
    if (compType == DIRECT) {
         outfile << "direct, "; 
    }
    else if (compType == INCREMENTAL) {
        outfile << "incremental, ";
    }
    if (indexing == UPPER_TRIANGLE_ROWS) {
        outfile << "rowise, ";
    }
    else if (indexing == UPPER_TRIANGLE_COLS) {
        outfile << "colwise, ";
    }
    outfile << "size" << "(" << leximps.size() << ")" << ", ";
    outfile << "time" << "(" << compTime  << ")" << ", [" << std::endl;
    
    for ( int i = 0; i < leximps.size(); ++i ) {
        outfile << lexImpToPlTerm(leximps[i]);
        if ( i < leximps.size() - 1 ) {
            outfile << ", " << std::endl;
        }
    }
    outfile << "]).";
    outfile.close();

}


void LexImplicationsFileWriter::write( int nvertices,
                                       const std::vector <LexImplication>& leximps, 
                                       const std::string& fileName,
                                       double compTime,
                                       CompType compType,
                                       CanonicalIndexing indexing,
                                       const std::string& subgraphStr ) const {

    std::ofstream outfile;
    outfile.open( fileName.c_str() , std::ios_base::app);
    outfile << "res(";
    outfile << nvertices << ", ";
    if (compType == DIRECT) {
         outfile << "direct, "; 
    }
    else if (compType == INCREMENTAL) {
        outfile << "incremental, ";
    }
    if (indexing == UPPER_TRIANGLE_ROWS) {
        outfile << "rowise, ";
    }
    else if (indexing == UPPER_TRIANGLE_COLS) {
        outfile << "colwise, ";
    }
    outfile <<  subgraphStr.c_str() << ", ";
    outfile << "size" << "(" << leximps.size() << ")" << ", ";
    outfile << "time" << "(" << compTime  << ")" << ", [" << std::endl;
    
    for ( int i = 0; i < leximps.size(); ++i ) {
        outfile << lexImpToPlTerm(leximps[i]);
        if ( i < leximps.size() - 1 ) {
            outfile << ", " << std::endl;
        }
    }
    outfile << "])." << std::endl;
    outfile.close();

}




std::string LexImplicationsFileWriter::lexImpToPlTerm ( const LexImplication& imp ) const {

    std::string str("leximp([");
    char buff[32];
    for ( int i = 0; i < imp.perm.size(); ++i) {
        sprintf(buff, "%d",imp.perm[i] + 1);
        str += std::string(buff);
        if ( i < imp.perm.size() - 1) {
            str += std::string(",");
        }
    }
    sprintf(buff, "%d", imp.length);
    str += std::string("],") + std::string(buff) + std::string(")");

    return str;
}
    
