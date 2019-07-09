/*
 * GridTools
 *
 * Copyright (c) 2019, ETH Zurich
 * All rights reserved.
 *
 * Please, refer to the LICENSE file in the root directory.
 * SPDX-License-Identifier: BSD-3-Clause
 */
#ifndef INCLUDED_GTEST_MAIN_BOOST_CPP
#define INCLUDED_GTEST_MAIN_BOOST_CPP

#include <fstream>
#include <mpi.h>
#include "gtest/gtest.h"
#include "gridtools/tools/mpi_unit_test_driver/mpi_listener.hpp"
//#include <boost/mpi/environment.hpp>


GTEST_API_ int main(int argc, char **argv) {

    int provided;
    int init_result = MPI_Init_thread(&argc, &argv, MPI_THREAD_MULTIPLE, &provided);
    // boost::mpi::environment env(argc, argv);

    // printf("Running main() from %s\n", __FILE__);
    testing::InitGoogleTest(&argc, argv);

    // set up a custom listener that prints messages in an MPI-friendly way
    auto &listeners = testing::UnitTest::GetInstance()->listeners();
    // first delete the original printer
    delete listeners.Release(listeners.default_result_printer());
    // now add our custom printer
    listeners.Append(new mpi_listener("results_global_communication"));

    // record the local return value for tests run on this mpi rank
    //      0 : success
    //      1 : failure
    auto result = RUN_ALL_TESTS();

    // perform global collective, to ensure that all ranks return
    // the same exit code
    decltype(result) global_result{};
    MPI_Allreduce(&result, &global_result, 1, MPI_INT, MPI_MAX, MPI_COMM_WORLD);

    MPI_Barrier(MPI_COMM_WORLD);

    MPI_Finalize();

    return global_result;

}

#endif /* INCLUDED_GTEST_MAIN_BOOST_CPP */
