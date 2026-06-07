#----------------------------------------------------------------------------
# Google tests
if(WITH_GOOGLE_TEST)
    include(FetchContent)

    FetchContent_Declare(
      googletest
      # URL !!! тут должна быть сслыка на архив
      GIT_REPOSITORY https://github.com/google/googletest.git
      # GIT_TAG v1.16.0 # последняя версия которая поддерживает С++14
      GIT_TAG v1.17.0
    )

    # Исключение тестов самого GoogleTest
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(googletest)

    include(GoogleTest)

    add_executable(test_gtest
        gtest_print_ip.cpp
    )

    # Линкуемся с GTest::gtest_main (автоматически добавляет функцию main())
    target_link_libraries(test_gtest
        PRIVATE GTest::gtest_main ${PROJECT_NAME}_lib compiler_flags
    )

    gtest_discover_tests(test_gtest)
 endif()
