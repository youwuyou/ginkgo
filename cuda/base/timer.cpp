#include <ginkgo/core/base/timer.hpp>
#include "ginkgo/core/base/exception_helpers.hpp"


#include <cuda.h>
#include <cuda_runtime_api.h>


namespace gko {


CudaTimer::CudaTimer(std::shared_ptr<const CudaExecutor> exec)
    : exec_{std::move(exec)}
{}


time_point CudaTimer::create_time_point()
{
    time_point result;
    result.type_ = time_point::type::cuda;
    GKO_ASSERT_NO_CUDA_ERRORS(cudaEventCreate(&result.data_.cuda_event));
    return result;
}


void CudaTimer::record(time_point& time)
{
    GKO_ASSERT(time.type_ == time_point::type::cuda);
    GKO_ASSERT_NO_CUDA_ERRORS(
        cudaEventRecord(time.data_.cuda_event, exec_->get_stream()));
}


int64 CudaTimer::difference(const time_point& start, const time_point& stop)
{
    GKO_ASSERT(start.type_ == time_point::type::cuda);
    GKO_ASSERT(stop.type_ == time_point::type::cuda);
    GKO_ASSERT_NO_CUDA_ERRORS(cudaEventSynchronize(stop.data_.cuda_event));
    float ms{};
    GKO_ASSERT_NO_CUDA_ERRORS(cudaEventElapsedTime(&ms, start.data_.cuda_event,
                                                   stop.data_.cuda_event));
    return static_cast<int64>(ms * double{1e6});
}


}  // namespace gko