/*******************************<GINKGO LICENSE>******************************
Copyright 2017-2018

Karlsruhe Institute of Technology
Universitat Jaume I
University of Tennessee

Redistribution and use in source and binary forms, with or without modification,
are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice,
   this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the documentation
   and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its contributors
   may be used to endorse or promote products derived from this software
   without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
******************************<GINKGO LICENSE>*******************************/

#include "core/base/exception_helpers.hpp"
#include "core/matrix/coo_kernels.hpp"
#include "core/matrix/csr_kernels.hpp"
#include "core/matrix/dense_kernels.hpp"
#include "core/matrix/ell_kernels.hpp"
#include "core/matrix/hybrid_kernels.hpp"
#include "core/matrix/sellp_kernels.hpp"
#include "core/preconditioner/block_jacobi_kernels.hpp"
#include "core/solver/bicgstab_kernels.hpp"
#include "core/solver/cg_kernels.hpp"
#include "core/solver/cgs_kernels.hpp"
#include "core/solver/fcg_kernels.hpp"
#include "core/solver/gmres_kernels.hpp"
#include "core/stop/criterion_kernels.hpp"
#include "core/stop/residual_norm_reduction_kernels.hpp"


#ifndef GKO_HOOK_MODULE
#error "Need to define GKO_HOOK_MODULE variable before including this file"
#endif  // GKO_HOOK_MODULE


namespace gko {
namespace kernels {
namespace GKO_HOOK_MODULE {
namespace dense {


template <typename ValueType>
GKO_DECLARE_DENSE_SIMPLE_APPLY_KERNEL(ValueType)
NOT_COMPILED(GKO_HOOK_MODULE);
GKO_INSTANTIATE_FOR_EACH_VALUE_TYPE(GKO_DECLARE_DENSE_SIMPLE_APPLY_KERNEL);

template <typename ValueType>
GKO_DECLARE_DENSE_APPLY_KERNEL(ValueType)
NOT_COMPILED(GKO_HOOK_MODULE);
GKO_INSTANTIATE_FOR_EACH_VALUE_TYPE(GKO_DECLARE_DENSE_APPLY_KERNEL);

template <typename ValueType>
GKO_DECLARE_DENSE_SCALE_KERNEL(ValueType)
NOT_COMPILED(GKO_HOOK_MODULE);
GKO_INSTANTIATE_FOR_EACH_VALUE_TYPE(GKO_DECLARE_DENSE_SCALE_KERNEL);

template <typename ValueType>
GKO_DECLARE_DENSE_ADD_SCALED_KERNEL(ValueType)
NOT_COMPILED(GKO_HOOK_MODULE);
GKO_INSTANTIATE_FOR_EACH_VALUE_TYPE(GKO_DECLARE_DENSE_ADD_SCALED_KERNEL);

template <typename ValueType>
GKO_DECLARE_DENSE_COMPUTE_DOT_KERNEL(ValueType)
NOT_COMPILED(GKO_HOOK_MODULE);
GKO_INSTANTIATE_FOR_EACH_VALUE_TYPE(GKO_DECLARE_DENSE_COMPUTE_DOT_KERNEL);

template <typename ValueType, typename IndexType>
GKO_DECLARE_DENSE_CONVERT_TO_COO_KERNEL(ValueType, IndexType)
NOT_COMPILED(GKO_HOOK_MODULE);
GKO_INSTANTIATE_FOR_EACH_VALUE_AND_INDEX_TYPE(
    GKO_DECLARE_DENSE_CONVERT_TO_COO_KERNEL);

template <typename ValueType, typename IndexType>
GKO_DECLARE_DENSE_CONVERT_TO_CSR_KERNEL(ValueType, IndexType)
NOT_COMPILED(GKO_HOOK_MODULE);
GKO_INSTANTIATE_FOR_EACH_VALUE_AND_INDEX_TYPE(
    GKO_DECLARE_DENSE_CONVERT_TO_CSR_KERNEL);

template <typename ValueType, typename IndexType>
GKO_DECLARE_DENSE_MOVE_TO_CSR_KERNEL(ValueType, IndexType)
NOT_COMPILED(GKO_HOOK_MODULE);
GKO_INSTANTIATE_FOR_EACH_VALUE_AND_INDEX_TYPE(
    GKO_DECLARE_DENSE_MOVE_TO_CSR_KERNEL);


template <typename ValueType, typename IndexType>
GKO_DECLARE_DENSE_CONVERT_TO_ELL_KERNEL(ValueType, IndexType)
NOT_COMPILED(GKO_HOOK_MODULE);
GKO_INSTANTIATE_FOR_EACH_VALUE_AND_INDEX_TYPE(
    GKO_DECLARE_DENSE_CONVERT_TO_ELL_KERNEL);

template <typename ValueType, typename IndexType>
GKO_DECLARE_DENSE_MOVE_TO_ELL_KERNEL(ValueType, IndexType)
NOT_COMPILED(GKO_HOOK_MODULE);
GKO_INSTANTIATE_FOR_EACH_VALUE_AND_INDEX_TYPE(
    GKO_DECLARE_DENSE_MOVE_TO_ELL_KERNEL);

template <typename ValueType, typename IndexType>
GKO_DECLARE_DENSE_CONVERT_TO_HYBRID_KERNEL(ValueType, IndexType)
NOT_COMPILED(GKO_HOOK_MODULE);
GKO_INSTANTIATE_FOR_EACH_VALUE_AND_INDEX_TYPE(
    GKO_DECLARE_DENSE_CONVERT_TO_HYBRID_KERNEL);

template <typename ValueType, typename IndexType>
GKO_DECLARE_DENSE_MOVE_TO_HYBRID_KERNEL(ValueType, IndexType)
NOT_COMPILED(GKO_HOOK_MODULE);
GKO_INSTANTIATE_FOR_EACH_VALUE_AND_INDEX_TYPE(
    GKO_DECLARE_DENSE_MOVE_TO_HYBRID_KERNEL);

template <typename ValueType, typename IndexType>
GKO_DECLARE_DENSE_CONVERT_TO_SELLP_KERNEL(ValueType, IndexType)
NOT_COMPILED(GKO_HOOK_MODULE);
GKO_INSTANTIATE_FOR_EACH_VALUE_AND_INDEX_TYPE(
    GKO_DECLARE_DENSE_CONVERT_TO_SELLP_KERNEL);

template <typename ValueType, typename IndexType>
GKO_DECLARE_DENSE_MOVE_TO_SELLP_KERNEL(ValueType, IndexType)
NOT_COMPILED(GKO_HOOK_MODULE);
GKO_INSTANTIATE_FOR_EACH_VALUE_AND_INDEX_TYPE(
    GKO_DECLARE_DENSE_MOVE_TO_SELLP_KERNEL);

template <typename ValueType>
GKO_DECLARE_DENSE_COUNT_NONZEROS_KERNEL(ValueType)
NOT_COMPILED(GKO_HOOK_MODULE);
GKO_INSTANTIATE_FOR_EACH_VALUE_TYPE(GKO_DECLARE_DENSE_COUNT_NONZEROS_KERNEL);

template <typename ValueType>
GKO_DECLARE_DENSE_CALCULATE_MAX_NNZ_PER_ROW_KERNEL(ValueType)
NOT_COMPILED(GKO_HOOK_MODULE);
GKO_INSTANTIATE_FOR_EACH_VALUE_TYPE(
    GKO_DECLARE_DENSE_CALCULATE_MAX_NNZ_PER_ROW_KERNEL);

template <typename ValueType>
GKO_DECLARE_DENSE_CALCULATE_NONZEROS_PER_ROW_KERNEL(ValueType)
NOT_COMPILED(GKO_HOOK_MODULE);
GKO_INSTANTIATE_FOR_EACH_VALUE_TYPE(
    GKO_DECLARE_DENSE_CALCULATE_NONZEROS_PER_ROW_KERNEL);

template <typename ValueType>
GKO_DECLARE_DENSE_CALCULATE_TOTAL_COLS_KERNEL(ValueType)
NOT_COMPILED(GKO_HOOK_MODULE);
GKO_INSTANTIATE_FOR_EACH_VALUE_TYPE(
    GKO_DECLARE_DENSE_CALCULATE_TOTAL_COLS_KERNEL);

template <typename ValueType>
GKO_DECLARE_TRANSPOSE_KERNEL(ValueType)
NOT_COMPILED(GKO_HOOK_MODULE);
GKO_INSTANTIATE_FOR_EACH_VALUE_TYPE(GKO_DECLARE_TRANSPOSE_KERNEL);

template <typename ValueType>
GKO_DECLARE_CONJ_TRANSPOSE_KERNEL(ValueType)
NOT_COMPILED(GKO_HOOK_MODULE);
GKO_INSTANTIATE_FOR_EACH_VALUE_TYPE(GKO_DECLARE_CONJ_TRANSPOSE_KERNEL);


}  // namespace dense


namespace cg {


template <typename ValueType>
GKO_DECLARE_CG_INITIALIZE_KERNEL(ValueType)
NOT_COMPILED(GKO_HOOK_MODULE);
GKO_INSTANTIATE_FOR_EACH_VALUE_TYPE(GKO_DECLARE_CG_INITIALIZE_KERNEL);

template <typename ValueType>
GKO_DECLARE_CG_STEP_1_KERNEL(ValueType)
NOT_COMPILED(GKO_HOOK_MODULE);
GKO_INSTANTIATE_FOR_EACH_VALUE_TYPE(GKO_DECLARE_CG_STEP_1_KERNEL);

template <typename ValueType>
GKO_DECLARE_CG_STEP_2_KERNEL(ValueType)
NOT_COMPILED(GKO_HOOK_MODULE);
GKO_INSTANTIATE_FOR_EACH_VALUE_TYPE(GKO_DECLARE_CG_STEP_2_KERNEL);


}  // namespace cg


namespace fcg {


template <typename ValueType>
GKO_DECLARE_FCG_INITIALIZE_KERNEL(ValueType)
NOT_COMPILED(GKO_HOOK_MODULE);
GKO_INSTANTIATE_FOR_EACH_VALUE_TYPE(GKO_DECLARE_FCG_INITIALIZE_KERNEL);

template <typename ValueType>
GKO_DECLARE_FCG_STEP_1_KERNEL(ValueType)
NOT_COMPILED(GKO_HOOK_MODULE);
GKO_INSTANTIATE_FOR_EACH_VALUE_TYPE(GKO_DECLARE_FCG_STEP_1_KERNEL);

template <typename ValueType>
GKO_DECLARE_FCG_STEP_2_KERNEL(ValueType)
NOT_COMPILED(GKO_HOOK_MODULE);
GKO_INSTANTIATE_FOR_EACH_VALUE_TYPE(GKO_DECLARE_FCG_STEP_2_KERNEL);


}  // namespace fcg


namespace bicgstab {


template <typename ValueType>
GKO_DECLARE_BICGSTAB_INITIALIZE_KERNEL(ValueType)
NOT_COMPILED(GKO_HOOK_MODULE);
GKO_INSTANTIATE_FOR_EACH_VALUE_TYPE(GKO_DECLARE_BICGSTAB_INITIALIZE_KERNEL);

template <typename ValueType>
GKO_DECLARE_BICGSTAB_STEP_1_KERNEL(ValueType)
NOT_COMPILED(GKO_HOOK_MODULE);
GKO_INSTANTIATE_FOR_EACH_VALUE_TYPE(GKO_DECLARE_BICGSTAB_STEP_1_KERNEL);

template <typename ValueType>
GKO_DECLARE_BICGSTAB_STEP_2_KERNEL(ValueType)
NOT_COMPILED(GKO_HOOK_MODULE);
GKO_INSTANTIATE_FOR_EACH_VALUE_TYPE(GKO_DECLARE_BICGSTAB_STEP_2_KERNEL);

template <typename ValueType>
GKO_DECLARE_BICGSTAB_STEP_3_KERNEL(ValueType)
NOT_COMPILED(GKO_HOOK_MODULE);
GKO_INSTANTIATE_FOR_EACH_VALUE_TYPE(GKO_DECLARE_BICGSTAB_STEP_3_KERNEL);

template <typename ValueType>
GKO_DECLARE_BICGSTAB_FINALIZE_KERNEL(ValueType)
NOT_COMPILED(GKO_HOOK_MODULE);
GKO_INSTANTIATE_FOR_EACH_VALUE_TYPE(GKO_DECLARE_BICGSTAB_FINALIZE_KERNEL);


}  // namespace bicgstab


namespace cgs {


template <typename ValueType>
GKO_DECLARE_CGS_INITIALIZE_KERNEL(ValueType)
NOT_COMPILED(GKO_HOOK_MODULE);
GKO_INSTANTIATE_FOR_EACH_VALUE_TYPE(GKO_DECLARE_CGS_INITIALIZE_KERNEL);

template <typename ValueType>
GKO_DECLARE_CGS_STEP_1_KERNEL(ValueType)
NOT_COMPILED(GKO_HOOK_MODULE);
GKO_INSTANTIATE_FOR_EACH_VALUE_TYPE(GKO_DECLARE_CGS_STEP_1_KERNEL);

template <typename ValueType>
GKO_DECLARE_CGS_STEP_2_KERNEL(ValueType)
NOT_COMPILED(GKO_HOOK_MODULE);
GKO_INSTANTIATE_FOR_EACH_VALUE_TYPE(GKO_DECLARE_CGS_STEP_2_KERNEL);

template <typename ValueType>
GKO_DECLARE_CGS_STEP_3_KERNEL(ValueType)
NOT_COMPILED(GKO_HOOK_MODULE);
GKO_INSTANTIATE_FOR_EACH_VALUE_TYPE(GKO_DECLARE_CGS_STEP_3_KERNEL);


}  // namespace cgs


namespace gmres {


template <typename ValueType>
GKO_DECLARE_GMRES_INITIALIZE_1_KERNEL(ValueType)
NOT_COMPILED(GKO_HOOK_MODULE);
GKO_INSTANTIATE_FOR_EACH_VALUE_TYPE(GKO_DECLARE_GMRES_INITIALIZE_1_KERNEL);

template <typename ValueType>
GKO_DECLARE_GMRES_INITIALIZE_2_KERNEL(ValueType)
NOT_COMPILED(GKO_HOOK_MODULE);
GKO_INSTANTIATE_FOR_EACH_VALUE_TYPE(GKO_DECLARE_GMRES_INITIALIZE_2_KERNEL);

template <typename ValueType>
GKO_DECLARE_GMRES_STEP_1_KERNEL(ValueType)
NOT_COMPILED(GKO_HOOK_MODULE);
GKO_INSTANTIATE_FOR_EACH_VALUE_TYPE(GKO_DECLARE_GMRES_STEP_1_KERNEL);

template <typename ValueType>
GKO_DECLARE_GMRES_STEP_2_KERNEL(ValueType)
NOT_COMPILED(GKO_HOOK_MODULE);
GKO_INSTANTIATE_FOR_EACH_VALUE_TYPE(GKO_DECLARE_GMRES_STEP_2_KERNEL);


}  // namespace gmres


namespace csr {


template <typename ValueType, typename IndexType>
GKO_DECLARE_CSR_SPMV_KERNEL(ValueType, IndexType)
NOT_COMPILED(GKO_HOOK_MODULE);
GKO_INSTANTIATE_FOR_EACH_VALUE_AND_INDEX_TYPE(GKO_DECLARE_CSR_SPMV_KERNEL);

template <typename ValueType, typename IndexType>
GKO_DECLARE_CSR_ADVANCED_SPMV_KERNEL(ValueType, IndexType)
NOT_COMPILED(GKO_HOOK_MODULE);
GKO_INSTANTIATE_FOR_EACH_VALUE_AND_INDEX_TYPE(
    GKO_DECLARE_CSR_ADVANCED_SPMV_KERNEL);

template <typename IndexType>
GKO_DECLARE_CSR_CONVERT_ROW_PTRS_TO_IDXS_KERNEL(IndexType)
NOT_COMPILED(GKO_HOOK_MODULE);
GKO_INSTANTIATE_FOR_EACH_INDEX_TYPE(
    GKO_DECLARE_CSR_CONVERT_ROW_PTRS_TO_IDXS_KERNEL);

template <typename ValueType, typename IndexType>
GKO_DECLARE_CSR_CONVERT_TO_DENSE_KERNEL(ValueType, IndexType)
NOT_COMPILED(GKO_HOOK_MODULE);
GKO_INSTANTIATE_FOR_EACH_VALUE_AND_INDEX_TYPE(
    GKO_DECLARE_CSR_CONVERT_TO_DENSE_KERNEL);

template <typename ValueType, typename IndexType>
GKO_DECLARE_CSR_MOVE_TO_DENSE_KERNEL(ValueType, IndexType)
NOT_COMPILED(GKO_HOOK_MODULE);
GKO_INSTANTIATE_FOR_EACH_VALUE_AND_INDEX_TYPE(
    GKO_DECLARE_CSR_MOVE_TO_DENSE_KERNEL);

template <typename ValueType, typename IndexType>
GKO_DECLARE_CSR_TRANSPOSE_KERNEL(ValueType, IndexType)
NOT_COMPILED(GKO_HOOK_MODULE);
GKO_INSTANTIATE_FOR_EACH_VALUE_AND_INDEX_TYPE(GKO_DECLARE_CSR_TRANSPOSE_KERNEL);

template <typename ValueType, typename IndexType>
GKO_DECLARE_CSR_CONJ_TRANSPOSE_KERNEL(ValueType, IndexType)
NOT_COMPILED(GKO_HOOK_MODULE);
GKO_INSTANTIATE_FOR_EACH_VALUE_AND_INDEX_TYPE(
    GKO_DECLARE_CSR_CONJ_TRANSPOSE_KERNEL);


}  // namespace csr


namespace coo {


template <typename ValueType, typename IndexType>
GKO_DECLARE_COO_SPMV_KERNEL(ValueType, IndexType)
NOT_COMPILED(GKO_HOOK_MODULE);
GKO_INSTANTIATE_FOR_EACH_VALUE_AND_INDEX_TYPE(GKO_DECLARE_COO_SPMV_KERNEL);

template <typename ValueType, typename IndexType>
GKO_DECLARE_COO_ADVANCED_SPMV_KERNEL(ValueType, IndexType)
NOT_COMPILED(GKO_HOOK_MODULE);
GKO_INSTANTIATE_FOR_EACH_VALUE_AND_INDEX_TYPE(
    GKO_DECLARE_COO_ADVANCED_SPMV_KERNEL);

template <typename ValueType, typename IndexType>
GKO_DECLARE_COO_SPMV2_KERNEL(ValueType, IndexType)
NOT_COMPILED(GKO_HOOK_MODULE);
GKO_INSTANTIATE_FOR_EACH_VALUE_AND_INDEX_TYPE(GKO_DECLARE_COO_SPMV2_KERNEL);

template <typename ValueType, typename IndexType>
GKO_DECLARE_COO_ADVANCED_SPMV2_KERNEL(ValueType, IndexType)
NOT_COMPILED(GKO_HOOK_MODULE);
GKO_INSTANTIATE_FOR_EACH_VALUE_AND_INDEX_TYPE(
    GKO_DECLARE_COO_ADVANCED_SPMV2_KERNEL);

template <typename IndexType>
GKO_DECLARE_COO_CONVERT_ROW_IDXS_TO_PTRS_KERNEL(IndexType)
NOT_COMPILED(GKO_HOOK_MODULE);
GKO_INSTANTIATE_FOR_EACH_INDEX_TYPE(
    GKO_DECLARE_COO_CONVERT_ROW_IDXS_TO_PTRS_KERNEL);

template <typename ValueType, typename IndexType>
GKO_DECLARE_COO_CONVERT_TO_DENSE_KERNEL(ValueType, IndexType)
NOT_COMPILED(GKO_HOOK_MODULE);
GKO_INSTANTIATE_FOR_EACH_VALUE_AND_INDEX_TYPE(
    GKO_DECLARE_COO_CONVERT_TO_DENSE_KERNEL);

template <typename ValueType, typename IndexType>
GKO_DECLARE_COO_TRANSPOSE_KERNEL(ValueType, IndexType)
NOT_COMPILED(GKO_HOOK_MODULE);
GKO_INSTANTIATE_FOR_EACH_VALUE_AND_INDEX_TYPE(GKO_DECLARE_COO_TRANSPOSE_KERNEL);

template <typename ValueType, typename IndexType>
GKO_DECLARE_COO_CONJ_TRANSPOSE_KERNEL(ValueType, IndexType)
NOT_COMPILED(GKO_HOOK_MODULE);
GKO_INSTANTIATE_FOR_EACH_VALUE_AND_INDEX_TYPE(
    GKO_DECLARE_COO_CONJ_TRANSPOSE_KERNEL);


}  // namespace coo


namespace ell {


template <typename ValueType, typename IndexType>
GKO_DECLARE_ELL_SPMV_KERNEL(ValueType, IndexType)
NOT_COMPILED(GKO_HOOK_MODULE);
GKO_INSTANTIATE_FOR_EACH_VALUE_AND_INDEX_TYPE(GKO_DECLARE_ELL_SPMV_KERNEL);

template <typename ValueType, typename IndexType>
GKO_DECLARE_ELL_ADVANCED_SPMV_KERNEL(ValueType, IndexType)
NOT_COMPILED(GKO_HOOK_MODULE);
GKO_INSTANTIATE_FOR_EACH_VALUE_AND_INDEX_TYPE(
    GKO_DECLARE_ELL_ADVANCED_SPMV_KERNEL);

template <typename ValueType, typename IndexType>
GKO_DECLARE_ELL_CONVERT_TO_DENSE_KERNEL(ValueType, IndexType)
NOT_COMPILED(GKO_HOOK_MODULE);
GKO_INSTANTIATE_FOR_EACH_VALUE_AND_INDEX_TYPE(
    GKO_DECLARE_ELL_CONVERT_TO_DENSE_KERNEL);


}  // namespace ell


namespace hybrid {


template <typename ValueType, typename IndexType>
GKO_DECLARE_HYBRID_CONVERT_TO_DENSE_KERNEL(ValueType, IndexType)
NOT_COMPILED(GKO_HOOK_MODULE);
GKO_INSTANTIATE_FOR_EACH_VALUE_AND_INDEX_TYPE(
    GKO_DECLARE_HYBRID_CONVERT_TO_DENSE_KERNEL);


}  // namespace hybrid


namespace sellp {


template <typename ValueType, typename IndexType>
GKO_DECLARE_SELLP_SPMV_KERNEL(ValueType, IndexType)
NOT_COMPILED(GKO_HOOK_MODULE);
GKO_INSTANTIATE_FOR_EACH_VALUE_AND_INDEX_TYPE(GKO_DECLARE_SELLP_SPMV_KERNEL);

template <typename ValueType, typename IndexType>
GKO_DECLARE_SELLP_ADVANCED_SPMV_KERNEL(ValueType, IndexType)
NOT_COMPILED(GKO_HOOK_MODULE);
GKO_INSTANTIATE_FOR_EACH_VALUE_AND_INDEX_TYPE(
    GKO_DECLARE_SELLP_ADVANCED_SPMV_KERNEL);

template <typename ValueType, typename IndexType>
GKO_DECLARE_SELLP_CONVERT_TO_DENSE_KERNEL(ValueType, IndexType)
NOT_COMPILED(GKO_HOOK_MODULE);
GKO_INSTANTIATE_FOR_EACH_VALUE_AND_INDEX_TYPE(
    GKO_DECLARE_SELLP_CONVERT_TO_DENSE_KERNEL);


}  // namespace sellp


namespace block_jacobi {


template <typename ValueType, typename IndexType>
GKO_DECLARE_BLOCK_JACOBI_FIND_BLOCKS_KERNEL(ValueType, IndexType)
NOT_COMPILED(GKO_HOOK_MODULE);
GKO_INSTANTIATE_FOR_EACH_VALUE_AND_INDEX_TYPE(
    GKO_DECLARE_BLOCK_JACOBI_FIND_BLOCKS_KERNEL);

template <typename ValueType, typename IndexType>
GKO_DECLARE_BLOCK_JACOBI_GENERATE_KERNEL(ValueType, IndexType)
NOT_COMPILED(GKO_HOOK_MODULE);
GKO_INSTANTIATE_FOR_EACH_VALUE_AND_INDEX_TYPE(
    GKO_DECLARE_BLOCK_JACOBI_GENERATE_KERNEL);

template <typename ValueType, typename IndexType>
GKO_DECLARE_BLOCK_JACOBI_APPLY_KERNEL(ValueType, IndexType)
NOT_COMPILED(GKO_HOOK_MODULE);
GKO_INSTANTIATE_FOR_EACH_VALUE_AND_INDEX_TYPE(
    GKO_DECLARE_BLOCK_JACOBI_APPLY_KERNEL);

template <typename ValueType, typename IndexType>
GKO_DECLARE_BLOCK_JACOBI_SIMPLE_APPLY_KERNEL(ValueType, IndexType)
NOT_COMPILED(GKO_HOOK_MODULE);
GKO_INSTANTIATE_FOR_EACH_VALUE_AND_INDEX_TYPE(
    GKO_DECLARE_BLOCK_JACOBI_SIMPLE_APPLY_KERNEL);

template <typename ValueType, typename IndexType>
GKO_DECLARE_BLOCK_JACOBI_CONVERT_TO_DENSE_KERNEL(ValueType, IndexType)
NOT_COMPILED(GKO_HOOK_MODULE);
GKO_INSTANTIATE_FOR_EACH_VALUE_AND_INDEX_TYPE(
    GKO_DECLARE_BLOCK_JACOBI_CONVERT_TO_DENSE_KERNEL);


}  // namespace block_jacobi


namespace adaptive_block_jacobi {


template <typename ValueType, typename IndexType>
GKO_DECLARE_ADAPTIVE_BLOCK_JACOBI_GENERATE_KERNEL(ValueType, IndexType)
NOT_COMPILED(GKO_HOOK_MODULE);
GKO_INSTANTIATE_FOR_EACH_VALUE_AND_INDEX_TYPE(
    GKO_DECLARE_ADAPTIVE_BLOCK_JACOBI_GENERATE_KERNEL);

template <typename ValueType, typename IndexType>
GKO_DECLARE_ADAPTIVE_BLOCK_JACOBI_APPLY_KERNEL(ValueType, IndexType)
NOT_COMPILED(GKO_HOOK_MODULE);
GKO_INSTANTIATE_FOR_EACH_VALUE_AND_INDEX_TYPE(
    GKO_DECLARE_ADAPTIVE_BLOCK_JACOBI_APPLY_KERNEL);

template <typename ValueType, typename IndexType>
GKO_DECLARE_ADAPTIVE_BLOCK_JACOBI_SIMPLE_APPLY_KERNEL(ValueType, IndexType)
NOT_COMPILED(GKO_HOOK_MODULE);
GKO_INSTANTIATE_FOR_EACH_VALUE_AND_INDEX_TYPE(
    GKO_DECLARE_ADAPTIVE_BLOCK_JACOBI_SIMPLE_APPLY_KERNEL);

template <typename ValueType, typename IndexType>
GKO_DECLARE_ADAPTIVE_BLOCK_JACOBI_CONVERT_TO_DENSE_KERNEL(ValueType, IndexType)
NOT_COMPILED(GKO_HOOK_MODULE);
GKO_INSTANTIATE_FOR_EACH_VALUE_AND_INDEX_TYPE(
    GKO_DECLARE_ADAPTIVE_BLOCK_JACOBI_CONVERT_TO_DENSE_KERNEL);


}  // namespace adaptive_block_jacobi


namespace set_all_statuses {


GKO_DECLARE_SET_ALL_STATUSES_KERNEL()
NOT_COMPILED(GKO_HOOK_MODULE);


}  // namespace set_all_statuses


namespace residual_norm_reduction {


template <typename ValueType>
GKO_DECLARE_RESIDUAL_NORM_REDUCTION_KERNEL(ValueType)
NOT_COMPILED(GKO_HOOK_MODULE);
GKO_INSTANTIATE_FOR_EACH_VALUE_TYPE(GKO_DECLARE_RESIDUAL_NORM_REDUCTION_KERNEL);


}  // namespace residual_norm_reduction
}  // namespace GKO_HOOK_MODULE
}  // namespace kernels
}  // namespace gko
