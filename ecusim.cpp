/*  www.lindwurm-can.org
 *  Copyright (C) 2023 Sascha Muenzberg <sascha@lindwurm-can.org>
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Affero General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Affero General Public License for more details.
 *
 *  You should have received a copy of the GNU Affero General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "ecusim.h"

#include <iostream>
#include <unistd.h>
#include <cstring>
#include <chrono>
#include <thread>

#include <sys/socket.h>
#include <net/if.h>
#include <linux/can.h>

#define MAX_DATA_SIZE 65536

ECUSim::ECUSim(unsigned short sourceAddress, unsigned short targetAddress, const std::string &name, const std::string &interface, const UdsDataIdMap& idMap)
    : m_sourceAddress(sourceAddress), m_targetAddress(targetAddress), m_name(name), m_interface(interface), m_idMap(idMap)
{
    std::cout << "Starting ECU: " << m_name << " -- SA: " << std::uppercase << std::hex << m_sourceAddress
                                            << " / TA: " << std::uppercase << std::hex << m_targetAddress
                                            << std::endl << std::flush;
}

void ECUSim::operator()()
{
    int canSocket;
    struct sockaddr_can tpAddress;

    unsigned char data[MAX_DATA_SIZE];

    tpAddress.can_addr.tp.tx_id = m_targetAddress;
    tpAddress.can_addr.tp.rx_id = m_sourceAddress;

    canSocket = socket(PF_CAN, SOCK_DGRAM, CAN_ISOTP);

    if ( canSocket < 0)
    {
        perror("socket");
        return;
    }

    tpAddress.can_family = AF_CAN;
    tpAddress.can_ifindex = if_nametoindex(m_interface.c_str());

    if ( ! tpAddress.can_ifindex )
    {
        perror("if_nametoindex");
        return;
    }

    if ( bind(canSocket, (struct sockaddr *)&tpAddress, sizeof(tpAddress)) < 0)
    {
        perror("bind");
        close(canSocket);
        return;
    }

    do
    {
        int numberOfBytes = read(canSocket, data, MAX_DATA_SIZE);

        if ( (numberOfBytes < 1) || (numberOfBytes > MAX_DATA_SIZE) )
        {
            continue;
        }

        dumpData(data, numberOfBytes, '>');

        if ( (numberOfBytes == 2) && (data[0] == 0x10) && (data[1] == 0x01) )
        {
            // start diagnostic session
            data[0] = 0x50;
            data[2] = 0x01;

            write(canSocket, data, 2);
            dumpData(data, 2, '<');

            continue;
        }

        // Read data by identifier
        if ( (numberOfBytes == 3) && (data[0] == 0x22) )
        {
            std::this_thread::sleep_for( std::chrono::milliseconds(10) );

            uint16_t dataId = (data[1] << 8) | data[2];

            const char *idData = m_idMap[dataId].data;

            if ( idData == nullptr )
            {
                data[0] = '\x7F';
                data[1] = '\x22';
                data[2] = '\x31';

                write(canSocket, data, 3 );
                dumpData(data, 3, '<');
                continue;
            }

            int responseSize = m_idMap[dataId].dataLength + 3;

            if ( responseSize > MAX_DATA_SIZE )
            {
                data[0] = '\x7F';
                data[1] = '\x22';
                data[2] = '\x31';

                write(canSocket, data, 3 );
                dumpData(data, 3, '<');
                continue;
            }

            data[0] = '\x62';
            memcpy( (data + 3), idData, m_idMap[dataId].dataLength);

            write(canSocket, data, responseSize );
            dumpData( data, responseSize, '<');
            continue;

        }

    }
    while (true);

    close(canSocket);
}

void ECUSim::dumpData(const unsigned char *data, int numberOfBytes, char prefix)
{
    printf("%c %s: ", prefix, m_name.c_str());

    for (int i=0; i < numberOfBytes; i++)
    {
        printf("%02X ", data[i]);
    }
    printf("\n");

    std::cout << std::flush;
}
