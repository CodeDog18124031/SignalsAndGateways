//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with this program.  If not, see http://www.gnu.org/licenses/.
// 

#ifndef RTRFIELDELEMENT_H_
#define RTRFIELDELEMENT_H_

#include <dataStructure/FieldElement.h>

namespace dataStruct {

class RTRFieldElement: public dataStruct::FieldElement {
public:
    RTRFieldElement();
    virtual ~RTRFieldElement();

    bool isRtr() const {
        return rtr;
    }

    void setRtr(bool rtr) {
        this->rtr = rtr;
    }

private:
    bool rtr;
};

}

#endif /* RTRFIELDELEMENT_H_ */