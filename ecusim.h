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

#ifndef ECUSIM_H
#define ECUSIM_H

#include <string>
#include <cstring>
#include <map>

struct DataValue
{
    DataValue()
    {
        data        = nullptr;
        dataLength  = 0;
    }

    DataValue(const char* setData)
    {
        data        = setData;
        dataLength  = strlen(data);
    }

    DataValue(const char* setData, size_t setLength)
    {
        data        = setData;
        dataLength  = setLength;
    }

    const char* data;
    size_t      dataLength;
};

using UdsDataIdMap = std::map<uint16_t, DataValue>;

namespace UdsDataId
{
    enum
    {
        BootSoftwareIdentification                  = 0xF180,
        ApplicationSoftwareIdentification           = 0xF181,
        ApplicationDataIdentification               = 0xF182,
        BootSoftwareFingerprint                     = 0xF183,
        ApplicationSoftwareFingerprint              = 0xF184,
        ApplicationDataFingerprint                  = 0xF185,
        ActiveDiagnosticSession                     = 0xF186,
        VehicleManufacturerSparePartNumber          = 0xF187,
        VehicleManufacturerECUSoftwareNumber        = 0xF188,
        VehicleManufacturerECUSoftwareVersionNumber = 0xF189,
        SystemSupplierIdentifierDataIdentifier      = 0xF18A,
        ECUManufacturingDate                        = 0xF18B,
        ECUSerialNumber                             = 0xF18C,
        SupportedFunctionalUnits                    = 0xF18D,
        VehicleManufacturerKitAssemblyPartNumber    = 0xF18E,
        ISOSAEReservedStandardized                  = 0xF18F,
        VIN                                         = 0xF190,
        VehicleManufacturerECUHardwareNumber        = 0xF191,
        SystemSupplierECUHardwareNumber             = 0xF192,
        SystemSupplierECUHardwareVersionNumber      = 0xF193,
        SystemSupplierECUSoftwareNumber             = 0xF194,
        SystemSupplierECUSoftwareVersionNumber      = 0xF195,
        ExhaustRegulationOrTypeApprovalNumber       = 0xF196,
        SystemNameOrEngineType                      = 0xF197,
        RepairShopCodeOrTesterSerialNumber          = 0xF198,
        ProgrammingDate                             = 0xF199,
        CalibrationRepairShopCodeOrCalibrationEquipmentSerialNumber = 0xF19A,
        CalibrationDate                             = 0xF19B,
        CalibrationEquipmentSoftwareNumber          = 0xF19C,
        ECUInstallationDate                         = 0xF19D,
        ODXFile                                     = 0xF19E,
        Entity                                      = 0xF19F,

        NumberOfEDRDevices                          = 0xFA10,
        EDRIdentification                           = 0xFA11,
        EDRDeviceAddressInformation                 = 0xFA12,

        EDREntry6                                   = 0xFA13,
        EDREntry5                                   = 0xFA14,
        EDREntry4                                   = 0xFA15,
        EDREntry3                                   = 0xFA16,
        EDREntry2                                   = 0xFA17,
        EDREntry1                                   = 0xFA18,

        UDSVersion                                  = 0xFF00
    };
}

class ECUSim
{
    public:

        ECUSim(unsigned short sourceAddress, unsigned short targetAddress, const std::string &name, const std::string &interface = "vcan0", const UdsDataIdMap& idMap = {});

        void    operator()();

    private:

        void    dumpData(const unsigned char *data, int numberOfBytes, char prefix);

    private:

        unsigned short  m_sourceAddress;
        unsigned short  m_targetAddress;
        std::string     m_name;
        std::string     m_interface;
        UdsDataIdMap    m_idMap;
};

#endif // ECUSIM_H
