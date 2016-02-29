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

#ifndef SIGNALSANDGATEWAYS_NOGATEWAYBUFFERING_H_
#define SIGNALSANDGATEWAYS_NOGATEWAYBUFFERING_H_

//SignalsAndGateways
#include "signalsandgateways/base/SignalsAndGateways_Defs.h"

namespace SignalsAndGateways {

/**
 * @brief This buffering module forwards incoming frames to the transformation module.
 *
 * This module doesn't have any buffering features. Its only functionality is the forwarding of an incoming frame to the next module.
 *
 * @author Till Steinbach
 */
class NoGatewayBuffering : public cSimpleModule
{
protected:
    /**
     * @brief Initialization of the module.
     */
    virtual void initialize();

    virtual void handleMessage(cMessage *msg);
};

} //namespace

#endif