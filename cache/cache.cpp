#include "include/cache.h"

namespace Kuiper {
	namespace Cache {
		Debug::Debug(const bool _debug, const bool _dump) {
			if(true == _dump) {
				spdlog::set_default_logger(spdlog::basic_logger_mt("cache_logger", 
				m_log_prefix + "log.txt"));
				spdlog::set_pattern("%v");
			}
		};

		auto Debug::SetDebug(const bool _debug, const bool _dump, int32_t _opt_id = 0) {
			m_self_save = true;
			m_debug = _debug;
			m_dump = _dump;
			m_opt_id = _opt_id;
		};

		auto Debug::DumpData(const std::string& _log_prefix, const std::string& _log_tail, const std::uint8_t* _data, std::size_t _len) {
			std::vector<std::uint8_t> buf;
			for (std::uint32_t i = 0; i < _len; i++)
				buf.push_back(*(_data + i));

			if (true == m_dump) {
				if (false == m_self_save) {
					m_hd->info("{:#n}", spdlog::to_hex(buf));
				}
				else {
					std::string log_file = m_log_prefix + _log_prefix +
						"_cacheline_" + _log_tail + "_optId_" + std::to_string(m_opt_id) + ".txt";
					if (false == file_exists(log_file)) {
						auto file_sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>(log_file, true);
						m_hd = std::make_shared<spdlog::logger>("NeuralEngine", file_sink);
						m_hd->set_pattern("%v");
					}
					m_hd->info("{:#n}", spdlog::to_hex(buf));
				}
			}
			else {
				spdlog::info("{:#n}", spdlog::to_hex(buf));
			}
		}

		// Cache::Cache(sc_core::sc_module_name _name) :
		// 	BaseCache(_name) {};


	} /* namespace Cache */
} /* namespace Kuiper */


