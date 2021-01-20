/*
 * Copyright (C) 2016-2019 Istituto Italiano di Tecnologia (IIT)
 *
 * This software may be modified and distributed under the terms of the
 * BSD 3-Clause license. See the accompanying LICENSE file for details.
 */

#include <BayesFilters/LTIStateModel.h>

using namespace bfl;
using namespace Eigen;


LTIStateModel::LTIStateModel(const Ref<const MatrixXd>& transition_matrix, const Ref<const MatrixXd>& noise_covariance_matrix) :
    F_(transition_matrix),
    Q_(noise_covariance_matrix)
{
    if ((F_.rows() == 0) || (F_.cols() == 0))
        throw std::runtime_error("ERROR::LTISTATEMODEL::CTOR\nERROR:\n\tState transition matrix dimensions cannot be 0.");
    else if ((Q_.rows() == 0) || (Q_.cols() == 0))
        throw std::runtime_error("ERROR::LTISTATEMODEL::CTOR\nERROR:\n\tNoise covariance matrix dimensions cannot be 0.");
    else if (F_.rows() != F_.cols())
        throw std::runtime_error("ERROR::LTISTATEMODEL::CTOR\nERROR:\n\tState transition matrix must be a square matrix.");
    else if (Q_.rows() != Q_.cols())
        throw std::runtime_error("ERROR::LTISTATEMODEL::CTOR\nERROR:\n\tNoise covariance matrix must be a square matrix.");
    else if (F_.rows() != Q_.rows())
        throw std::runtime_error("ERROR::LTISTATEMODEL::CTOR\nERROR:\n\tNumber of rows of the state transition matrix must be the same as the size of the noise covariance matrix.");
}


LTIStateModel::LTIStateModel(LTIStateModel&& state_model) noexcept :
    F_(std::move(state_model.F_)),
    Q_(std::move(state_model.Q_))
{ }


LTIStateModel& LTIStateModel::operator=(LTIStateModel&& state_model) noexcept
{
    if (this == &state_model)
        return *this;

    F_ = std::move(state_model.F_);

    Q_ = std::move(state_model.Q_);

    return *this;
}


bool LTIStateModel::setProperty(const std::string& property)
{
    return false;
}


MatrixXd LTIStateModel::getNoiseCovarianceMatrix()
{
    return Q_;
}


MatrixXd LTIStateModel::getStateTransitionMatrix()
{
    return F_;
}


MatrixXd LTIStateModel::getJacobian()
{
    return F_;
}
