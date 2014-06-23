
#ifndef DATAFIELDELEMENT_H_
#define DATAFIELDELEMENT_H_

#include <dataStructure/FieldElement.h>

namespace dataStruct {

/**
 * @brief FieldElement for the FieldSequenceMessages which stores the payload
 *
 * @see FieldElement
 *
 * @author Sebastian Mueller
 */
class DataFieldElement: public dataStruct::FieldElement {
public:
    DataFieldElement(int dataLength);
    virtual ~DataFieldElement();

    int getDataLength() const {
        return dataLength;
    }

    const char getData(int index) const {
        return data[index];
    }

    void setData(int index, char data) {
        this->data[index] = data;
    }

    void setDataLength(int dataLength) {
        this->dataLength = dataLength;
    }

private:
    int dataLength;
    char *data;
};

} /* namespace Transformation */

#endif /* DATAFIELDELEMENT_H_ */
