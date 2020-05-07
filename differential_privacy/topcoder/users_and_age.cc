//
// Created by Jerrish Varghese on 03/05/20.
//

#include "differential_privacy/topcoder/users_and_age.h"


#include <fstream>

#include "absl/strings/numbers.h"
#include "absl/strings/str_split.h"
#include "differential_privacy/algorithms/bounded-mean.h"
#include "differential_privacy/algorithms/bounded-sum.h"
#include "differential_privacy/algorithms/count.h"
#include "differential_privacy/algorithms/order-statistics.h"

namespace differential_privacy {
    namespace topcoder {

        UsersAgeReporter::UsersAgeReporter(std::string data_filename, double epsilon)
                : epsilon_(epsilon) {
            std::ifstream file(data_filename);
            std::string line;
            while (getline(file, line)) {
                std::vector <std::string> people_and_age = absl::StrSplit(line, ',');
                CHECK_EQ(people_and_age.size(), 11);
                int count;
                CHECK(absl::SimpleAtoi(people_and_age[8], &count));
                user_with_age[people_and_age[0]] = count;
            }
        }

        int UsersAgeReporter::Sum() {
            int sum = 0;
            for (const auto &pair : user_with_age) {
                sum += pair.second;
            }
            return sum;
        }

        double UsersAgeReporter::Mean() {
            return static_cast<double>(Sum()) / user_with_age.size();
        }

        int UsersAgeReporter::CountAbove(int limit) {
            int count = 0;
            for (const auto &pair : user_with_age) {
                if (pair.second > limit) {
                    ++count;
                }
            }
            return count;
        }

        int UsersAgeReporter::Max() {
            int max = 0;
            for (const auto &pair : user_with_age) {
                max = std::max(pair.second, max);
            }
            return max;
        }

        double UsersAgeReporter::PrivacyBudget() { return privacy_budget_; }

        base::StatusOr <Output> UsersAgeReporter::PrivateSum(double privacy_budget) {
            if (privacy_budget_ < privacy_budget) {
                return base::InvalidArgumentError("Not enough privacy budget.");
            }
            privacy_budget_ -= privacy_budget;
            ASSIGN_OR_RETURN(std::unique_ptr < BoundedSum < int >> sum_algorithm,
                             BoundedSum<int>::Builder()
                                     .SetEpsilon(epsilon_)
                                     .SetLower(0)
                                     .SetUpper(150)
                                     .Build());
            for (const auto &pair : user_with_age) {
                sum_algorithm->AddEntry(pair.second);
            }
            return sum_algorithm->PartialResult(privacy_budget);
        }

        base::StatusOr <Output> UsersAgeReporter::PrivateMean(double privacy_budget) {
            if (privacy_budget_ < privacy_budget) {
                return base::InvalidArgumentError("Not enough privacy budget.");
            }
            privacy_budget_ -= privacy_budget;
            ASSIGN_OR_RETURN(std::unique_ptr < BoundedMean < int >> mean_algorithm,
                             BoundedMean<int>::Builder().SetEpsilon(epsilon_).Build());
            for (const auto &pair : user_with_age) {
                mean_algorithm->AddEntry(pair.second);
            }
            return mean_algorithm->PartialResult(privacy_budget);
        }

        base::StatusOr <Output> UsersAgeReporter::PrivateCountAbove(double privacy_budget,
                                                                    int limit) {
            if (privacy_budget_ < privacy_budget) {
                return base::InvalidArgumentError("Not enough privacy budget.");
            }
            privacy_budget_ -= privacy_budget;
            ASSIGN_OR_RETURN(std::unique_ptr < Count < std::string >> count_algorithm,
                             Count<std::string>::Builder().SetEpsilon(epsilon_).Build());

            for (const auto &pair : user_with_age) {
                if (pair.second > limit) {
                    count_algorithm->AddEntry(pair.first);
                }
            }
            return count_algorithm->PartialResult(privacy_budget);
        }

        base::StatusOr <Output> UsersAgeReporter::PrivateMax(double privacy_budget) {
            if (privacy_budget_ < privacy_budget) {
                return base::InvalidArgumentError("Not enough privacy budget.");
            }
            privacy_budget_ -= privacy_budget;
            ASSIGN_OR_RETURN(std::unique_ptr < continuous::Max < int >> max_algorithm,
                             continuous::Max<int>::Builder()
                                     .SetEpsilon(epsilon_)
                                     .SetLower(0)
                                     .SetUpper(150)
                                     .Build());
            for (const auto &pair : user_with_age) {
                max_algorithm->AddEntry(pair.second);
            }
            return max_algorithm->PartialResult(privacy_budget);
        }
    }
}