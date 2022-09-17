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

#include "core/matrix/fbcsr_kernels.hpp"


#include <random>


#include <gtest/gtest.h>


#include <ginkgo/core/base/executor.hpp>
#include <ginkgo/core/matrix/fbcsr.hpp>


#include "core/test/matrix/fbcsr_sample.hpp"
#include "core/test/utils.hpp"
#include "core/test/utils/fb_matrix_generator.hpp"
#include "test/utils/executor.hpp"


template <typename T>
class Fbcsr : public CommonTestFixture {
protected:
    using value_type = T;
    using index_type = int;
    using real_type = gko::remove_complex<value_type>;
    using Mtx = gko::matrix::Fbcsr<value_type, index_type>;
    using Dense = gko::matrix::Dense<value_type>;

    Fbcsr() : distb(), engine(42)
    {
        const index_type rand_brows = 100;
        const index_type rand_bcols = 70;
        const int block_size = 3;
        rsorted = gko::test::generate_random_fbcsr<value_type, index_type>(
            ref, rand_brows, rand_bcols, block_size, false, false,
            std::default_random_engine(43));
    }

    std::unique_ptr<const Mtx> rsorted;

    std::normal_distribution<gko::remove_complex<T>> distb;
    std::default_random_engine engine;

    value_type get_random_value()
    {
        return gko::test::detail::get_rand_value<T>(distb, engine);
    }

    void generate_sin(Dense* const x)
    {
        value_type* const xarr = x->get_values();
        for (index_type i = 0; i < x->get_size()[0] * x->get_size()[1]; i++) {
            xarr[i] =
                static_cast<real_type>(2.0) *
                std::sin(static_cast<real_type>(i / 2.0) + get_random_value());
        }
    }
};

#ifdef GKO_COMPILING_HIP
TYPED_TEST_SUITE(Fbcsr, gko::test::RealValueTypes, TypenameNameGenerator);
#else
TYPED_TEST_SUITE(Fbcsr, gko::test::ValueTypes, TypenameNameGenerator);
#endif

TYPED_TEST(Fbcsr, CanWriteFromMatrixOnDevice)
{
    using Mtx = typename TestFixture::Mtx;
    using value_type = typename Mtx::value_type;
    using index_type = typename Mtx::index_type;
    using MatData = gko::matrix_data<value_type, index_type>;
    gko::testing::FbcsrSample<value_type, index_type> sample(this->ref);
    auto mat = sample.generate_fbcsr();
    auto dmat = gko::clone(this->exec, mat);
    MatData refdata;
    MatData cudadata;

    mat->write(refdata);
    dmat->write(cudadata);

    ASSERT_TRUE(refdata.nonzeros == cudadata.nonzeros);
}


TYPED_TEST(Fbcsr, TransposeIsEquivalentToRefSortedBS3)
{
    using Mtx = typename TestFixture::Mtx;
    using value_type = typename Mtx::value_type;
    using index_type = typename Mtx::index_type;
    auto drand = Mtx::create(this->exec);
    drand->copy_from(gko::lend(this->rsorted));
    auto trans_ref_linop = this->rsorted->transpose();
    std::unique_ptr<const Mtx> trans_ref =
        gko::as<const Mtx>(std::move(trans_ref_linop));

    auto trans_cuda_linop = drand->transpose();
    std::unique_ptr<const Mtx> trans_cuda =
        gko::as<const Mtx>(std::move(trans_cuda_linop));

    GKO_ASSERT_MTX_EQ_SPARSITY(trans_ref, trans_cuda);
    GKO_ASSERT_MTX_NEAR(trans_ref, trans_cuda, 0.0);
}


TYPED_TEST(Fbcsr, TransposeIsEquivalentToRefSortedBS7)
{
    using Mtx = typename TestFixture::Mtx;
    using value_type = typename Mtx::value_type;
    using index_type = typename Mtx::index_type;
    auto drand = Mtx::create(this->exec);
    const index_type rand_brows = 50;
    const index_type rand_bcols = 40;
    const int block_size = 7;
    auto rsorted2 = gko::test::generate_random_fbcsr<value_type, index_type>(
        this->ref, rand_brows, rand_bcols, block_size, false, false,
        std::default_random_engine(43));
    drand->copy_from(gko::lend(rsorted2));

    auto trans_ref_linop = rsorted2->transpose();
    std::unique_ptr<const Mtx> trans_ref =
        gko::as<const Mtx>(std::move(trans_ref_linop));
    auto trans_cuda_linop = drand->transpose();
    std::unique_ptr<const Mtx> trans_cuda =
        gko::as<const Mtx>(std::move(trans_cuda_linop));

    GKO_ASSERT_MTX_EQ_SPARSITY(trans_ref, trans_cuda);
    GKO_ASSERT_MTX_NEAR(trans_ref, trans_cuda, 0.0);
}


TYPED_TEST(Fbcsr, SpmvIsEquivalentToRefSorted)
{
    using Mtx = typename TestFixture::Mtx;
    using Dense = typename TestFixture::Dense;
    using value_type = typename Mtx::value_type;
    auto drand = Mtx::create(this->exec);
    drand->copy_from(gko::lend(this->rsorted));
    auto x_ref =
        Dense::create(this->ref, gko::dim<2>(this->rsorted->get_size()[1], 1));
    this->generate_sin(x_ref.get());
    auto x_cuda = Dense::create(this->exec);
    x_cuda->copy_from(x_ref.get());
    auto prod_ref =
        Dense::create(this->ref, gko::dim<2>(this->rsorted->get_size()[0], 1));
    auto prod_cuda = Dense::create(this->exec, prod_ref->get_size());

    drand->apply(x_cuda.get(), prod_cuda.get());
    this->rsorted->apply(x_ref.get(), prod_ref.get());

    const double tol = r<value_type>::value;
    GKO_ASSERT_MTX_NEAR(prod_ref, prod_cuda, 5 * tol);
}


TYPED_TEST(Fbcsr, SpmvMultiIsEquivalentToRefSorted)
{
    using Mtx = typename TestFixture::Mtx;
    using Dense = typename TestFixture::Dense;
    using value_type = typename Mtx::value_type;
    auto drand = Mtx::create(this->exec);
    drand->copy_from(gko::lend(this->rsorted));
    auto x_ref =
        Dense::create(this->ref, gko::dim<2>(this->rsorted->get_size()[1], 3));
    this->generate_sin(x_ref.get());
    auto x_cuda = Dense::create(this->exec);
    x_cuda->copy_from(x_ref.get());
    auto prod_ref =
        Dense::create(this->ref, gko::dim<2>(this->rsorted->get_size()[0], 3));
    auto prod_cuda = Dense::create(this->exec, prod_ref->get_size());

    drand->apply(x_cuda.get(), prod_cuda.get());
    this->rsorted->apply(x_ref.get(), prod_ref.get());

    const double tol = r<value_type>::value;
    GKO_ASSERT_MTX_NEAR(prod_ref, prod_cuda, 5 * tol);
}


TYPED_TEST(Fbcsr, AdvancedSpmvIsEquivalentToRefSorted)
{
    using Mtx = typename TestFixture::Mtx;
    using Dense = typename TestFixture::Dense;
    using value_type = typename TestFixture::value_type;
    using real_type = typename TestFixture::real_type;
    auto drand = Mtx::create(this->exec);
    drand->copy_from(gko::lend(this->rsorted));
    auto x_ref =
        Dense::create(this->ref, gko::dim<2>(this->rsorted->get_size()[1], 1));
    this->generate_sin(x_ref.get());
    auto x_cuda = Dense::create(this->exec);
    x_cuda->copy_from(x_ref.get());
    auto prod_ref =
        Dense::create(this->ref, gko::dim<2>(this->rsorted->get_size()[0], 1));
    this->generate_sin(prod_ref.get());
    auto prod_cuda = Dense::create(this->exec);
    prod_cuda->copy_from(prod_ref.get());
    auto alpha_ref = Dense::create(this->ref, gko::dim<2>(1, 1));
    alpha_ref->get_values()[0] =
        static_cast<real_type>(2.4) + this->get_random_value();
    auto beta_ref = Dense::create(this->ref, gko::dim<2>(1, 1));
    beta_ref->get_values()[0] = -1.2;
    auto alpha = Dense::create(this->exec);
    alpha->copy_from(alpha_ref.get());
    auto beta = Dense::create(this->exec);
    beta->copy_from(beta_ref.get());

    drand->apply(alpha.get(), x_cuda.get(), beta.get(), prod_cuda.get());
    this->rsorted->apply(alpha_ref.get(), x_ref.get(), beta_ref.get(),
                         prod_ref.get());

    const double tol = r<value_type>::value;
    GKO_ASSERT_MTX_NEAR(prod_ref, prod_cuda, 5 * tol);
}


TYPED_TEST(Fbcsr, AdvancedSpmvMultiIsEquivalentToRefSorted)
{
    using Mtx = typename TestFixture::Mtx;
    using Dense = typename TestFixture::Dense;
    using value_type = typename TestFixture::value_type;
    using real_type = typename TestFixture::real_type;
    auto drand = Mtx::create(this->exec);
    drand->copy_from(gko::lend(this->rsorted));
    auto x_ref =
        Dense::create(this->ref, gko::dim<2>(this->rsorted->get_size()[1], 3));
    this->generate_sin(x_ref.get());
    auto x_cuda = Dense::create(this->exec);
    x_cuda->copy_from(x_ref.get());
    auto prod_ref =
        Dense::create(this->ref, gko::dim<2>(this->rsorted->get_size()[0], 3));
    this->generate_sin(prod_ref.get());
    auto prod_cuda = Dense::create(this->exec);
    prod_cuda->copy_from(prod_ref.get());
    auto alpha_ref = Dense::create(this->ref, gko::dim<2>(1, 1));
    alpha_ref->get_values()[0] =
        static_cast<real_type>(2.4) + this->get_random_value();
    auto beta_ref = Dense::create(this->ref, gko::dim<2>(1, 1));
    beta_ref->get_values()[0] = -1.2;
    auto alpha = Dense::create(this->exec);
    alpha->copy_from(alpha_ref.get());
    auto beta = Dense::create(this->exec);
    beta->copy_from(beta_ref.get());

    drand->apply(alpha.get(), x_cuda.get(), beta.get(), prod_cuda.get());
    this->rsorted->apply(alpha_ref.get(), x_ref.get(), beta_ref.get(),
                         prod_ref.get());

    const double tol = r<value_type>::value;
    GKO_ASSERT_MTX_NEAR(prod_ref, prod_cuda, 5 * tol);
}


TYPED_TEST(Fbcsr, ConjTransposeIsEquivalentToRefSortedBS3)
{
    using Mtx = typename TestFixture::Mtx;
    using value_type = typename Mtx::value_type;
    using index_type = typename Mtx::index_type;
    auto drand = Mtx::create(this->exec);
    drand->copy_from(gko::lend(this->rsorted));
    auto trans_ref_linop = this->rsorted->conj_transpose();
    std::unique_ptr<const Mtx> trans_ref =
        gko::as<const Mtx>(std::move(trans_ref_linop));

    auto trans_cuda_linop = drand->conj_transpose();
    std::unique_ptr<const Mtx> trans_cuda =
        gko::as<const Mtx>(std::move(trans_cuda_linop));

    GKO_ASSERT_MTX_EQ_SPARSITY(trans_ref, trans_cuda);
    GKO_ASSERT_MTX_NEAR(trans_ref, trans_cuda, 0.0);
}


TYPED_TEST(Fbcsr, RecognizeSortedMatrix)
{
    using Mtx = typename TestFixture::Mtx;
    auto drand = Mtx::create(this->exec);
    drand->copy_from(gko::lend(this->rsorted));

    ASSERT_TRUE(drand->is_sorted_by_column_index());
}


TYPED_TEST(Fbcsr, RecognizeUnsortedMatrix)
{
    using Mtx = typename TestFixture::Mtx;
    using index_type = typename TestFixture::index_type;
    auto mat = this->rsorted->clone();
    index_type* const colinds = mat->get_col_idxs();
    std::swap(colinds[0], colinds[1]);
    auto unsrt_cuda = Mtx::create(this->exec);
    unsrt_cuda->copy_from(gko::lend(mat));

    ASSERT_FALSE(unsrt_cuda->is_sorted_by_column_index());
}


TYPED_TEST(Fbcsr, InplaceAbsoluteMatrixIsEquivalentToRef)
{
    using Mtx = typename TestFixture::Mtx;
    using value_type = typename Mtx::value_type;
    auto rand_ref = Mtx::create(this->ref);
    rand_ref->copy_from(this->rsorted.get());
    auto drand = Mtx::create(this->exec);
    drand->copy_from(gko::lend(this->rsorted));

    rand_ref->compute_absolute_inplace();
    drand->compute_absolute_inplace();

    const double tol = r<value_type>::value;
    GKO_ASSERT_MTX_NEAR(rand_ref, drand, tol);
}


TYPED_TEST(Fbcsr, OutplaceAbsoluteMatrixIsEquivalentToRef)
{
    using Mtx = typename TestFixture::Mtx;
    using value_type = typename Mtx::value_type;
    auto drand = Mtx::create(this->exec);
    drand->copy_from(gko::lend(this->rsorted));

    auto abs_mtx = this->rsorted->compute_absolute();
    auto dabs_mtx = drand->compute_absolute();

    const double tol = r<value_type>::value;
    GKO_ASSERT_MTX_NEAR(abs_mtx, dabs_mtx, tol);
}
