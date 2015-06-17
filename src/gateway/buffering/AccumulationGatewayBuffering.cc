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

#include "AccumulationGatewayBuffering.h"

using namespace FiCo4OMNeT;
using namespace std;

namespace SignalsAndGateways {

Define_Module(AccumulationGatewayBuffering);

void AccumulationGatewayBuffering::initialize()
{
    poolSizeSignal = registerSignal("poolSizeSignal");
    totalHoldUpTimeSignal = registerSignal("totalHoldUpTimeSignal");
    readConfigXML();

    unsigned int numPools = scheduledHoldUpTimes.size();
    for (unsigned int i = 0; i < numPools; i++) {
        char strBuf[32];
        snprintf(strBuf, 32, "pool%dHoldUpTime", i);

        simsignal_t signal = registerSignal(strBuf);

        cProperty *statisticTemplate = getProperties()->get("statisticTemplate",
                "poolHoldUpTime");
        ev.addResultRecorders(this, signal, strBuf, statisticTemplate);

        poolHoldUpTimeSignals.push_back(signal);
    }
}

void AccumulationGatewayBuffering::readConfigXML(){
    cXMLElement* xmlDoc = par("configXML").xmlValue();
    string gatewayName = this->getParentModule()->getParentModule()->getName();
    string xpath = "/config/gateway[@id='" + gatewayName + "']/buffering";
    cXMLElement* xmlBuffering = xmlDoc->getElementByPath(xpath.c_str(), xmlDoc);
    if(xmlBuffering){
        cXMLElementList xmlPools = xmlBuffering->getChildrenByTagName("pool");
        for(size_t i=0; i<xmlPools.size(); i++){
            cMessagePointerList* poolList = new cMessagePointerList();
            cMessage* holdUpTimeEvent = new cMessage();
            scheduledHoldUpTimes.insert(pair<cMessagePointerList*,cMessage*>(poolList, holdUpTimeEvent));
            scheduledTimes.insert(pair<cMessage*,simtime_t>(holdUpTimeEvent,0));
            list<simtime_t>* arrivalTimes = new list<simtime_t>();
            poolArrivalTimes.insert(pair<cMessagePointerList*,list<simtime_t>*>(poolList, arrivalTimes));
            cXMLElementList xmlHoldUpTimes = xmlPools[i]->getChildren();
            for (size_t j= 0; j < xmlHoldUpTimes.size(); j++) {
                simtime_t holdUpTime = STR_SIMTIME(xmlHoldUpTimes[j]->getAttribute("time"));
                cXMLElementList xmlPoolMessages = xmlHoldUpTimes[j]->getChildren();
                for (size_t k= 0; k < xmlPoolMessages.size(); k++) {
                    unsigned int canID = static_cast<unsigned int>(atoi(xmlPoolMessages[k]->getAttribute("canId")));
                    poolMap.insert(pair<unsigned int,cMessagePointerList*>(canID, poolList));
                    holdUpTimes.insert(pair<unsigned int,simtime_t>(canID, holdUpTime));
                }
            }
        }
    }
}

void AccumulationGatewayBuffering::handleMessage(cMessage *msg)
{
    if (msg->isSelfMessage()) {
        PoolMessage* poolMsg = new PoolMessage();
        cMessagePointerList* poolList;
        poolList=getPoolList(msg);
        emit(poolSizeSignal, poolList->size());
        emitArrivalTimes(poolList);
        poolMsg->setPool(*poolList);
        poolList->clear();
        send(poolMsg, gate("out"));
    } else if(CanDataFrame* dataFrame = dynamic_cast<CanDataFrame*> (msg)) {
        unsigned int canID = dataFrame->getCanID();
        cMessagePointerList* poolList;
        poolList = getPoolList(canID);
        if (poolList != NULL) {
            cMessage* poolHoldUpTimeEvent = getPoolHoldUpTimeEvent(poolList);
            simtime_t IDHoldUpTime = getIDHoldUpTime(canID);
            if (poolList->empty() || (IDHoldUpTime < getCurrentPoolHoldUpTime(poolHoldUpTimeEvent))) {
                simtime_t eventTime = IDHoldUpTime + simTime();
                cancelEvent(poolHoldUpTimeEvent);
                scheduleAt(eventTime, poolHoldUpTimeEvent);
                scheduledTimes[poolHoldUpTimeEvent] = eventTime;
            }
            poolList->push_back(dataFrame);
            poolArrivalTimes[poolList]->push_back(simTime());
        } else {
            send(msg, gate("out"));
        }
    } else if(dynamic_cast<EthernetIIFrame*>(msg)) {
        send(msg, gate("out"));
    }
}

cMessagePointerList* AccumulationGatewayBuffering::getPoolList(unsigned int canID){
    return poolMap[canID];
}

cMessagePointerList* AccumulationGatewayBuffering::getPoolList(cMessage* holdUpTimeEvent){
    map<cMessagePointerList*,cMessage*>::iterator it;
    for (it = scheduledHoldUpTimes.begin(); it != scheduledHoldUpTimes.end(); it++)
    {
        if (it->second == holdUpTimeEvent) {
            return it->first;
        }
    }
    throw 0;
}

simtime_t AccumulationGatewayBuffering::getIDHoldUpTime(unsigned int canID){
    return holdUpTimes[canID];
}

cMessage* AccumulationGatewayBuffering::getPoolHoldUpTimeEvent(cMessagePointerList* poolList){
    return scheduledHoldUpTimes[poolList];
}

simtime_t AccumulationGatewayBuffering::getCurrentPoolHoldUpTime(cMessage* poolHoldUpTimeEvent){
    simtime_t scheduledTime = scheduledTimes[poolHoldUpTimeEvent];
    return scheduledTime - simTime();
}

void AccumulationGatewayBuffering::emitArrivalTimes(cMessagePointerList* poolList){
    map<cMessagePointerList*,cMessage*>::iterator it1;
    unsigned int poolNumber = 0;
    for (it1= scheduledHoldUpTimes.begin(); it1 != scheduledHoldUpTimes.end(); ++it1){
        if (it1->first == poolList) {
            break;
        }
        poolNumber++;
    }
    list<simtime_t>* arrivalTimes = poolArrivalTimes[poolList];
    list<simtime_t>::iterator it;
    for (it = arrivalTimes->begin(); it != arrivalTimes->end(); ++it) {
        simtime_t holdUpTime = simTime()-*it;
        emit(totalHoldUpTimeSignal, holdUpTime);
        emit(poolHoldUpTimeSignals[poolNumber], holdUpTime);
    }
    arrivalTimes->clear();
}

} //namespace










