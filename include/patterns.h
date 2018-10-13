//==============================================================================
// Copyright (c) 2018 Eric Seguin, all rights reserved.
//==============================================================================

#pragma once

#include <memory>

namespace beewatch
{

    //==============================================================================
    /**
     * @struct singleton_t
     *
     * Class paradigm restricting class to a single instance.
     *
     * NB: Derived classes should have a default constructor in their protected section
     *     in order for the singleton accessor method to work.
     *
     * Properties:
     *   - Prohibits object construction
     *   - Prohibits copy operations
     *   - Prohibits move operations
     *   - Provides get() method giving access to the singleton instance
     *
     * @tparam Class    Name of class to apply pattern to
     */
    template <typename Class>
    struct singleton_t
    {
    private:
        struct class_t : public Class {};
        
    public:
        // Allow default constructor
        singleton_t() = default;

        // Delete copy constructor & assignment operator
        singleton_t(const singleton_t&) = delete;
        singleton_t& operator=(const singleton_t&) = delete;

        // Allow move constructor & assignment operator
        singleton_t(singleton_t&&) = default;
        singleton_t& operator=(singleton_t&&) = default;
        
        /// Returns a reference to the singleton instance
        static Class& get()
        {
            static class_t singleton;
            return singleton;
        }
    };

    //==============================================================================
    /**
     * @struct unique_ownership_t
     *
     * Class paradigm restricting each instance to a single owner.
     *
     * Properties:
     *   - Allows object construction
     *   - Prohibits copy operations
     *   - Allows move operations
     *   - Sets a using-declaration mapping Ptr to std::shared_ptr<Class>
     *
     * @tparam Class    Name of class to apply pattern to
     */
    template <typename Class>
    struct unique_ownership_t
    {
        // Allow default constructor
        unique_ownership_t() = default;

        // Delete copy constructor & assignment operator
        unique_ownership_t(const unique_ownership_t&) = delete;
        unique_ownership_t& operator=(const unique_ownership_t&) = delete;

        // Allow move constructor & assignment operator
        unique_ownership_t(unique_ownership_t&&) = default;
        unique_ownership_t& operator=(unique_ownership_t&&) = default;
        
        // Define Ptr as a unique_ptr
        using Ptr = std::unique_ptr<Class>;
    };

    //==============================================================================
    /**
     * @struct shared_ownership_t
     *
     * Class paradigm allowing shared ownership of each instance.
     *
     * Properties:
     *   - Allows object construction
     *   - Allows copy operations
     *   - Allows move operations
     *   - Sets a using-declaration mapping Ptr to std::shared_ptr<Class>
     *
     * @tparam Class    Name of class to apply pattern to
     */
    template <typename Class>
    struct shared_ownership_t
    {
        // Allow default constructor
        shared_ownership_t() = default;

        // Allow copy constructor & assignment operator
        shared_ownership_t(const shared_ownership_t&) = default;
        shared_ownership_t& operator=(const shared_ownership_t&) = default;

        // Allow move constructor & assignment operator
        shared_ownership_t(shared_ownership_t&&) = default;
        shared_ownership_t& operator=(shared_ownership_t&&) = default;
        
        // Define Ptr as a shared_ptr
        using Ptr = std::shared_ptr<Class>;
    };

}
