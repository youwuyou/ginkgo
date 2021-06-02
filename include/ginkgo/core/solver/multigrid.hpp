/*******************************<GINKGO LICENSE>******************************
Copyright (c) 2017-2021, the Ginkgo authors
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions
are met:

1. Redistributions of source code must retain the above copyright
notice, this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright
notice, this list of conditions and the following disclaimer in the
documentation and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its
contributors may be used to endorse or promote products derived from
this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS
IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
******************************<GINKGO LICENSE>*******************************/

#ifndef GKO_PUBLIC_CORE_SOLVER_MULTIGRID_HPP_
#define GKO_PUBLIC_CORE_SOLVER_MULTIGRID_HPP_


#include <functional>
#include <memory>
#include <utility>


// #include <ginkgo/core/base/abstract_factory.hpp>
// #include <ginkgo/core/base/array.hpp>
// #include <ginkgo/core/base/dim.hpp>
// #include <ginkgo/core/base/exception_helpers.hpp>
// #include <ginkgo/core/base/lin_op.hpp>
// #include <ginkgo/core/base/math.hpp>
// #include <ginkgo/core/base/polymorphic_object.hpp>
// #include <ginkgo/core/base/types.hpp>
// #include <ginkgo/core/base/utils.hpp>
// #include <ginkgo/core/log/logger.hpp>
// #include <ginkgo/core/matrix/csr.hpp>
// #include <ginkgo/core/matrix/dense.hpp>
// #include <ginkgo/core/matrix/identity.hpp>
// #include <ginkgo/core/multigrid/multigrid_level.hpp>
// #include <ginkgo/core/stop/combined.hpp>
// #include <ginkgo/core/stop/criterion.hpp>

#include <ginkgo/core/base/array.hpp>
#include <ginkgo/core/base/exception_helpers.hpp>
#include <ginkgo/core/base/lin_op.hpp>
#include <ginkgo/core/base/math.hpp>
#include <ginkgo/core/base/types.hpp>
#include <ginkgo/core/log/logger.hpp>
#include <ginkgo/core/matrix/dense.hpp>
#include <ginkgo/core/matrix/identity.hpp>
#include <ginkgo/core/multigrid/multigrid_level.hpp>
#include <ginkgo/core/stop/combined.hpp>
#include <ginkgo/core/stop/criterion.hpp>


namespace gko {
namespace solver {


enum class multigrid_cycle { v, f, w, kfcg, kgcr };


enum class multigrid_mid_uses { pre, mid, post };
/**
 * Multigrid
 *
 * @tparam ValueType  precision of matrix elements
 *
 * @ingroup solvers
 * @ingroup LinOp
 */
template <typename ValueType = default_precision>
class Multigrid : public EnableLinOp<Multigrid<ValueType>> {
    friend class EnableLinOp<Multigrid>;
    friend class EnablePolymorphicObject<Multigrid, LinOp>;

public:
    using value_type = ValueType;
    using vector_type = matrix::Dense<ValueType>;

    /**
     * Return true as iterative solvers use the data in x as an initial guess.
     *
     * @return true as iterative solvers use the data in x as an initial guess.
     */
    bool apply_uses_initial_guess() const override { return true; }

    /**
     * Gets the stopping criterion factory of the solver.
     *
     * @return the stopping criterion factory
     */
    std::shared_ptr<const stop::CriterionFactory> get_stop_criterion_factory()
        const
    {
        return stop_criterion_factory_;
    }

    /**
     * Sets the stopping criterion of the solver.
     *
     * @param other  the new stopping criterion factory
     */
    void set_stop_criterion_factory(
        std::shared_ptr<const stop::CriterionFactory> other)
    {
        stop_criterion_factory_ = std::move(other);
    }

    /**
     * Gets the system operator of the linear system.
     *
     * @return the system operator
     */
    std::shared_ptr<const LinOp> get_system_matrix() const
    {
        return system_matrix_;
    }

    /**
     * Gets the list of MultigridLevel operator.
     *
     * @return the list of MultigridLevel opertor
     */
    const std::vector<std::shared_ptr<const gko::multigrid::MultigridLevel>>
    get_mg_level_list() const
    {
        return mg_level_list_;
    }

    /**
     * Gets the list of pre-smoother operator.
     *
     * @return the list of pre-smoother operator
     */
    const std::vector<std::shared_ptr<LinOp>> get_pre_smoother_list() const
    {
        return pre_smoother_list_;
    }

    /**
     * Gets the list of mid-smoother operator.
     *
     * @return the list of mid-smoother operator
     */
    const std::vector<std::shared_ptr<LinOp>> get_mid_smoother_list() const
    {
        return mid_smoother_list_;
    }

    /**
     * Gets the list of post-smoother operator.
     *
     * @return the list of post-smoother operator
     */
    const std::vector<std::shared_ptr<LinOp>> get_post_smoother_list() const
    {
        return post_smoother_list_;
    }

    /**
     * Gets the operator at the coarsest level.
     *
     * @return the coarsest operator
     */
    std::shared_ptr<const LinOp> get_coarsest_solver() const
    {
        return coarsest_solver_;
    }

    /**
     * Get the cycle of multigrid
     *
     * @return the cycle
     */
    multigrid_cycle get_cycle() const { return cycle_; }

    /**
     * Set the cycle of multigrid
     *
     * @param cycle the new cycle
     */
    void set_cycle(multigrid_cycle cycle) { cycle_ = cycle; }

    GKO_CREATE_FACTORY_PARAMETERS(parameters, Factory)
    {
        /**
         * Criterion factories.
         */
        std::vector<std::shared_ptr<const stop::CriterionFactory>>
            GKO_FACTORY_PARAMETER_VECTOR(criteria, nullptr);

        /**
         * MultigridLevel Factory list
         */
        std::vector<std::shared_ptr<const gko::LinOpFactory>>
            GKO_FACTORY_PARAMETER_VECTOR(mg_level, nullptr);

        /**
         * Custom selector (level, matrix)
         * default selector: use the first factory when mg_level size = 1
         *                   use the level as the index when mg_level size > 1
         */
        std::function<size_type(const size_type, const LinOp *)>
            GKO_FACTORY_PARAMETER_SCALAR(mg_level_index, nullptr);

        /**
         * Pre-smooth Factory list.
         * Its size must be 0, 1 or be the same as mg_level's.
         * when size = 0, do not use pre_smoother
         * when size = 1, use the first factory
         * when size > 1, use the same selector as mg_level
         * nullptr skips this pre_smoother at the level, which is different from
         * Identity Factory. Identity Factory updates x = x + relaxation *
         * residual.
         */
        std::vector<std::shared_ptr<const LinOpFactory>>
            GKO_FACTORY_PARAMETER_VECTOR(pre_smoother, nullptr);

        /**
         * Post-smooth Factory list.
         * It is similar to Pre-smooth Factory list. It is ignored if
         * post_uses_pre = true.
         */
        std::vector<std::shared_ptr<const LinOpFactory>>
            GKO_FACTORY_PARAMETER_VECTOR(post_smoother, nullptr);

        /**
         * Mid-smooth Factory list. If it contains availble elements, multigrid
         * always generate the corresponding smoother. However, it only involve
         * in the procedure when cycle is k or f.
         * It is similar to Pre-smooth Factory list. It is ignored if
         * multigrid_mid_uses is not mid.
         */
        std::vector<std::shared_ptr<const LinOpFactory>>
            GKO_FACTORY_PARAMETER_VECTOR(mid_smoother, nullptr);

        /**
         * Whether Post-related calls use corresponding pre-related calls.
         */
        bool GKO_FACTORY_PARAMETER_SCALAR(post_uses_pre, true);

        /**
         * Which Mid-related calls use pre/mid/post-related calls.
         * Availble options: pre/mid/post.
         */
        multigrid_mid_uses GKO_FACTORY_PARAMETER_SCALAR(
            mid_case, multigrid_mid_uses::pre);

        /**
         * The maximum level can be generated
         */
        size_type GKO_FACTORY_PARAMETER_SCALAR(max_levels, 10);

        /**
         * The minimal coarse rows.
         * If generation gets the matrix which contains less than
         * `min_coarse_rows`, the generation stops.
         */
        size_type GKO_FACTORY_PARAMETER_SCALAR(min_coarse_rows, 2);

        /**
         * Coarsest factory list.
         */
        std::vector<std::shared_ptr<const LinOpFactory>>
            GKO_FACTORY_PARAMETER_VECTOR(coarsest_solver, nullptr);

        /**
         * Custom solver selector (level, matrix)
         * default selector: use the first factory
         */
        std::function<size_type(const size_type, const LinOp *)>
            GKO_FACTORY_PARAMETER_SCALAR(solver_index, nullptr);

        /**
         * Multigrid cycle type
         * Options: v, f, w, kfcg and kgcr
         */
        multigrid_cycle GKO_FACTORY_PARAMETER_SCALAR(cycle, multigrid_cycle::v);

        /**
         * kcycle_base is a factor to choose how often enable FCG/GCR step.
         * This parameter is ignored on v, w, f cycle.
         * Enable the FCG/GCR step when level % kcycle_base == 0
         */
        size_type GKO_FACTORY_PARAMETER_SCALAR(kcycle_base, 1);

        /**
         * kcycle_rel_tol decides whether run the second iteration of FCG/GCR
         * step.
         * kcycle_rel_tol <= 0: always run one iterations.
         * kcycle_rel_tol == inf: always run two iterations.
         * ||updated_r|| <= kcycle_rel_tol ||r||: run second iteration.
         */
        remove_complex<ValueType> GKO_FACTORY_PARAMETER_SCALAR(kcycle_rel_tol,
                                                               0.25);
    };
    GKO_ENABLE_LIN_OP_FACTORY(Multigrid, parameters, Factory);
    GKO_ENABLE_BUILD_METHOD(Factory);

protected:
    void apply_impl(const LinOp *b, LinOp *x) const override;

    void apply_impl(const LinOp *alpha, const LinOp *b, const LinOp *beta,
                    LinOp *x) const override;

    /**
     * Generates the analysis structure from the system matrix and the right
     * hand side needed for the level solver.
     */
    void generate();

    explicit Multigrid(std::shared_ptr<const Executor> exec)
        : EnableLinOp<Multigrid>(exec)
    {}

    explicit Multigrid(const Factory *factory,
                       std::shared_ptr<const LinOp> system_matrix)
        : EnableLinOp<Multigrid>(factory->get_executor(),
                                 transpose(system_matrix->get_size())),
          parameters_{factory->get_parameters()},
          system_matrix_{system_matrix},
          one_op_{std::move(initialize<vector_type>({one<ValueType>()},
                                                    factory->get_executor()))},
          neg_one_op_{std::move(initialize<vector_type>(
              {-one<ValueType>()}, factory->get_executor()))}
    {
        GKO_ASSERT_IS_SQUARE_MATRIX(system_matrix);

        stop_criterion_factory_ =
            stop::combine(std::move(parameters_.criteria));
        if (!parameters_.mg_level_index) {
            if (parameters_.mg_level.size() == 1) {
                mg_level_index_ = [](const size_type, const LinOp *) {
                    return size_type{0};
                };
            } else if (parameters_.mg_level.size() > 1) {
                mg_level_index_ = [](const size_type level, const LinOp *) {
                    return level;
                };
            }
        } else {
            mg_level_index_ = parameters_.mg_level_index;
        }
        if (!parameters_.solver_index) {
            if (parameters_.coarsest_solver.size() >= 1) {
                solver_index_ = [](const size_type, const LinOp *) {
                    return size_type{0};
                };
            }
        } else {
            solver_index_ = parameters_.solver_index;
        }
        const auto mg_level_len = parameters_.mg_level.size();
        if (mg_level_len == 0) {
            GKO_NOT_SUPPORTED(this);
        } else {
            // each mg_level can not be nullptr
            for (size_type i = 0; i < mg_level_len; i++) {
                if (parameters_.mg_level.at(i) == nullptr) {
                    GKO_NOT_SUPPORTED(this);
                }
            }
        }
        // verify pre-related parameters
        this->verify_legal_length(true, parameters_.pre_smoother.size(),
                                  mg_level_len);
        // verify post-related parameters when post does not use pre
        this->verify_legal_length(!parameters_.post_uses_pre,
                                  parameters_.post_smoother.size(),
                                  mg_level_len);
        // verify mid-related parameters when mid does not use pre/post.
        this->verify_legal_length(
            parameters_.mid_case == multigrid_mid_uses::mid,
            parameters_.mid_smoother.size(), mg_level_len);

        cycle_ = parameters_.cycle;
        if (system_matrix_->get_size()[0] != 0) {
            // generate on the existed matrix
            this->generate();
        }
    }
    void verify_legal_length(bool checked, size_type len, size_type ref_len)
    {
        if (checked) {
            // len = 0 uses default behaviour
            // len = 1 uses the first one
            // len > 1 : must contain the same len as ref(mg_level)
            if (len > 1 && len != ref_len) {
                GKO_NOT_SUPPORTED(this);
            }
        }
    }

private:
    std::shared_ptr<const LinOp> system_matrix_{};
    std::shared_ptr<const stop::CriterionFactory> stop_criterion_factory_{};
    std::vector<std::shared_ptr<const gko::multigrid::MultigridLevel>>
        mg_level_list_{};
    std::vector<std::shared_ptr<LinOp>> pre_smoother_list_{};
    std::vector<std::shared_ptr<LinOp>> mid_smoother_list_{};
    std::vector<std::shared_ptr<LinOp>> post_smoother_list_{};
    std::shared_ptr<LinOp> coarsest_solver_{};
    std::function<size_type(const size_type, const LinOp *)> mg_level_index_;
    std::function<size_type(const size_type, const LinOp *)> solver_index_;
    std::shared_ptr<matrix::Dense<ValueType>> one_op_;
    std::shared_ptr<matrix::Dense<ValueType>> neg_one_op_;
    multigrid_cycle cycle_;
};


}  // namespace solver
}  // namespace gko


#endif  // GKO_PUBLIC_CORE_SOLVER_MULTIGRID_HPP_
