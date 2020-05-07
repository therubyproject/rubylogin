/**
 * Ruby Login
 * Copyright (C) 2020 Leandro Matheus de Oliveira Sarna
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#ifndef THREAD_HOLDER_H
#define THREAD_HOLDER_H

#include <thread>
#include <atomic>

template <typename Derived>
class ThreadHolder {
    public:
        enum class ThreadState {
            Running,
            Closing,
            Terminated,
        };

        ThreadHolder() = default;
        ~ThreadHolder() = default;

        // non-copyable
        ThreadHolder(const ThreadHolder&) = delete;
        ThreadHolder& operator=(const ThreadHolder&) = delete;
        
        void start() {
            setState(ThreadState::Running);
            thread = std::thread(&Derived::threadMain, static_cast<Derived*>(this));
        }

        void stop() {
            setState(ThreadState::Closing);
        }

        void join() {
            if (thread.joinable()) {
                thread.join();
            }
        }
    protected:
        void setState(ThreadState newState) {
            threadState.store(newState, std::memory_order_relaxed);
        }

        ThreadState getState() const {
            return threadState.load(std::memory_order_relaxed);
        }
    private:
        std::atomic<ThreadState> threadState{ThreadState::Terminated};
        std::thread thread;
};

#endif
