#pragma once

#include "LinecardStateBase.h"
#include "LinecardConfigContainer.h"
#include "RealObjectIdManager.h"
#include "LinecardStateBase.h"
#include "EventQueue.h"
#include "EventPayloadPacket.h"
#include "EventPayloadNetLinkMsg.h"

#include "lib/inc/LaiInterface.h"

#include "meta/Meta.h"

#include <string>
#include <vector>
#include <map>

namespace laivs
{
    class VirtualLinecardLaiInterface:
        public lairedis::LaiInterface
    {
        public:

            VirtualLinecardLaiInterface(
                    _In_ const std::shared_ptr<LinecardConfigContainer> scc);

            virtual ~VirtualLinecardLaiInterface();

        public:

            virtual lai_status_t initialize(
                    _In_ uint64_t flags,
                    _In_ const lai_service_method_table_t *service_method_table) override;

            virtual lai_status_t uninitialize(void) override;

            virtual lai_status_t linkCheck(_Out_ bool *up) override;
        public: // LAI interface overrides

            virtual lai_status_t create(
                    _In_ lai_object_type_t objectType,
                    _Out_ lai_object_id_t* objectId,
                    _In_ lai_object_id_t linecardId,
                    _In_ uint32_t attr_count,
                    _In_ const lai_attribute_t *attr_list) override;

            virtual lai_status_t remove(
                    _In_ lai_object_type_t objectType,
                    _In_ lai_object_id_t objectId) override;

            virtual lai_status_t set(
                    _In_ lai_object_type_t objectType,
                    _In_ lai_object_id_t objectId,
                    _In_ const lai_attribute_t *attr) override;

            virtual lai_status_t get(
                    _In_ lai_object_type_t objectType,
                    _In_ lai_object_id_t objectId,
                    _In_ uint32_t attr_count,
                    _Inout_ lai_attribute_t *attr_list) override;

        public: // stats API

            virtual lai_status_t getStats(
                    _In_ lai_object_type_t object_type,
                    _In_ lai_object_id_t object_id,
                    _In_ uint32_t number_of_counters,
                    _In_ const lai_stat_id_t *counter_ids,
                    _Out_ lai_stat_value_t *counters) override;

            virtual lai_status_t getStatsExt(
                    _In_ lai_object_type_t object_type,
                    _In_ lai_object_id_t object_id,
                    _In_ uint32_t number_of_counters,
                    _In_ const lai_stat_id_t *counter_ids,
                    _In_ lai_stats_mode_t mode,
                    _Out_ lai_stat_value_t *counters) override;

            virtual lai_status_t clearStats(
                    _In_ lai_object_type_t object_type,
                    _In_ lai_object_id_t object_id,
                    _In_ uint32_t number_of_counters,
                    _In_ const lai_stat_id_t *counter_ids) override;

        public: // LAI API

            virtual lai_status_t objectTypeGetAvailability(
                    _In_ lai_object_id_t linecardId,
                    _In_ lai_object_type_t objectType,
                    _In_ uint32_t attrCount,
                    _In_ const lai_attribute_t *attrList,
                    _Out_ uint64_t *count) override;

            virtual lai_status_t queryAttributeCapability(
                    _In_ lai_object_id_t linecard_id,
                    _In_ lai_object_type_t object_type,
                    _In_ lai_attr_id_t attr_id,
                    _Out_ lai_attr_capability_t *capability) override;

            virtual lai_status_t queryAattributeEnumValuesCapability(
                    _In_ lai_object_id_t linecard_id,
                    _In_ lai_object_type_t object_type,
                    _In_ lai_attr_id_t attr_id,
                    _Inout_ lai_s32_list_t *enum_values_capability) override;

            virtual lai_object_type_t objectTypeQuery(
                    _In_ lai_object_id_t objectId) override;

            virtual lai_object_id_t linecardIdQuery(
                    _In_ lai_object_id_t objectId) override;

            virtual lai_status_t logSet(
                    _In_ lai_api_t api,
                    _In_ lai_log_level_t log_level) override;

        private: // QUAD API helpers

            lai_status_t create(
                    _In_ lai_object_id_t linecardId,
                    _In_ lai_object_type_t objectType,
                    _In_ const std::string& serializedObjectId,
                    _In_ uint32_t attr_count,
                    _In_ const lai_attribute_t *attr_list);

            lai_status_t remove(
                    _In_ lai_object_id_t linecardId,
                    _In_ lai_object_type_t objectType,
                    _In_ const std::string& serializedObjectId);

            lai_status_t set(
                    _In_ lai_object_id_t linecardId,
                    _In_ lai_object_type_t objectType,
                    _In_ const std::string& serializedObjectId,
                    _In_ const lai_attribute_t *attr);

            lai_status_t get(
                    _In_ lai_object_id_t linecardId,
                    _In_ lai_object_type_t objectType,
                    _In_ const std::string& serializedObjectId,
                    _In_ uint32_t attr_count,
                    _Inout_ lai_attribute_t *attr_list);

        private: // QUAD pre

            lai_status_t preSet(
                    _In_ lai_object_type_t objectType,
                    _In_ lai_object_id_t objectId,
                    _In_ const lai_attribute_t *attr);

            lai_status_t preSetPort(
                    _In_ lai_object_id_t objectId,
                    _In_ const lai_attribute_t *attr);

        private:

            std::shared_ptr<LinecardStateBase> init_linecard(
                    _In_ lai_object_id_t linecard_id,
                    _In_ std::shared_ptr<LinecardConfig> config,
                    _In_ std::weak_ptr<laimeta::Meta> meta,
                    _In_ uint32_t attr_count,
                    _In_ const lai_attribute_t *attr_list);

            void removeLinecard(
                    _In_ lai_object_id_t linecardId);

        public:

            void setMeta(
                    _In_ std::weak_ptr<laimeta::Meta> meta);

            void ageFdbs();

            void syncProcessEventNetLinkMsg(
                    _In_ std::shared_ptr<EventPayloadNetLinkMsg> payload);

        public:

            std::string getHardwareInfo(
                    _In_ uint32_t attr_count,
                    _In_ const lai_attribute_t *attr_list) const;

        private:

            std::weak_ptr<laimeta::Meta> m_meta;

            std::shared_ptr<LinecardConfigContainer> m_linecardConfigContainer;

            std::shared_ptr<RealObjectIdManager> m_realObjectIdManager;

            LinecardStateBase::LinecardStateMap m_linecardStateMap;
    };
}
