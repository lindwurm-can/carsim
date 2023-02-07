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

#include <iostream>
#include <thread>
#include <vector>

#include "ecusim.h"

int main(int argc, char* argv[])
{
    std::cout << "Car UDS diagnostic simulator..." << std::endl;

    const char *canInterface;

    if (argc < 2)
    {
        std::cout << "Default to vcan0 interface." << std::endl;
        std::cout << "To select interface call " << argv[0] << " <ifname>" << std::endl;
        canInterface = "vcan0";
    }
    else
    {
        canInterface = argv[1];
        std::cout << "Using can interface " << canInterface << std::endl;
    }

    std::cout << std::endl;

    UdsDataIdMap ECMData
    {
        { UdsDataId::VehicleManufacturerECUSoftwareNumber,  "ECM-SW-Version" },
        { UdsDataId::VehicleManufacturerSparePartNumber,    "ECM-Part-Number" },
        { UdsDataId::ECUSerialNumber,                       "ECM-SerialNo" },
        { UdsDataId::ApplicationSoftwareIdentification,     "ECM-Application" },
        { UdsDataId::BootSoftwareFingerprint,              {"BYTES#\x01\x02\x00\x03\x04\xFF#BYTES", sizeof("BYTES#\x01\x02\x00\x03\x04\xFF#BYTES") - 1} }, // sizeof counts terminating NULL
        { 0x0100,                                           "MASTER-S3CRET" },
        { 0x0201,                                           "VIN-1133M-33113-ZZZ" }
    };


    UdsDataIdMap CMUData
    {
        { UdsDataId::VehicleManufacturerECUSoftwareNumber,  "CMU-SW-Version" },
        { UdsDataId::VehicleManufacturerSparePartNumber,    "CMU-Part-Number" },
        { UdsDataId::ECUSerialNumber,                       "CMU-SerialNo" },
        { UdsDataId::ApplicationSoftwareIdentification,     "CMU-Application" },
        { 0x0100,                                           "CMU-S3CRET" },
        { 0x0201,                                           "VIN-1133M-33113-ZZZ" }
    };

    UdsDataIdMap PDCData
    {
        { UdsDataId::VehicleManufacturerECUSoftwareNumber,  "PDC-SW-Version" },
        { UdsDataId::VehicleManufacturerSparePartNumber,    "PDC-Part-Number" },
        { UdsDataId::ECUSerialNumber,                       "PDC-SerialNo" },
        { UdsDataId::ApplicationSoftwareIdentification,     "PDC-Application" },
        { 0x0100,                                           "PDC-S3CRET" },
        { 0x0201,                                           "VIN-1133M-33113-ZZZ" }
    };

    std::vector<ECUSim> ECUs
    {
        {0x7E0, 0x7E8, "ECM", canInterface, ECMData},
        {0x707, 0x70F, "CMU", canInterface, CMUData},  // Connectivity Master Unit
        {0x720, 0x728, "AFS", canInterface},  // Adaptive Frontlight System
        {0x721, 0x729, "IC ", canInterface},  // Instrument Cluster
        {0x736, 0x73E, "ICA", canInterface},  // Integrated Cruise Assist
        {0x745, 0x74D, "PDC", canInterface, PDCData},  // Park Distance Control
        {0x750, 0x758, "DCU", canInterface},  // Door Control Unit
        {0x760, 0x768, "SCU", canInterface},  // Speed Control Unit
        {0x7C2, 0x7CA, "TCU", canInterface},  // Telematics Control Unit
        {0x706, 0x70E, "ACC", canInterface}   // Adaptive Cruise Control
    };

    int numberOfEcus = ECUs.size();

    std::vector<std::thread> threads;
    threads.reserve( numberOfEcus );

    for (int i = 0; i < numberOfEcus; i++)
    {
        threads.push_back( std::thread( ECUs[i]) );
    }

    for (int i = 0; i < numberOfEcus; i++)
    {
        threads[i].join();
    }

    return 0;
}
