#ifndef LexImplicationsFileWriter_H
#define LexImplicationsFileWriter_H

#include "core/defs.h"
#include <vector>
#include <string>


class LexImplicationsFileWriter {

public:
    
    LexImplicationsFileWriter ();

    ~LexImplicationsFileWriter ();

    void write ( 
        int nvertices,
        const std::vector <LexImplication>& leximps, 
        const std::string& filename,
        double compTime,
        CompType compType,
        CanonicalIndexing indexing ) const;

    void write ( 
        int nvertices,
        const std::vector <LexImplication>& leximps, 
        const std::string& filename,
        double compTime,
        CompType compType,
        CanonicalIndexing indexing,
        const std::string& subgraphStr ) const;

private:
    
    std::string lexImpToPlTerm ( const LexImplication& imp ) const;
    
};


#endif