//
// Created by Jerrish Varghese on 03/05/20.
//

#ifndef DIFFERENTIAL_PRIVACY_USERS_AND_AGE_H
#define DIFFERENTIAL_PRIVACY_USERS_AND_AGE_H

#include <map>
#include <string>

#include "differential_privacy/proto/data.pb.h"
#include "differential_privacy/base/statusor.h"

namespace differential_privacy {
    namespace topcoder {
        class UsersAgeReporter {
        public:
            UsersAgeReporter(std::string data_filename, double epsilon);

            int Sum();

            // True mean age of the population
            double Mean();

            // Count of population above the given age
            int CountAbove(int limit);

            // True maximum of the ages among the population.
            int Max();

            // Returns the remaining privacy budget.
            double PrivacyBudget();

            base::StatusOr <Output> PrivateSum(double privacy_budget);

            // DP mean age of the population
            base::StatusOr <Output> PrivateMean(double privacy_budget);

            // DP count of population above the given age
            base::StatusOr <Output> PrivateCountAbove(double privacy_budget, int limit);

            // DP maximum of the ages among the population.
            base::StatusOr <Output> PrivateMax(double privacy_budget);

        private:
            // Map from the user id as key and age as value
            std::map<std::string, int> user_with_age;

            // Differential privacy parameter epsilon. A larger epsilon corresponds to
            // less privacy and more accuracy.
            double epsilon_;

            // The privacy budget given to Farmer Fred at the beginning of the day. If
            // this budget depletes, Farmer Fred cannot ask anymore questions about the
            // carrot data. Privacy budget is represented as a fraction on [0, 1].
            double privacy_budget_ = 1;
        };
    }
}


#endif //DIFFERENTIAL_PRIVACY_USERS_AND_AGE_H
