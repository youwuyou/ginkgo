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

#include <ginkgo/core/reorder/rcm.hpp>


#if defined(__GNUC__) || defined(__clang__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"

#endif
#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable : 5211, 4973, 4974)
#endif


#include <memory>


#include <gtest/gtest.h>


#include <ginkgo/core/base/executor.hpp>


#include "core/test/utils.hpp"


namespace {

class Rcm : public ::testing::Test {
protected:
    using v_type = double;
    using i_type = int;
    using reorder_type = gko::reorder::Rcm<v_type, i_type>;
    using new_reorder_type = gko::experimental::reorder::Rcm<i_type>;

    Rcm()
        : exec(gko::ReferenceExecutor::create()),
          rcm_factory(reorder_type::build().on(exec))
    {}

    std::shared_ptr<const gko::Executor> exec;
    std::unique_ptr<reorder_type::Factory> rcm_factory;
};

TEST_F(Rcm, RcmFactoryKnowsItsExecutor)
{
    ASSERT_EQ(this->rcm_factory->get_executor(), this->exec);
}


TEST_F(Rcm, NewInterfaceDefaults)
{
    auto param = new_reorder_type::build();

    ASSERT_EQ(param.skip_symmetrize, false);
    ASSERT_EQ(param.strategy,
              gko::reorder::starting_strategy::pseudo_peripheral);
}


TEST_F(Rcm, NewInterfaceSetParameters)
{
    auto param =
        new_reorder_type::build().with_skip_symmetrize(true).with_strategy(
            gko::reorder::starting_strategy::minimum_degree);

    ASSERT_EQ(param.skip_symmetrize, true);
    ASSERT_EQ(param.strategy, gko::reorder::starting_strategy::minimum_degree);
}

}  // namespace
