//
// Copyright 2019 Google LLC
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//

#include "absl/flags/flag.h"
#include "absl/strings/str_format.h"
#include "differential_privacy/algorithms/confidence-interval.pb.h"
#include "differential_privacy/algorithms/util.h"
#include "differential_privacy/topcoder/users_and_age.h"
#include "differential_privacy/proto/data.pb.h"
#include "differential_privacy/proto/util.h"

using absl::PrintF;
using differential_privacy::BoundingReport;
using differential_privacy::ConfidenceInterval;
using differential_privacy::DefaultEpsilon;
using differential_privacy::GetValue;
using differential_privacy::Output;
using differential_privacy::topcoder::UsersAgeReporter;
using differential_privacy::base::StatusOr;

ABSL_FLAG(std::string, UserDataFile,
"differential_privacy/topcoder/users_sample.csv",
"Path to the datafile where the data is stored on the age of "
"the user.");

int main(int argc, char **argv) {
    PrintF(
            "\nUser age reporting using DP.\n");

    // Load the user data into the UsersAgeReporter. We use a higher epsilon to
    // obtain a higher accuracy since our dataset is very small.
    const double epsilon = 4 * DefaultEpsilon();
    UsersAgeReporter reporter(absl::GetFlag(FLAGS_UserDataFile), epsilon);

    // Query for the mean with a bounding report.
    PrintF(
            "\nGetting the average age of the crowd \n");
    PrintF("\nPrivacy budget remaining: %.2f\n", reporter.PrivacyBudget());
    PrintF("True mean: %.2f\n", reporter.Mean());
    StatusOr <Output> mean_status = reporter.PrivateMean(.25);
    if (!mean_status.ok()) {
        PrintF("Error obtaining mean: %s\n", mean_status.status().message());
        PrintF("Cannot query more on user data\n");
    } else {
        Output mean_output = mean_status.ValueOrDie();
        BoundingReport report = mean_output.error_report().bounding_report();
        double mean = GetValue<double>(mean_output);
        int lower_bound = GetValue<int>(report.lower_bound());
        int upper_bound = GetValue<int>(report.upper_bound());
        double num_inputs = report.num_inputs();
        double num_outside = report.num_outside();
        PrintF("DP mean output:\n%s\n", mean_output.DebugString());
        PrintF(
                "Here is how to interpret the result. %.2f is the DP mean. "
                "Since no bounds were set for  the DP mean algorithm, bounds on the "
                "input data were automatically determined. Most of the data fell "
                "between [%d, %d]. Thus, these bounds were used to determine clamping "
                "and global sensitivity. In addition, around %.0f input values fell "
                "inside of these bounds, and around %.0f inputs fell outside of these "
                "bounds. num_inputs and num_outside are themselves DP counts.\n",
                mean, lower_bound, upper_bound, num_inputs, num_outside);
    }

    // Query for the count with a noise confidence interval.
    {
        PrintF(
                "\nHow many people are above 50? And how accurate is the result?\n");
        PrintF("\nPrivacy budget remaining: %.2f\n", reporter.PrivacyBudget());
        Output count_output = reporter.PrivateCountAbove(.25, 50).ValueOrDie();
        int count = GetValue<int>(count_output);
        ConfidenceInterval ci =
                count_output.error_report().noise_confidence_interval();
        double confidence_level = ci.confidence_level();
        double lower_bound = ci.lower_bound();
        double upper_bound = ci.upper_bound();
        PrintF("True count: %d\n", reporter.CountAbove(50));
        PrintF("DP count output:\n%s\n", count_output.DebugString());
        PrintF(
                "The algorithm tells %d is the DP count. [%.2f, %.2f] is the "
                "%.2f confidence interval of the noise added to the count.\n",
                count, lower_bound, upper_bound, confidence_level);
    }

    // Query for the maximum.
    PrintF(
            "\n'Oldest person in the crowd.\n");
    PrintF("\nPrivacy budget remaining: %.2f\n", reporter.PrivacyBudget());
    PrintF("True max: %d\n", reporter.Max());
    PrintF("DP max:   %d\n",
           GetValue<int>(reporter.PrivateMax(.25).ValueOrDie()));
}
