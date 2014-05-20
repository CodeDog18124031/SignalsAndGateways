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

#include "InOutComing.h"
#include "CanDataFrame_m.h"
#include "CTFrame.h"
#include "TransportMessage_m.h"
#include "InterConnectMsg_m.h"
#include "MultipleFieldSequenceMessage.h"
#include "FieldElement.h"
#include "IdentifierFieldElement.h"
#include "Utility.h"

Define_Module(InOutComing);

void InOutComing::initialize()
{

}

void InOutComing::handleMessage(cMessage *msg)
{

    if(msg->arrivedOn("appInterface$i",0) || msg->arrivedOn("appInterface$i",1)){
        TransportMessage *transportMsg = dynamic_cast<TransportMessage*>(msg);
        InterConnectMsg *interDataStructure = new InterConnectMsg;
        interDataStructure->setFirstArrivalTimeOnCan(transportMsg->getFirstArrivalTimeOnCan());

        cPacket *delivery = transportMsg->decapsulate();
        if(dynamic_cast<FiCo4OMNeT::CanDataFrame*>(delivery) != NULL){
            interDataStructure->encapsulate(delivery);
        }else if (dynamic_cast<EtherFrame*>(delivery) != NULL){
            CoRE4INET::CTFrame *ethernetFrame = dynamic_cast<CoRE4INET::CTFrame*>(delivery);
            MultipleFieldSequenceMessage *multiFieldSequence = dynamic_cast<MultipleFieldSequenceMessage*>(ethernetFrame->decapsulate());
            interDataStructure->encapsulate(multiFieldSequence);
        }
        send(interDataStructure, "out");
    }else if(msg->arrivedOn("in")){
        InterConnectMsg *interDataStructure = dynamic_cast<InterConnectMsg*>(msg);
        TransportMessage *transportMsg = new TransportMessage;
        cPacket *delivery = interDataStructure->decapsulate();
        if(dynamic_cast<FiCo4OMNeT::CanDataFrame*>(delivery) != NULL){
            cXMLElementList destination;
            for(auto &element : interDataStructure->getRoutingData()){
                if(strcmp(element->getTagName(), "destination") == 0){
                    destination = element->getParentNode()->getChildrenByTagName("destination");
                }
            }
            int assignedDestinationCount = interDataStructure->getAssignedDestinationCount();
            int currentDestinationCount = 0;
            for(auto &element : destination){
                if(currentDestinationCount == assignedDestinationCount){
                    std::string backboneTransferType = element->getFirstChildWithTag("backboneTransferType")->getNodeValue();
                    UTLTY::Utility::stripNonAlphaNum(backboneTransferType);
                    transportMsg->setBackboneTransferType(backboneTransferType.c_str());
                    break;
                }
                currentDestinationCount++;
            }
            transportMsg->encapsulate(delivery);
            send(transportMsg, "appInterface$o", 0);
        }else if(dynamic_cast<MultipleFieldSequenceMessage*>(delivery) != NULL){
            CoRE4INET::CTFrame *ethernetFrame = new CoRE4INET::CTFrame("");
            EV << "CTID: " << interDataStructure->getBackboneCTID();
            ethernetFrame->setCtID(interDataStructure->getBackboneCTID());
            ethernetFrame->encapsulate(delivery);
            transportMsg->encapsulate(ethernetFrame);
            send(transportMsg, "appInterface$o", 1);
        }
    }
    delete msg;
}
