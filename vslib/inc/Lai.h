#pragma once

#include "VirtualLinecardLaiInterface.h"
#include "LaneMapContainer.h"
#include "EventQueue.h"
#include "EventPayloadNotification.h"
#include "ResourceLimiterContainer.h"
#include "CorePortIndexMapContainer.h"

#include "meta/Meta.h"

#include "swss/selectableevent.h"
#include "swss/dbconnector.h"
#include "swss/notificationconsumer.h"

#include <string>
#include <vector>
#include <memory>
#include <mutex>

namespace laivs
{
    class Lai:
        public lairedis::LaiInterface
    {
        public:

            Lai();

            virtual ~Lai();

        public:

            lai_status_t initialize(
                    _In_ uint64_t flags,
                    _In_ const lai_service_method_table_t *service_method_table);

            lai_status_t uninitialize(void) override;

            lai_status_t linkCheck(_Out_ bool *up);

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
                    _In_ lai_object_type_t objectType,
                    _In_ const std::string& serializedObjectId,
                    _In_ uint32_t attr_count,
                    _In_ const lai_attribute_t *attr_list);

            lai_status_t remove(
                    _In_ lai_object_type_t objectType,
                    _In_ const std::string& serializedObjectId);

            lai_status_t set(
                    _In_ lai_object_type_t objectType,
                    _In_ const std::string& serializedObjectId,
                    _In_ const lai_attribute_t *attr);

            lai_status_t get(
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

        private: // unittests

            void startUnittestThread();

            void stopUnittestThread();

            void unittestChannelThreadProc();

            void channelOpEnableUnittest(
                    _In_ const std::string &key,
                    _In_ const std::vector<swss::FieldValueTuple> &values);

            void channelOpSetReadOnlyAttribute(
                    _In_ const std::string &key,
                    _In_ const std::vector<swss::FieldValueTuple> &values);

            void channelOpSetStats(
                    _In_ const std::string &key,
                    _In_ const std::vector<swss::FieldValueTuple> &values);

            void handleUnittestChannelOp(
                    _In_ const std::string &op,
                    _In_ const std::string &key,
                    _In_ const std::vector<swss::FieldValueTuple> &values);

        private: // check link
            void startCheckLinkThread();
            void stopCheckLinkThread();
            void checkLinkThreadProc();

        private: // event queue

            void startEventQueueThread();

            void stopEventQueueThread();

            void eventQueueThreadProc();

            void processQueueEvent(
                    _In_ std::shared_ptr<Event> event);

            void syncProcessEventNetLinkMsg(
                    _In_ std::shared_ptr<EventPayloadNetLinkMsg> payload);

            void asyncProcessEventNotification(
                    _In_ std::shared_ptr<EventPayloadNotification> payload);

        private: // unittests

            bool m_unittestChannelRun;

            std::shared_ptr<swss::SelectableEvent> m_unittestChannelThreadEvent;

            std::shared_ptr<std::thread> m_unittestChannelThread;

            std::shared_ptr<swss::NotificationConsumer> m_unittestChannelNotificationConsumer;

            std::shared_ptr<swss::DBConnector> m_dbNtf;

        private: // link check
            bool m_checkLinkRun;
            std::shared_ptr<std::thread> m_checkLinkThread;

        private: // event queue

            bool m_eventQueueThreadRun;

            std::shared_ptr<std::thread> m_eventQueueThread;

            std::shared_ptr<EventQueue> m_eventQueue;

            std::shared_ptr<Signal> m_signal;

        private:

            bool m_apiInitialized;

            bool m_isLinkUp;

            bool m_isAlarm;

            bool m_isEvent;

            std::recursive_mutex m_apimutex;

            std::shared_ptr<laimeta::Meta> m_meta;

            std::shared_ptr<VirtualLinecardLaiInterface> m_vsLai;

            lai_service_method_table_t m_service_method_table;

            std::shared_ptr<LaneMapContainer> m_laneMapContainer;

            std::shared_ptr<LaneMapContainer> m_fabricLaneMapContainer;

            std::shared_ptr<ResourceLimiterContainer> m_resourceLimiterContainer;

            std::shared_ptr<CorePortIndexMapContainer> m_corePortIndexMapContainer;
    };
}
