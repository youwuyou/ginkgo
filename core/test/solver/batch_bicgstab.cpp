/*******************************<GINKGO LICENSE>******************************
Copyright (c) 2017-2023, the Ginkgo authors
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

#include <ginkgo/core/solver/batch_bicgstab.hpp>


#include <gtest/gtest.h>


#include <ginkgo/core/base/batch_multi_vector.hpp>
#include <ginkgo/core/base/executor.hpp>
#include <ginkgo/core/matrix/batch_dense.hpp>


#include "core/base/batch_utilities.hpp"
#include "core/test/utils.hpp"
#include "core/test/utils/batch_helpers.hpp"


namespace {


template <typename T>
class BatchBicgstab : public ::testing::Test {
protected:
    using value_type = T;
    using real_type = gko::remove_complex<T>;
    using Mtx = gko::batch::matrix::Dense<value_type>;
    using MVec = gko::batch::MultiVector<value_type>;
    using Solver = gko::batch::solver::Bicgstab<value_type>;

    BatchBicgstab()
        : exec(gko::ReferenceExecutor::create()),
          mtx(gko::test::generate_3pt_stencil_batch_matrix<Mtx>(
              this->exec->get_master(), nrows, nbatch)),
          solver_factory(Solver::build()
                             .with_default_max_iterations(def_max_iters)
                             .with_default_residual_tol(def_abs_res_tol)
                             .with_tolerance_type(def_tol_type)
                             .on(exec)),
          solver(solver_factory->generate(mtx))
    {}

    std::shared_ptr<const gko::Executor> exec;
    const gko::size_type nbatch = 3;
    const int nrows = 5;
    std::shared_ptr<Mtx> mtx;
    std::unique_ptr<typename Solver::Factory> solver_factory;
    const int def_max_iters = 100;
    const real_type def_abs_res_tol = 1e-11;
    const gko::batch::stop::ToleranceType def_tol_type =
        gko::batch::stop::ToleranceType::absolute;
    std::unique_ptr<gko::batch::BatchLinOp> solver;
};

TYPED_TEST_SUITE(BatchBicgstab, gko::test::ValueTypes);


TYPED_TEST(BatchBicgstab, FactoryKnowsItsExecutor)
{
    ASSERT_EQ(this->solver_factory->get_executor(), this->exec);
}


TYPED_TEST(BatchBicgstab, FactoryCreatesCorrectSolver)
{
    using Solver = typename TestFixture::Solver;
    for (size_t i = 0; i < this->nbatch; i++) {
        ASSERT_EQ(this->solver->get_common_size(),
                  gko::dim<2>(this->nrows, this->nrows));
    }
    auto solver = static_cast<Solver*>(this->solver.get());
    ASSERT_NE(solver->get_system_matrix(), nullptr);
    ASSERT_EQ(solver->get_system_matrix(), this->mtx);
}


TYPED_TEST(BatchBicgstab, CanBeCopied)
{
    using Mtx = typename TestFixture::Mtx;
    using Solver = typename TestFixture::Solver;
    auto copy = this->solver_factory->generate(Mtx::create(this->exec));

    copy->copy_from(this->solver.get());

    ASSERT_EQ(copy->get_common_size(), gko::dim<2>(this->nrows, this->nrows));
    ASSERT_EQ(copy->get_num_batch_items(), this->nbatch);
    auto copy_mtx = static_cast<Solver*>(copy.get())->get_system_matrix();
    const auto copy_batch_mtx = static_cast<const Mtx*>(copy_mtx.get());
    GKO_ASSERT_BATCH_MTX_NEAR(this->mtx.get(), copy_batch_mtx, 0.0);
}


TYPED_TEST(BatchBicgstab, CanBeMoved)
{
    using Mtx = typename TestFixture::Mtx;
    using Solver = typename TestFixture::Solver;
    auto copy = this->solver_factory->generate(Mtx::create(this->exec));

    copy->move_from(this->solver);

    ASSERT_EQ(copy->get_common_size(), gko::dim<2>(this->nrows, this->nrows));
    ASSERT_EQ(copy->get_num_batch_items(), this->nbatch);
    auto copy_mtx = static_cast<Solver*>(copy.get())->get_system_matrix();
    const auto copy_batch_mtx = static_cast<const Mtx*>(copy_mtx.get());
    GKO_ASSERT_BATCH_MTX_NEAR(this->mtx.get(), copy_batch_mtx, 0.0);
}


TYPED_TEST(BatchBicgstab, CanBeCloned)
{
    using Mtx = typename TestFixture::Mtx;
    using Solver = typename TestFixture::Solver;
    auto clone = this->solver->clone();

    ASSERT_EQ(clone->get_common_size(), gko::dim<2>(this->nrows, this->nrows));
    ASSERT_EQ(clone->get_num_batch_items(), this->nbatch);
    auto clone_mtx = static_cast<Solver*>(clone.get())->get_system_matrix();
    const auto clone_batch_mtx = static_cast<const Mtx*>(clone_mtx.get());
    GKO_ASSERT_BATCH_MTX_NEAR(this->mtx.get(), clone_batch_mtx, 0.0);
}


TYPED_TEST(BatchBicgstab, CanBeCleared)
{
    using Solver = typename TestFixture::Solver;

    this->solver->clear();

    ASSERT_EQ(this->solver->get_num_batch_items(), 0);
    auto solver_mtx =
        static_cast<Solver*>(this->solver.get())->get_system_matrix();
    ASSERT_EQ(solver_mtx, nullptr);
}


TYPED_TEST(BatchBicgstab, CanSetCriteriaInFactory)
{
    using Solver = typename TestFixture::Solver;
    using real_type = typename TestFixture::real_type;

    auto solver_factory =
        Solver::build()
            .with_default_max_iterations(22)
            .with_default_residual_tol(static_cast<real_type>(0.25))
            .with_tolerance_type(gko::batch::stop::ToleranceType::relative)
            .on(this->exec);
    auto solver = solver_factory->generate(this->mtx);

    ASSERT_EQ(solver->get_parameters().default_max_iterations, 22);
    ASSERT_EQ(solver->get_parameters().default_residual_tol, 0.25);
    ASSERT_EQ(solver->get_parameters().tolerance_type,
              gko::batch::stop::ToleranceType::relative);
}


TYPED_TEST(BatchBicgstab, CanSetResidualTol)
{
    using Solver = typename TestFixture::Solver;
    using real_type = typename TestFixture::real_type;
    auto solver_factory =
        Solver::build()
            .with_default_max_iterations(22)
            .with_default_residual_tol(static_cast<real_type>(0.25))
            .with_tolerance_type(gko::batch::stop::ToleranceType::relative)
            .on(this->exec);
    auto solver = solver_factory->generate(this->mtx);

    solver->set_residual_tolerance(0.5);

    ASSERT_EQ(solver->get_parameters().default_max_iterations, 22);
    ASSERT_EQ(solver->get_parameters().default_residual_tol, 0.25);
    ASSERT_EQ(solver->get_residual_tolerance(), 0.5);
}


TYPED_TEST(BatchBicgstab, CanSetMaxIterations)
{
    using Solver = typename TestFixture::Solver;
    using real_type = typename TestFixture::real_type;
    auto solver_factory =
        Solver::build()
            .with_default_max_iterations(22)
            .with_default_residual_tol(static_cast<real_type>(0.25))
            .with_tolerance_type(gko::batch::stop::ToleranceType::relative)
            .on(this->exec);
    auto solver = solver_factory->generate(this->mtx);

    solver->set_max_iterations(10);

    ASSERT_EQ(solver->get_parameters().default_residual_tol, 0.25);
    ASSERT_EQ(solver->get_parameters().default_max_iterations, 22);
    ASSERT_EQ(solver->get_max_iterations(), 10);
}


TYPED_TEST(BatchBicgstab, ThrowsOnRectangularMatrixInFactory)
{
    using Mtx = typename TestFixture::Mtx;
    using Solver = typename TestFixture::Solver;
    std::shared_ptr<Mtx> rectangular_mtx =
        Mtx::create(this->exec, 2, gko::dim<2>{3, 5});

    ASSERT_THROW(this->solver_factory->generate(rectangular_mtx),
                 gko::BadDimension);
}


}  // namespace
