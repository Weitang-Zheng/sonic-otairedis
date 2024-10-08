#pragma once

extern "C" {
#include "otai.h"
}

#include "OtaiAttrWrap.h"
#include "LinecardConfig.h"

#include "meta/Meta.h"

#include "swss/selectableevent.h"

#include <map>
#include <memory>
#include <thread>
#include <string>
#include <mutex>

namespace otaivs
{
    class LinecardState
    {
        public:

            /**
             * @brief AttrHash key is attribute ID, value is actual attribute
             */
            typedef std::map<std::string, std::shared_ptr<OtaiAttrWrap>> AttrHash;

            /**
             * @brief ObjectHash is map indexed by object type and then serialized object id.
             */
            typedef std::map<otai_object_type_t, std::map<std::string, AttrHash>> ObjectHash;

        public:

            LinecardState(
                    _In_ otai_object_id_t linecard_id,
                    _In_ std::shared_ptr<LinecardConfig> config);

            virtual ~LinecardState();

        public:

            void setMeta(
                    std::weak_ptr<otaimeta::Meta> meta);

        public:

            otai_status_t getStatsExt(
                    _In_ otai_object_type_t obejct_type,
                    _In_ otai_object_id_t object_id,
                    _In_ uint32_t number_of_counters,
                    _In_ const otai_stat_id_t* counter_ids,
                    _In_ otai_stats_mode_t mode,
                    _Out_ otai_stat_value_t *counters);

        public:

            otai_object_id_t getLinecardId() const;

        protected:
            std::shared_ptr<otaimeta::Meta> getMeta();

        public: // TODO make private

            ObjectHash m_objectHash;

        protected:

            std::map<std::string, std::map<int, uint64_t>> m_countersMap;

            otai_object_id_t m_linecard_id;

        private : // tap device related objects
            std::mutex m_mutex;

        protected:

            std::weak_ptr<otaimeta::Meta> m_meta;

            std::shared_ptr<LinecardConfig> m_linecardConfig;

        public:

            std::set<std::string> m_otdrOidList;

    };
}
