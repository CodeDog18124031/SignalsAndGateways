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

#include "CanSourceGatewayAppBase.h"
#include "TransportMessage_m.h"
#include "candataframe_m.h"
#include "TransportCanDataFrame_m.h"

Define_Module(CanSourceGatewayAppBase);

void CanSourceGatewayAppBase::initialize() {
    canVersion = getParentModule()->par("version").stdstringValue();
    //initialDataFrameCreation();
    //initialRemoteFrameCreation();
}

void CanSourceGatewayAppBase::handleMessage(cMessage *msg) {
    //CanDataFrame *df = check_and_cast<CanDataFrame *>(msg);
    //dataFrameTransmission(df);
    if(msg->arrivedOn("routingIn")){
        TransportMessage *transFrame = check_and_cast<TransportMessage*>(msg);
        TransportCanDataFrame *transDataFrame = check_and_cast<TransportCanDataFrame *>(transFrame->decapsulate());
        CanDataFrame *dataFrame = &(transDataFrame->getCanDataFrame());
        dataFrame->setStartTime(simTime());
        send(dataFrame, "out");
    }
}

void CanSourceGatewayAppBase::initialRemoteFrameCreation() { //TODO was, wenn keine frames vorhanden?
    /*
    if (getParentModule()->par("idRemoteFrames").stdstringValue() != "0") {
        cStringTokenizer remoteFrameIDsTokenizer(
                getParentModule()->par("idRemoteFrames"), ",");
        vector<int> remoteFrameIDs = remoteFrameIDsTokenizer.asIntVector();
        cStringTokenizer remoteFramesPeriodicityTokenizer(
                getParentModule()->par("periodicityRemoteFrames"), ",");

        cStringTokenizer dataLengthRemoteFramesTokenizer(
                getParentModule()->par("dataLengthRemoteFrames"), ",");

        for (unsigned int i = 0; i < remoteFrameIDs.size(); i++) {
            CanDataFrame *can_msg = new CanDataFrame("remoteFrame");
            can_msg->setNode(getParentModule()->par("node"));
            can_msg->setCanID(checkAndReturnID(remoteFrameIDs.at(i)));
            can_msg->setLength(
                    calculateLength(
                            atoi(dataLengthRemoteFramesTokenizer.nextToken())));
            can_msg->setRtr(true);
            can_msg->setPeriod(
                    atoi(remoteFramesPeriodicityTokenizer.nextToken()));
            if (can_msg->getPeriod() == 0) {
                EV<<"Remote frame with ID "<< can_msg->getCanID() << " has no period. Hence it will be ignored.\n";
            } else {
                outgoingRemoteFrames.push_back(can_msg); // TODO brauch ich das? ich glaube nicht
                scheduleAt(simTime() + (can_msg->getPeriod() / 1000.), can_msg);
            }
        }
    }
    */
}

void CanSourceGatewayAppBase::initialDataFrameCreation() { //TODO was, wenn keine frames vorhanden?
    /*if (getParentModule()->par("idDataFrames").stdstringValue() != "0") {
        cStringTokenizer dataFrameIDsTokenizer(
                getParentModule()->par("idDataFrames"), ",");
        vector<int> dataFrameIDs = dataFrameIDsTokenizer.asIntVector();

        cStringTokenizer dataFramesPeriodicityTokenizer(
                getParentModule()->par("periodicityDataFrames"), ",");

        cStringTokenizer dataLengthDataFramesTokenizer(
                getParentModule()->par("dataLengthDataFrames"), ",");

        for (unsigned int i = 0; i < dataFrameIDs.size(); i++) {
            CanDataFrame *can_msg = new CanDataFrame("message");
            can_msg->setNode(getParentModule()->par("node"));
            can_msg->setCanID(checkAndReturnID(dataFrameIDs.at(i)));
            can_msg->setLength(
                    calculateLength(
                            atoi(dataLengthDataFramesTokenizer.nextToken())));
            can_msg->setPeriod(
                    atoi(dataFramesPeriodicityTokenizer.nextToken()));
            outgoingDataFrames.push_back(can_msg); // TODO brauch ich das? ich glaube schon
            if (can_msg->getPeriod() != 0) {
                scheduleAt(simTime() + (can_msg->getPeriod() / 1000.), can_msg);
            }
        }
    }*/
}

int CanSourceGatewayAppBase::checkAndReturnID(int id) {
    if (canVersion.compare("2.0A") == 0) {           //2.0A
        if (id < 0 || id > VERSIONAMAX) {
            EV<< "ID " << id << " not valid." << endl;
            endSimulation();
        }
    } else {                                    //2.0B
        if(id < 0 || id > VERSIONBMAX) {
            EV << "ID " << id << " not valid." << endl;
            endSimulation();
        }
    }
    return id;
}

int CanSourceGatewayAppBase::calculateLength(int dataLength) {
    int frameLength = 0;
    if (canVersion.compare("2.0B") == 0) {
        frameLength += ARBITRATIONFIELD29BIT;
    }
    return frameLength + DATAFRAMEOVERHEAD + (dataLength << 3); //TODO + StuffingBits
}

void CanSourceGatewayAppBase::dataFrameTransmission(CanDataFrame *df) {
    CanDataFrame *outgoingFrame;
    if (df->isSelfMessage()) {
        outgoingFrame = df->dup();
        scheduleAt(simTime() + (df->getPeriod() / 1000.), df);
    } else if (df->arrivedOn("remoteIn")) {
        for (std::vector<CanDataFrame*>::iterator it = outgoingDataFrames.begin();
                it != outgoingDataFrames.end(); ++it) {
            CanDataFrame *tmp = *it;
            if (tmp->getCanID() == df->getCanID()) {
                outgoingFrame = tmp->dup();
                break;
            }
        }
        delete df;
    }
    outgoingFrame->setStartTime(simTime());
    send(outgoingFrame, "out");
}
