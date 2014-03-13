/*
 * DataFieldElement.h
 *
 *  Created on: Mar 11, 2014
 *      Author: sebastian
 */

#ifndef DATAFIELDELEMENT_H_
#define DATAFIELDELEMENT_H_

#include <dataStructure/FieldElement.h>

namespace dataStruct {

class DataFieldElement: public dataStruct::FieldElement {
public:
    DataFieldElement(int dataLength);
    virtual ~DataFieldElement();

    const char* getData() const {
        return data;
    }

private:
    int dataLength;
    char* data;
};

} /* namespace Transformation */

#endif /* DATAFIELDELEMENT_H_ */
