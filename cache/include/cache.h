#ifndef __CACHE_H__
#define __CACHE_H__

#include<cstdint>
#include<iostream>
#include "cache_base.h"

#include "spdlog/cfg/env.h"  // support for loading levels from the environment variable
#include "spdlog/fmt/ostr.h" // support for user defined types
#include "spdlog/fmt/bin_to_hex.h"
#include "spdlog/sinks/basic_file_sink.h"
#include "spdlog/sinks/callback_sink.h"
#include "spdlog/logger.h"
#include "spdlog/spdlog.h"

namespace Kuiper {
	namespace Cache {
		class Debug {
		public:
			Debug();
			virtual ~Debug() {  };

		public:
			bool inline file_exists(const std::string& file_name) {
				struct stat buffer;
				return (stat(file_name.c_str(), &buffer) == 0);
			}
			auto SetDebug(const bool _debug = false, 
				const bool _dump = false, 
				int32_t _opt_id = 0);
			auto DumpData(const std::string& _log_prefix, const std::string& _log_tail, 
				const std::uint8_t* _data, std::size_t _len);
			
		private:
			std::shared_ptr<spdlog::logger> m_hd;
			std::shared_ptr<spdlog::logger> m_hp;
			bool m_debug;
			bool m_dump;
			bool m_self_save = false;
			std::uint32_t m_opt_id;
			std::string m_log_prefix = "logs/";
		};

		class Cache : public BaseCache, public Debug {
		public:
			Cache(sc_core::sc_module_name _name);

		};


	} // namespace  Cache
} // namespace Kuiper
#endif /* __CACHE_H__ */