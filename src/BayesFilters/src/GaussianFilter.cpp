/*
 * Copyright (C) 2016-2019 Istituto Italiano di Tecnologia (IIT)
 *
 * This software may be modified and distributed under the terms of the
 * BSD 3-Clause license. See the accompanying LICENSE file for details.
 */

#include <BayesFilters/GaussianFilter.h>

using namespace bfl;


GaussianFilter::GaussianFilter(std::unique_ptr<GaussianPrediction> prediction, std::unique_ptr<GaussianCorrection> correction) noexcept :
    prediction_(std::move(prediction)),
    correction_(std::move(correction))
{ }


bool GaussianFilter::skip(const std::string& what_step, const bool status)
{
    if (what_step == "prediction" ||
        what_step == "state" ||
        what_step == "exogenous")
        return prediction_->skip(what_step, status);

    if (what_step == "correction")
        return correction_->skip(status);

    if (what_step == "all")
    {
        bool return_status = true;

        return_status &= prediction_->skip("prediction", status);

        return_status &= correction_->skip(status);

        return return_status;
    }

    return false;
}


GaussianPrediction& GaussianFilter::prediction()
{
    return *prediction_;
}


GaussianCorrection& GaussianFilter::correction()
{
    return *correction_;
}
