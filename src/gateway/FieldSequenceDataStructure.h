
#ifndef FIELDSEQUENCE_H_
#define FIELDSEQUENCE_H_

#include <vector>
#include <memory>

#include "FieldElement.h"

/**
 * @brief Holds FieldElements in a vector of shared pointers.
 *
 * FieldElements like DataFieldElement, IdentifierFieldElement, TimestampFieldElement and TransportHeaderFieldElement
 * can be added and gotten by parametric functions.
 *
 * @author Sebastian Mueller
 */
class FieldSequenceDataStructure{
    private:
        std::vector<FieldElement*> fieldSequence;
    public:
        FieldSequenceDataStructure(){
        }

        ~FieldSequenceDataStructure(){
            clear();
        }

        /**
         * @brief Adds the given element to the sequence.
         *
         * @param element an object that inherit FieldElement
         */
        template<typename T>
        void pushField(T element){
            fieldSequence.push_back(element);
        }

        /**
         * @brief Returnes first occurrence of specified type. The Element will not be deleted!
         *
         * Example: object.getField<DataFieldElement>()
         *
         * @return shared_ptr<T> shared pointer of T
         */
        template<typename T>
        T* getField(){
            T* specificElement = NULL;
            for (size_t i = 0; i < fieldSequence.size(); i++) {
                FieldElement* element = fieldSequence.at(i);
                if(dynamic_cast<T*>(element) != NULL){
                    specificElement  = dynamic_cast<T*>(element);
                    break;
                }
            }
            if(specificElement == NULL){
                throw cException("Specified FieldElement not found in FieldSequenceDataStructure!");
            }
            return specificElement;
        }

        /**
         * @brief Deletes all Elements of the sequence.
         */
        void clear(){
//            for (size_t i = 0; i < fieldSequence.size(); i++) {
//                FieldElement* element = fieldSequence.at(i);
//                if(element != NULL) {
//                    delete element;
//                }
//            }
            fieldSequence.clear();
        }

        /**
         * @brief Returns the number of elements within the datastructure
         *
         * @return int number of element within the FieldSequenceList
         */
        int size(){
            return fieldSequence.size();
        }

        /**
         * @brief Returns an object of current state of the FieldSeqenceList
         *
         * @return object of current FieldSequenceList
         */
        std::vector<FieldElement*> getFieldSequenceList(){
            return fieldSequence;
        }
};


#endif /* FIELDSEQUENCE_H_ */
