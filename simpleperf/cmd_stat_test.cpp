/*
 * Copyright (C) 2015 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <gtest/gtest.h>

#include <base/stringprintf.h>

#include "command.h"
#include "test_util.h"

static std::unique_ptr<Command> StatCmd() {
  return CreateCommandInstance("stat");
}

TEST(stat_cmd, no_options) {
  ASSERT_TRUE(StatCmd()->Run({"sleep", "1"}));
}

TEST(stat_cmd, event_option) {
  ASSERT_TRUE(StatCmd()->Run({"-e", "cpu-clock,task-clock", "sleep", "1"}));
}

TEST(stat_cmd, system_wide_option) {
  ASSERT_TRUE(StatCmd()->Run({"-a", "sleep", "1"}));
}

TEST(stat_cmd, verbose_option) {
  ASSERT_TRUE(StatCmd()->Run({"--verbose", "sleep", "1"}));
}

TEST(stat_cmd, tracepoint_event) {
  ASSERT_TRUE(StatCmd()->Run({"-a", "-e", "sched:sched_switch", "sleep", "1"}));
}

TEST(stat_cmd, event_modifier) {
  ASSERT_TRUE(StatCmd()->Run({"-e", "cpu-cycles:u,sched:sched_switch:k", "sleep", "1"}));
}

TEST(stat_cmd, existing_processes) {
  std::vector<std::unique_ptr<Workload>> workloads;
  CreateProcesses(2, &workloads);
  std::string pid_list =
      android::base::StringPrintf("%d,%d", workloads[0]->GetPid(), workloads[1]->GetPid());
  ASSERT_TRUE(StatCmd()->Run({"-p", pid_list}));
}

TEST(stat_cmd, existing_threads) {
  std::vector<std::unique_ptr<Workload>> workloads;
  CreateProcesses(2, &workloads);
  // Process id can be used as thread id in linux.
  std::string tid_list =
      android::base::StringPrintf("%d,%d", workloads[0]->GetPid(), workloads[1]->GetPid());
  ASSERT_TRUE(StatCmd()->Run({"-t", tid_list}));
}

TEST(stat_cmd, no_monitored_threads) {
  ASSERT_FALSE(StatCmd()->Run({""}));
}