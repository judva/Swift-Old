#include "../roblox.h"
#include "Windows.h"
#include <string>

#include "Scheduler.hpp"





namespace swift {
	scheduler* scheduler::singleton{ nullptr };
	auto scheduler::get_singleton() -> scheduler* {
		if (singleton == nullptr)
			singleton = new scheduler();

		return singleton;
	};

	scheduler::scheduler() {
		this->task_scheduler = reinterpret_cast<uintptr_t(*)()>(addresses::GetTaskScheduler)();

		uintptr_t Start = *reinterpret_cast<uintptr_t*>(this->task_scheduler + offsets::jobs_start_offset);
		uintptr_t End = *reinterpret_cast<uintptr_t*>(this->task_scheduler + offsets::jobs_end_offset);

		for (uintptr_t JobAddress = Start; JobAddress < End; JobAddress += 0x10) {
			this->job_vector.push_back(*reinterpret_cast<uintptr_t*>(JobAddress));
		}
	};

	// needed for teleporting and shi
	auto scheduler::re_init() -> void {
		this->task_scheduler = reinterpret_cast<uintptr_t(*)()>(addresses::GetTaskScheduler)();
		this->job_vector.clear();

		uintptr_t Start = *reinterpret_cast<uintptr_t*>(this->task_scheduler + offsets::jobs_start_offset);
		uintptr_t End = *reinterpret_cast<uintptr_t*>(this->task_scheduler + offsets::jobs_end_offset);

		for (uintptr_t JobAddress = Start; JobAddress < End; JobAddress += 0x10) {
			this->job_vector.push_back(*reinterpret_cast<uintptr_t*>(JobAddress));
		}
	}

	auto scheduler::get_job(std::string_view name) -> uintptr_t {
		for (uintptr_t job : this->job_vector) {
			std::string job_name = *reinterpret_cast<std::string*>(job + offsets::jobs_name_offset);
			if (job_name == name) {
				return job;
			}
		}
	}

	auto scheduler::get_script_context() -> uintptr_t {
		auto script_job = this->get_job("WaitingHybridScriptsJob");

		return *reinterpret_cast<uintptr_t*>(script_job + offsets::ScriptContextJob); //Gettting scriptcontext
	}

	auto scheduler::get_state() -> uintptr_t {
		uintptr_t script = NULL;
		uintptr_t script_context_state = *reinterpret_cast<uintptr_t*>(this->get_script_context() + 0x110);
		uintptr_t identity = (uintptr_t)(this->identity);
		uintptr_t encrypted_state = reinterpret_cast<uintptr_t(*)(uintptr_t, uintptr_t*, uintptr_t*)>(addresses::LuaGetState)(this->get_script_context() + 0x110, &identity, &script);
		return reinterpret_cast<uintptr_t(*)(uintptr_t)>(addresses::LuaDecryptState)(encrypted_state + offsets::DecryptState);
	}

	auto scheduler::get_identity() -> int {
		return this->identity;
	}
	auto scheduler::set_identity(int new_identity) -> void {
		this->identity = new_identity;
	}
}