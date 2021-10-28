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

#ifndef GKO_PUBLIC_CORE_BASE_MPI_HPP_
#define GKO_PUBLIC_CORE_BASE_MPI_HPP_


#include <map>
#include <memory>
#include <mutex>
#include <sstream>
#include <string>
#include <tuple>
#include <type_traits>


#include <ginkgo/config.hpp>
#include <ginkgo/core/base/exception.hpp>
#include <ginkgo/core/base/exception_helpers.hpp>
#include <ginkgo/core/base/polymorphic_object.hpp>
#include <ginkgo/core/base/types.hpp>


#if GKO_HAVE_MPI


#include <mpi.h>


template <typename T>
using array_manager = std::unique_ptr<T, std::function<void(T*)>>;


namespace gko {
namespace mpi {

/*
 * This enum is used for selecting the operation type for functions that take
 * MPI_Op. For example the MPI_Reduce operations.
 */
enum class op_type {
    sum = 1,
    min = 2,
    max = 3,
    product = 4,
    custom = 5,
    logical_and = 6,
    bitwise_and = 7,
    logical_or = 8,
    bitwise_or = 9,
    logical_xor = 10,
    bitwise_xor = 11,
    max_val_and_loc = 12,
    min_val_and_loc = 13
};


/*
 * Class that allows an RAII of initialization and calls MPI_Finalize at the
 * end of its scope. Therefore this must be called before any of the MPI
 * functions.
 */
class init_finalize {
public:
    init_finalize(int& argc, char**& argv, const size_type num_threads = 1);

    init_finalize() = delete;

    init_finalize(init_finalize& other) = default;

    init_finalize& operator=(const init_finalize& other) = default;

    init_finalize(init_finalize&& other) = default;

    init_finalize& operator=(init_finalize&& other) = default;

    static bool is_finalized();

    static bool is_initialized();

    ~init_finalize();

private:
    int num_args_;
    int required_thread_support_;
    int provided_thread_support_;
    char** args_;
};


/**
 * A class holding and operating on the MPI_Info class. Stores the key value
 * pair as a map and provides methods to access these values with keys as
 * strings.
 */
class info {
public:
    info();

    info(MPI_Info input) { this->info_ = input; }

    void remove(std::string key);

    std::string& at(std::string& key) { return this->key_value_.at(key); }

    void add(std::string key, std::string value);

    MPI_Info get() { return this->info_; }

    ~info();

private:
    std::map<std::string, std::string> key_value_;
    MPI_Info info_;
};


/**
 * A request class that takes in the given request and duplicates it
 * for our purposes. As the class or object goes out of scope, the request
 * is freed.
 */
class request : public EnableSharedCreateMethod<request> {
public:
    request(const int size) : req_(new MPI_Request[size]) {}

    request() : req_(new MPI_Request[1]) {}

    ~request()
    {
        if (req_) delete[] req_;
    }

    MPI_Request* get_requests() const { return req_; }

private:
    MPI_Request* req_;
};


/**
 * A status class that takes in the given status and duplicates it
 * for our purposes. As the class or object goes out of scope, the status
 * is freed.
 */
class status : public EnableSharedCreateMethod<status> {
public:
    status(const int size) : status_(new MPI_Status[size]) {}

    status() : status_(new MPI_Status[1]) {}

    ~status()
    {
        if (status_) delete[] status_;
    }

    MPI_Status* get_statuses() const { return status_; }

private:
    MPI_Status* status_;
};


/**
 * A communicator class that takes in the given communicator and duplicates it
 * for our purposes. As the class or object goes out of scope, the communicator
 * is freed.
 */
class communicator : public EnableSharedCreateMethod<communicator> {
public:
    communicator(const MPI_Comm& comm);

    communicator(const MPI_Comm& comm, int color, int key);

    communicator();

    communicator(communicator& other);

    communicator& operator=(const communicator& other);

    communicator(communicator&& other);

    communicator& operator=(communicator&& other);

    static MPI_Comm get_comm_world() { return MPI_COMM_WORLD; }

    static std::shared_ptr<communicator> create_world()
    {
        return std::make_shared<communicator>(get_comm_world());
    }

    MPI_Comm get() const { return comm_; }

    int size() const { return size_; }

    int rank() const { return rank_; };

    int local_rank() const { return local_rank_; };

    bool compare(const MPI_Comm& other) const;

    bool operator==(const communicator& rhs) { return compare(rhs.get()); }

    ~communicator();

private:
    MPI_Comm comm_;
    int size_{};
    int rank_{};
    int local_rank_{};
};


/**
 * A type helper which can be used to create MPI_Datatype from other types.
 */
class mpi_type {
public:
    mpi_type(const int count, MPI_Datatype& old);
    ~mpi_type();
    const MPI_Datatype& get() const { return this->type_; }

private:
    MPI_Datatype type_{};
};


/**
 * This class wraps the MPI_Window class with RAII functionality. Different
 * create and lock type methods are setup with enums.
 *
 * MPI_Window is primarily used for one sided communication and this class
 * provides functionalities to fence, lock, unlock and flush the communication
 * buffers.
 */
template <typename ValueType>
class window {
public:
    enum class win_type { allocate = 1, create = 2, dynamic_create = 3 };
    enum class lock_type { shared = 1, exclusive = 2 };

    window() : window_(MPI_WIN_NULL) {}
    window(window& other) = default;
    window& operator=(const window& other) = default;
    window(window&& other) = default;
    window& operator=(window&& other) = default;

    window(ValueType* base, unsigned int size,
           std::shared_ptr<const communicator> comm,
           const int disp_unit = sizeof(ValueType),
           info input_info = info(MPI_INFO_NULL),
           win_type create_type = win_type::create);

    MPI_Win get() { return this->window_; }

    void fence(int assert = 0);

    void lock(int rank, int assert = 0, lock_type lock_t = lock_type::shared);

    void unlock(int rank);

    void lock_all(int assert = 0);

    void unlock_all();

    void flush(int rank);

    void flush_local(int rank);

    void flush_all();

    void flush_all_local();

    ~window();

private:
    MPI_Win window_;
};


/**
 * This function is used to synchronize between the ranks of a given
 * communicator.
 *
 * @param comm  the communicator
 */
void synchronize(const communicator& comm = communicator::get_comm_world());


/**
 * Allows a rank to wait on a particular request handle.
 *
 * @param req  The request to wait on.
 * @param status  The status variable that can be queried.
 */
void wait(std::shared_ptr<request> req, std::shared_ptr<status> status = {});


double get_walltime();


/**
 * Get the rank in the communicator of the calling process.
 *
 * @param comm  the communicator
 */
int get_my_rank(const communicator& comm = communicator::get_comm_world());


/**
 * Get the node local rank in the communicator of the calling process.
 *
 * @param comm  the communicator
 */
int get_local_rank(const communicator& comm = communicator::get_comm_world());


/**
 * Get the number of ranks in the communicator of the calling process.
 *
 * @param comm  the communicator
 */
int get_num_ranks(const communicator& comm = communicator::get_comm_world());


/**
 * Send data from calling process to destination rank.
 *
 * @param send_buffer  the buffer to send
 * @param send_count  the number of elements to send
 * @param destination_rank  the rank to send the data to
 * @param send_tag  the tag for the send call
 * @param req  the request handle for the send call
 * @param comm  the communicator
 */
template <typename SendType>
void send(const SendType* send_buffer, const int send_count,
          const int destination_rank, const int send_tag,
          std::shared_ptr<request> req = {},
          std::shared_ptr<const communicator> comm = {});


/**
 * Receive data from source rank.
 *
 * @param recv_buffer  the buffer to send
 * @param recv_count  the number of elements to send
 * @param source_rank  the rank to send the data to
 * @param recv_tag  the tag for the send call
 * @param req  the request handle for the send call
 * @param comm  the communicator
 */
template <typename RecvType>
void recv(RecvType* recv_buffer, const int recv_count, const int source_rank,
          const int recv_tag, std::shared_ptr<request> req = {},
          std::shared_ptr<status> status = {},
          std::shared_ptr<const communicator> comm = {});


/**
 * Put data into the target window.
 *
 * @param origin_buffer  the buffer to send
 * @param origin_count  the number of elements to put
 * @param target_rank  the rank to put the data to
 * @param target_disp  the displacement at the target window
 * @param target_count  the request handle for the send call
 * @param window  the window to put the data into
 * @param req  the request handle
 */
template <typename PutType>
void put(const PutType* origin_buffer, const int origin_count,
         const int target_rank, const unsigned int target_disp,
         const int target_count, window<PutType>& window,
         std::shared_ptr<request> req = {});


/**
 * Get data from the target window.
 *
 * @param origin_buffer  the buffer to send
 * @param origin_count  the number of elements to get
 * @param target_rank  the rank to get the data from
 * @param target_disp  the displacement at the target window
 * @param target_count  the request handle for the send call
 * @param window  the window to put the data into
 * @param req  the request handle
 */
template <typename GetType>
void get(GetType* origin_buffer, const int origin_count, const int target_rank,
         const unsigned int target_disp, const int target_count,
         window<GetType>& window, std::shared_ptr<request> req = {});


/**
 * Broadcast data from calling process to all ranks in the communicator
 *
 * @param buffer  the buffer to broadcsat
 * @param count  the number of elements to broadcast
 * @param root_rank  the rank to broadcast from
 * @param comm  the communicator
 */
template <typename BroadcastType>
void broadcast(BroadcastType* buffer, int count, int root_rank,
               std::shared_ptr<const communicator> comm = {});


/**
 * Reduce data into root from all calling processes on the same communicator.
 *
 * @param send_buffer  the buffer to reduce
 * @param recv_buffer  the reduced result
 * @param count  the number of elements to reduce
 * @param op_enum  the reduce operation. See @op_type
 * @param comm  the communicator
 * @param req  the request handle
 */
template <typename ReduceType>
void reduce(const ReduceType* send_buffer, ReduceType* recv_buffer, int count,
            op_type op_enum, int root_rank,
            std::shared_ptr<const communicator> comm = {},
            std::shared_ptr<request> req = {});


/**
 * Reduce data from all calling processes from all calling processes on same
 * communicator.
 *
 * @param recv_buffer  the data to reduce and the reduced result
 * @param count  the number of elements to reduce
 * @param op_enum  the reduce operation. See @op_type
 * @param comm  the communicator
 * @param req  the request handle
 */
template <typename ReduceType>
void all_reduce(ReduceType* recv_buffer, int count,
                op_type op_enum = op_type::sum,
                std::shared_ptr<const communicator> comm = {},
                std::shared_ptr<request> req = {});


/**
 * Reduce data from all calling processes from all calling processes on same
 * communicator.
 *
 * @param send_buffer  the data to reduce
 * @param recv_buffer  the reduced result
 * @param count  the number of elements to reduce
 * @param op_enum  the reduce operation. See @op_type
 * @param comm  the communicator
 * @param req  the request handle
 */
template <typename ReduceType>
void all_reduce(const ReduceType* send_buffer, ReduceType* recv_buffer,
                int count, op_type op_enum = op_type::sum,
                std::shared_ptr<const communicator> comm = {},
                std::shared_ptr<request> req = {});


/**
 * Gather data onto the root rank from all ranks in the communicator.
 *
 * @param send_buffer  the buffer to gather from
 * @param send_count  the number of elements to send
 * @param recv_buffer  the buffer to gather into
 * @param recv_count  the number of elements to receive
 * @param root_rank  the rank to gather into
 * @param comm  the communicator
 */
template <typename SendType, typename RecvType>
void gather(const SendType* send_buffer, const int send_count,
            RecvType* recv_buffer, const int recv_count, int root_rank,
            std::shared_ptr<const communicator> comm = {});


/**
 * Gather data onto the root rank from all ranks in the communicator with
 * offsets.
 *
 * @param send_buffer  the buffer to gather from
 * @param send_count  the number of elements to send
 * @param recv_buffer  the buffer to gather into
 * @param recv_count  the number of elements to receive
 * @param displacements  the offsets for the buffer
 * @param root_rank  the rank to gather into
 * @param comm  the communicator
 */
template <typename SendType, typename RecvType>
void gather(const SendType* send_buffer, const int send_count,
            RecvType* recv_buffer, const int* recv_counts,
            const int* displacements, int root_rank,
            std::shared_ptr<const communicator> comm = {});


/**
 * Gather data onto all ranks from all ranks in the communicator.
 *
 * @param send_buffer  the buffer to gather from
 * @param send_count  the number of elements to send
 * @param recv_buffer  the buffer to gather into
 * @param recv_count  the number of elements to receive
 * @param comm  the communicator
 */
template <typename SendType, typename RecvType>
void all_gather(const SendType* send_buffer, const int send_count,
                RecvType* recv_buffer, const int recv_count,
                std::shared_ptr<const communicator> comm = {});


/**
 * Scatter data from root rank to all ranks in the communicator.
 *
 * @param send_buffer  the buffer to gather from
 * @param send_count  the number of elements to send
 * @param recv_buffer  the buffer to gather into
 * @param recv_count  the number of elements to receive
 * @param comm  the communicator
 */
template <typename SendType, typename RecvType>
void scatter(const SendType* send_buffer, const int send_count,
             RecvType* recv_buffer, const int recv_count, int root_rank,
             std::shared_ptr<const communicator> comm = {});


/**
 * Scatter data from root rank to all ranks in the communicator with offsets.
 *
 * @param send_buffer  the buffer to gather from
 * @param send_count  the number of elements to send
 * @param recv_buffer  the buffer to gather into
 * @param recv_count  the number of elements to receive
 * @param displacements  the offsets for the buffer
 * @param comm  the communicator
 */
template <typename SendType, typename RecvType>
void scatter(const SendType* send_buffer, const int* send_counts,
             const int* displacements, RecvType* recv_buffer,
             const int recv_count, int root_rank,
             std::shared_ptr<const communicator> comm = {});


/**
 * Communicate data from all ranks to all other ranks in place (MPI_Alltoall).
 * See MPI documentation for more details.
 *
 * @param buffer  the buffer to send and the buffer receive
 * @param recv_count  the number of elements to receive
 * @param comm  the communicator
 * @param req  the request handle
 *
 * @note This overload uses MPI_IN_PLACE and the source and destination buffers
 *       are the same.
 */
template <typename RecvType>
void all_to_all(RecvType* buffer, const int recv_count,
                std::shared_ptr<const communicator> comm = {},
                std::shared_ptr<request> req = {});


/**
 * Communicate data from all ranks to all other ranks (MPI_Alltoall).
 * See MPI documentation for more details.
 *
 * @param send_buffer  the buffer to send
 * @param send_count  the number of elements to send
 * @param recv_buffer  the buffer to receive
 * @param recv_count  the number of elements to receive
 * @param comm  the communicator
 * @param req  the request handle
 */
template <typename SendType, typename RecvType>
void all_to_all(const SendType* send_buffer, const int send_count,
                RecvType* recv_buffer, const int recv_count = {},
                std::shared_ptr<const communicator> comm = {},
                std::shared_ptr<request> req = {});


/**
 * Communicate data from all ranks to all other ranks with
 * offsets (MPI_Alltoallv). See MPI documentation for more details.
 *
 * @param send_buffer  the buffer to send
 * @param send_count  the number of elements to send
 * @param send_offsets  the offsets for the send buffer
 * @param recv_buffer  the buffer to gather into
 * @param recv_count  the number of elements to receive
 * @param recv_offsets  the offsets for the recv buffer
 * @param stride  the stride to be used in case of sending concatenated data
 * @param comm  the communicator
 * @param req  the request handle
 */
template <typename SendType, typename RecvType>
void all_to_all(const SendType* send_buffer, const int* send_counts,
                const int* send_offsets, RecvType* recv_buffer,
                const int* recv_counts, const int* recv_offsets,
                const int stride = 1,
                std::shared_ptr<const communicator> comm = {},
                std::shared_ptr<request> req = {});


/**
 * Does a scan operation with the given operator.
 * (MPI_Scan). See MPI documentation for more details.
 *
 * @param send_buffer  the buffer to scan from
 * @param recv_buffer  the result buffer
 * @param recv_count  the number of elements to scan
 * @param op_enum  the operation type to be used for the scan. See @op_type
 * @param comm  the communicator
 * @param req  the request handle
 */
template <typename ReduceType>
void scan(const ReduceType* send_buffer, ReduceType* recv_buffer, int count,
          op_type op_enum = op_type::sum,
          std::shared_ptr<const communicator> comm = {},
          std::shared_ptr<request> req = {});


}  // namespace mpi
}  // namespace gko


#endif  // GKO_HAVE_MPI


#endif  // GKO_PUBLIC_CORE_BASE_MPI_HPP_
