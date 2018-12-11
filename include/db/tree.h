//==============================================================================
// Copyright (c) 2018 Eric Seguin, all rights reserved.
//==============================================================================

#pragma once

#include "patterns.hpp"

#include <shared_mutex>
#include <string>
#include <vector>


namespace beewatch::db
{

    //==============================================================================
    class Node : public shared_ownership_t<Node>,
                 public std::enable_shared_from_this<Node>
    {
    protected:
        //==============================================================================
        /**
         * @brief Construct node with a given value and list of child nodes
         */
        Node(Ptr parent = nullptr, std::string value = "", std::vector<Ptr> children = {});

    public:
        //==============================================================================
        /**
         * @brief Construct node, returning a shared pointer
         */
        static Ptr create(Ptr parent = nullptr, std::string value = "", std::vector<Ptr> children = {});

        /**
         * @brief Get shared pointer to instance
         */
        Ptr get();

        //==============================================================================
        /**
         * @brief Add child to this node (NB: child node will now have this node as its parent)
         */
        void addChild(Ptr child);

        /**
         * @brief Remove child node
         */
        void removeChild(Ptr child);

        //==============================================================================
        std::string str() const;


    private:
        //==============================================================================
        Ptr _parent;
        std::string _value;
        std::vector<Ptr> _children;

        mutable std::shared_mutex _mutex;
    };

} // namespace beewatch::db
