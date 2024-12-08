#pragma once
#include <Windows.h>
#include <cstdint>
#include <vector>
#include <string_view>



namespace swift {
	class scheduler {
	private:
		static scheduler* singleton;
		uintptr_t task_scheduler{ 0 };
		std::vector<uintptr_t> job_vector{};
		int identity = 8;
		
	public:
		static auto get_singleton() -> scheduler*;
		explicit scheduler();
		auto re_init() -> void;
		
		auto get_job(std::string_view) -> uintptr_t;
		auto get_script_context() -> uintptr_t;
		auto get_state() -> uintptr_t;

		auto get_identity() -> int;
		auto set_identity(int new_identity) -> void;
		





	};
}