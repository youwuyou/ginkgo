/*******************************<GINKGO LICENSE>******************************
Copyright (c) 2017-2022, the Ginkgo authors
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

#ifndef GKO_PUBLIC_CORE_SOLVER_GMRES_HPP_
#define GKO_PUBLIC_CORE_SOLVER_GMRES_HPP_


#include <vector>


#include <ginkgo/core/base/array.hpp>
#include <ginkgo/core/base/exception_helpers.hpp>
#include <ginkgo/core/base/lin_op.hpp>
#include <ginkgo/core/base/math.hpp>
#include <ginkgo/core/base/types.hpp>
#include <ginkgo/core/log/logger.hpp>
#include <ginkgo/core/matrix/dense.hpp>
#include <ginkgo/core/matrix/identity.hpp>
#include <ginkgo/core/solver/solver_base.hpp>
#include <ginkgo/core/stop/combined.hpp>
#include <ginkgo/core/stop/criterion.hpp>


namespace gko {
namespace solver {


constexpr size_type default_krylov_dim = 100u;


/**
 * GMRES or the generalized minimal residual method is an iterative type Krylov
 * subspace method which is suitable for nonsymmetric linear systems.
 *
 * The implementation in Ginkgo makes use of the merged kernel to make the best
 * use of data locality. The inner operations in one iteration of GMRES are
 * merged into 2 separate steps. Modified Gram-Schmidt is used.
 *
 * @tparam ValueType  precision of matrix elements
 *
 * @ingroup solvers
 * @ingroup LinOp
 */
template <typename ValueType = default_precision>
class Gmres
    : public EnableLinOp<Gmres<ValueType>>,
      public EnablePreconditionedIterativeSolver<ValueType, Gmres<ValueType>>,
      public Transposable {
    friend class EnableLinOp<Gmres>;
    friend class polymorphic_object_traits<Gmres>;

public:
    using value_type = ValueType;
    using transposed_type = Gmres<ValueType>;

    std::unique_ptr<LinOp> transpose() const override;

    std::unique_ptr<LinOp> conj_transpose() const override;

    /**
     * Return true as iterative solvers use the data in x as an initial guess.
     *
     * @return true as iterative solvers use the data in x as an initial guess.
     */
    bool apply_uses_initial_guess() const override { return true; }

    /**
     * Gets the Krylov dimension of the solver
     *
     * @return the Krylov dimension
     */
    size_type get_krylov_dim() const { return parameters_.krylov_dim; }

    /**
     * Sets the Krylov dimension
     *
     * @param other  the new Krylov dimension
     */
    void set_krylov_dim(size_type other) { parameters_.krylov_dim = other; }

    GKO_CREATE_FACTORY_PARAMETERS(parameters, Factory)
    {
        /**
         * Criterion factories.
         */
        std::vector<std::shared_ptr<const stop::CriterionFactory>>
            GKO_FACTORY_PARAMETER_VECTOR(criteria, nullptr);

        /**
         * Preconditioner factory.
         */
        std::shared_ptr<const LinOpFactory> GKO_FACTORY_PARAMETER_SCALAR(
            preconditioner, nullptr);

        /**
         * Already generated preconditioner. If one is provided, the factory
         * `preconditioner` will be ignored.
         */
        std::shared_ptr<const LinOp> GKO_FACTORY_PARAMETER_SCALAR(
            generated_preconditioner, nullptr);

        /**
         * Krylov dimension factory.
         */
        size_type GKO_FACTORY_PARAMETER_SCALAR(krylov_dim, 0u);
    };
    GKO_ENABLE_LIN_OP_FACTORY(Gmres, parameters, Factory);
    GKO_ENABLE_BUILD_METHOD(Factory);

protected:
    void apply_impl(const LinOp* b, LinOp* x) const override;

    void apply_dense_impl(const matrix::Dense<ValueType>* b,
                          matrix::Dense<ValueType>* x) const;

    void apply_impl(const LinOp* alpha, const LinOp* b, const LinOp* beta,
                    LinOp* x) const override;

    explicit Gmres(std::shared_ptr<const Executor> exec)
        : EnableLinOp<Gmres>(std::move(exec))
    {}

    explicit Gmres(const Factory* factory,
                   std::shared_ptr<const LinOp> system_matrix)
        : EnableLinOp<Gmres>(factory->get_executor(),
                             gko::transpose(system_matrix->get_size())),
          EnablePreconditionedIterativeSolver<ValueType, Gmres<ValueType>>{
              std::move(system_matrix), factory->get_parameters()},
          parameters_{factory->get_parameters()}
    {
        if (!parameters_.krylov_dim) {
            parameters_.krylov_dim = default_krylov_dim;
        }
    }
};


template <typename ValueType>
struct workspace_traits<Gmres<ValueType>> {
    using Solver = Gmres<ValueType>;
    // number of vectors used by this workspace
    static int num_vectors(const Solver&);
    // number of arrays used by this workspace
    static int num_arrays(const Solver&);
    // array containing the num_vectors names for the workspace vectors
    static std::vector<std::string> op_names(const Solver&);
    // array containing the num_arrays names for the workspace vectors
    static std::vector<std::string> array_names(const Solver&);
    // array containing all varying scalar vectors (independent of problem size)
    static std::vector<int> scalars(const Solver&);
    // array containing all varying vectors (dependent on problem size)
    static std::vector<int> vectors(const Solver&);

    // residual vector
    constexpr static int residual = 0;
    // preconditioned vector
    constexpr static int preconditioned_vector = 1;
    // krylov basis multivector
    constexpr static int krylov_bases = 2;
    // hessenberg matrix
    constexpr static int hessenberg = 3;
    // givens sin parameters
    constexpr static int givens_sin = 4;
    // givens cos parameters
    constexpr static int givens_cos = 5;
    // coefficients of the residual in Krylov space
    constexpr static int residual_norm_collection = 6;
    // residual norm scalar
    constexpr static int residual_norm = 7;
    // solution of the least-squares problem in Krylov space
    constexpr static int y = 8;
    // solution of the least-squares problem mapped to the full space
    constexpr static int before_preconditioner = 9;
    // preconditioned solution of the least-squares problem
    constexpr static int after_preconditioner = 10;
    // constant 1.0 scalar
    constexpr static int one = 11;
    // constant -1.0 scalar
    constexpr static int minus_one = 12;

    // stopping status array
    constexpr static int stop = 0;
    // reduction tmp array
    constexpr static int tmp = 1;
    // reduction tmp array
    constexpr static int final_iter_nums = 2;
};


}  // namespace solver
}  // namespace gko


#endif  // GKO_PUBLIC_CORE_SOLVER_GMRES_HPP_
