/*
 * Copyright (C) 2016-2019 Istituto Italiano di Tecnologia (IIT)
 *
 * This software may be modified and distributed under the terms of the
 * BSD 3-Clause license. See the accompanying LICENSE file for details.
 */

#include <BayesFilters/SIS.h>
#include <BayesFilters/utils.h>

#include <fstream>
#include <iostream>
#include <utility>

#include <Eigen/Dense>

using namespace bfl;
using namespace Eigen;


SIS::SIS
(
    unsigned int num_particle,
    std::size_t state_size_linear,
    std::size_t state_size_circular,
    std::unique_ptr<ParticleSetInitialization> initialization,
    std::unique_ptr<PFPrediction> prediction,
    std::unique_ptr<PFCorrection> correction,
    std::unique_ptr<Resampling> resampling
) noexcept :
    ParticleFilter(std::move(initialization), std::move(prediction), std::move(correction), std::move(resampling)),
    num_particle_(num_particle),
    state_size_(state_size_linear + state_size_circular),
    pred_particle_(num_particle_, state_size_linear, state_size_circular),
    cor_particle_(num_particle_, state_size_linear, state_size_circular)
{ }


SIS::SIS
(
    unsigned int num_particle,
    std::size_t state_size_linear,
    std::unique_ptr<ParticleSetInitialization> initialization,
    std::unique_ptr<PFPrediction> prediction,
    std::unique_ptr<PFCorrection> correction,
    std::unique_ptr<Resampling> resampling
) noexcept :
    SIS(num_particle, state_size_linear, 0, std::move(initialization), std::move(prediction), std::move(correction), std::move(resampling))
{ }


SIS::SIS(SIS&& sir_pf) noexcept :
    ParticleFilter(std::move(sir_pf)),
    num_particle_(sir_pf.num_particle_),
    state_size_(sir_pf.state_size_),
    pred_particle_(std::move(sir_pf.pred_particle_)),
    cor_particle_(std::move(sir_pf.cor_particle_))
{ }


SIS& SIS::operator=(SIS&& sir_pf) noexcept
{
    if (this == &sir_pf)
        return *this;

    ParticleFilter::operator=(std::move(sir_pf));

    num_particle_ = sir_pf.num_particle_;

    state_size_ = sir_pf.state_size_;

    pred_particle_ = std::move(sir_pf.pred_particle_);

    cor_particle_ = std::move(sir_pf.cor_particle_);

    return *this;
}


bool SIS::initialization()
{
    return initialization_->initialize(pred_particle_);
}


void SIS::filteringStep()
{
    if (getFilteringStep() != 0)
        prediction_->predict(cor_particle_, pred_particle_);

    if (correction_->freeze_measurements())
    {
        correction_->correct(pred_particle_, cor_particle_);

        /* Normalize weights using LogSumExp. */
        cor_particle_.weight().array() -= utils::log_sum_exp(cor_particle_.weight());
    }
    else
        cor_particle_ = pred_particle_;

    log();

    if (resampling_->neff(cor_particle_.weight()) < static_cast<double>(num_particle_)/3.0)
    {
        ParticleSet res_particle(num_particle_, state_size_);
        VectorXi res_parent(num_particle_, 1);

        resampling_->resample(cor_particle_, res_particle, res_parent);

        cor_particle_ = res_particle;
    }
}


bool SIS::runCondition()
{
    return true;
}


void SIS::log()
{
    logger(pred_particle_.state().transpose(), pred_particle_.weight().transpose(),
           cor_particle_.state().transpose(), cor_particle_.weight().transpose());
}
