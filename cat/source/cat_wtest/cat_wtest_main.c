////////////////////////////////////////////////////////////////////////////////
/// Copyright 2025 Daniel S. Buckstein
/// 
/// Licensed under the Apache License, Version 2.0 (the "License");
/// you may not use this file except in compliance with the License.
/// You may obtain a copy of the License at
/// 
///     http://www.apache.org/licenses/LICENSE-2.0
/// 
/// Unless required by applicable law or agreed to in writing, software
/// distributed under the License is distributed on an "AS IS" BASIS,
/// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
/// See the License for the specific language governing permissions and
/// limitations under the License.
////////////////////////////////////////////////////////////////////////////////

/*
* cat_wtest_main.c
* Windowed program entry point.
*/

#include "cat/cat.h"


#ifdef _WIN32
#include <Windows.h>
#include <assert.h>


static HMODULE cat_dylib_load(cstr_t const filepath)
{
    assert(filepath);
    return LoadLibraryA(filepath);
}

static bool cat_dylib_unload(HMODULE const hModule)
{
    assert(hModule);
    return FreeLibrary(hModule);
}

static ptr_t cat_dylib_symbol(HMODULE const hModule, cstr_t const symbol)
{
    assert(hModule);
    assert(symbol);
    return (ptr_t)GetProcAddress(hModule, symbol);
}


extern int cat_test_all(int const argc, char const* const argv[]);

#include <stdio.h>
#include <inttypes.h>

#define NUM_WORKERS 4

typedef struct manager_t {
    mtx_t lock;

    int64_t* summands;
    size_t summands_count;
    int64_t sum; // result of work
} manager_t;


typedef struct worker_t {
    thrd_t thread;
    manager_t* manager;
    uint64_t id;

    int result;
    int pad;
} worker_t;

#define TEST_PASS_STR "TEST PASSED"
#define TEST_FAIL_STR "TEST FAILED"

typedef float vec3f[3];
typedef float vec2f[2];
typedef int vec3i[3];
typedef int vec2i[2];

// this is defining a type of function that accepts two parameters
// typedef is a synonym for an existing type
typedef float (*testf_vec3f_vec3f)(const vec3f, const vec3f);
typedef float* (*testfp_vec3f_vec3f_vec3f)(vec3f, const vec3f, const vec3f);

float dotProduct(const vec3f v1, const vec3f v2) {
    return (v1[0] * v2[0]) + (v1[1] * v2[1]) + (v1[2] + v1[2]);
}
// cross product isnt finished it doesnt actually evaluate correctly
float* crossProduct(vec3f result, const vec3f v1, const vec3f v2) {
    result[0] = v1[0] + v2[0];

    return result;
}

// defining a function pointer
// this wouldnt really make sense? what if you have a function with multiple parameters?
float(*initTest_f_v3f_v3f)(float*, float*) = crossProduct;
float(*initTest_fp_v3f_v3f_v3f)(float*, float*, float*) = crossProduct;

#define testFunction(X) _Generic((X), \
    testf_vec3f_vec3f: initTest_f_v3f_v3f,\
    testfp_vec3f_vec3f_vec3f: initTest_fp_v3f_v3f_v3f\
    )(X)
//int(*testFuncFloat)(float*, float*) = dotProduct;

// the _Generic keyword takes
// _Generic(controlling_expression, association_list)
// controlling_expression is a type defined at compile time
// association_list is every 

// this is a struct that encapsulates one test object, make something iterate through test objects and call them all and collect results
typedef struct test_t {
    void* func;
    void* expected;
    void* result;
} test_t;

// test_t 
// 

int worker_thread_work(worker_t* worker)
{
    assert(worker);

    // Rename worker
    char name[32] = { 0 };
    snprintf(name, sizeof(name), "worker_thread_%"PRIu64, worker->id);
    cat_thread_rename(name);

    // Identify section to compute
    size_t start = (worker->id) * worker->manager->summands_count / NUM_WORKERS;
    size_t end = (worker->id + 1) * worker->manager->summands_count / NUM_WORKERS;

    // Compute the partial sum
    size_t partial_sum = 0;
    for (size_t i = start; i < end; i++)
        partial_sum += worker->manager->summands[i];

    // Safely update manager with sum
    mtx_lock(&worker->manager->lock);
    worker->manager->sum += partial_sum;
    mtx_unlock(&worker->manager->lock);

    return (int)partial_sum;
}

int worker_thread_entry(void* arg)
{
    return worker_thread_work((worker_t*)arg);
}

void spawn_worker(worker_t* worker, manager_t* manager, uint64_t id) {
    worker->manager = manager;
    worker->id = id;
    thrd_create(&worker->thread, &worker_thread_entry, worker);
}

void test_unit_tests(void) 
{
    // this would be taken from a file reader or something

    //test_t testA;
    //test_t testB;
    //test_t testC;


}

__declspec(spectre(nomitigation))
void test_tasks(void)
{
    // Create manager
    manager_t manager;
    mtx_init(&manager.lock, 0);
    manager.summands_count = 1024;
    manager.summands = malloc(manager.summands_count * sizeof * manager.summands);
    for (size_t i = 0; i < manager.summands_count; i++)
        manager.summands[i] = 1;
    manager.sum = 0;

    // Spawn workers
    worker_t workers[NUM_WORKERS];
    for (int i = 0; i < NUM_WORKERS; i++)
        spawn_worker(&workers[i], &manager, (uint64_t)i);

    // Wait for workers to finish
    for (int i = 0; i < NUM_WORKERS; i++)
        thrd_join(workers[i].thread, &workers[i].result);

    // Print total sum
    printf("\nTotal sum = %"PRId64"!\n", manager.sum);
    printf("Enter to exit\n");
    getchar();

    // Destroy manager
    mtx_destroy(&manager.lock);
    free(manager.summands);
}


// Windows entry point: 
// https://learn.microsoft.com/en-us/windows/win32/api/winbase/nf-winbase-winmain
int WINAPI WinMain(
    _In_     HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_     LPSTR     lpCmdLine,
    _In_     int       nShowCmd
)
{
    typedef int(*cat_main_t)(int const argc, char const* const argv[]);

    int result = 0;
    HMODULE dylib = NULL;
    cat_main_t cat_plugin_test_all = NULL;
    unused2(hInstance, hPrevInstance);
    unused(nShowCmd);

    _set_error_mode(_OUT_TO_MSGBOX);
    cat_console_create();
    dylib = cat_dylib_load("cat_dtest.dll");
    if (dylib)
        cat_plugin_test_all = (cat_main_t)cat_dylib_symbol(dylib, "cat_plugin_test_all");

    result |= cat_test_all(1, &lpCmdLine);
    if (cat_plugin_test_all)
        result |= cat_plugin_test_all(1, &lpCmdLine);
    
    if (dylib)
    {
        cat_plugin_test_all = NULL;
        cat_dylib_unload(dylib);
    }

    test_unit_tests();
    test_tasks();

    cat_console_destroy();
    return result;
}


#else // #ifdef _WIN32
#error Unsupported platform.
#endif // #else // #ifdef _WIN32